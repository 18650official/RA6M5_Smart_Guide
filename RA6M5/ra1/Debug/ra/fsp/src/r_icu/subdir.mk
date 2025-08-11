################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/fsp/src/r_icu/r_icu.c 

SREC += \
ra1.srec 

C_DEPS += \
./ra/fsp/src/r_icu/r_icu.d 

OBJS += \
./ra/fsp/src/r_icu/r_icu.o 

MAP += \
ra1.map 


# Each subdirectory must supply rules for building sources it contributes
ra/fsp/src/r_icu/%.o: ../ra/fsp/src/r_icu/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM33 -D_RA_ORDINAL=1 -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/src" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/sys" -I"." -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra/fsp/inc" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra/fsp/inc/api" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra/fsp/inc/instances" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra_gen" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra_cfg/fsp_cfg/bsp" -I"C:/Users/Snowmiku/e2_studio/workspace2/ra1/ra_cfg/fsp_cfg" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

