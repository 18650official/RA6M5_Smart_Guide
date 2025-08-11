################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/OLED.c \
../src/SEGGER_RTT.c \
../src/SEGGER_RTT_printf.c \
../src/dmx_tof400c.c \
../src/hal_entry.c \
../src/minmea.c \
../src/mk_gps.c \
../src/mk_i2c.c \
../src/mk_pinctrl.c \
../src/mk_rtt.c \
../src/mpu6050.c \
../src/syscalls.c 

SREC += \
ra1.srec 

C_DEPS += \
./src/OLED.d \
./src/SEGGER_RTT.d \
./src/SEGGER_RTT_printf.d \
./src/dmx_tof400c.d \
./src/hal_entry.d \
./src/minmea.d \
./src/mk_gps.d \
./src/mk_i2c.d \
./src/mk_pinctrl.d \
./src/mk_rtt.d \
./src/mpu6050.d \
./src/syscalls.d 

OBJS += \
./src/OLED.o \
./src/SEGGER_RTT.o \
./src/SEGGER_RTT_printf.o \
./src/dmx_tof400c.o \
./src/hal_entry.o \
./src/minmea.o \
./src/mk_gps.o \
./src/mk_i2c.o \
./src/mk_pinctrl.o \
./src/mk_rtt.o \
./src/mpu6050.o \
./src/syscalls.o 

MAP += \
ra1.map 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM33 -D_RA_ORDINAL=1 -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/src" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/sys" -I"." -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra/fsp/inc" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra/fsp/inc/api" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra/fsp/inc/instances" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra_gen" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra_cfg/fsp_cfg/bsp" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra_cfg/fsp_cfg" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

