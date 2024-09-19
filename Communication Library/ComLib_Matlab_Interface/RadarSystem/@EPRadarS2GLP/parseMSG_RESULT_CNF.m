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

if (length(payload) > 1)
	offset = 1;
	% read string list
	result_data.result_cnt = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	result_data.frame_count = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	result_data.velocity_mps = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	result_data.level = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	result_data.is_target_departing = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	result_data.is_target_approaching = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	result_data.is_motion_detected = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	result_data.doppler_frequency_hz = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	
    obj.m_result_data = result_data;
	obj.m_result_pending = false;
else
    fprintf('[%s.%s] Error! Wrong payload size.\n',mfilename('Class'),mfilename);
end
