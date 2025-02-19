################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_conj_f32.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_conj_q15.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_conj_q31.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_dot_prod_f32.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_dot_prod_q15.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_dot_prod_q31.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_f32.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_q15.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_q31.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_squared_f32.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_squared_q15.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_squared_q31.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_real_f32.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_real_q15.c \
../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_real_q31.c 

C_DEPS += \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_conj_f32.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_conj_q15.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_conj_q31.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_dot_prod_f32.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_dot_prod_q15.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_dot_prod_q31.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_f32.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_q15.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_q31.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_squared_f32.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_squared_q15.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_squared_q31.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_real_f32.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_real_q15.d \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_real_q31.d 

OBJS += \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_conj_f32.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_conj_q15.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_conj_q31.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_dot_prod_f32.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_dot_prod_q15.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_dot_prod_q31.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_f32.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_q15.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_q31.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_squared_f32.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_squared_q15.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_squared_q31.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_real_f32.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_real_q15.o \
./Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_real_q31.o 


# Each subdirectory must supply rules for building sources it contributes
Dave/Generated/CMSIS_DSP/ComplexMathFunctions/%.o: ../Dave/Generated/CMSIS_DSP/ComplexMathFunctions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM-GCC C Compiler'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-gcc" -MMD -MT "$@" -DXMC4700_E196x2048 -DUSE_S2GLPULSE_BOARD -DUSE_MICRIUM_GUI -DDS_USE_EEPROM -I"$(PROJECT_LOC)" -I"$(PROJECT_LOC)/Dave/Generated/TIMER" -I"$(PROJECT_LOC)/Application/inc" -I"$(PROJECT_LOC)/Algorithms/inc" -I"$(PROJECT_LOC)/BSP/Sense2GoLPulse/inc" -I"$(PROJECT_LOC)/BSP/radar_mcu4/inc" -I"$(PROJECT_LOC)/BSP/dave_fixes" -I"$(PROJECT_LOC)/BSP/dave_fixes/I2C_MASTER" -I"$(PROJECT_LOC)/Dave/Generated" -I"$(PROJECT_LOC)/Dave/Generated/CMSIS_DSP" -I"$(PROJECT_LOC)/DSP_LIB/inc" -I"$(PROJECT_LOC)/HostCommunication/inc" -I"$(PROJECT_LOC)/Libraries/XMCLib/inc" -I"$(PROJECT_LOC)/Libraries/CMSIS/Include" -I"$(PROJECT_LOC)/Libraries/CMSIS/Infineon/XMC4700_series/Include" -I"$(PROJECT_LOC)/Radar_Control/inc" -I"$(PROJECT_LOC)/Store/inc" -O3 -ffunction-sections -fdata-sections -Wall -std=gnu99 -mfloat-abi=softfp -Wa,-adhlns="$@.lst" -pipe -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $@" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -g -gdwarf-2 -o "$@" "$<" 
	@echo 'Finished building: $<'
	@echo.

