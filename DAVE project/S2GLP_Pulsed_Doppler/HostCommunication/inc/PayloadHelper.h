/**
 * \file PayloadHelper.h
 *
 * \brief This file defines an internal protocol API to be used by endpoint
 *        implementations.
 *
 * This file defines some light weight function that help to access data in
 * protocol payload. Different implementation allow to adapt to the
 * capabilities of the target machine. Special implementations can be chosen
 * by defining the according compile time switch. If none of the available
 * compile time switches is chosen, the default implementation will be used.
 *
 * The default implementation is safe on all target machines and can handle
 * both, big endian CPUs and little endian CPUs. It may be slower than other
 * special implementations.
 *
 * If the compile time switch COMLIB_ALLOW_PAYLOAD_CASTS is set, a special
 * implementation for little endian machines is used, that can access data
 * faster by casting data to the right data type. This implementation works
 * only for CPUs that have no alignment requirement for 16 bit and 32 bit
 * integers.
 */

/* ===========================================================================
** Copyright (C) 2016-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

#ifndef HOSTCOMMUNICATION_PAYLOAD_HELPER_H_
#define HOSTCOMMUNICATION_PAYLOAD_HELPER_H_

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */
#include <stdint.h>
#include <stdlib.h>


#if !defined(STATIC_ASSERT)
#define STATIC_ASSERT(test_for_true) _Static_assert((test_for_true), "(" #test_for_true ") failed")
#endif

/*
==============================================================================
   2. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
 */

/**
 * \internal
 * \brief The function extracts a signed 8 bit integer from a payload buffer.
 *
 * The function reads one byte from the given payload buffer at the requested
 * position and interprets it as a signed 8 bit integer value, which will be
 * returned.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline int8_t rd_payload_i8(const uint8_t* payload, size_t offset)
{
    return (int8_t)payload[offset];
}

/**
 * \internal
 * \brief The function extracts an unsigned 8 bit integer from a payload
 *        buffer.
 *
 * The function reads one byte from the given payload buffer at the requested
 * position and interprets it as an unsigned 8 bit integer value, which will
 * be returned.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline uint8_t rd_payload_u8(const uint8_t* payload, size_t offset)
{
    return payload[offset];
}

/**
 * \internal
 * \brief The function extracts a signed 16 bit integer from a payload buffer.
 *
 * The function reads two bytes from the given payload buffer starting at the
 * requested position and interprets them as a signed 16 bit integer value,
 * which will be returned. If needed the byte order is swapped to convert from
 * the payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline int16_t rd_payload_i16(const uint8_t* payload, size_t offset);

/**
 * \internal
 * \brief The function extracts an unsigned 16 bit integer from a payload
 *        buffer.
 *
 * The function reads two bytes from the given payload buffer starting at the
 * requested position and interprets them as an unsigned 16 bit integer value,
 * which will be returned. If needed the byte order is swapped to convert from
 * the payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline uint16_t rd_payload_u16(const uint8_t* payload,
		size_t offset);

/**
 * \internal
 * \brief The function extracts a signed 32 bit integer from a payload buffer.
 *
 * The function reads four bytes from the given payload buffer starting at the
 * requested position and interprets them as a signed 32 bit integer value,
 * which will be returned. If needed the byte order is swapped to convert from
 * the payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline int32_t rd_payload_i32(const uint8_t* payload, size_t offset);

/**
 * \internal
 * \brief The function extracts an unsigned 32 bit integer from a payload
 *        buffer.
 *
 * The function reads four bytes from the given payload buffer starting at the
 * requested position and interprets them as an unsigned 32 bit integer value,
 * which will be returned. If needed the byte order is swapped to convert from
 * the payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline uint32_t rd_payload_u32(const uint8_t* payload,
		size_t offset);

/**
 * \internal
 * \brief The function extracts a signed 64 bit integer from a payload buffer.
 *
 * The function reads four bytes from the given payload buffer starting at the
 * requested position and interprets them as a signed 64 bit integer value,
 * which will be returned. If needed the byte order is swapped to convert from
 * the payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline int64_t rd_payload_i64(const uint8_t* payload, size_t offset);

/**
 * \internal
 * \brief The function extracts an unsigned 64 bit integer from a payload
 *        buffer.
 *
 * The function reads four bytes from the given payload buffer starting at the
 * requested position and interprets them as an unsigned 64 bit integer value,
 * which will be returned. If needed the byte order is swapped to convert from
 * the payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline uint64_t rd_payload_u64(const uint8_t* payload,
		size_t offset);

/**
 * \internal
 * \brief The function writes a signed 8 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline size_t wr_payload_i8(uint8_t* payload, size_t offset,
                                 int8_t value)
{
    payload[offset] = (uint8_t)value;
    return sizeof(int8_t);
}

/**
 * \internal
 * \brief The function writes an unsigned 8 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline size_t wr_payload_u8(uint8_t* payload, size_t offset,
                                 uint8_t value)
{
    payload[offset] = value;
    return sizeof(uint8_t);
}

/**
 * \internal
 * \brief The function writes a signed 16 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position. If needed the byte order is swapped to convert from the
 * payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline size_t wr_payload_i16(uint8_t* payload, size_t offset,
                                  int16_t value);

/**
 * \internal
 * \brief The function writes an unsigned 16 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position. If needed the byte order is swapped to convert from the
 * payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline size_t wr_payload_u16(uint8_t* payload, size_t offset,
                                  uint16_t value);

/**
 * \internal
 * \brief The function writes a signed 32 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position. If needed the byte order is swapped to convert from the
 * payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline size_t wr_payload_i32(uint8_t* payload, size_t offset,
                                  int32_t value);

/**
 * \internal
 * \brief The function writes an unsigned 32 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position. If needed the byte order is swapped to convert from the
 * payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline size_t wr_payload_u32(uint8_t* payload, size_t offset,
                                  uint32_t value);

/**
 * \internal
 * \brief The function writes a signed 64 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position. If needed the byte order is swapped to convert from the
 * payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline size_t wr_payload_i64(uint8_t* payload, size_t offset,
                                  int64_t value);

/**
 * \internal
 * \brief The function writes an unsigned 64 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position. If needed the byte order is swapped to convert from the
 * payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline size_t wr_payload_u64(uint8_t* payload, size_t offset,
                                  uint64_t value);
/*
==============================================================================
   3. INLINE FUNCTIONS
==============================================================================
 */
#ifdef COMLIB_ALLOW_PAYLOAD_CASTS
/*
 * The following function implementations wan only be used on machines with
 * little endian CPUs, but are faster than the generic implementations.
 */

static inline int16_t rd_payload_i16(const uint8_t* payload, uint16_t offset)
{
    return *(int16_t*)(payload + offset);
}

static inline uint16_t rd_payload_u16(const uint8_t* payload, uint16_t offset)
{
    return *(uint16_t*)(payload + offset);
}

static inline int32_t rd_payload_i32(const uint8_t* payload, uint16_t offset)
{
    return *(int32_t*)(payload + offset);
}

static inline uint32_t rd_payload_u32(const uint8_t* payload, uint16_t offset)
{
    return *(uint32_t*)(payload + offset);
}

static inline int64_t rd_payload_i64(const uint8_t* payload, uint16_t offset)
{
    union
    {
        uint32_t u32[2];
        int64_t i64;
    } value;
    value.u32[0] = *(uint32_t*)(payload + offset);
    value.u32[1] = *(uint32_t*)(payload + offset + 4);

    return value.i64;
}

static inline uint64_t rd_payload_u64(const uint8_t* payload, uint16_t offset)
{
    union
    {
        uint32_t u32[2];
        uint64_t u64;
    } value;
    value.u32[0] = *(uint32_t*)(payload + offset);
    value.u32[1] = *(uint32_t*)(payload + offset + 4);

    return value.u64;
}

static inline void wr_payload_i16(uint8_t* payload, uint16_t offset,
                                  int16_t iValue)
{
    *(int16_t*)(payload + offset) = iValue;
}

static inline void wr_payload_u16(uint8_t* payload, uint16_t offset,
                                  uint16_t value)
{
    *(uint16_t*)(payload + offset) = value;
}

static inline void wr_payload_i32(uint8_t* payload, uint16_t offset,
                                  int32_t value)
{
    *(int32_t*)(payload + offset) = value;
}

static inline void wr_payload_u32(uint8_t* payload, uint16_t offset,
                                  uint32_t value)
{
    *(uint32_t*)(payload + offset) = value;
}

static inline void wr_payload_i64(uint8_t* payload, uint16_t offset,
                                  int64_t value)
{
    union
    {
        uint32_t u32[2];
        int64_t i64;
    } value_;
    value_.i64 = value;
    *(uint32_t*)(payload + offset)     = value_.u32[0];
    *(uint32_t*)(payload + offset + 4) = value_.u32[1];
}

static inline void wr_payload_u64(uint8_t* payload, uint16_t offset,
                                  uint64_t value)
{
    union
    {
        uint32_t u32[2];
        uint64_t u64;
    } value_;
    value_.u64 = value;
    *(uint32_t*)(payload + offset)     = value_.u32[0];
    *(uint32_t*)(payload + offset + 4) = value_.u32[1];
}

/*
==============================================================================
   4. INLINE FUNCTIONS
==============================================================================
 */
#else
/*
 * The following function implementations are safe on any platform, but slower
 * than the native little endian implementations.
 */

static inline int16_t rd_payload_i16(const uint8_t* payload, size_t offset)
{
    return (int16_t)((((uint16_t)payload[offset + 1]) <<  8) |
                      ((uint16_t)payload[offset    ]));
}

static inline uint16_t rd_payload_u16(const uint8_t* payload, size_t offset)
{
    return (((uint16_t)payload[offset + 1]) <<  8) |
            ((uint16_t)payload[offset    ]);
}

static inline int32_t rd_payload_i32(const uint8_t* payload, size_t offset)
{
    return (int32_t)((((uint32_t)payload[offset + 3]) << 24) |
                     (((uint32_t)payload[offset + 2]) << 16) |
                     (((uint32_t)payload[offset + 1]) <<  8) |
                      ((uint32_t)payload[offset    ]));
}

static inline uint32_t rd_payload_u32(const uint8_t* payload, size_t offset)
{
    return (((uint32_t)payload[offset + 3]) << 24) |
           (((uint32_t)payload[offset + 2]) << 16) |
           (((uint32_t)payload[offset + 1]) <<  8) |
            ((uint32_t)payload[offset    ]);
}

static inline int64_t rd_payload_i64(const uint8_t* payload, size_t offset)
{
    return (int64_t)((((uint64_t)payload[offset + 7]) << 56) |
                     (((uint64_t)payload[offset + 6]) << 48) |
                     (((uint64_t)payload[offset + 5]) << 40) |
                     (((uint64_t)payload[offset + 4]) << 32) |
                     (((uint64_t)payload[offset + 3]) << 24) |
                     (((uint64_t)payload[offset + 2]) << 16) |
                     (((uint64_t)payload[offset + 1]) <<  8) |
                      ((uint64_t)payload[offset    ]));
}

static inline uint64_t rd_payload_u64(const uint8_t* payload, size_t offset)
{
    return (((uint64_t)payload[offset + 7]) << 56) |
           (((uint64_t)payload[offset + 6]) << 48) |
           (((uint64_t)payload[offset + 5]) << 40) |
           (((uint64_t)payload[offset + 4]) << 32) |
           (((uint64_t)payload[offset + 3]) << 24) |
           (((uint64_t)payload[offset + 2]) << 16) |
           (((uint64_t)payload[offset + 1]) <<  8) |
            ((uint64_t)payload[offset    ]);
}


static inline float rd_payload_float(const uint8_t* payload, size_t offset)
{
	STATIC_ASSERT(sizeof(float) == sizeof(uint32_t));
		typedef union {
			uint32_t u32_value;
			float float_value;
		} temp_union;

	temp_union data;

	data.u32_value =
    		(((uint32_t)payload[offset + 3]) << 24) |
           (((uint32_t)payload[offset + 2]) << 16) |
           (((uint32_t)payload[offset + 1]) <<  8) |
            ((uint32_t)payload[offset    ]);

    return data.float_value;
}


static inline size_t wr_payload_i16(uint8_t* payload, size_t offset,
                                  int16_t value)
{
    payload[offset + 1] = (((uint16_t)value) >>  8) & 0xFF;
    payload[offset    ] =  ((uint16_t)value)        & 0xFF;
    return sizeof(int16_t);
}

static inline size_t wr_payload_u16(uint8_t* payload, size_t offset,
                                  uint16_t value)
{
    payload[offset + 1] = (value >>  8) & 0xFF;
    payload[offset    ] =  value        & 0xFF;
    return sizeof(uint16_t);

}

static inline size_t wr_payload_i32(uint8_t* payload, size_t offset,
                                  int32_t value)
{
    payload[offset + 3] = (((uint32_t)value) >> 24) & 0xFF;
    payload[offset + 2] = (((uint32_t)value) >> 16) & 0xFF;
    payload[offset + 1] = (((uint32_t)value) >>  8) & 0xFF;
    payload[offset    ] =  ((uint32_t)value)        & 0xFF;

    return sizeof(int32_t);
}

static inline size_t wr_payload_u32(uint8_t* payload, size_t offset,
                                  uint32_t value)
{
    payload[offset + 3] = (value >> 24) & 0xFF;
    payload[offset + 2] = (value >> 16) & 0xFF;
    payload[offset + 1] = (value >>  8) & 0xFF;
    payload[offset    ] =  value        & 0xFF;

    return sizeof(uint32_t);
}

static inline size_t wr_payload_i64(uint8_t* payload, size_t offset,
                                  int64_t value)
{
    payload[offset + 7] = (((uint64_t)value) >> 56) & 0xFF;
    payload[offset + 6] = (((uint64_t)value) >> 48) & 0xFF;
    payload[offset + 5] = (((uint64_t)value) >> 40) & 0xFF;
    payload[offset + 4] = (((uint64_t)value) >> 32) & 0xFF;
    payload[offset + 3] = (((uint64_t)value) >> 24) & 0xFF;
    payload[offset + 2] = (((uint64_t)value) >> 16) & 0xFF;
    payload[offset + 1] = (((uint64_t)value) >>  8) & 0xFF;
    payload[offset    ] =  ((uint64_t)value)        & 0xFF;

    return (uint16_t) sizeof(int64_t);
}

static inline size_t wr_payload_u64(uint8_t* payload, size_t offset,
                                  uint64_t value)
{
    payload[offset + 7] = (value >> 56) & 0xFF;
    payload[offset + 6] = (value >> 48) & 0xFF;
    payload[offset + 5] = (value >> 40) & 0xFF;
    payload[offset + 4] = (value >> 32) & 0xFF;
    payload[offset + 3] = (value >> 24) & 0xFF;
    payload[offset + 2] = (value >> 16) & 0xFF;
    payload[offset + 1] = (value >>  8) & 0xFF;
    payload[offset    ] =  value        & 0xFF;

    return sizeof(uint64_t);
}


size_t wr_payload_float(uint8_t* payload, size_t offset, float value);

size_t wr_payload_string(uint8_t *payload, size_t offset, char const *ptr);

size_t wr_payload_helper_string_list(uint8_t* payload, size_t offset, size_t elements,  char * const *pp_str);

size_t wr_payload_helper_float_list(uint8_t* payload, size_t offset, size_t elements,  float *p_value);

size_t wr_payload_helper_uint32_list(uint8_t* payload, size_t offset, size_t elements,  uint32_t *p_value);

size_t wr_payload_helper_uint16_list(uint8_t* payload, size_t offset, size_t elements,  uint16_t *p_value);

#endif

/* --- Close open blocks -------------------------------------------------- */

#endif /* HOSTCOMMUNICATION_PAYLOAD_HELPER_H_ */

/* --- End of File -------------------------------------------------------- */
