function parseEPStatus(uErrorCode)

% No error has occurred.
EP_STATUS_OK                              = hex2dec('0000');

% The requsted operation can't be executed. A possible reason is that a
% certain test mode is activated or the automatic trigger is active.
EP_RADAR_ERR_BUSY                         = hex2dec('0002');

% The requested operation is not supported by the currently active mode of
% operation.
EP_RADAR_ERR_INCOMPATIBLE_MODE            = hex2dec('0003');

% A timeout has occurred while waiting for a data frame to be acquired.
EP_RADAR_ERR_TIME_OUT                     = hex2dec('0004');

% The requested time interval between two frames is out of range.
EP_RADAR_ERR_UNSUPPORTED_FRAME_INTERVAL   = hex2dec('0005');

% One or more of the selected RX or TX antennas is not present on the device.
EP_RADAR_ERR_ANTENNA_DOES_NOT_EXIST       = hex2dec('0006');

% The requested temperature sensor does not exist.
EP_RADAR_ERR_SENSOR_DOES_NOT_EXIST        = hex2dec('0007');

% The combination of chirps per frame, samples per chirp and number of
% antennas is not supported by the driver. A possible reason is the limit of
% the driver internal data memory.
EP_RADAR_ERR_UNSUPPORTED_FRAME_FORMAT     = hex2dec('0008');

% The specified RF frequency is not in the supported range of the device.
EP_RADAR_ERR_FREQUENCY_OUT_OF_RANGE       = hex2dec('0009');

% The specified transmission power is not in the valid range of
% 0...max_tx_power (see \ref Device_Info_t).
EP_RADAR_ERR_POWER_OUT_OF_RANGE           = hex2dec('000A');

% The device is not capable to capture the requested part of the complex
% signal (see \ref Device_Info_t).
EP_RADAR_ERR_UNAVAILABLE_SIGNAL_PART      = hex2dec('000B');

% Parameter out of range
EP_RADAR_ERR_PARAMETER_OUT_OF_RANGE      = hex2dec('000C');

% The specified FMCW ramp direction is not supported by the device.
EP_RADAR_ERR_UNSUPPORTED_DIRECTION        = hex2dec('0020');

% The specified sampling rate is out of range.
EP_RADAR_ERR_SAMPLERATE_OUT_OF_RANGE      = hex2dec('0050');

% The specified sample resolution is out of range.
EP_RADAR_ERR_UNSUPPORTED_RESOLUTION       = hex2dec('0051');

% The specified PGA gain level is out of range
EP_RADAR_ERR_UNSUPPORTED_PGA_GAIN       = hex2dec('0052');

% The specified TX mode is not supported by the device.
EP_RADAR_ERR_UNDEFINED_TX_MODE            = hex2dec('0100');

% The specified high pass filter gain is not defined.
EP_RADAR_ERR_UNDEFINED_HP_GAIN            = hex2dec('0101');

% The specified high pass filter cutoff frequency is not defined.
EP_RADAR_ERR_UNDEFINED_HP_CUTOFF          = hex2dec('0102');

% The specified gain adjustment setting is not defined.
EP_RADAR_ERR_UNDEFINED_VGA_GAIN           = hex2dec('0103');

% The specified reset timer period is out of range.
EP_RADAR_ERR_RESET_TIMER_OUT_OF_RANGE     = hex2dec('0104');

% The specified charge pump current is out of range.
EP_RADAR_ERR_INVALID_CHARGE_PUMP_CURRENT  = hex2dec('0105');

% The specified charge pump pulse width is not defined.
EP_RADAR_ERR_INVALID_PULSE_WIDTH          = hex2dec('0106');

% The specified modulator order is not defined.
EP_RADAR_ERR_INVALID_FRAC_ORDER           = hex2dec('0107');

% The specified dither mode is not defined.
EP_RADAR_ERR_INVALID_DITHER_MODE          = hex2dec('0108');

% The specified cycle slip reduction mode is not defined.
EP_RADAR_ERR_INVALID_CYCLE_SLIP_MODE      = hex2dec('0109');

% The calibration of phase settings or base band chain did not succeed.
EP_RADAR_ERR_CALIBRATION_FAILED           = hex2dec('010A');

% The provided oscillator phase setting is not valid. It's forbidden to
% disable both phase modulators.
EP_RADAR_ERR_INVALID_PHASE_SETTING        = hex2dec('010B');

% The specified ADC tracking mode is not supported by the device.
EP_RADAR_ERR_UNDEFINED_TRACKING_MODE      = hex2dec('0110');

% The specified ADC sampling time is not supported by the device.
EP_RADAR_ERR_UNDEFINED_ADC_SAMPLE_TIME    = hex2dec('0111');

% The specified shape sequence is not supported. There must not be a gap
% between used shapes.
EP_RADAR_ERR_NONCONTINUOUS_SHAPE_SEQUENCE = hex2dec('0120');

% One or more specified number of repetition is not supported. Only powers of
% two are allowed. Total numbers of shape groups must not exceed 4096.
EP_RADAR_ERR_UNSUPPORTED_NUM_REPETITIONS  = hex2dec('0121');

% One or more of the specified power modes is not supported.
EP_RADAR_ERR_UNSUPPORTED_POWER_MODE       = hex2dec('0122');

% One or more of the specified post shape / post shape set delays is not
% supported.
EP_RADAR_ERR_POST_DELAY_OUT_OF_RANGE      = hex2dec('0123');

% The specified number of frames is out of range.
EP_RADAR_ERR_NUM_FRAMES_OUT_OF_RANGE      = hex2dec('0124');

% The requested shape does not exist.
EP_RADAR_ERR_SHAPE_NUMBER_OUT_OF_RANGE    = hex2dec('0125');

% The specified pre-chirp delay is out of range.
EP_RADAR_ERR_PRECHIRPDELAY_OUT_OF_RANGE   = hex2dec('0126');

% The specified post-chirp delay is out of range.
EP_RADAR_ERR_POSTCHIRPDELAY_OUT_OF_RANGE  = hex2dec('0127');

% The specified PA delay is out of range.
EP_RADAR_ERR_PADELAY_OUT_OF_RANGE         = hex2dec('0128');

% The specified ADC delay is out of range.
EP_RADAR_ERR_ADCDELAY_OUT_OF_RANGE        = hex2dec('0129');

% The specified wake up time is out of range.
EP_RADAR_ERR_WAKEUPTIME_OUT_OF_RANGE      = hex2dec('012A');

% The specified PLL settle time is out of range.
EP_RADAR_ERR_SETTLETIME_OUT_OF_RANGE      = hex2dec('012B');

% The specified FIFO slice size is not supported.
EP_RADAR_ERR_UNSUPPORTED_FIFO_SLICE_SIZE  = hex2dec('012C');

% The FIFO slice can't be released. It has not been used.
EP_RADAR_ERR_SLICES_NOT_RELEASABLE        = hex2dec('012D');

% A FIFO overflow has occurred. A reset is needed.
EP_RADAR_ERR_FIFO_OVERFLOW                = hex2dec('012E');

% No memory buffer has been provided to store the radar data.
EP_RADAR_ERR_NO_MEMORY                    = hex2dec('012F');

% The received SPI data sequence does not match the expected one.
EP_RADAR_ERR_SPI_TEST_FAILED              = hex2dec('0130');

% The chip could not be programmed.
EP_RADAR_ERR_CHIP_SETUP_FAILED            = hex2dec('0131');

% The On-Chip FIFO memory test failed.
EP_RADAR_ERR_MEMORY_TEST_FAILED           = hex2dec('0132');

% The Device is not supported by the driver.
EP_RADAR_ERR_DEVICE_NOT_SUPPORTED         = hex2dec('0133');

% The requested feature is not supported by the connected device.
EP_RADAR_ERR_FEATURE_NOT_SUPPORTED        = hex2dec('0134');

% The PA Delay is shorter than the pre-chirp delay.
EP_RADAR_ERR_PRECHIRP_EXCEEDS_PADELAY     = hex2dec('0135');

% The register selected for override does not exist.
EP_RADAR_ERR_REGISTER_OUT_OF_RANGE        = hex2dec('0136');

% The selected reference clock frequency is not supported
% by the device.
EP_RADAR_ERR_UNSUPPORTED_FREQUENCY        = hex2dec('0137');

% The specified FIFO power mode is not supported.
EP_RADAR_ERR_UNSUPPORTED_FIFO_POWER_MODE  = hex2dec('0140');

% The specified pad driver mode is not supported.
EP_RADAR_ERR_UNSUPPORTED_PAD_DRIVER_MODE  = hex2dec('0141');

% The specified bandgap startup delay is out of range.
EP_RADAR_ERR_BANDGAP_DELAY_OUT_OF_RANGE   = hex2dec('0142');

% The specified MADC startup delay is out of range.
EP_RADAR_ERR_MADC_DELAY_OUT_OF_RANGE      = hex2dec('0143');

% The specified PLL startup delay is out of range.
EP_RADAR_ERR_PLL_ENABLE_DELAY_OUT_OF_RANGE = hex2dec('0144');

% The specified PLL Divider startup delay is out of range.
EP_RADAR_ERR_PLL_DIVIDER_DELAY_OUT_OF_RANGE = hex2dec('0145');

% The specified clock doubler mode is not supported.
EP_RADAR_ERR_DOUBLER_MODE_NOT_SUPPORTED   = hex2dec('0146');

% The specified input duty cycle correction is out of range.
EP_RADAR_ERR_DC_IN_CORRECTION_OUT_OF_RANGE = hex2dec('0147');

% The specified output duty cycle correction is out of range.
EP_RADAR_ERR_DC_OUT_CORRECTION_OUT_OF_RANGE = hex2dec('0148');

% The specified anti alias filter frequency is not supported.
EP_RADAR_ERR_AAF_FREQ_NOT_SUPPORTED       = hex2dec('0149');

% The specified TX toggle mode is not supported.
EP_RADAR_ERR_TX_TOGGLE_MODE_NOT_SUPPORTED = hex2dec('014A');

% Baseband test and TX toggle test can't be used simultaniously.
EP_RADAR_ERR_TEST_CONFLICT_BB_TX          = hex2dec('014B');

% The specified power sensing delay is out of range.
EP_RADAR_ERR_POWER_SENS_DELAY_OUT_OF_RANGE = hex2dec('014C');

% The period between chirp start and power measurement is not long enough to
% allow switching the MADC input to power sensor channel.
EP_RADAR_ERR_NO_SWITCH_TIME_MADC_POWER     = hex2dec('014D');

% The period between power measurement and chirp acquisition is not long
% enough to switch MADC input to the RX channel.
EP_RADAR_ERR_NO_SWITCH_TIME_MADC_RX        = hex2dec('014E');

% The period between end of chirp acquisition and temperature measurement is
% not long enough to switch MADC input to temperature sensor channel.
EP_RADAR_ERR_NO_SWITCH_TIME_MADC_TEMP      = hex2dec('014F');

% The chirp end delay is not long enough for temperature measurement.
EP_RADAR_ERR_NO_MEASURE_TIME_MADC_TEMP     = hex2dec('0150');

% When temperature sensing is enabled, chirps with all RX channels disabled
% are not allowed.
EP_RADAR_ERR_TEMP_SENSING_WITH_NO_RX       = hex2dec('0151');

% The command sent to endpoint 0 is not defined.
PROTOCOL_STATUS_DEVICE_BAD_COMMAND        = hex2dec('FFFF');

switch (uErrorCode)
    case EP_STATUS_OK
        return;
        
    case EP_RADAR_ERR_BUSY
        szErrorMessage = '[Endpoint] The device is busy. Maybe test mode or automatic trigger is active.';
        
    case EP_RADAR_ERR_INCOMPATIBLE_MODE
        szErrorMessage = '[Endpoint] The requested operation is not supported by the currently active mode of operation.';
        
    case EP_RADAR_ERR_TIME_OUT
        szErrorMessage = '[Endpoint] A timeout has occurred while waiting for a data frame to be acquired.';
        
    case EP_RADAR_ERR_UNSUPPORTED_FRAME_INTERVAL
        szErrorMessage = '[Endpoint] The requested time interval is out of range.';
        
    case EP_RADAR_ERR_ANTENNA_DOES_NOT_EXIST
        szErrorMessage = '[Endpoint] One or more of the selected antennas is not present on the device.';
        
    case EP_RADAR_ERR_SENSOR_DOES_NOT_EXIST
        szErrorMessage = '[Endpoint] The requested temperature sensor does not exist.';
        
    case EP_RADAR_ERR_UNSUPPORTED_FRAME_FORMAT
        szErrorMessage = '[Endpoint] The specified frame format is not supported.';
        
    case EP_RADAR_ERR_FREQUENCY_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified RF frequency is out of range.';
        
    case EP_RADAR_ERR_POWER_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified TX power is out of range.';
        
    case EP_RADAR_ERR_UNAVAILABLE_SIGNAL_PART
        szErrorMessage = '[Endpoint] The device is not capable to capture the requested part of the complex signal.';
        
    case EP_RADAR_ERR_UNSUPPORTED_DIRECTION
        szErrorMessage = '[Endpoint] The specified FMCW ramp direction is not supported by the device.';
        
    case EP_RADAR_ERR_SAMPLERATE_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified sampling rate is out of range.';
        
    case EP_RADAR_ERR_UNSUPPORTED_RESOLUTION
        szErrorMessage = '[Endpoint] The specified ADC resolution is out of range.';
        
    case EP_RADAR_ERR_UNSUPPORTED_PGA_GAIN
        szErrorMessage = '[Endpoint] The specified PGA gain level is out of range.';
        
    case EP_RADAR_ERR_UNDEFINED_TX_MODE
        szErrorMessage = '[Endpoint] The specified TX mode is not supported by the device.';
        
    case EP_RADAR_ERR_UNDEFINED_HP_GAIN
        szErrorMessage = '[Endpoint] The specified high pass filter gain is not defined.';
        
    case EP_RADAR_ERR_UNDEFINED_HP_CUTOFF
        szErrorMessage = '[Endpoint] The specified high pass filter cutoff frequency is not defined.';
        
    case EP_RADAR_ERR_UNDEFINED_VGA_GAIN
        szErrorMessage = '[Endpoint] The specified gain adjustment setting is not defined.';
        
    case EP_RADAR_ERR_RESET_TIMER_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified reset timer period is out of range.';
        
    case EP_RADAR_ERR_INVALID_CHARGE_PUMP_CURRENT
        szErrorMessage = '[Endpoint] The specified charge pump current is out of range.';
        
    case EP_RADAR_ERR_INVALID_PULSE_WIDTH
        szErrorMessage = '[Endpoint] The specified charge pump pulse width is not defined.';
        
    case EP_RADAR_ERR_INVALID_FRAC_ORDER
        szErrorMessage = '[Endpoint] The specified modulator order is not defined.';
        
    case EP_RADAR_ERR_INVALID_DITHER_MODE
        szErrorMessage = '[Endpoint] The specified dither mode is not defined.';
        
    case EP_RADAR_ERR_INVALID_CYCLE_SLIP_MODE
        szErrorMessage = '[Endpoint] The specified cycle slip reduction mode is not defined.';
        
    case EP_RADAR_ERR_CALIBRATION_FAILED
        szErrorMessage = '[Endpoint] The calibration of phase settings or base band chain did not succeed.';
        
    case EP_RADAR_ERR_INVALID_PHASE_SETTING
        szErrorMessage = '[Endpoint] The provided oscillator phase setting is not valid. It''s forbidden to disable both phase modulators.';
        
    case EP_RADAR_ERR_UNDEFINED_TRACKING_MODE
        szErrorMessage = '[Endpoint] The specified ADC tracking mode is not supported by the device.';
        
    case EP_RADAR_ERR_UNDEFINED_ADC_SAMPLE_TIME
        szErrorMessage = '[Endpoint] The specified ADC sampling time is not supported by the device.';

    case PROTOCOL_STATUS_DEVICE_BAD_COMMAND
        szErrorMessage = '[Endpoint] The device received a message that is not supported.';
        
    case EP_RADAR_ERR_NONCONTINUOUS_SHAPE_SEQUENCE
        szErrorMessage = '[Endpoint] The specified shape sequence is not supported. There must not be a gap between used shapes.';
        
    case EP_RADAR_ERR_UNSUPPORTED_NUM_REPETITIONS
        szErrorMessage = '[Endpoint] One or more specified number of repetition is not supported. Only powers of two are allowed. Total numbers of shape groups must not exceed 4096.';
        
    case EP_RADAR_ERR_UNSUPPORTED_POWER_MODE
        szErrorMessage = '[Endpoint] One or more of the specified power modes is not supported.';
        
    case EP_RADAR_ERR_POST_DELAY_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] One or more of the specified post shape / post shape set delays is not supported.';
        
    case EP_RADAR_ERR_NUM_FRAMES_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified number of frames is out of range.';
        
    case EP_RADAR_ERR_SHAPE_NUMBER_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The requested shape does not exist.';
        
    case EP_RADAR_ERR_PRECHIRPDELAY_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified pre-chirp delay is out of range.';
        
    case EP_RADAR_ERR_POSTCHIRPDELAY_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified post-chirp delay is out of range.';
        
    case EP_RADAR_ERR_PADELAY_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified PA delay is out of range.';
        
    case EP_RADAR_ERR_ADCDELAY_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified ADC delay is out of range.';
        
    case EP_RADAR_ERR_WAKEUPTIME_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified wake up time is out of range.';
        
    case EP_RADAR_ERR_SETTLETIME_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified PLL settle time is out of range.';
        
    case EP_RADAR_ERR_UNSUPPORTED_FIFO_SLICE_SIZE
        szErrorMessage = '[Endpoint] The specified FIFO slice size is not supported.';
        
    case EP_RADAR_ERR_SLICES_NOT_RELEASABLE
        szErrorMessage = '[Endpoint] The FIFO slice can''t be released. It has not been used.';
        
    case EP_RADAR_ERR_FIFO_OVERFLOW
        szErrorMessage = '[Endpoint] A FIFO overflow has occurred. A reset is needed.';
        
    case EP_RADAR_ERR_NO_MEMORY
        szErrorMessage = '[Endpoint] No memory buffer has been provided to store the radar data.';
        
    case EP_RADAR_ERR_SPI_TEST_FAILED
        szErrorMessage = '[Endpoint] The received SPI data sequence does not match the expected one.';
        
    case EP_RADAR_ERR_CHIP_SETUP_FAILED
        szErrorMessage = '[Endpoint] The chip could not be programmed.';
        
    case EP_RADAR_ERR_MEMORY_TEST_FAILED
        szErrorMessage = '[Endpoint] The On-Chip FIFO memory test faild.';
        
    case EP_RADAR_ERR_DEVICE_NOT_SUPPORTED
        szErrorMessage = '[Endpoint] The Device is not supported by the driver.';
        
    case EP_RADAR_ERR_FEATURE_NOT_SUPPORTED
        szErrorMessage = '[Endpoint] The requested feature is not supported by the connected device.';
        
    case EP_RADAR_ERR_PRECHIRP_EXCEEDS_PADELAY
        szErrorMessage = '[Endpoint] The PA Delay is shorter than the pre chirp delay.';
        
    case EP_RADAR_ERR_REGISTER_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The register selected for override does not exist.';

    case EP_RADAR_ERR_UNSUPPORTED_FREQUENCY
        szErrorMessage = '[Endpoint] The selected reference clock frequency is not supported by the device.';

    case EP_RADAR_ERR_UNSUPPORTED_FIFO_POWER_MODE
        szErrorMessage = '[Endpoint] The specified FIFO power mode is not supported.';
 
    case EP_RADAR_ERR_UNSUPPORTED_PAD_DRIVER_MODE
        szErrorMessage = '[Endpoint] The specified pad driver mode is not supported.';
    case EP_RADAR_ERR_BANDGAP_DELAY_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified bandgap startup delay is out of range.';

    case EP_RADAR_ERR_MADC_DELAY_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified MADC startup delay is out of range.';

    case EP_RADAR_ERR_PLL_ENABLE_DELAY_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified PLL startup delay is out of range.';
 
    case EP_RADAR_ERR_PLL_DIVIDER_DELAY_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified PLL divider startup delay is out of range.';

    case EP_RADAR_ERR_DOUBLER_MODE_NOT_SUPPORTED
        szErrorMessage = '[Endpoint] The specified clock doubler mode is not supported.';

    case EP_RADAR_ERR_DC_IN_CORRECTION_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified input duty cycle correction is out of range.';

    case EP_RADAR_ERR_DC_OUT_CORRECTION_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] The specified output duty cycle correction is out of range.';

    case EP_RADAR_ERR_AAF_FREQ_NOT_SUPPORTED
        szErrorMessage = '[Endpoint] The specified anti alias filter frequency is not supported.';

    case EP_RADAR_ERR_TX_TOGGLE_MODE_NOT_SUPPORTED
        szErrorMessage = '[Endpoint] The specified TX toggle mode is not supported.';

    case EP_RADAR_ERR_TEST_CONFLICT_BB_TX
        szErrorMessage = '[Endpoint] Baseband test and TX toggle test cannot be used simultaniously.';

    case EP_RADAR_ERR_PARAMETER_OUT_OF_RANGE
        szErrorMessage = '[Endpoint] Parameter out of range.';

	case EP_RADAR_ERR_POWER_SENS_DELAY_OUT_OF_RANGE
	    szErrorMessage = '[Endpoint] The specified power sensing delay is out of range.';

    case EP_RADAR_ERR_NO_SWITCH_TIME_MADC_POWER
        szErrorMessage = '[Endpoint] The period between chirp start and power measurement is not long enough to allow switching the MADC input to power sensor channel.';

    case EP_RADAR_ERR_NO_SWITCH_TIME_MADC_RX
        szErrorMessage = '[Endpoint] The period between power measurement and chirp acquisition is not long enough to switch MADC input to the RX channel.';

    case EP_RADAR_ERR_NO_SWITCH_TIME_MADC_TEMP
        szErrorMessage = '[Endpoint] The period between end of chirp acquisition and temperature measurement is not long enough to switch MADC input to temperature sensor channel.';

    case EP_RADAR_ERR_NO_MEASURE_TIME_MADC_TEMP
        szErrorMessage = '[Endpoint] The chirp end delay is not long enough for temperature measurement.';

    case EP_RADAR_ERR_TEMP_SENSING_WITH_NO_RX
        szErrorMessage = '[Endpoint]  When temperature sensing is enabled, chirps with all RX channels disabled are not allowed.';

    otherwise
            if( uErrorCode == 0xFFFF)
			% the case seems not to catch the 0xFFFF value! (PROTOCOL_STATUS_DEVICE_BAD_COMMAND)
                szErrorMessage = '[Endpoint] The device received a message that is not supported.';
            end
        szErrorMessage = '[Endpoint] Unknown Error';
end

ME = MException('CommLib:Error', sprintf('%s (Error Code 0x%04X)', szErrorMessage, uErrorCode));
throw(ME);
