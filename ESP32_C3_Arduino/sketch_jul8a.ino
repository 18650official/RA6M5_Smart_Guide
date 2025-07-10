#include <Arduino.h>
#include "USB.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <time.h>

#include <base64.h>
#include <WiFiClientSecure.h>

// Location structure
struct GpsLocation {
  float latitude;
  float longitude;
  char valid;
} loc;

// Network func definition
// Global variables for WiFi credentials
void Wifi_init_process(); //Wifi启动流程
void LoadWifiConfig(void); //加载数据
void saveWifiConfig(String newssid, String newpswd); //存储数据
bool ConnectWifi(); //网卡连接操作
void handleRoot(); //web配置界面
void handleSave(); //web服务器保存
void sendMail(String s); //发送邮件函数
bool get_loc_online(GpsLocation* loc); //定位函数

// Variables
String ssid = "";
String pswd = "";

#define EEPROM_SIZE 64 // Define the size of EEPROM
#define SSID_ADDR 0 // Address to store SSID
#define PSWD_ADDR 32 // Address to store Password

// Mail
#define SMTP_HOST "live.smtp.mailtrap.io"
#define SMTP_PORT 587 // SSL端口
#define AUTHOR_EMAIL "smtp@mailtrap.io"
#define AUTHOR_FROM "dev@snowmiku-home.top"
#define AUTHOR_PASS "db95ea0cd46c04c03eaf950a6e780440"

// Email recipient address
const String mail_addr = "1352218398@qq.com"; 

// Server / Client objects
WebServer server(80); 
HTTPClient my_http;

// 脱机测试标志位
const bool Offline_test = true;

// Serial data
#define CMD_BUFFER_SIZE 64

bool cmdReady = false;            // 是否接收到一条完整命令
char cmdBuffer[CMD_BUFFER_SIZE];  // 接收缓冲区
uint8_t cmdIndex = 0;
String command = "";              // 解析后的命令（去掉 #）

void pollSerial1();
void findRA(); // RA MCU Support

void setup() 
{
  Serial.begin(115200);
  while (!Serial); // 等待串口
  Serial1.begin(115200, SERIAL_8N1, 20, 21); // Serial1 for GPS module
  while(!Serial1);
  delay(400);
  Serial.println("[INFO] ESP32 Init!");  
  // Find RA MCU
  delay(10000);
  Serial.println("[INFO] Finding RA MCU...");
  findRA();
  Serial.println("[INFO] RA MCU found, starting Wi-Fi configuration...");
  // Initialize Wi-Fi
  Wifi_init_process();
  Serial.println("[INFO] Wi-Fi initialized, start loop...");
}

void loop() 
{
  pollSerial1();
  if(cmdReady)
  {
    cmdReady = false; // 重置标志位
    if(command == "QUERY")
    {
      Serial1.println("@OK");
    }
    else if(command == "SOS")
    {
      Serial1.println("@OK");
      SOSAction();
    }
  }
}

// RA MCU Support
// 串口接收函数
void pollSerial1() {
  while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\n' || c == '\r') {  // 命令结束
      cmdBuffer[cmdIndex] = '\0';  // 字符串结束符
      cmdIndex = 0;

      if (cmdBuffer[0] == '#') {
        command = String(cmdBuffer + 1);  // 去掉 #
        cmdReady = true;
      }
    } else {
      if (cmdIndex < CMD_BUFFER_SIZE - 1) {
        cmdBuffer[cmdIndex++] = c;
      } else {
        // 超出缓冲区，重置
        cmdIndex = 0;
      }
    }
  }
}

void findRA()
{
  static unsigned long long count = 0;
  Serial1.println("@QUERY"); // 发送查询命令
  while(1)
  {
    count++;
    if(count >= 200000)
    {
      Serial1.println("@QUERY"); // 发送查询命令
      Serial.println("[INFO] Ping!");
      count = 0;
    }
    delay(1);
    pollSerial1();
    if(cmdReady)
    {
      cmdReady = false; // 重置标志位
      if(command == "OK")
      {
        Serial1.println("@OK");
        break;
      }
    }
  }
}

void SOSAction()
{
  get_loc_online(&loc); // 获取在线定位信息
  if (loc.valid == 0) {
    Serial.println("[Warning] Location not valid, cannot send SOS.");
    Serial1.println("@LOC_INVALID");
    return; // 如果定位无效，直接返回
  }
  const String base_url = "设备发出了求助信号，此链接是设备的实时位置:https://uri.amap.com/marker?position=";

    String url = base_url;
    url += String(loc.longitude, 6);  // 经度
    url += ",";
    url += String(loc.latitude, 6);   // 纬度
    delay(1000);
    sendMail(url);
    Serial1.println("@SOS_SENT");
}

// Custom functions
void LoadWifiConfig(void) //加载数据
{
  //初始化EEPROM
  char pswd_buf[32] = {0};
  char ssid_buf[32] = {0};
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(SSID_ADDR, ssid_buf);
  EEPROM.get(PSWD_ADDR, pswd_buf);
  EEPROM.end();
  ssid = String(ssid_buf);
  pswd = String(pswd_buf);
}

void saveWifiConfig(String newssid, String newpswd) //存储数据
{
    EEPROM.begin(EEPROM_SIZE);
    int i = 0;
    //Write SSID
    while(newssid[i]){
      EEPROM.write(SSID_ADDR+i, newssid[i]);
      delay(1);
      i++;
    }
    EEPROM.write(SSID_ADDR+i, 0);
    //Write PSWD
    i = 0;
    while(newpswd[i]){
      EEPROM.write(PSWD_ADDR+i, newpswd[i]);
      delay(1);
      i++;
    }
    EEPROM.write(PSWD_ADDR+i, 0);
    delay(2);
    EEPROM.end();
    delay(2);
}

bool ConnectWifi()
{
    WiFi.begin(ssid.c_str(), pswd.c_str());
    Serial.print("Connecting");
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 10) {
        delay(1000);
        attempt++;
        Serial.print(".");
    }
    Serial.println();
    Serial.printf("[INFO] Wi-Fi state: %d\n", WiFi.status() == WL_CONNECTED);
    return WiFi.status() == WL_CONNECTED; //返回WiFi连接状态
}

// AP 配置页面
void handleRoot() {
    server.send(200, "text/html",
        "<html><body>"
        "<h1>ESP32 Wi-Fi Configure</h1>"
        "<form action='/save' method='POST'>"
        "SSID: <input type='text' name='ssid'><br>"
        "Password: <input type='password' name='pass'><br>"
        "<input type='submit' value='Save'>"
        "</form></body></html>"
    );
}

// 处理 Wi-Fi 配置提交
void handleSave() {
    if (server.hasArg("ssid") && server.hasArg("pass")) {
        String newSSID = server.arg("ssid");
        String newPass = server.arg("pass");
        saveWifiConfig(newSSID, newPass); //保存从网页获取的配置信息
        server.send(200, "text/plain", "Wi-Fi config saved, rebooting...");
        Serial.println("[INFO] Configuration saved, system will reboot after 2 secs...");
        Serial1.println("@CONFIG_END");
        delay(2000); // 等待2秒
        ESP.restart(); //重启设备
    } else {
        server.send(400, "text/plain", "Error!");
    }
}

// 网络连接程序
void Wifi_init_process()
{
  // 读取存储的 Wi-Fi 信息
    LoadWifiConfig();

    // 连接 Wi-Fi
    if (ssid.length() > 0 && ConnectWifi()) {
    } else {
        WiFi.softAP("ESP32_Config", "12345678");
        IPAddress IP = WiFi.softAPIP();
        Serial1.println("@NEED_CONFIG");
        Serial.printf("[INFO] Please configure network: IP: %d.%d.%d.%d\n", IP[0], IP[1], IP[2], IP[3]);

        server.on("/", handleRoot); //给定服务器回调函数
        server.on("/save", handleSave);
        server.begin();
        while(1)
        {
          server.handleClient();
        }
    }
}

// Send mail
void sendMail(String s)
{
  String smtp_server = SMTP_HOST; // SMTP服务器地址
  int smtp_port = SMTP_PORT; // SMTP服务器端口
  String smtp_user = AUTHOR_EMAIL; // SMTP用户名
  String smtp_pass = AUTHOR_PASS; // SMTP密码
  String sender_email = AUTHOR_EMAIL; // 发件人邮箱
  String recipient_email = mail_addr; // 收件人邮箱
  String author_from = AUTHOR_FROM; // 发件人名称

    if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://send.api.mailtrap.io/api/send");
    http.addHeader("Authorization", "Bearer " + smtp_pass);
    http.addHeader("Content-Type", "application/json");

    // 构造 JSON 字符串
    String postData = "{";
    postData += "\"from\":{\"email\":\"" + author_from + "\",\"name\":\"" + String("ESP32 Dev Module") + "\"},";
    postData += "\"to\":[{\"email\":\"" + mail_addr + "\"}],";
    postData += "\"subject\":\"" + String("Caution") + "\",";
    postData += "\"text\":\"" + s + "\"";
    postData += "}";

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode == 200 ? "[INFO] Sent" : "[Warning] Send failed: Service error");
    } else {
      Serial.println("[Warning] Send failed: Connect failure\n");
    }
    http.end();
  }
}

#define UUID "1af66560-58ac-11f0-a777-298edf4011b5"
#define KEY "Q6GdLHvfESScK1hPpmzesn3s4Hsm0nbo"

typedef struct
{
  String mac;
  signed char rssi;
} ap_info_typedef;

ap_info_typedef ApInfo_array[5]; // 定义AP信息结构体
int ApInfo_count = 0; // AP信息计数

void wifi_scan()
{
  ApInfo_count = WiFi.scanNetworks(); // 扫描Wi-Fi网络
  if (ApInfo_count == 0) {
    return; // 没有找到网络
  } 
  else {
    for (int i = 0; i < 5; ++i) 
    {
      ApInfo_array[i].rssi = WiFi.RSSI(i);
      ApInfo_array[i].mac = WiFi.BSSIDstr(i);
      delay(10);
    }
    return; // 扫描成功
  }
}

// 定义位置信息JSON结构
DynamicJsonDocument doc(1024);
DynamicJsonDocument rep(1024);

// 获取当前时间戳
// 设置时区（中国为 UTC+8）
const long gmtOffset_sec = 8 * 3600;
const int daylightOffset_sec = 0;

void getTimeStamp()
{
// 初始化 NTP
  configTime(gmtOffset_sec, daylightOffset_sec, "ntp.aliyun.com", "ntp.ntsc.ac.cn", "pool.ntp.org");

  // 等待同步完成
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    delay(500);
  }
  // 获取时间戳
  time_t now = time(nullptr); // 获取当前秒级时间戳
  Serial.printf("[INFO] Current timestamp:%d\n", now);
}


// JSON封装
String JsonSerialization()
{
  String message;
  long long timestamp_ms = time(nullptr) * 1000LL;

  doc["timestamp"] = timestamp_ms; // 获取当前时间戳
  doc["id"] = "esp32-" + String(random(1000000));
  doc["asset"]["id"] = UUID;
  doc["asset"]["manufacturer"] = "esp32-s2";
  doc["location"]["timestamp"] = time(nullptr); // 获取当前时间戳
  for(int i = 0;i < min(5, ApInfo_count);i++)
  {
    doc["location"]["wifis"][i]["macAddress"] = ApInfo_array[i].mac; // 获取MAC地址
    doc["location"]["wifis"][i]["signalStrength"] = ApInfo_array[i].rssi; // 获取信号强度
  }
  serializeJson(doc, message);  // 序列化JSON数据并导出字符串
  return message;
}

bool get_loc_online(GpsLocation *loc)
{
  // 获取当前时间戳
  getTimeStamp();
  // 扫描Wi-Fi网络
  ap_info_typedef ApInfo;
  wifi_scan();
  String msg_to_send = JsonSerialization();
  // 发送HTTP POST请求
  HTTPClient http;
  http.begin("https://api.newayz.com/location/hub/v1/track_points?access_key=Q6GdLHvfESScK1hPpmzesn3s4Hsm0nbo");      
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Host", "api.newayz.com");
  http.addHeader("Connection", "keep-alive");
  int httpCode = http.POST(msg_to_send); // 发送POST请求并获取响应代码
  String payload = http.getString();                                    
  // 检查HTTP响应代码
  DeserializationError error = deserializeJson(rep, payload);
  if (error) 
  {
    Serial.println("[Warning] Failed to parse JSON response");
    delay(1000);
    return false; // 如果解析失败，返回false
  }
  double longitude = rep["location"]["position"]["point"]["longitude"];
  double latitude = rep["location"]["position"]["point"]["latitude"];
  const char * name = rep["location"]["address"]["name"];
  const char * source = rep["location"]["position"]["source"];
  const char * spatialReference = rep["location"]["position"]["spatialReference"];
  http.end(); 
  // 将解析后的数据存储到loc结构体中
  loc->latitude = latitude;
  loc->longitude = longitude;
  loc->valid = 1; // 设置有效状态
  Serial.printf("[INFO] Get Location: lat:%f, lon:%f\n", latitude, longitude);
  return true; // 返回true表示成功
}
