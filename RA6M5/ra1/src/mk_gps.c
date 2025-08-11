#include "mk_gps.h"

bool parseGNRMCData(GpsLocation_Typedef* loc, uint8_t* line)
{
	float lat, lon;
	bool isSucceed = false;

	// 判断并解析语句类型
	if (minmea_sentence_id(line, false) == MINMEA_SENTENCE_RMC) {
	    struct minmea_sentence_rmc frame;
	    if (minmea_parse_rmc(&frame, line))
	    {
	        lat = minmea_tocoord(&frame.latitude);
	        lon = minmea_tocoord(&frame.longitude);
	        isSucceed = frame.valid;
	    }
	    else
	    {
	        	isSucceed = false;
	    }
	    // Write out data
	    	loc->isSucceed = isSucceed;
	    loc->lat = lat;
	    loc->lon = lon;
	    	return true;
	}
	else return false; //解析的不是GNRMC
}
