
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "PayloadHelper.h"

#if !defined(STATIC_ASSERT)
#define STATIC_ASSERT(test_for_true) _Static_assert((test_for_true), "(" #test_for_true ") failed")
#endif


size_t wr_payload_float(uint8_t* payload, size_t offset, float value)
{

	STATIC_ASSERT(sizeof(float) == sizeof(uint32_t));
	typedef union {
		uint32_t u32_value;
		float float_value;
	} temp_union;

	temp_union data;
	data.float_value = value;

	memcpy(&payload[offset], &data.u32_value, sizeof(uint32_t));
    return sizeof(uint32_t);
}


size_t wr_payload_string(uint8_t *payload, size_t offset, char const *ptr)
{
	size_t increment = 0;
	if (ptr)
	{
		uint8_t len = strlen(ptr);
		increment += wr_payload_u8(payload, offset + increment, len);  // attach the str len
		for(int k = 0; k < len; k++)
		{
			increment += wr_payload_u8(payload, offset + increment, ptr[k]);
		}
	}
	else
	{
		increment += wr_payload_u8(payload, offset + increment, 0);  // attach the str len 0, to indicate empty string!!
	}
	return increment;
}


size_t wr_payload_helper_string_list(uint8_t* payload, size_t offset, size_t elements,  char * const *pp_str)
{
	size_t increment = 0;
	increment += wr_payload_u8(payload, offset, elements);
	for(int i = 0; i < elements; i++)
	{
		const char *ptr = pp_str[i];
		increment += wr_payload_string(payload, offset + increment, ptr);
	}
	return increment;
}

size_t wr_payload_helper_float_list(uint8_t* payload, size_t offset, size_t elements,  float *p_value)
{
	size_t increment = 0;
	increment += wr_payload_u8(payload, offset, elements);
	for(int i = 0; i < elements; i++)
	{
		float value = p_value[i];
		increment += wr_payload_float(payload, offset +increment, value);
	}
	return increment;
}

size_t wr_payload_helper_uint32_list(uint8_t* payload, size_t offset, size_t elements,  uint32_t *p_value)
{
	size_t increment = 0;
	increment += wr_payload_u8(payload, offset, elements);
	for(int i = 0; i < elements; i++)
	{
		uint32_t value = p_value[i];
		increment += wr_payload_u32(payload, offset +increment, value);
	}
	return increment;
}

size_t wr_payload_helper_uint16_list(uint8_t* payload, size_t offset, size_t elements,  uint16_t *p_value)
{
	size_t increment = 0;
	increment += wr_payload_u8(payload, offset, elements);
	for(int i = 0; i < elements; i++)
	{
		uint16_t value = p_value[i];
		increment += wr_payload_u16(payload, offset +increment, value);
	}
	return increment;
}
