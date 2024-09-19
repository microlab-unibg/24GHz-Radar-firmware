/**
  @file acq_buffer.c

*/

/* ===========================================================================
** Copyright (C) 2018-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

#include "radar_api.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
 */

/*
==============================================================================
   4. DATA
==============================================================================
 */

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

/*
==============================================================================
   6. EXPORTED FUNCTIONS
==============================================================================
 */

uint8_t *get_buffer_address_by_chirp(acq_buf_obj *p_obj, uint32_t channel_index, uint32_t chirpCnt)
{
  size_t offset = 0;
  
  if (p_obj->p_acq_buf == NULL)
    return NULL;
  
  // since the chirps are interleaved, we calculate the buffer base address for a new chirp and the related channel here:
  offset = (p_obj->internals.chirp_inc * chirpCnt) + (p_obj->internals.channel_inc * channel_index);
  
  /*TODO: Throw some error, if channel_index is larger then the internal number of channels! */
  while (channel_index >= p_obj->params.num_of_rx_values);

  return &p_obj->p_acq_buf[offset];
}

//============================================================================

acq_buf_obj create_acq_data_buffer_obj(uint8_t *p_allocated_buffer, size_t allocated_buffer_size,
                                       uint16_t max_num_of_chirps_per_frame, uint16_t max_num_of_samples_per_chirp,
                                       uint16_t max_num_of_adc_channels, uint16_t adc_resolution,
                                       uint32_t num_sample_skip_count)
{
  acq_buf_obj obj;
  size_t required_size = 0;
  size_t value_size = ((adc_resolution - 1) / 8 ) + 1;
  
  obj.status = allocation_ok;
  obj.access_lock = 0;
  obj.acq_timestime = 0;
  obj.internals.is_dynamic_allocated = 0;
  obj.internals.skip_count = num_sample_skip_count;
  
  obj.params.num_of_chirps_per_frame = max_num_of_chirps_per_frame;
  obj.params.num_of_samples_per_chirp = max_num_of_samples_per_chirp;
  obj.params.num_of_rx_values = max_num_of_adc_channels;
  obj.params.size_of_value_in_bytes = value_size;
  
  obj.data_type = data_type_unknown;

#if 0
  // data_type currently not used!
  // derive format type from parameters:
  if ((value_size == 2) && (max_num_of_I_values == max_num_of_Q_values) && max_num_of_I_values == 2)
  {
    obj.data_type = uint16_2x_IandQ;
  }
  else if ((value_size == 2) && (max_num_of_I_values == max_num_of_Q_values) && max_num_of_I_values == 1)
  {
    obj.data_type = uint16_1x_IandQ;
  }
#endif
  
  // calculate the size of allocation buffer
  // consider that an internal acquisition buffer needs to be uint32_t bit aligned on ARM!
  required_size = obj.params.num_of_chirps_per_frame * obj.params.num_of_samples_per_chirp * obj.params.num_of_rx_values * obj.params.size_of_value_in_bytes;
  
  obj.p_acq_buf = p_allocated_buffer;
  obj.internals.size_of_acq_buffer = allocated_buffer_size;
  obj.used_size_of_acq_buffer = required_size;
  
  if (max_num_of_adc_channels > MAX_SUPPORTED_RX_SIGNALS_INT)
  {
    obj.status = num_of_RX_not_supported;
    obj.p_acq_buf = NULL;
    obj.internals.size_of_acq_buffer = 0;
    obj.used_size_of_acq_buffer = 0;
    return obj;
  }
  
  if (p_allocated_buffer != NULL && allocated_buffer_size == 0)
  {
    obj.status = allocation_size_is_zero;
    obj.p_acq_buf = NULL;
    obj.internals.size_of_acq_buffer = 0;
    obj.used_size_of_acq_buffer = 0;
    return obj;
  }
  
  if (((obj.p_acq_buf != NULL) && (obj.internals.size_of_acq_buffer != 0)) && obj.internals.size_of_acq_buffer < required_size)
  {
    obj.status = allocation_buffer_too_small;
    obj.p_acq_buf = NULL;
    obj.internals.size_of_acq_buffer = 0;
    obj.used_size_of_acq_buffer = 0;
    return obj;
  }
  
#if defined(USE_MALLOC_FOR_BUFFER) && (USE_MALLOC_FOR_BUFFER)
  if (obj.p_acq_buf == NULL) {
    // The buffer was not pre-allocated
    obj.internals.size_of_acq_buffer = required_size;
    obj.p_acq_buf = (uint8_t*) malloc(obj.internals.size_of_acq_buffer);
    if (obj.p_acq_buf == NULL) 
    {
      obj.status = allocation_fails;
      obj.internals.size_of_acq_buffer = 0;
      return obj;
    }
    else 
    {
      // allocated by this module?
      obj.internals.is_dynamic_allocated = 1;
    }
  }
#endif
  
  // calculated internal buffer offsets
  obj.internals.channel_inc = obj.params.num_of_samples_per_chirp * obj.params.size_of_value_in_bytes;
  obj.internals.chirp_inc   = obj.internals.channel_inc * obj.params.num_of_rx_values;
  
  if (obj.p_acq_buf != NULL)
  {
    memset(obj.p_acq_buf, 0, sizeof(obj.used_size_of_acq_buffer));
  }
  
  return obj;
}

//============================================================================

void delete_acq_buffer_obj(acq_buf_obj *p_obj)
{
  p_obj->status = allocation_is_freed;
  p_obj->used_size_of_acq_buffer = 0;

#if defined(USE_MALLOC_FOR_BUFFER) && (USE_MALLOC_FOR_BUFFER)

  if (p_obj->internals.is_dynamic_allocated && p_obj->p_acq_buf)
  {
    free(p_obj->p_acq_buf);
    p_obj->internals.is_dynamic_allocated = 0;
  }
#endif
}

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
