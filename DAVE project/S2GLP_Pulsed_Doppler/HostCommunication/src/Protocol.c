/**
 * @file: Protocol.c
 *
 * @brief: This module implements the host communication protocol.
 *
 * For more information about the host communication protocol see
 * \ref Protocol.h.
 *
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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

#include "Protocol.h"
#include "StreamInterface.h"
#include "PayloadHelper.h"

 /********** TIMING DEBUG - Protocol End *************/
#ifdef TIMING_DEBUG
	#include "DIGITAL_IO/digital_io.h"
#endif

#include <stdlib.h> // due to NULL!

#include <stdio.h>

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */

/**
 * \defgroup InternalConstants
 *
 * @brief: Constants used to define the message format.
 *
 * The messages exchanged between host and device contain some of those
 * constants to define the limits of a message in the byte stream.
 *
 * @{
 */
#define CNST_STARTBYTE_DATA           0x5A    /**< A payload message begins
                                                   with this code byte. */
#define CNST_STARTBYTE_STATUS         0x5B    /**< A status message begins
                                                   with this code byte. */
#define CNST_END_OF_PAYLOAD           0xE0DB  /**< Every payload message ends
                                                   with this 16 bit value
                                                   (transmitted with low byte
                                                   first!). */

#define CNST_MSG_QUERY_ENDPOINT_INFO  0x00    /**< This command code sent to
                                                   endpoint 0 tells the device
                                                   to send information about
                                                   the protocol endpoints. */
#define CNST_MSG_ENDPOINT_INFO        0x00    /**< A message containing
                                                   endpoint information sent
                                                   from endpoint 0 starts with
                                                   this message type code. */
#define CNST_MSG_QUERY_FW_INFO        0x01    /**< This command code sent to
                                                   endpoint 0 tells the device
                                                   to send information about
                                                   its firmware. */
#define CNST_MSG_FW_INFO              0x01    /**< A message containing
                                                   firmware information sent
                                                   from endpoint 0 starts with
                                                   this message type code. */
#define CNST_MSG_FIRMWARE_RESET       0x02    /**< This command code sent to
                                                   endpoint 0 tells the device
                                                   to do a firmware reset. */
/** @} */

/**
 * \defgroup StatusCodes
 *
 * @brief: General protocol status codes.
 *
 * Whenever a message with improper format is received one of the following
 * status code will be returned. The status codes will always be sent from
 * endpoint 0.
 *
 * @{
 */
#define PROTOCOL_ERROR_OK                0x0000  /**< No error occurred. */
#define PROTOCOL_ERROR_TIMEOUT           0x0001  /**< The message seemed to
                                                      end, but more data was
                                                      expected. */
#define PROTOCOL_ERROR_BAD_MESSAGE_START 0x0002  /**< The start byte of the
                                                      message was incorrect.
                                                      */
#define PROTOCOL_ERROR_BAD_ENDPOINT_ID   0x0003  /**< An endpoint with the
                                                      requested ID is not
                                                      present. */
#define PROTOCOL_ERROR_NO_PAYLOAD        0x0005  /**< A message with no
                                                      payload is not
                                                      supported. */
#define PROTOCOL_ERROR_OUT_OF_MEMORY     0x0006  /**< There is not enough
                                                      memory to store the
                                                      payload. */
#define PROTOCOL_ERROR_BAD_PAYLOAD_END   0x0007  /**< The message did not end
                                                      with the expected
                                                      sequence. */
#define PROTOCOL_ERROR_BAD_COMMAND       0xFFFF  /**< The command sent to
                                                      endpoint 0 is not
                                                      understood. */

#ifndef PROTOCOL_MAX_PAYLOAD_SIZE
	#define PROTOCOL_MAX_PAYLOAD_SIZE    (512U)    /**< The maximum buffer size
													  for the received message. */
#endif
/** @} */

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
 */

/**
 * @brief: This enumeration lists receive state of the protocol.
 *
 * The protocol uses a variable as the state of a finite state machine which
 * controls what to do with received data.
 */
typedef enum
{
    PROTOCOL_STATE_IDLE,               /**< No reception is going on. The
                                            protocol is waiting for a message
                                            header. */
    PROTOCOL_STATE_RECEIVING_HEADER,   /**< The begin of a message header was
                                            received but the header has not
                                            been received completely. */
    PROTOCOL_STATE_RECEIVING_PAYLOAD,  /**< A message header has been received
                                            completely. Now the reception of
                                            the message payload is ongoing. */
    PROTOCOL_STATE_RECEIVING_TAIL,     /**< The message payload has been
                                            received completely. Now the
                                            reception of the message tail is
                                            ongoing. */
    PROTOCOL_STATE_CONFUSED            /**< An error occurred during message
                                            reception. The incoming data can't
                                            be parsed properly any more. All
                                            following data is ignored until a
                                            timeout occurs to sync again. */
} Protocol_State_t;

/**
 * @brief: This structure contains all state information of the communication
 *        protocol.
 *
 * This structure contains the current receive state and some pointer to store
 * received data temporarily. It also contains a pointer to the list of
 * endpoints and the information about timeout function and period.
 *
 * Theoretically multiple instances of the protocol could coexist, each of
 * them having an instance of this team. Currently only a single instance is
 * supported as the only instance of this type is \ref instance.
 */
typedef struct
{
    Endpoint_t*      endpoints;          /**< The list of endpoints. */
    uint8_t          num_endpoints;      /**< The number of endpoints. */
    Protocol_State_t state;              /**< The current receive state. */
    uint8_t*         receive_pointer;    /**< A pointer to the memory
                                              location, where the next
                                              received bytes are stored. */
    uint16_t         num_bytes_awaited;  /**< The number of bytes to be
                                              received before the current
                                              section of message data is
                                              completed. */
    uint32_t         time_of_last_byte;  /**< The system time when the last
                                              bytes were received. This is
                                              used to detect a timeout. */
    uint8_t          message_frame[6];   /**< A buffer to store the received
                                              message header and tail. */
    uint8_t*         payload;            /**< A pointer to the payload. (Data
                                              buffer is allocated during
                                              reception) */
    uint32_t         (*get_time)(void);  /**< A pointer to the function that
                                              returns the current system time.
                                              */
    uint32_t         timeout_interval;   /**< The timeout period with respect
                                              to the unit of the return value
                                              of the get_time function. */
    void       (*do_system_reset)(void); /**< A pointer to a function that
                                              does a system reset. */
	size_t			max_payload;		 /**< holds the maximum payload buffer size, after allocation. */
} Instance_t;

/*
==============================================================================
   4. DATA
==============================================================================
 */

/**
 * @brief: The state of the protocol is stored here.
 *
 * See \ref Instance_t for more information.
 */
static Instance_t instance;

/**
 * @brief: This array contains the firmware version.
 *
 * This array must be defined somewhere in the firmware project. The array
 * contains the following numbers:
 * - firmware_version[0] - The major firmware version.
 * - firmware_version[1] - The minor firmware version.
 * - firmware_version[2] - The firmware version revision.
 */
extern Firmware_Information_t firmware_information;
/**
 * @brief: Static buffer used to store the received messages,
 *        in case of the payload size is less than PROTOCOL_MAX_PAYLOAD_SIZE.
 */
static uint8_t data_payload[PROTOCOL_MAX_PAYLOAD_SIZE] = { 0 };

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

/**
 * @brief: This function sends a status code to the host.
 *
 * Whenever the protocol receives a message from the host, it returns a status
 * code when processing of that message is done. This function builds the
 * according status message and sends it.
 *
 * \param[in] endpoint     The number of the endpoint that returned the status
 *                         code.
 * \param[in] status_code  The 16 bit error code to be sent.
 */
static void send_status_message(uint8_t endpoint, uint16_t status_code);

/**
 * @brief: This function resets the receive state of the protocol.
 *
 * This function is called after a message has been received or if the
 * protocols recovers from a receive error. The function resets the receive
 * state and prepares the protocol to receive another message from the host.
 */
static void reset_state(void);

/**
 * @brief: This function retrieves data from the host.
 *
 * This functions is repeatedly called during the function protocol_run. It
 * reads data bytes from the host that have been received through the
 * communication interface and stores them according to the current receive
 * state.
 *
 * The function also performs a timeout check when a valid get_time function
 * is available.
 */
static void receive_data(void);

/**
 * @brief: This function handles payload message that are sent to the protocol
 *        itself.
 *
 * When the protocol receives a payload message from the host that is
 * addressed to endpoint 0 that message is forwarded to this function, where
 * it is parsed.
 *
 * The only valid message that is known to the protocol is the message to
 * query endpoint information. This function checks for that message and calls
 * \ref send_endpoint_info on success.
 *
 * \param[in] message_data  A pointer to the buffer containing the payload.
 * \param[in] num_bytes     The number of bytes in the payload buffer.
 *
 * \return The function returns a status code indicating if the payload could
 *         be handled successfully.
 */
static uint16_t handle_message(uint8_t *message_data, uint16_t num_bytes);

/**
 * @brief: This function sends endpoint information to the host.
 *
 * This function is called by \ref handle_message if a message was received to
 * query information about the endpoints of the firmware. Type and version
 * information of all types are packed into a message, which is sent to the
 * host.
 */
static void send_endpoint_info(void);

/**
 * @brief: This function sends information about the firmware to the host.
 *
 * This function is called by \ref handle_message if a message was received to
 * query the firmware version. The firmware information is expected in
 * an external structure. See \ref Firmware_Information_t for more
 * information.
 * \code
 * Firmware_Information_t firmware_information;
 * \endcode
 */
static void send_firmware_info(void);

/*
==============================================================================
   6. EXPORTED FUNCTIONS
==============================================================================
 */

void protocol_init(Endpoint_t* endpoints, uint8_t num_endpoints,
                   uint32_t (*get_time)(void), uint32_t timeout_interval,
                   void (*do_system_reset)(void))
{
    /* initialize the internal state structure */
    instance.get_time = get_time;
    instance.timeout_interval = timeout_interval;
    instance.do_system_reset = do_system_reset;

    instance.endpoints = endpoints;
    instance.num_endpoints = num_endpoints;

    instance.state = PROTOCOL_STATE_IDLE;
    instance.receive_pointer = NULL;
    instance.num_bytes_awaited = 0;
    instance.time_of_last_byte = (get_time != NULL) ? instance.get_time() : 0;
    instance.payload = data_payload;
	instance.max_payload = PROTOCOL_MAX_PAYLOAD_SIZE;

    /* initialize state */
    reset_state();

    /* initialize communication interface */
    com_init();
}

//============================================================================

void protocol_run(void)
{
	if (instance.state != PROTOCOL_STATE_CONFUSED)
    {
        /* if the protocol is in a defined state, read the next bytes from the
         * input stream
         */
        receive_data();

        /*
         * now process data until the end of a message has been reached
         * (note the missing brakes, the fall through is intended to proceed
         * with the message, so don't wonder)
         */
        switch (instance.state)
        {
        case PROTOCOL_STATE_IDLE:
            /* if at least one byte has been received, the state is not idle
             *  any more
             */
            if (instance.num_bytes_awaited < 4)
                instance.state = PROTOCOL_STATE_RECEIVING_HEADER;

            /* no break, fall through and continue with
             * PROTOCOL_STATE_RECEIVING_HEADER
             */

        case PROTOCOL_STATE_RECEIVING_HEADER:
            if (instance.num_bytes_awaited == 0)
            {
                uint8_t  sync_byte;
                uint8_t  endpoint;
                uint16_t payload_size;

                /* when the header has been received completely, check the
                 * header information
                 */
                sync_byte    = rd_payload_u8 (instance.message_frame, 0);
                endpoint     = rd_payload_u8 (instance.message_frame, 1);
                payload_size = rd_payload_u16(instance.message_frame, 2);

                /* check sync bye, message must be a payload message */
                if (sync_byte != CNST_STARTBYTE_DATA)
                {
                    instance.state = PROTOCOL_STATE_CONFUSED;
                    send_status_message(0, PROTOCOL_ERROR_BAD_MESSAGE_START);
                    break;
                }

                /* check message header for endpoint number */
                if (endpoint <= instance.num_endpoints)
                {
                    /* check payload size (A message with no payload makes no
                     * sense)
                     */
                    if (payload_size == 0)
                    {
                        instance.state = PROTOCOL_STATE_CONFUSED;
                        send_status_message(0, PROTOCOL_ERROR_NO_PAYLOAD);
                        break;
                    }

                    /* prepare for receiving payload */
                    if (payload_size > instance.max_payload)
                    {
                      instance.state = PROTOCOL_STATE_CONFUSED;
                      send_status_message(0, PROTOCOL_ERROR_OUT_OF_MEMORY);
                      break;
                    }

                    instance.receive_pointer = instance.payload;
                    instance.num_bytes_awaited = payload_size;
                    instance.state = PROTOCOL_STATE_RECEIVING_PAYLOAD;
                }
                else
                {
                    instance.state = PROTOCOL_STATE_CONFUSED;
                    send_status_message(0, PROTOCOL_ERROR_BAD_ENDPOINT_ID);
                    break;
                }

                /* try to read payload */
                receive_data();

                /* no break, fall through and continue with
                 * PROTOCOL_STATE_RECEIVING_PAYLOAD
                 */
            }
            else
            {
                /* header is not complete, but no more data has been received,
                 * stop and try again later
                 */
                break;
            }

        case PROTOCOL_STATE_RECEIVING_PAYLOAD:
            if (instance.num_bytes_awaited == 0)
            {
                /* prepare for receiving message tail */
                instance.receive_pointer = instance.message_frame + 4;
                instance.num_bytes_awaited = 2;
                instance.state = PROTOCOL_STATE_RECEIVING_TAIL;

                /* try to read payload */
                receive_data();

                /* no break, fall through and continue with
                 * PROTOCOL_STATE_RECEIVING_TAIL
                 */
            }
            else
            {
                /* payload is not complete, but no more data has been
                 * received, stop and try again later
                 */
                break;
            }

        case PROTOCOL_STATE_RECEIVING_TAIL:
            if (instance.num_bytes_awaited == 0)
            {
                uint8_t  endpoint;
                uint16_t payload_size;
                uint16_t end_sequence;

                /* message is complete, now process it */
                endpoint     = rd_payload_u8 (instance.message_frame, 1);
                payload_size = rd_payload_u16(instance.message_frame, 2);
                end_sequence = rd_payload_u16(instance.message_frame, 4);

                /* check end of payload sequence */
                if (end_sequence != CNST_END_OF_PAYLOAD)
                {
                    instance.state = PROTOCOL_STATE_CONFUSED;
                    send_status_message(0, PROTOCOL_ERROR_BAD_PAYLOAD_END);
                    break;
                }

         	  /********** TIMING DEBUG - Protocol Start *************/
          	  #ifdef TIMING_DEBUG
                  	//DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D9);
          	  #endif

                if (endpoint != 0)
                {
                    uint16_t status_code;
                    Endpoint_t* current_ep;

                    /* now send the payload package to the addressed endpoint
                     * and send the status code back
                     */
                    current_ep = &(instance.endpoints[endpoint-1]);
                    status_code = current_ep->handle_message(endpoint,
                                                             instance.payload,
                                                             payload_size,
                                                             current_ep->
                                                               context);
                    send_status_message(endpoint, status_code);
                }
                else
                {
                    uint16_t status_code;
                    /* endpoint 0 is the protocol itself, so handle message
                     * by the protocol
                     */
                    status_code = handle_message(instance.payload,
                                                 payload_size);
                    send_status_message(0, status_code);
                }

                /********** TIMING DEBUG - Protocol End *************/
                #ifdef TIMING_DEBUG
                  	//DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D9);
                #endif

                /* go back to idle state to receive the next message */
                reset_state();
                break;
            }
            else
            {
                /* tail is not complete, but no more data has been received,
                 * stop and try again later
                 */
                break;
            }

        default:
            /* This shouldn't occur. Put this default branch here to suppress
             * a warning.
             */
            break;
        };
    }

    /* if state is confused, keep on reading into a dummy buffer, until
     * receive buffer is empty
     */
    if (instance.state == PROTOCOL_STATE_CONFUSED)
    {
        uint8_t dummy_buffer[64];
        do
        {
            instance.receive_pointer = dummy_buffer;
            instance.num_bytes_awaited = sizeof(dummy_buffer);
            receive_data();
        } while (instance.num_bytes_awaited == 0);

        /*
         * The receive_data function may switch the state back to idle, if a
         * timeout occurs, this is the  only way to get out of confused state.
         */
    }
}

//============================================================================

void protocol_send_header(uint8_t endpoint, uint16_t num_bytes)
{
    /* setup message header and send it */
    uint8_t header[4];
    wr_payload_u8 (header, 0, CNST_STARTBYTE_DATA);
    wr_payload_u8 (header, 1, endpoint);
    wr_payload_u16(header, 2, num_bytes);

    com_send_data(header, sizeof(header));
}

void protocol_send_payload(const uint8_t *message_data, uint16_t num_bytes)
{
    /* send the data through the communication stream interface */
    com_send_data(message_data, num_bytes);
}

void protocol_send_tail(void)
{
    /* setup message tail and send it */
    uint8_t tail[2];
    wr_payload_u16(tail, 0, CNST_END_OF_PAYLOAD);

    com_send_data(tail, sizeof(tail));
    com_flush();
}

//============================================================================

void protocol_broadcast_change(void* context, uint32_t what)
{
    uint8_t i;

    /* broadcast change notification to all endpoints that have a
     * handle_change function
     */
    for (i = 0; i < instance.num_endpoints; ++i)
    {
        if ((instance.endpoints[i].handle_change) &&
            (instance.endpoints[i].context == context))
        {
            instance.endpoints[i].handle_change(i + 1, context, what);
        }
    }
}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

static void send_status_message(uint8_t endpoint, uint16_t status_code)
{
    /* setup status message */
    uint8_t status_message[4];
    wr_payload_u8 (status_message, 0, CNST_STARTBYTE_STATUS);
    wr_payload_u8 (status_message, 1, endpoint);
    wr_payload_u16(status_message, 2, status_code);

    /* now send message */
    com_send_data(status_message, sizeof(status_message));
    com_flush();
}

//============================================================================

static void reset_state(void)
{
    /* set to idle state and store incoming data in message frame buffer */
    instance.state             = PROTOCOL_STATE_IDLE;
    instance.receive_pointer   = instance.message_frame;
    instance.num_bytes_awaited = 4;
}

//============================================================================

static void receive_data(void)
{
    /* receive data from communication interface */
    uint16_t num_received_bytes = com_get_data(instance.receive_pointer,
                                               instance.num_bytes_awaited);
    instance.receive_pointer += num_received_bytes;
    instance.num_bytes_awaited -= num_received_bytes;

    /* check for timeout */
    if (instance.get_time != NULL)
    {
        /* get current time */
        uint32_t current_time = instance.get_time();

        /* if no data has been received, check the time since last received
         * bytes
         */
        if (num_received_bytes == 0)
        {
        	if (instance.time_of_last_byte > current_time)
        	{
        		/* Handle time wrap-around */
        		instance.time_of_last_byte = current_time;
        	}
        	else {

				if ((instance.state != PROTOCOL_STATE_IDLE) &&
					(current_time >= instance.time_of_last_byte +
									 instance.timeout_interval))
				{
					/* a timeout has occurred, reset protocol state */
					if (instance.state != PROTOCOL_STATE_CONFUSED)
						send_status_message(0, PROTOCOL_ERROR_TIMEOUT);
					reset_state();
				}
        	}
        }
        else
        {
            /* data has been received, so remember current time for next
             * timeout check
             */
            instance.time_of_last_byte = current_time;
        }
    }
}

//============================================================================

static uint16_t handle_message(uint8_t *message_data, uint16_t num_bytes)
{
    /* check command byte of received message */
    switch (message_data[0])
    {
    case CNST_MSG_QUERY_ENDPOINT_INFO:
        if (num_bytes == 1)
        {
            send_endpoint_info();
            {
            	extern void app_reset_protocol(void);
            	app_reset_protocol();
            }

            return PROTOCOL_ERROR_OK;
        }
        break;

    case CNST_MSG_QUERY_FW_INFO:
        if (num_bytes == 1)
        {
            send_firmware_info();
            return PROTOCOL_ERROR_OK;
        }
        break;

    case CNST_MSG_FIRMWARE_RESET:
        if (num_bytes == 1)
        {
            /* if user did not set system reset function, this feature is not
             * supported.
             */
            if (instance.do_system_reset == NULL)
                return PROTOCOL_STATUS_INVALID_PAYLOAD;

            /* send status message first, after the reset it won't be
             * possible.
             */
            send_status_message(0, PROTOCOL_ERROR_OK);

            /* now do the system reset */
            instance.do_system_reset();

            return PROTOCOL_ERROR_OK;
        }
        break;

    default:
        break;
    }

    return PROTOCOL_ERROR_BAD_COMMAND;
}

//============================================================================

static void send_endpoint_info(void)
{
    uint8_t i;

    /* setup message start (type und number of endpoints */
    uint8_t message_header[2];
    wr_payload_u8(message_header, 0, CNST_MSG_ENDPOINT_INFO);
    wr_payload_u8(message_header, 1, instance.num_endpoints);

    /* calculate message size and send start of message */
    protocol_send_header(0, 6 * instance.num_endpoints +
                            sizeof(message_header));
    protocol_send_payload(message_header, sizeof(message_header));

    /* now send type and version for each endpoint */
    for (i = 0; i < instance.num_endpoints; ++i)
    {
        uint8_t ep_info[6];
        wr_payload_u32(ep_info, 0, instance.endpoints[i].endpoint_type);
        wr_payload_u16(ep_info, 4, instance.endpoints[i].endpoint_version);
        protocol_send_payload(ep_info, sizeof(ep_info));
    }

    /* finish the message */
    protocol_send_tail();
}

//============================================================================

static void send_firmware_info(void)
{
    uint16_t string_length = 0;

    /* setup message start (type and number of endpoints */
    uint8_t message[7];
    wr_payload_u8 (message, 0, CNST_MSG_FW_INFO);
    wr_payload_u16(message, 1, firmware_information.version_major);
    wr_payload_u16(message, 3, firmware_information.version_minor);
    wr_payload_u16(message, 5, firmware_information.version_build);

    /* count length of firmware description string */
    while (firmware_information.description[string_length] != 0)
        ++string_length;
    ++string_length;

    /* send message */
    protocol_send_header(0, sizeof(message) + string_length);
    protocol_send_payload(message, sizeof(message));
    protocol_send_payload((const uint8_t*)firmware_information.description,
                          string_length);
    protocol_send_tail();
}

/* --- End of File -------------------------------------------------------- */
