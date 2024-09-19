%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Copyright (C) 2014-2021 Infineon Technologies AG
% All rights reserved.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This document contains proprietary information of Infineon Technologies AG.
% Passing on and copying of this document, and communication of its contents
% is not permitted without Infineon's prior written authorisation.
%
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
% AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
% IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
% ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
% LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
% CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
% SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
% INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
% CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
% ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
% POSSIBILITY OF SUCH DAMAGE.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
classdef EPRadarS2GLP < handle

    %% methods
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Hidden) % constructor, general class
        function obj = EPRadarS2GLP(oRS, uEndpointID, uEndpointVer)
            obj.uEndpointID = uEndpointID;
            obj.uEndpointVer = uEndpointVer;
            obj.oRS = oRS;
        end

        function initValues(obj)
            obj.get_shield_info;
			obj.get_parameters_def;
			obj.get_parameters;
            obj.m_new_parameters = {};
        end

        parsePayload(obj, RX_content)
        doTransmission(obj, message, poll_response, wait)
    end

    methods (Hidden) % communication with endpoint
		param = get_parameters(obj)		
		parameters_def = get_parameters_def(obj)
		result = get_result_data(obj)
		shield_info = get_shield_info(obj)
		raw_data = get_raw_data(obj)
        frame_data = get_frame_data(obj)
        set_parameter(obj, new_parameter)
	end

    methods (Hidden) % parsing of incoming messages
		parseMSG_PARA_VALUES_CNF(obj, payload)    
		parseMSG_PARA_VALUES_DEF_CNF(obj, payload)
		parseMSG_RESULT_CNF(obj, payload)		 	
		parseMSG_GET_SHIELD_INFO_CNF(obj, payload)
		parseMSG_GET_RAW_DATA_CNF(obj, payload)   
    end

	%% properties
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Hidden, Constant) % endpoint tag
        szEPTag = 'TJPU'; % ep_radar_tjpu_definition    0x544A5055
        uMinVer = 1;      % min version of this endpoint on FW supported by this host code
        uMaxVer = 1;      % max version of this endpoint on FW supported by this host code
    end

    properties (Hidden) % general class
        uEndpointID;
        uEndpointVer;
        oRS;
    end

    properties (Hidden) % data properties
		m_read_parameters;
        m_new_parameters;
		m_parameters_def;
        m_shield_info;
		
		m_result_data;
		m_result_pending;
		
		m_raw_data;
		m_raw_data_pending;
        m_last_frame_time_sec;
    end
    
    properties (Dependent)
		parameters;
		parameters_def;
        shield_info;
		result_data;
		raw_data;
        frame_data;
	end

    properties (Access = private, Constant)	% message ids
        MSG_PARA_VALUES_REQ 	= uint8(hex2dec('30')); 
        MSG_PARA_VALUES_CNF     = uint8(hex2dec('31')); 
		MSG_PARA_VALUES_DEF_REQ = uint8(hex2dec('32')); 
		MSG_PARA_VALUES_DEF_CNF = uint8(hex2dec('33')); 
		MSG_PARA_SET_VALUES_REQ = uint8(hex2dec('34')); 
		MSG_RESULT_REQ		 	= uint8(hex2dec('35')); 
		MSG_RESULT_CNF		 	= uint8(hex2dec('36')); 
		
		MSG_GET_SHIELD_INFO_REQ = uint8(hex2dec('40')); 
		MSG_GET_SHIELD_INFO_CNF = uint8(hex2dec('41')); 
		
		MSG_GET_RAW_DATA_REQ    = uint8(hex2dec('46')); 
		MSG_GET_RAW_DATA_CNF    = uint8(hex2dec('47')); 
    end
    
    %% set/get methods
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods % set/get
        function val = get.parameters_def(obj)
            val = obj.m_parameters_def;
        end
        function val = get.parameters(obj)
            val = obj.m_new_parameters;
        end
        function val = get.shield_info(obj)
            val = obj.m_shield_info;
        end
        function val = get.result_data(obj)
            val = obj.m_result_data;
        end
        function val = get.raw_data(obj)
            val = obj.m_raw_data;
        end
        function val = get.frame_data(obj)
            val = obj.m_raw_data;
        end
        %%
        function set.parameters(obj, param_new)
		
           % check on additional fieldnames
           test = fieldnames(rmfield(param_new,intersect(fieldnames(obj.m_read_parameters),fieldnames(param_new))));
           if( ~isempty(test))
               disp([test]);
               error('invalid parameters names are added!');
           end
           
            if isfield(param_new, 'max_speed_mps')
                obj.m_new_parameters.max_speed_mps = param_new.max_speed_mps;
            end

            if isfield(param_new, 'min_speed_mps')
                obj.m_new_parameters.min_speed_mps = param_new.min_speed_mps;
            end

            if isfield(param_new, 'frame_time_sec')
                obj.m_new_parameters.frame_time_sec = param_new.frame_time_sec;
            end
            if isfield(param_new, 'number_of_samples')
                obj.m_new_parameters.number_of_samples = param_new.number_of_samples;
            end
            if isfield(param_new, 'sampling_freq_hz')
                obj.m_new_parameters.sampling_freq_hz = param_new.sampling_freq_hz;
            end

            if isfield(param_new, 'doppler_sensitivity')
                obj.m_new_parameters.doppler_sensitivity = param_new.doppler_sensitivity;
            end

            if isfield(param_new, 'motion_sensitivity')
               obj.m_new_parameters.motion_sensitivity = param_new.motion_sensitivity;
            end

            if isfield(param_new, 'baseband_gain_stage')
                obj.m_new_parameters.baseband_gain_stage = param_new.baseband_gain_stage;
            end
            if isfield(param_new, 'continuous_mode')
              obj.m_new_parameters.continuous_mode = param_new.continuous_mode;
            end

            if isfield(param_new, 'number_of_skip_samples')
                obj.m_new_parameters.number_of_skip_samples = param_new.number_of_skip_samples;
            end
			
			if isfield(param_new, 'pulse_width_usec')
                obj.m_new_parameters.pulse_width_usec = param_new.pulse_width_usec;
            end
         

        end
		
        %%
        function [status, message] = check_parameter_limits(obj, param_new )
            
            status = 0;
            message = '';

           
            if isfield(param_new, 'max_speed_mps')
                if(round(param_new.max_speed_mps,3) < obj.m_parameters_def.max_speed_mps_lower_boundary)
                    message = [message sprintf('max_speed_mps needs to be larger then lower boundary of %f\n',obj.m_parameters_def.max_speed_mps_lower_boundary)];
                    status = status + 1;
                end
                if(round(param_new.max_speed_mps,3)> obj.m_parameters_def.max_speed_mps_upper_boundary)
                    message = [message sprintf('max_speed_mps exceeds lower limit of %f\n',obj.m_parameters_def.max_speed_mps_upper_boundary)] ;
                    status = status +1;
                end     
            end

            if isfield(param_new, 'min_speed_mps')
                if(round(param_new.min_speed_mps,3) < obj.m_parameters_def.min_speed_mps_lower_boundary)
                    message = [message sprintf('min_speed_mps exceeds upper limit of %f\n',obj.m_parameters_def.min_speed_mps_lower_boundary)] ;
                    status = status +1;
                end
                if(round(param_new.min_speed_mps,4) > obj.m_parameters_def.min_speed_mps_upper_boundary)
                    message = [message sprintf('min_speed_mps exceeds lower limit of %f\n',obj.m_parameters_def.min_speed_mps_upper_boundary)] ;
                    status = status +1;
                end    
            end

            if isfield(param_new, 'frame_time_sec')
                if(param_new.frame_time_sec < obj.m_parameters_def.frame_time_sec_lower_boundary)
                    message = [message sprintf('frame_time_sec exceeds lower limit of %f\n',obj.m_parameters_def.frame_time_sec_lower_boundary)] ;
                    status = status +1;
                end
                 if(param_new.frame_time_sec > obj.m_parameters_def.frame_time_sec_upper_boundary)
                     message = [message sprintf('frame_time_sec exceeds upper limit of %f\n',obj.m_parameters_def.frame_time_sec_upper_boundary)] ;
                     status = status +1;
                 end    
              %  obj.m_last_frame_time_sec = param_new.frame_time_sec;
            end
            
            if isfield(param_new, 'number_of_samples')
                if( ~ismember(param_new.number_of_samples, obj.m_parameters_def.sample_list))
                    message = [message sprintf('number_of_samples can only be a value out of: [') sprintf('%d ', obj.m_parameters_def.sample_list) sprintf(']\n')];
                    status = status +1;
                end
            end
            if isfield(param_new, 'sampling_freq_hz')
                if(param_new.sampling_freq_hz < obj.m_parameters_def.sampling_freq_hz_lower_boundary)
                    message = [message sprintf('sampling_freq_hz exceeds lower limit of %f\n',obj.m_parameters_def.sampling_freq_hz_lower_boundary)] ;
                    status = status +1;
                end
                if(param_new.sampling_freq_hz > obj.m_parameters_def.sampling_freq_hz_upper_boundary)
                    message = [message sprintf('sampling_freq_hz exceeds upper limit of %f\n',obj.m_parameters_def.sampling_freq_hz_upper_boundary)] ;
                    status = status +1;
                end
            end

            if isfield(param_new, 'doppler_sensitivity')
            	if(param_new.doppler_sensitivity < obj.m_parameters_def.doppler_sensitivity_lower_boundary)
                    message = [message sprintf('doppler_sensitivity exceeds lower limit of %f\n',obj.m_parameters_def.doppler_sensitivity_lower_boundary)] ;
                    status = status +1;
                end
                  if(param_new.doppler_sensitivity > obj.m_parameters_def.doppler_sensitivity_upper_boundary)
                    message = [message sprintf('doppler_sensitivity exceeds upper limit of %f\n',obj.m_parameters_def.doppler_sensitivity_upper_boundary)] ;
                    status = status +1;
                end                  
            end

            if isfield(param_new, 'motion_sensitivity')
                if(param_new.motion_sensitivity < obj.m_parameters_def.motion_sensitivity_lower_boundary)
                    message = [message sprintf('motion_sensitivity exceeds lower limit of %f\n',obj.m_parameters_def.motion_sensitivity_lower_boundary)] ;
                    status = status +1;
                end
                if(param_new.motion_sensitivity > obj.m_parameters_def.motion_sensitivity_upper_boundary)
                    message = [message sprintf('motion_sensitivity exceeds lower limit of %f\n',obj.m_parameters_def.motion_sensitivity_upper_boundary) ];
                    status = status +1;
                end  
            end
            if isfield(param_new, 'number_of_skip_samples')
                if(param_new.number_of_skip_samples < obj.m_parameters_def.number_of_skip_samples_lower_boundary)
                    message = [message sprintf('number_of_skip_samples exceeds lower limit of %d\n',obj.m_parameters_def.number_of_skip_samples_lower_boundary)] ;
                    status = status +1;
                end
                if(param_new.number_of_skip_samples > obj.m_parameters_def.number_of_skip_samples_upper_boundary)
                    message = [message sprintf('number_of_skip_samples exceeds upper limit of %d\n',obj.m_parameters_def.number_of_skip_samples_upper_boundary)] ;
                    status = status +1;
                end
				if( param_new.number_of_skip_samples ~= ceil( param_new.number_of_skip_samples ) | ...
                        param_new.number_of_skip_samples ~= floor( param_new.number_of_skip_samples ))
                    message = [message sprintf('number_of_skip_samples must be integer value\n')] ;
                    status = status +1;                    
                end  
            end
            if isfield(param_new, 'pulse_width_usec')
                if(param_new.pulse_width_usec < obj.m_parameters_def.pulse_width_usec_lower_boundary)
                    message = [message sprintf('pulse_width_usec exceeds lower limit of %d\n',obj.m_parameters_def.pulse_width_usec_lower_boundary)] ;
                    status = status +1;
                end
                if(param_new.pulse_width_usec > obj.m_parameters_def.pulse_width_usec_upper_boundary)
                    message = [message sprintf('pulse_width_usec exceeds upper limit of %d\n',obj.m_parameters_def.pulse_width_usec_upper_boundary)] ;
                    status = status +1;
                end  
				if( param_new.pulse_width_usec ~= ceil( param_new.pulse_width_usec ) | ...
	                        param_new.pulse_width_usec ~= floor( param_new.pulse_width_usec ))
	                    message = [message sprintf('pulse_width_usec must be integer value\n')] ;
	                    status = status +1;                    
	            end  

            end

        end
    end
    
end
