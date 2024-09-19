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
function parseMSG_RESULT_CNF(obj, payload)

payload = payload(2:end); % legacy from one-based indexing (Matlab), also all -1 in protocol_read_payload...(... -1)
uHeaderLength = 17; 
if (length(payload) > uHeaderLength)
	offset = 1;
    % read frame info values
    frame_info.frame_number = protocol_read_payload_uint32(payload, 1-1);
    frame_info.num_chirps = protocol_read_payload_uint32(payload, 5-1);
    frame_info.num_rx_antennas = protocol_read_payload_uint8 (payload, 9-1);
    frame_info.num_samples_per_chirp = protocol_read_payload_uint32(payload, 10-1);
    frame_info.rx_mask = protocol_read_payload_uint8 (payload, 14-1);
    switch protocol_read_payload_uint8 (payload, 15-1); % 0 real, 1: buffers, 2: interleaved complex
        case 0 % EP_RADAR_BASE_RX_DATA_REAL The frame data contains only I or Q signal.
            frame_info.data_format = 'Real Data';
        case 1 % EP_RADAR_BASE_RX_DATA_COMPLEX The frame data contains I and Q signals in separate data blocks.
            frame_info.data_format = 'Complex Data';
        case 2 % EP_RADAR_BASE_RX_DATA_COMPLEX_INTERLEAVED The frame data contains I and Q signals in one interleaved data block.
            frame_info.data_format = 'Complex Interleaved Data';
    end
    frame_info.adc_resolution = protocol_read_payload_uint8 (payload, 16-1);
    frame_info.interleaved_rx = protocol_read_payload_uint8 (payload, 17-1); % if non-zero, data from RX antennas is interleaved
    
    % calculate expected message size
    total_samples = frame_info.num_chirps * frame_info.num_samples_per_chirp * uint32(frame_info.num_rx_antennas);
    if ~strcmp(frame_info.data_format, 'Real Data') % frame_info.data_format ~= EP_RADAR_BASE_RX_DATA_REAL;
        total_samples = total_samples * 2; % i.e. complex data (or buffers)
    end
    
    expected_message_size = uHeaderLength + bitshift(total_samples * cast(frame_info.adc_resolution,'uint32'), -3)...
        + cast(bitand(total_samples * cast(frame_info.adc_resolution,'uint32'), 7)>0 ,'uint32');
    
    % get sample data
    if (length(payload) == expected_message_size)
        payload = payload(uHeaderLength+1:end);
        
        % unpack 2 samples from 3 bytes
        raw_sample_data(1:2:total_samples) = cast(payload(1:3:end), 'double') + cast(bitand(payload(2:3:end), 15, 'uint8'), 'double') * 256;
        raw_sample_data(2:2:total_samples) = cast(bitand(payload(2:3:end), 240, 'uint8'), 'double') / 16 + cast(payload(3:3:end), 'double') * 16;
        
        % deinterleave data if necessary
        sample_data = zeros(1, total_samples);
        if(frame_info.interleaved_rx)
            uStride = uint32(frame_info.num_rx_antennas);
            for uChirp = 0:uint32(frame_info.num_chirps - 1) % for each chirp
                for uRX = 0:uint32(frame_info.num_rx_antennas - 1); % for each active antenna
                    for ind = 0:uint32(frame_info.num_samples_per_chirp - 1) % for each sample
                        sample_data(uChirp * frame_info.num_samples_per_chirp * uint32(frame_info.num_rx_antennas) + uRX * frame_info.num_samples_per_chirp + ind +1) = ...
                            raw_sample_data(uChirp * uint32(frame_info.num_rx_antennas) * frame_info.num_samples_per_chirp + uRX + ind * uStride + 1);
                    end
                end
            end
        else % non interleaved data
            sample_data = raw_sample_data;
        end
        
        % convert received data to float (real and imaginary part are interleaved -> convert to complex)
        if strcmp(frame_info.data_format, 'Real Data') % frame_info.data_format == EP_RADAR_BASE_RX_DATA_REAL;
            sample_data = reshape(sample_data, frame_info.num_samples_per_chirp, frame_info.num_rx_antennas, frame_info.num_chirps);
        else
            sample_data = reshape(sample_data, 2*frame_info.num_samples_per_chirp, frame_info.num_rx_antennas, frame_info.num_chirps);
            
            % split array and build complex values
            if strcmp(frame_info.data_format, 'Complex Data')
                sample_data = sample_data(1:frame_info.num_samples_per_chirp,:,:) + 1i * sample_data((frame_info.num_samples_per_chirp+1):end,:,:);
            else
                sample_data = sample_data(1:2:end,:,:) + 1i * sample_data(2:2:end,:,:);
            end
        end
    end
    
    % scale values
    sample_bit_mask = bitshift(1, frame_info.adc_resolution, 'uint32')-1;
    norm_factor = 1 / sample_bit_mask;
    frame_info.sample_data = sample_data .* norm_factor;
	
    obj.m_raw_data = frame_info;
	obj.m_raw_data_pending = false;
else
    fprintf('[%s.%s] Error! Wrong payload size.\n',mfilename('Class'),mfilename);
end
