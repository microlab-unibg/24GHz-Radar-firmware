################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Radar_Control/src/acq_buffer.c \
../Radar_Control/src/radar_base.c \
../Radar_Control/src/radar_control.c \
../Radar_Control/src/radar_device_status.c \
../Radar_Control/src/radar_mcubase.c \
../Radar_Control/src/radar_pulse.c 

C_DEPS += \
./Radar_Control/src/acq_buffer.d \
./Radar_Control/src/radar_base.d \
./Radar_Control/src/radar_control.d \
./Radar_Control/src/radar_device_status.d \
./Radar_Control/src/radar_mcubase.d \
./Radar_Control/src/radar_pulse.d 

OBJS += \
./Radar_Control/src/acq_buffer.o \
./Radar_Control/src/radar_base.o \
./Radar_Control/src/radar_control.o \
./Radar_Control/src/radar_device_status.o \
./Radar_Control/src/radar_mcubase.o \
./Radar_Control/src/radar_pulse.o 


# Each subdirectory must supply rules for building sources it contributes
Radar_Control/src/%.o: ../Radar_Control/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM-GCC C Compiler'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-gcc" -MMD -MT "$@" -DXMC4700_E196x2048 -DUSE_S2GLPULSE_BOARD -DUSE_MICRIUM_GUI -DDS_USE_EEPROM -I"$(PROJECT_LOC)" -I"$(PROJECT_LOC)/Dave/Generated/TIMER" -I"$(PROJECT_LOC)/Application/inc" -I"$(PROJECT_LOC)/Algorithms/inc" -I"$(PROJECT_LOC)/BSP/Sense2GoLPulse/inc" -I"$(PROJECT_LOC)/BSP/radar_mcu4/inc" -I"$(PROJECT_LOC)/BSP/dave_fixes" -I"$(PROJECT_LOC)/BSP/dave_fixes/I2C_MASTER" -I"$(PROJECT_LOC)/Dave/Generated" -I"$(PROJECT_LOC)/Dave/Generated/CMSIS_DSP" -I"$(PROJECT_LOC)/DSP_LIB/inc" -I"$(PROJECT_LOC)/HostCommunication/inc" -I"$(PROJECT_LOC)/Libraries/XMCLib/inc" -I"$(PROJECT_LOC)/Libraries/CMSIS/Include" -I"$(PROJECT_LOC)/Libraries/CMSIS/Infineon/XMC4700_series/Include" -I"$(PROJECT_LOC)/Radar_Control/inc" -I"$(PROJECT_LOC)/Store/inc" -O3 -ffunction-sections -fdata-sections -Wall -std=gnu99 -mfloat-abi=softfp -Wa,-adhlns="$@.lst" -pipe -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $@" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -g -gdwarf-2 -o "$@" "$<" 
	@echo 'Finished building: $<'
	@echo.

