/**
 * \file COMPort_Unix.c
 *
 * \brief This file implements the API to access a serial communication port
 *        for macOS and Linux.
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
#define _DEFAULT_SOURCE // for cfmakeraw
#include "COMPort.h"

#if (defined __APPLE__) || (defined LINUX) || (defined __linux__)
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/
/**
 * \brief This struct contains all information about an open connection.
 */
struct com_s
{
    int handle;                  /**< The handle to the connection. */
    uint32_t timeout_period_ms;  /**< The period after that reading is stopped
                                      if no more data arrives. */
};

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

#if defined __APPLE__
uint32_t com_get_port_list(char* port_list, size_t buffer_size)
{
    /*
    * On Mac devices are represented as files in /dev, search for the Com
    * ports (cu) that have a USB in its name
    * Note: use cu* instead of tty* because tty blocks on open for carrier
    *       detection
    */
    const char dev_dir[] = "/dev";
    const char search_pattern[] = "cu.usb";

    uint32_t num_available_ports = 0;
    DIR* dir_handle;

    /* init port list to empty string */
    if (buffer_size > 0)
    {
        port_list[0] = 0;
    }

    /* open directory that contains the serial device files */
    dir_handle = opendir(dev_dir);

    if (dir_handle != NULL)
    {
        /* read entry per entry */
        struct dirent* dir_entry;

        while ((dir_entry = readdir(dir_handle)) != NULL)
        {
            size_t name_length;

            /* compare device file name with the search pattern */
            char current_device[128];

            if (strstr(dir_entry->d_name, search_pattern) == NULL)
            {
                continue;
            }

            /* add the port name to the list, if buffer capacity is
             * sufficient
             */
            strcpy(current_device, dev_dir);
            strcat(current_device, "/");
            strcat(current_device, dir_entry->d_name);

            name_length = strlen(current_device);

            if (num_available_ports > 0)
            {
                /* add one for the separator */
                ++name_length;
            }

            if (name_length < buffer_size)
            {
                if (num_available_ports > 0)
                {
                    strcat(port_list, ";");
                }

                strcat(port_list, current_device);

                /* update ramaining capacity of string buffer */
                buffer_size -= name_length;
            }

            /* port is available, so increase counter */
            ++num_available_ports;
        }

        /* close directory stream */
        closedir(dir_handle);
    }

    return num_available_ports;
}
#else /* Linux */
/* check that file exists */
static bool file_exists(const char* path)
{
    FILE* fh = fopen(path, "r");
    if (fh == NULL)
        return false;

    fclose(fh);
    return true;
}

/* check for PID and VID in uevent file */
static bool check_uevent(const char* path, const char* vid, const char* pid)
{
    char buffer[2048];
    bool ret = false;
    long vid_ = strtol(vid, NULL, 16), pid_ = strtol(pid, NULL, 16);

    FILE* fh = fopen(path, "r");
    if (fh == NULL)
        return false;

    while (fgets(buffer, sizeof(buffer), fh))
    {
        char* p = NULL;
        if (strncmp(buffer, "PRODUCT=", 8) != 0)
            continue;

        /* PRODUCT=58b/58/1 */
        char* s_vid = buffer + 8;
        if ((p = strchr(s_vid, '/')) == NULL)
            break;
        *p = '\0';
        char* s_pid = p + 1;

        if ((p = strchr(s_pid, '/')) == NULL)
            break;
        *p = '\0';

        if ((strtol(s_vid, NULL, 16) == vid_) && (strtol(s_pid, NULL, 16) == pid_))
        {
            ret = true;
            break;
        }
    }

    fclose(fh);

    return ret;
}

uint32_t com_get_port_list(char* port_list, size_t buffer_size)
{
    uint32_t num_available_ports = 0;

    memset(port_list, 0, buffer_size);

    const char* sysdir = "/sys/class/tty/";
    DIR* dir_handle = opendir(sysdir);
    if (dir_handle == NULL)
        return 0;

    struct dirent* dir_entry;
    while ((dir_entry = readdir(dir_handle)) != NULL)
    {
        /* search for /sys/class/tty/ttyACMXXX where XXX is a number */
        const char* fname = dir_entry->d_name;
        if (strncmp(fname, "ttyACM", 6) != 0)
            continue;

        /* check for correct vendor and product id in /sys/class/tty/ttyACMXXX/device/uevent */
        char path[PATH_MAX] = { 0 };
        snprintf(path, sizeof(path) - 1, "%s/%s/device/uevent", sysdir, fname);
        if (check_uevent(path, IFX_RADAR_VID, IFX_RADAR_PID) || check_uevent(path, IFX_RADAR_VID, IFX_RADAR_FF_PID))
        {
            char port[512] = { 0 };
            snprintf(port, sizeof(port) - 1, "/dev/%s", fname);
            if (!file_exists(port))
                continue;

            size_t len = strlen(port);
            if (num_available_ports > 0)
                len++;

            if (buffer_size > len)
            {
                /* append port to port_list */
                if (num_available_ports)
                    strcat(port_list, ";");

                strcat(port_list, port);
                buffer_size -= len;
            }

            /* we found one more port */
            num_available_ports++;
        }
    }

    closedir(dir_handle);
    return num_available_ports;
}
#endif

com_t* com_open(const char* port_name)
{
    struct termios options;

    com_t* com_port = malloc(sizeof(com_t));
    if(com_port == NULL)
        return NULL;

    /*
     * Open the serial port read/write, with no controlling terminal, and
     * don't wait for a connection. See open(2) ("man 2 open") for details.
     */
    com_port->handle = open(port_name, O_RDWR | O_NOCTTY);

    if (com_port->handle == -1)
        goto fail;

    /*
     * Note that open() follows POSIX semantics: multiple open() calls to the
     * same file will succeed unless the TIOCEXCL ioctl is issued. This will
     * prevent additional opens except by root-owned processes.
     * See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
     */
    if (ioctl(com_port->handle, TIOCEXCL) == -1)
    {
        /* Don't treat as fatal if the command is just not supported. */
        if (errno != ENOTTY)
            goto fail;
    }

    /* Get the current options and save them so we can restore the default
     * settings later.
     */
    if (tcgetattr(com_port->handle, &options) == -1)
    {
        goto fail;
    }

    /*
     * The serial port attributes such as timeouts and baud rate are set by
     * modifying the termios structure and then calling tcsetattr to  cause
     * the changes to take effect. Note that the changes will not take effect
     * without the tcsetattr() call. See tcsetattr(4) ("man 4 tcsetattr") for
     * details.
     *
     * Set raw input (non-canonical) mode, with non-blocking reads.
     * See tcsetattr(4) ("man 4 tcsetattr") and termios(4) ("man 4 termios")
     * for details.
     */
    cfmakeraw(&options);
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 0;

    /* Enable local mode, because the USBD_VCOM APP does not handle the
     * virtual flow control lines
     */
    options.c_cflag |= CLOCAL;

    /* Cause the new options to take effect immediately. */
    if (tcsetattr(com_port->handle, TCSANOW, &options) == -1)
        goto fail;

    com_port->timeout_period_ms = 1000;
    return com_port;

fail:
    free(com_port);
    return NULL;
}

void com_close(com_t* com_port)
{
    /* close COM port */
    close(com_port->handle);

    /* free allocated memory */
    free(com_port);
}

size_t com_send_data(com_t* com_port, const void* data, size_t num_bytes)
{
    /* send data */
    return write(com_port->handle, data, num_bytes);

}

size_t com_get_data(com_t* com_port,
                    void* data, size_t num_requested_bytes)
{
    size_t num_received_bytes = 0;
    char* read_buffer = (char*)data;
    struct timespec time_of_last_byte;

    /* read data */
    clock_gettime(CLOCK_MONOTONIC, &time_of_last_byte);
    while (num_received_bytes < num_requested_bytes)
    {
        ssize_t num_bytes = read(com_port->handle,
                                 read_buffer + num_received_bytes,
                                 num_requested_bytes - num_received_bytes);
        num_received_bytes += num_bytes;
          
        /* check for timeout */
        struct timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        if (num_bytes != 0)
        {
            time_of_last_byte = current_time;
        }
        else
        {
            if ((current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000) >
                (time_of_last_byte.tv_sec * 1000 +
                 time_of_last_byte.tv_nsec / 1000000 +
                 com_port->timeout_period_ms))
            {
                break;
            }
        }
    }
    return num_received_bytes;
}

void com_set_timeout(com_t* com_port, uint32_t timeout_period_ms)
{
    com_port->timeout_period_ms = timeout_period_ms;
}

/* --- Close open blocks -------------------------------------------------- */

/* End of UNIX only code */
#endif /* (defined __APPLE__) || (defined LINUX) || (defined __linux__) */

/* --- End of File -------------------------------------------------------- */
