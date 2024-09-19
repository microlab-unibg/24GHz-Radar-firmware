/**
 * \file COMPort_Windows.c
 *
 * \brief This file implements the API to access a serial communication port
 *        for Windows.
 *
 * \see COMPort.h for details
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
#include "COMPort.h"

#if defined _WIN32

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <setupapi.h>

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/

struct com_s {
    HANDLE handle;
};

typedef struct
{
    char* port_list;
    uint32_t ports_available;
    size_t buffer_available;
} Port_List_t;

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

/**
 * \brief Check if vendor id and product id are valid
 *
 * The vendor id (VID) and product id (pid) are valid if the string consists
 * of exactly 4 hexadecimal characters, so each of the 4 characters must be
 * one of [0-9abcdefABCDEF].
 *
 * \param [in] vid  vendor id
 * \param [in] pid  product id
 * \retval true     if vendor and product id are valid
 * \retval false    if vendor and product id are invalid
 */
static bool is_valid_vid_pid(const char* vid, const char* pid)
{
    if (strlen(vid) != 4 || strlen(pid) != 4)
        return false;

    for (int i = 0; i < 4; i++)
    {
        if (!isxdigit(vid[i]) || !isxdigit(pid[i]))
            return false;
    }

    return true;
}

/**
 * \brief Find all com ports corresponding to a USB device with given VID and PID
 *
 * Foreach USB device with vendor id vid and product id pid that corresponds to
 * a COM port, the function will call the function callback. The first argument
 * com_port is the name of the COM port (e.g. "COM10"), the second argument is
 * a context pointer given by the caller.
 *
 * \param [in] vid          vendor id
 * \param [in] pid          product id
 * \param [in] callback     function that is called when device is found
 * \param [in] context      context pointer handed to callback
 * \retval false            an error occured
 * \retval true             function returned successful
 */
static bool com_find(const char* vid, const char* pid,
                     void(*callback)(const char* com_port, void* context),
                     void* context)
{
    if (!is_valid_vid_pid(vid, pid))
        return false;

    // plug and play id: USB\VID_XXXX&PID_XXXX where XXXX are hexadecimal characters
    char pnpid[32];
    snprintf(pnpid, sizeof(pnpid), "USB\\VID_%c%c%c%c&PID_%c%c%c%c",
             toupper(vid[0]), toupper(vid[1]), toupper(vid[2]), toupper(vid[3]),
             toupper(pid[0]), toupper(pid[1]), toupper(pid[2]), toupper(pid[3]));

    // SetupDiGetClassDevs returns a handle to a device information set that
    // contains requested device information elements for a local computer
    HDEVINFO DeviceInfoSet = SetupDiGetClassDevs(NULL, TEXT("USB"), NULL,
                                                 DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (DeviceInfoSet == INVALID_HANDLE_VALUE)
        return false;

    DWORD DeviceIndex = 0;
    SP_DEVINFO_DATA DeviceInfoData = {0};
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    // SetupDiEnumDeviceInfo returns a SP_DEVINFO_DATA structure that specifies
    // a device information element in a device information set
    while (SetupDiEnumDeviceInfo(DeviceInfoSet, DeviceIndex++, &DeviceInfoData))
    {
        BYTE buffer[256] = {0};
        DEVPROPTYPE ulPropertyType;
        DWORD dwSize1;

        // get specified plug and play device property
        if (SetupDiGetDeviceRegistryPropertyA(DeviceInfoSet, &DeviceInfoData,
                                              SPDRP_HARDWAREID, &ulPropertyType,
                                              buffer, sizeof(buffer), &dwSize1))
        {
            // check if this is the device we are looking for
            if (strncmp((char*)buffer, pnpid, strlen(pnpid)) != 0)
                continue;

            // open registry
            HKEY hDeviceRegistryKey = SetupDiOpenDevRegKey(DeviceInfoSet,
                                                           &DeviceInfoData,
                                                           DICS_FLAG_GLOBAL, 0,
                                                           DIREG_DEV, KEY_READ);
            if (hDeviceRegistryKey == INVALID_HANDLE_VALUE)
                continue;

            // query
            char port_name[32] = {0};
            DWORD dwSize2 = sizeof(port_name);
            DWORD dwType = REG_SZ;
            LSTATUS ret = RegQueryValueExA(hDeviceRegistryKey, "PortName",
                                           NULL, &dwType, (LPBYTE)port_name, &dwSize2);

            // close registry key
            RegCloseKey(hDeviceRegistryKey);

            if (ret == ERROR_SUCCESS && (dwType == REG_SZ))
            {
                // check if the usb device corresponds to a COM port
                if (strncmp(port_name, "COM", 3) == 0)
                    callback(port_name, context);
            }
        }
    }

    SetupDiDestroyDeviceInfoList(DeviceInfoSet);

    return true;
}

static void callback(const char* com_port, void* context)
{
    Port_List_t* ports = (Port_List_t*)context;
    const size_t len = strlen(com_port);

    if (ports->ports_available == 0 && ports->buffer_available >= len)
    {
        memcpy(ports->port_list, com_port, len);
        ports->port_list += len;
        ports->buffer_available -= len;
    }
    else if (ports->ports_available > 0 && ports->buffer_available >= (len + 1))
    {
        *ports->port_list++ = ';';
        memcpy(ports->port_list, com_port, len);
        ports->port_list += len;
        ports->buffer_available -= len + 1;
    }

    ports->ports_available++;
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

uint32_t com_get_port_list(char* port_list, size_t buffer_size)
{
    Port_List_t ports;
    ports.port_list = port_list;
    ports.buffer_available = buffer_size - 1;
    ports.ports_available = 0;

    memset(port_list, 0, buffer_size);

    com_find(IFX_RADAR_VID, IFX_RADAR_PID, callback, &ports);
    com_find(IFX_RADAR_VID, IFX_RADAR_FF_PID, callback, &ports);

    return ports.ports_available;
}

com_t* com_open(const char* port_name)
{
    COMMTIMEOUTS timeouts;
    DCB com_configuration = {0};

    com_t* com_port = malloc(sizeof(com_t));
    if (com_port == NULL)
        return NULL;

    /*
     * use full path because Windows won't find "COM10" and higher
     * (see knowledge base http://support.microsoft.com/kb/115831/en-us)
     *
     * To work with both Multi-Byte-Charset and Unicode Charset convert
     * port_name to TCHAR, because that type is needed by Win32 API.
     * I did not find a suitable out-of-the-box function for this conversion
     * so it's done explicitly here.
     */
    #define T_BUFFER_LENGTH 32
    TCHAR full_port_name[T_BUFFER_LENGTH] = TEXT("\\\\.\\");
    unsigned idx = 0;
    while ((port_name[idx] != 0) && (idx + 5 < T_BUFFER_LENGTH))
    {
        full_port_name[4 + idx] = port_name[idx];
        ++idx;
    }
    full_port_name[4 + idx] = 0;

    /* open the COM port */
    /* ----------------- */
    com_port->handle = CreateFile(full_port_name,  /* name of the COM port */
                                  /*lint -e620 we don't really have a chance to change a MSYS/MinGW header file ... */
                                  GENERIC_READ |   /* access type */
                                  /*lint -e620 */
                                  GENERIC_WRITE,
                                  0,               /* shared mode */
                                  NULL,            /* security attributes */
                                  OPEN_EXISTING,   /* creation disposition */
                                  0,               /* flags and attributes */
                                  0);              /* template file */

    /* if COM port could not be opened, return negative Windows error code */
    if (com_port->handle == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }

    /* set timeouts */
    timeouts.ReadIntervalTimeout         = 0;
    timeouts.ReadTotalTimeoutMultiplier  = 0;
    timeouts.ReadTotalTimeoutConstant    = 1000;
    timeouts.WriteTotalTimeoutConstant   = 100;
    timeouts.WriteTotalTimeoutMultiplier = 1;
    (void)SetCommTimeouts(com_port->handle, &timeouts);

    /* configure COM Port (even though it's virtual) */
    com_configuration.DCBlength = sizeof(DCB);
    (void)GetCommState (com_port->handle, &com_configuration);
    com_configuration.BaudRate = 115200;
    com_configuration.ByteSize = 8;
    com_configuration.StopBits = ONESTOPBIT;
    (void)SetCommState (com_port->handle, &com_configuration);

    return com_port;
}

void com_close(com_t* com_port)
{
    if (com_port == NULL)
        return;

    /* stop all transfers are in progress */
    (void)CancelIo(com_port->handle);

    /* close COM port */
    (void)CloseHandle(com_port->handle);

    /* free memory for com_port */
    free(com_port);
}

size_t com_send_data(com_t* com_port, const void* data, size_t num_bytes)
{
    /* send data */
    DWORD num_bytes_written;
    WriteFile(com_port->handle, data, (DWORD)num_bytes, &num_bytes_written, NULL);
    return num_bytes_written;
}

size_t com_get_data(com_t* com_port,
                    void* data, size_t num_requested_bytes)
{
    /* read data */
    DWORD num_bytes_read = 0;

    (void)ReadFile(com_port->handle, data, (DWORD)num_requested_bytes, &num_bytes_read, NULL);

    return num_bytes_read;
}

void com_set_timeout(com_t* com_port, uint32_t timeout_period_ms)
{
    /* set timeouts */
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout         = 0;
    timeouts.ReadTotalTimeoutMultiplier  = 0;
    timeouts.ReadTotalTimeoutConstant    = timeout_period_ms;
    timeouts.WriteTotalTimeoutConstant   = 100;
    timeouts.WriteTotalTimeoutMultiplier = 1;
    (void)SetCommTimeouts(com_port->handle, &timeouts);
}

/* --- Close open blocks -------------------------------------------------- */

/* End of Windows only code */
#endif /* _WIN32 */

/* --- End of File -------------------------------------------------------- */
