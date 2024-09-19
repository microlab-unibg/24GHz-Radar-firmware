/**
 * \file Protocol.h
 *
 * \brief This file defines the API of the host communication protocol.
 *
 * The communication protocol is a generic protocol to exchange messages with
 * a host over a generic byte stream connection (e.g. typically a virtual
 * serial port over USB). The communication is always initiated by the host,
 * while the device responds to messages received from the host.
 *
 * The protocol defines two types of messages, payload messages and status
 * messages. A payload message contains a block of data with arbitrary size
 * and meaning. A status message contains a 16 bit status code. Messages from
 * the host to the device are always payload messages. Whenever the device
 * receives a payload message from the host, it responds with one (and only
 * one) status message. The device may also send an arbitrary number of
 * additional payload messages preceding that status message. Once the device
 * has sent the status message, it won't send any more messages until it
 * receives a new message from the host.
 *
 * Each message from the host to the device is addressed to a certain logical
 * endpoint and each message from the device to the host is sent from a
 * logical endpoint. The number of logical endpoints is defined by the device.
 * Endpoints are continuously enumerated starting with 1. At least one logical
 * endpoint is always present. Endpoints are used to define functional groups
 * in the device.
 *
 * Each logical endpoint is of a defined type, which defines the meaning of
 * the message payload exchanged with that endpoint. Additionally each
 * endpoint has a version number that allows to distinguish between slight
 * modifications of the endpoints set of known messages. The host can query
 * type and version of the endpoints present in the device.
 *
 * The protocol uses the communication interface defined by
 * \ref StreamInterface.h. An implementation of this interface must be
 * available when using the communication protocol.
 *
 * The firmware's main function should call \ref protocol_init to initialize
 * the protocol and register the endpoint implementations. The function
 * \ref protocol_run should be called from the main loop. The protocol
 * processes received data in this function and dispatches messages to the
 * endpoints.
 *
 * Endpoint implementation must use the functions
 * \ref protocol_send_header, \ref protocol_send_payload and
 * \ref protocol_send_tail to send payload messages to the host. Status
 * messages are sent automatically by the protocol, the endpoint
 * implementation does not have to deal with status messages.
 *
 * An endpoint implementation must provide a function to handle incoming
 * messages addressed to that endpoint. The message payload, endpoint number
 * is passed to that function along with a context pointer to additional data.
 * After processing the payload that function must return a status code that
 * is sent back to the host by the protocol.
 *
 * During initialization an endpoint list is provided to the protocol to
 * register all endpoints in a firmware. At least one endpoint must be
 * registered. See \ref Endpoint_t for more information about
 * endpoint registration.
 *
 * The protocol implementation allows the host to read information about the
 * firmware. The protocol expects the firmware information in an externally
 * defined structure.
 * \code
 * Firmware_Information_t firmware_information;
 * \endcode
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

#ifndef HOSTCOMMUNICATION_PROTOCOL_H_
#define HOSTCOMMUNICATION_PROTOCOL_H_

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */
#include <stdint.h>

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */
/**
 * \defgroup StatusCodes
 *
 * \brief These status code are common to all end points.
 *
 * Every end point may define it's own status codes. Never the less there are
 * situations that can occur in every end point, so according status codes are
 * defined once here.
 *
 * @{
 */
#define PROTOCOL_STATUS_OK              0x0000  /**< No error has occurred. */
#define PROTOCOL_STATUS_INVALID_PAYLOAD 0xFFFF  /**< The message payload could
                                                     not be parsed correctly
                                                     by the endpoint, because
                                                     it is of unknown type or
                                                     wrong size. */
/** @} */

/*
==============================================================================
   3. TYPES
==============================================================================
 */

/**
 * \brief This structure contains all relevant information about an endpoint.
 *
 * An array containing elements of this type must be provided to the function
 * \ref protocol_init to register all endpoints to the protocol.
 *
 * This structure contains type and version information about the endpoint
 * that is reported to the host. Furthermore this structure contains a
 * function pointer to the function to be called when a message for that
 * endpoint is received. That function must have the following signature.
 * \code
 * uint16_t handle_message(uint8_t endpoint, uint8_t *message_data,
 *                         uint16_t num_bytes, void* context);
 * \endcode
 * The endpoint number and the message payload is forwarded to that
 * handle_message function. Also a context pointer to additional data provided
 * to this structure is forwarded to the handle_message function. The meaning
 * of the context data is defined by the according endpoint implementation.
 *
 * It is recommended that endpoint implementation provide a macro to setup the
 * endpoint list entry, so the fields of this structure don't have to be
 * filled directly by the user.
 */
typedef struct
{
    uint32_t endpoint_type;                         /**< A number identifying
                                                         the type of the
                                                         endpoint. That number
                                                         will be reported to
                                                         the host. */
    uint16_t endpoint_version;                      /**< The version of the
                                                         endpoint
                                                         implementation.
                                                         The version will be
                                                         reported to the host.
                                                         */
    uint16_t (*handle_message)(uint8_t endpoint,
                               uint8_t *message_data,
                               uint16_t num_bytes,
                               void* context);      /**< The handle_message
                                                         function of the
                                                         endpoint
                                                         implementation. */
    void*    context;                               /**< The pointer to
                                                         additional context
                                                         data that will be
                                                         forwarded to the
                                                         handle_message
                                                         function. */
    void     (*handle_change)(uint8_t endpoint,
                              void* context,
                              uint32_t what);       /**< This function handles
                                                         a change to the
                                                         context of the
                                                         endpoint. It is
                                                         invoked when
                                                         another end point
                                                         calls
                                                         /ref protocol_broadcast_change.
                                                         */
} Endpoint_t;

/**
 * \brief This structure contains information about the firmware.
 *
 * The protocol expects an instance of this structure to be externally defined
 * in the firmware project.
 * \code
 * Firmware_Information_t firmware_information;
 * \endcode
 * The information in firmware_information will be sent to the host on
 * request.
 */
typedef struct
{
    const char* description;   /**< A pointer to a zero-terminated string
                                    containing a firmware description. */
    uint16_t    version_major; /**< The firmware version major number. */
    uint16_t    version_minor; /**< The firmware version minor number. */
    uint16_t    version_build; /**< The firmware version build number. */
} Firmware_Information_t;

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
 */

/**
 * \brief This function initialized the protocol.
 *
 * This function must be called once before the protocol can be used. It
 * initialized the internal protocol state and the communication interface.
 *
 * This function also registers the endpoints to the protocol. Therefore a
 * pointer to an array of \ref Endpoint_t must be provided, which
 * contains type and version information about each endpoint and call
 * information to the handle_message function.
 *
 * The protocol also allows timeout checks for incoming message. If this
 * feature is used, delays between parts of a message must not exceed the
 * given timeout period, or the message will be rejected with an error code.
 *
 * To use the timeout check a pointer to a function that returns the current
 * time must be provided. The unit of the value returned by that function is
 * arbitrary, but the provided timeout interval must be given with respect to
 * the unit of the get_time function. If no function pointer is provided,
 * timeout checks are disabled.
 *
 * \param[in] endpoints         An array containing endpoint information.
 * \param[in] num_endpoints     The number of array entries in endpoints.
 * \param[in] get_time          A pointer to a function returning the current
 *                              time. This may be NULL to disable timeout
 *                              checks.
 * \param[in] timeout_interval  The timeout period with respect to the unit
 *                              returned by the get_time function. If get_time
 *                              is NULL, this value is ignored.
 * \param[in] do_system_reset   A pointer to a function that does a system
 *                              reset. This may be NULL, but then the host
 *                              won't be able to trigger a firmware reset.
 */
void protocol_init(Endpoint_t* endpoints, uint8_t num_endpoints,
                   uint32_t (*get_time)(void), uint32_t timeout_interval,
                   void (*do_system_reset)(void));

/**
 * \brief This function does the main work of the protocol.
 *
 * This function must be called regularly to keep the protocol working.
 * Ideally this function is called from the main program loop.
 *
 * This function reads incoming data from the communication interfaces and
 * checks the received messages for integrity. When a valid message is
 * received the payload from that message is forwarded to the addressed
 * endpoint. The endpoint processes the payload with its handle_message
 * function and returns a status code, which is sent back to the host by the
 * protocol.
 */
void protocol_run();

/**
 * \brief This function sends the header of a payload message.
 *
 * Whenever an endpoint sends a payload message to the host, it must call this
 * function once to send the header containing its endpoint number and the
 * size of the payload that will follow.
 *
 * After the header is sent, the payload must be sent by calling
 * \ref protocol_send_payload. Finally the message must be finished by
 * calling \ref protocol_send_tail.
 *
 * The endpoint must take care to keep the correct order of send_header,
 * send_payload and send_tail calls and to announce the exact
 * payload size in the message header.
 *
 * \param[in] endpoint  The number of the endpoint that sends the message.
 * \param[in] num_bytes  The number of payload bytes that will follow.
 */
void protocol_send_header(uint8_t endpoint, uint16_t num_bytes);

/**
 * \brief This function sends the payload of a message.
 *
 * This function sends the data from the provided buffer to the host. The
 * message payload may be split into multiple blocks each sent by calling this
 * function. The total number of bytes sent by calling this function must
 * match the payload size that was announced in the message header.
 *
 * Before the payload can be sent, a message header must be sent by calling
 * \ref protocol_send_header. After the payload has been sent completely
 * the message must be finished by calling \ref protocol_send_tail.
 *
 * The endpoint must take care to keep the correct order of send_header,
 * send_Payload and send_Tail calls and to announce the exact payload size in
 * the message header.
 *
 * \param[in] message_data  A pointer to the buffer containing the payload
 *                          data to be sent.
 * \param[in] num_bytes     The number of payload bytes to be sent.
 */
void protocol_send_payload(const uint8_t *message_data, uint16_t num_bytes);

/**
 * \brief This function sends the tail of a payload message.
 *
 * At the end of each payload message the message tail must be sent by calling
 * this function.
 *
 * The end point must take care to keep the correct order of
 * send_header, send_payload and send_tail calls and to
 * announce the exact payload size in the message header.
 */
void protocol_send_tail(void);

/**
 * \brief This function sends a change notification to all end points.
 *
 * When multiple end points share the same context they may want to inform
 * each other when they apply a change to that context. If this change happens
 * in response to a message received from the host, all the related end points
 * may send an update message to the host before the originating sends it
 * status message.
 *
 * An end point just needs to call this function to inform all end points that
 * a change has happened. Each end point that has a change handler in function
 * in its \ref Endpoint_t structure will be called if it has the the
 * same context set in its \ref Endpoint_t structure as the one
 * provided by the calling end point.
 *
 * Furthermore a code can be passed to the change handler indicating, what
 * exactly has changed. The meaning of that code depends on the involved end
 * point implementations.
 *
 * \param[in] context  The context that has changed. Only endpoints with the
 *                     same context will be notified about that change.
 * \param[in] what     A numerical code indicating the kind of change. End
 *                     points may define the meaning of this code according
 *                     to their needs.
 */
void protocol_broadcast_change(void* context, uint32_t what);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* HOSTCOMMUNICATION_PROTOCOL_H_ */

/* --- End of File -------------------------------------------------------- */
