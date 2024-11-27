################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Store/src/subdir.mk
-include Startup/subdir.mk
-include Radar_Control/src/subdir.mk
-include Libraries/XMCLib/src/subdir.mk
-include Libraries/Newlib/subdir.mk
-include HostCommunication/src/subdir.mk
-include Dave/Generated/USBD_VCOM/virtual_com/subdir.mk
-include Dave/Generated/USBD_VCOM/class_driver/device/subdir.mk
-include Dave/Generated/USBD_VCOM/subdir.mk
-include Dave/Generated/USBD/usb/core/xmc4000/subdir.mk
-include Dave/Generated/USBD/usb/core/subdir.mk
-include Dave/Generated/USBD/subdir.mk
-include Dave/Generated/TIMER/subdir.mk
-include Dave/Generated/PWM_CCU8/subdir.mk
-include Dave/Generated/INTERRUPT/subdir.mk
-include Dave/Generated/GLOBAL_DMA/subdir.mk
-include Dave/Generated/GLOBAL_CCU8/subdir.mk
-include Dave/Generated/GLOBAL_CCU4/subdir.mk
-include Dave/Generated/GLOBAL_ADC/subdir.mk
-include Dave/Generated/DMA_CH/subdir.mk
-include Dave/Generated/DIGITAL_IO/subdir.mk
-include Dave/Generated/CRC_SW/subdir.mk
-include Dave/Generated/CPU_CTRL_XMC4/subdir.mk
-include Dave/Generated/CMSIS_DSP/TransformFunctions/subdir.mk
-include Dave/Generated/CMSIS_DSP/SupportFunctions/subdir.mk
-include Dave/Generated/CMSIS_DSP/StatisticsFunctions/subdir.mk
-include Dave/Generated/CMSIS_DSP/MatrixFunctions/subdir.mk
-include Dave/Generated/CMSIS_DSP/FilteringFunctions/subdir.mk
-include Dave/Generated/CMSIS_DSP/FastMathFunctions/subdir.mk
-include Dave/Generated/CMSIS_DSP/ControllerFunctions/subdir.mk
-include Dave/Generated/CMSIS_DSP/ComplexMathFunctions/subdir.mk
-include Dave/Generated/CMSIS_DSP/CommonTables/subdir.mk
-include Dave/Generated/CMSIS_DSP/BasicMathFunctions/subdir.mk
-include Dave/Generated/CMSIS_DSP/subdir.mk
-include Dave/Generated/CLOCK_XMC4/subdir.mk
-include Dave/Generated/ADC_MEASUREMENT_ADV/subdir.mk
-include Dave/Generated/subdir.mk
-include DSP_LIB/src/subdir.mk
-include BSP/radar_mcu4/src/subdir.mk
-include BSP/dave_fixes/I2C_MASTER/subdir.mk
-include BSP/Sense2GoLPulse/src/subdir.mk
-include Application/src/subdir.mk
-include Algorithms/src/subdir.mk
-include .metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \

SECONDARY_FLASH += \
S2GLP_Pulsed_Doppler.lst

SECONDARY_SIZE += \
S2GLP_Pulsed_Doppler.siz


LIST_FILES = $(addsuffix .o.lst, $(basename $(OBJS))) 
MAP_FILE = "S2GLP_Pulsed_Doppler.map"

TOOLCHAIN_ROOT=C:/Infineon/Tools/DAVE IDE/4.5.0.202105191637/eclipse/ARM-GCC-49
PROJECT_LOC=C:/Users/Utente/Desktop/Tesi/Firmware_Software/DAVE project/S2GLP_Pulsed_Doppler

# All Target
all: S2GLP_Pulsed_Doppler.elf secondary-outputs

# Tool invocations
S2GLP_Pulsed_Doppler.elf: $(OBJS) ../linker_script.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM-GCC C++ Linker'
	$(file >objects.rsp,$(OBJS))
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-g++" -T"../linker_script.ld" -nostartfiles -Xlinker --gc-sections -specs=nano.specs -specs=nosys.specs -Wl,-Map,$(MAP_FILE) -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "S2GLP_Pulsed_Doppler.elf" "@objects.rsp" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo.
	$(MAKE) --no-print-directory post-build

S2GLP_Pulsed_Doppler.hex: S2GLP_Pulsed_Doppler.elf
	@echo 'Invoking: ARM-GCC Create Flash Image'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-objcopy" -O ihex "S2GLP_Pulsed_Doppler.elf" "S2GLP_Pulsed_Doppler.hex"
	@echo 'Finished building: $@'
	@echo.
S2GLP_Pulsed_Doppler.lst: S2GLP_Pulsed_Doppler.elf
	@echo 'Invoking: ARM-GCC Create Listing'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-objdump" -h -S "S2GLP_Pulsed_Doppler.elf" > "S2GLP_Pulsed_Doppler.lst"
	@echo 'Finished building: $@'
	@echo.
S2GLP_Pulsed_Doppler.siz: S2GLP_Pulsed_Doppler.elf
	@echo 'Invoking: ARM-GCC Print Size'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-size" --format=berkeley "S2GLP_Pulsed_Doppler.elf"
	@echo 'Finished building: $@'
	@echo.
# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(C_DEPS)$(CC_DEPS)$(C++_DEPS)$(CXX_DEPS)$(SECONDARY_FLASH)$(ARM_GCC_FOR_XMC_LINKER_OUTPUT_TYPE_OUTPUTS)$(S_UPPER_DEPS)$(CPP_DEPS)
	-$(RM) .metadata/.plugins/org.eclipse.cdt.make.core/specs.o Algorithms/src/doppler.o Application/src/application.o BSP/Sense2GoLPulse/src/sense2gol_bsp.o BSP/Sense2GoLPulse/src/sense2gol_usb.o BSP/dave_fixes/I2C_MASTER/i2c_master.o BSP/dave_fixes/I2C_MASTER/i2c_master_conf.o BSP/radar_mcu4/src/radar_mcu4_bsp.o BSP/radar_mcu4/src/stream_interface_usb.o DSP_LIB/src/dsp_lib.o Dave/Generated/ADC_MEASUREMENT_ADV/adc_measurement_adv.o Dave/Generated/ADC_MEASUREMENT_ADV/adc_measurement_adv_conf.o Dave/Generated/CLOCK_XMC4/clock_xmc4.o Dave/Generated/CLOCK_XMC4/clock_xmc4_conf.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_abs_f32.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_abs_q15.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_abs_q31.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_abs_q7.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_add_f32.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_add_q15.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_add_q31.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_add_q7.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_dot_prod_f32.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_dot_prod_q15.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_dot_prod_q31.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_dot_prod_q7.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_mult_f32.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_mult_q15.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_mult_q31.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_mult_q7.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_negate_f32.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_negate_q15.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_negate_q31.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_negate_q7.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_offset_f32.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_offset_q15.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_offset_q31.o 
	-$(RM) Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_offset_q7.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_scale_f32.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_scale_q15.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_scale_q31.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_scale_q7.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_shift_q15.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_shift_q31.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_shift_q7.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_sub_f32.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_sub_q15.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_sub_q31.o Dave/Generated/CMSIS_DSP/BasicMathFunctions/arm_sub_q7.o Dave/Generated/CMSIS_DSP/CommonTables/arm_common_tables.o Dave/Generated/CMSIS_DSP/CommonTables/arm_const_structs.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_conj_f32.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_conj_q15.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_conj_q31.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_dot_prod_f32.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_dot_prod_q15.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_dot_prod_q31.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_f32.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_q15.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_q31.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_squared_f32.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_squared_q15.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mag_squared_q31.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.o 
	-$(RM) Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_real_f32.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_real_q15.o Dave/Generated/CMSIS_DSP/ComplexMathFunctions/arm_cmplx_mult_real_q31.o Dave/Generated/CMSIS_DSP/ControllerFunctions/arm_pid_init_f32.o Dave/Generated/CMSIS_DSP/ControllerFunctions/arm_pid_init_q15.o Dave/Generated/CMSIS_DSP/ControllerFunctions/arm_pid_init_q31.o Dave/Generated/CMSIS_DSP/ControllerFunctions/arm_pid_reset_f32.o Dave/Generated/CMSIS_DSP/ControllerFunctions/arm_pid_reset_q15.o Dave/Generated/CMSIS_DSP/ControllerFunctions/arm_pid_reset_q31.o Dave/Generated/CMSIS_DSP/ControllerFunctions/arm_sin_cos_f32.o Dave/Generated/CMSIS_DSP/ControllerFunctions/arm_sin_cos_q31.o Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_cos_f32.o Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_cos_q15.o Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_cos_q31.o Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sin_f32.o Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sin_q15.o Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sin_q31.o Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sqrt_q15.o Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sqrt_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df1_32x64_init_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df1_32x64_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df1_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df1_fast_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df1_fast_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df1_init_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df1_init_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df1_init_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df1_q15.o 
	-$(RM) Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df1_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df2T_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df2T_f64.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df2T_init_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_df2T_init_f64.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_stereo_df2T_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_biquad_cascade_stereo_df2T_init_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_fast_opt_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_fast_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_fast_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_opt_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_opt_q7.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_partial_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_partial_fast_opt_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_partial_fast_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_partial_fast_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_partial_opt_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_partial_opt_q7.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_partial_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_partial_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_partial_q7.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_conv_q7.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_correlate_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_correlate_fast_opt_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_correlate_fast_q15.o 
	-$(RM) Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_correlate_fast_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_correlate_opt_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_correlate_opt_q7.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_correlate_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_correlate_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_correlate_q7.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_decimate_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_decimate_fast_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_decimate_fast_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_decimate_init_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_decimate_init_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_decimate_init_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_decimate_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_decimate_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_fast_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_fast_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_init_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_init_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_init_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_init_q7.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_interpolate_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_interpolate_init_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_interpolate_init_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_interpolate_init_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_interpolate_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_interpolate_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_lattice_f32.o 
	-$(RM) Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_lattice_init_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_lattice_init_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_lattice_init_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_lattice_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_lattice_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_q7.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_sparse_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_sparse_init_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_sparse_init_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_sparse_init_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_sparse_init_q7.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_sparse_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_sparse_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_fir_sparse_q7.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_iir_lattice_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_iir_lattice_init_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_iir_lattice_init_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_iir_lattice_init_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_iir_lattice_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_iir_lattice_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_lms_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_lms_init_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_lms_init_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_lms_init_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_lms_norm_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_lms_norm_init_f32.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_lms_norm_init_q15.o 
	-$(RM) Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_lms_norm_init_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_lms_norm_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_lms_norm_q31.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_lms_q15.o Dave/Generated/CMSIS_DSP/FilteringFunctions/arm_lms_q31.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_add_f32.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_add_q15.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_add_q31.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_cmplx_mult_f32.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_cmplx_mult_q15.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_cmplx_mult_q31.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_init_f32.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_init_q15.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_init_q31.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_inverse_f32.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_inverse_f64.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_mult_f32.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_mult_fast_q15.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_mult_fast_q31.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_mult_q15.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_mult_q31.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_scale_f32.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_scale_q15.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_scale_q31.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_sub_f32.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_sub_q15.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_sub_q31.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_trans_f32.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_trans_q15.o Dave/Generated/CMSIS_DSP/MatrixFunctions/arm_mat_trans_q31.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_max_f32.o 
	-$(RM) Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_max_q15.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_max_q31.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_max_q7.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_mean_f32.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_mean_q15.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_mean_q31.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_mean_q7.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_min_f32.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_min_q15.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_min_q31.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_min_q7.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_power_f32.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_power_q15.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_power_q31.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_power_q7.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_rms_f32.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_rms_q15.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_rms_q31.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_std_f32.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_std_q15.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_std_q31.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_var_f32.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_var_q15.o Dave/Generated/CMSIS_DSP/StatisticsFunctions/arm_var_q31.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_f32.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_q15.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_q31.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_q7.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_f32.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_q15.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_q31.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_q7.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_float_to_q15.o 
	-$(RM) Dave/Generated/CMSIS_DSP/SupportFunctions/arm_float_to_q31.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_float_to_q7.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q15_to_float.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q15_to_q31.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q15_to_q7.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q31_to_float.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q31_to_q15.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q31_to_q7.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q7_to_float.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q7_to_q15.o Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q7_to_q31.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_bitreversal.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_bitreversal2.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_f32.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_q15.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_q31.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix2_f32.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix2_init_f32.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix2_init_q15.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix2_init_q31.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix2_q15.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix2_q31.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix4_f32.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix4_init_f32.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix4_init_q15.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix4_init_q31.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix4_q15.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix4_q31.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_cfft_radix8_f32.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_dct4_f32.o 
	-$(RM) Dave/Generated/CMSIS_DSP/TransformFunctions/arm_dct4_init_f32.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_dct4_init_q15.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_dct4_init_q31.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_dct4_q15.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_dct4_q31.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_rfft_f32.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_rfft_fast_f32.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_rfft_fast_init_f32.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_rfft_init_f32.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_rfft_init_q15.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_rfft_init_q31.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_rfft_q15.o Dave/Generated/CMSIS_DSP/TransformFunctions/arm_rfft_q31.o Dave/Generated/CMSIS_DSP/cmsis_dsp.o Dave/Generated/CPU_CTRL_XMC4/cpu_ctrl_xmc4.o Dave/Generated/CPU_CTRL_XMC4/cpu_ctrl_xmc4_conf.o Dave/Generated/CRC_SW/crc_sw.o Dave/Generated/CRC_SW/crc_sw_conf.o Dave/Generated/DAVE.o Dave/Generated/DIGITAL_IO/digital_io.o Dave/Generated/DIGITAL_IO/digital_io_conf.o Dave/Generated/DMA_CH/dma_ch.o Dave/Generated/DMA_CH/dma_ch_conf.o Dave/Generated/GLOBAL_ADC/global_adc.o Dave/Generated/GLOBAL_ADC/global_adc_conf.o Dave/Generated/GLOBAL_CCU4/global_ccu4.o Dave/Generated/GLOBAL_CCU4/global_ccu4_conf.o Dave/Generated/GLOBAL_CCU8/global_ccu8.o Dave/Generated/GLOBAL_CCU8/global_ccu8_conf.o Dave/Generated/GLOBAL_DMA/global_dma.o Dave/Generated/GLOBAL_DMA/global_dma_conf.o Dave/Generated/INTERRUPT/interrupt.o Dave/Generated/INTERRUPT/interrupt_conf.o Dave/Generated/PWM_CCU8/pwm_ccu8.o Dave/Generated/PWM_CCU8/pwm_ccu8_conf.o Dave/Generated/TIMER/timer.o Dave/Generated/TIMER/timer_conf.o Dave/Generated/USBD/usb/core/usb_task.o Dave/Generated/USBD/usb/core/xmc4000/usbd_endpoint_stream_xmc4000.o Dave/Generated/USBD/usb/core/xmc4000/usbd_endpoint_xmc4000.o Dave/Generated/USBD/usbd.o 
	-$(RM) Dave/Generated/USBD/usbd_conf.o Dave/Generated/USBD_VCOM/class_driver/device/cdc_class_device.o Dave/Generated/USBD_VCOM/usbd_vcom.o Dave/Generated/USBD_VCOM/usbd_vcom_conf.o Dave/Generated/USBD_VCOM/virtual_com/descriptors.o HostCommunication/src/EndpointRadarDeviceStatus.o HostCommunication/src/EndpointRadarMcuBase.o HostCommunication/src/EndpointRadars2glpulse.o HostCommunication/src/Protocol.o HostCommunication/src/ProtocolHelper.o Libraries/Newlib/syscalls.o Libraries/XMCLib/src/xmc4_eru.o Libraries/XMCLib/src/xmc4_flash.o Libraries/XMCLib/src/xmc4_gpio.o Libraries/XMCLib/src/xmc4_rtc.o Libraries/XMCLib/src/xmc4_scu.o Libraries/XMCLib/src/xmc_can.o Libraries/XMCLib/src/xmc_ccu4.o Libraries/XMCLib/src/xmc_ccu8.o Libraries/XMCLib/src/xmc_common.o Libraries/XMCLib/src/xmc_dac.o Libraries/XMCLib/src/xmc_dma.o Libraries/XMCLib/src/xmc_dsd.o Libraries/XMCLib/src/xmc_ebu.o Libraries/XMCLib/src/xmc_ecat.o Libraries/XMCLib/src/xmc_eru.o Libraries/XMCLib/src/xmc_eth_mac.o Libraries/XMCLib/src/xmc_eth_phy_dp83848.o Libraries/XMCLib/src/xmc_eth_phy_ksz8031rnl.o Libraries/XMCLib/src/xmc_eth_phy_ksz8081rnb.o Libraries/XMCLib/src/xmc_fce.o Libraries/XMCLib/src/xmc_gpio.o Libraries/XMCLib/src/xmc_hrpwm.o Libraries/XMCLib/src/xmc_i2c.o Libraries/XMCLib/src/xmc_i2s.o Libraries/XMCLib/src/xmc_ledts.o Libraries/XMCLib/src/xmc_posif.o Libraries/XMCLib/src/xmc_rtc.o Libraries/XMCLib/src/xmc_sdmmc.o Libraries/XMCLib/src/xmc_spi.o Libraries/XMCLib/src/xmc_uart.o Libraries/XMCLib/src/xmc_usbd.o Libraries/XMCLib/src/xmc_usbh.o Libraries/XMCLib/src/xmc_usic.o Libraries/XMCLib/src/xmc_vadc.o Libraries/XMCLib/src/xmc_wdt.o Radar_Control/src/acq_buffer.o Radar_Control/src/radar_base.o Radar_Control/src/radar_control.o Radar_Control/src/radar_device_status.o Radar_Control/src/radar_mcubase.o Radar_Control/src/radar_pulse.o Startup/startup_XMC4700.o Startup/system_XMC4700.o Store/src/datastore.o Store/src/datastore_data.o 
	-$(RM) Store/src/datastore_eep.o Store/src/ds_radar_interface.o main.o  S2GLP_Pulsed_Doppler.elf $(LIST_FILES) $(MAP_FILE) objects.rsp
	-@echo.
post-build:
	-"C:\Infineon\Tools\DAVE IDE\4.5.0.202105191637\eclipse\ARM-GCC-49/bin/arm-none-eabi-objcopy" -O ihex "S2GLP_Pulsed_Doppler.elf" "S2GLP_Pulsed_Doppler.hex"
	-@echo.
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
