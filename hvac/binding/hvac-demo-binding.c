/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Copyright (C) 2016 Konsulko Group
 * Author "Romain Forlot"
 * Author "Jose Bolo"
 * Author "Scott Murray <scott.murray@konsulko.com>"
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define _GNU_SOURCE

#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <math.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>   
#include <stdlib.h>

#include <json-c/json.h>

#define AFB_BINDING_VERSION   1
#include <afb/afb-binding.h>

#define CAN_DEV "vcan0"
#define SER_DEV "/dev/tty96B0"

static const struct afb_binding_interface *interface;

/*****************************************************************************************/
/*****************************************************************************************/
/**											**/
/**											**/
/**	   SECTION: UTILITY FUNCTIONS							**/
/**											**/
/**											**/
/*****************************************************************************************/
/*****************************************************************************************/

/*
 * @brief initialize serial port
 * 
 * @return : fd if ok, -1 if failed
 */
static int init_serial(void)
{
	int fd;
	
	fd = open(SER_DEV,O_RDWR | O_NOCTTY | O_NDELAY);
	if(fd == -1) {
		AFB_ERROR(interface, "failed to open serial port");
		return fd;
	}

	struct termios sport;
	tcgetattr(fd, &sport);
	cfsetispeed(&sport,B9600);
	cfsetospeed(&sport,B9600);
	
	sport.c_cflag &= ~PARENB;
	sport.c_cflag &= ~CSTOPB;
	sport.c_cflag &= ~CSIZE;
	sport.c_cflag |=  CS8;

	sport.c_cflag &= ~CRTSCTS;
	sport.c_cflag |= CREAD | CLOCAL;
	sport.c_iflag &= ~(IXON | IXOFF | IXANY);
	sport.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	sport.c_oflag &= ~OPOST;

	tcflush(fd, TCIFLUSH);

	if((tcsetattr(fd,TCSANOW,&sport)) != 0) {
		AFB_ERROR(interface, "failed to set serial port attributes");
		return -1;
	}

	return fd;
}


/*
 * @brief Retry a function 3 times
 *
 * @param int function(): function that return an int wihtout any parameter
 *
 * @ return : 0 if ok, -1 if failed
 *
 */
static int retry( int(*func)())
{
	int i;

	for (i=0;i<4;i++)
	{
		if ( (*func)() >= 0)
		{
			return 0;
		}
		usleep(100000);
	}
	return -1;
}

/*****************************************************************************************/
/*****************************************************************************************/
/**											**/
/**											**/
/**	   SECTION: HANDLE CAN DEVICE							**/
/**											**/
/**											**/
/*****************************************************************************************/
/*****************************************************************************************/

// Initialize CAN hvac array that will be sent trough the socket
static struct {
	const char *name;
	uint8_t value;
} hvac_values[] = {
	{ "LeftTemperature", 0 },
	{ "RightTemperature", 0 },
	{ "Temperature", 0 },
	{ "FanSpeed", 0 }
};

struct can_handler {
	int socket;
	bool simulation;
	char *send_msg;
	struct sockaddr_can txAddress;
};

static struct can_handler can_handler = { .socket = -1, .simulation = false, .send_msg = "SENDING CAN FRAME"};

static int open_can_dev_helper()
{
	struct ifreq ifr;

	AFB_ERROR(interface, "CAN Handler socket : %d", can_handler.socket);
	close(can_handler.socket);

	can_handler.socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (can_handler.socket < 0)
	{
		AFB_ERROR(interface, "socket could not be created");
	}
	else
	{
		// Attempts to open a socket to CAN bus
		strcpy(ifr.ifr_name, CAN_DEV);
		if(ioctl(can_handler.socket, SIOCGIFINDEX, &ifr) < 0)
		{
			AFB_ERROR(interface, "ioctl failed");
		}
		else
		{
			can_handler.txAddress.can_family = AF_CAN;
			can_handler.txAddress.can_ifindex = ifr.ifr_ifindex;

			// And bind it to txAddress
			if (bind(can_handler.socket, (struct sockaddr *)&can_handler.txAddress, sizeof(can_handler.txAddress)) < 0)
			{
				AFB_ERROR(interface, "bind failed");
			}
			else {
				return 0;
			}
		}
		close(can_handler.socket);
		can_handler.socket = -1;
	}
	return -1;
}

static int open_can_dev()
{
	int rc = retry(open_can_dev_helper);
	if(rc < 0)
	{
		AFB_ERROR(interface, "Open of interface %s failed. Falling back to simulation mode", CAN_DEV);
		can_handler.socket = 0;
		can_handler.simulation = true;
		can_handler.send_msg = "FAKE CAN FRAME";
		rc = 0;
	}
	return rc;
}

// Get original get temperature function from cpp hvacplugin code
static uint8_t to_can_temp(uint8_t value)
{
	int result = ((0xF0 - 0x10) / 15) * (value - 15) + 0x10;
	if (result < 0x10)
		result = 0x10;
	if (result > 0xF0)
		result = 0xF0;

	return (uint8_t)result;
}

static uint8_t read_temp_left_zone()
{
	return hvac_values[0].value;
}

static uint8_t read_temp_right_zone()
{
	return hvac_values[1].value;
}

static uint8_t read_temp()
{
	return (uint8_t)(((int)read_temp_left_zone() + (int)read_temp_right_zone()) >> 1);
}

static uint8_t read_fanspeed()
{
	return hvac_values[3].value;
}

static int write_can()
{
	struct can_frame txCanFrame;
	int rc = 0;

	rc = can_handler.socket;
	if (rc >= 0)
	{
		// Hardcoded can_id and dlc (data lenght code)
		txCanFrame.can_id = 0x30;
		txCanFrame.can_dlc = 8;
		txCanFrame.data[0] = to_can_temp(read_temp_left_zone());
		txCanFrame.data[1] = to_can_temp(read_temp_right_zone());
		txCanFrame.data[2] = to_can_temp(read_temp());
		txCanFrame.data[3] = 0xf0;
		txCanFrame.data[4] = read_fanspeed();
		txCanFrame.data[5] = 1;
		txCanFrame.data[6] = 0;
		txCanFrame.data[7] = 0;

		AFB_ERROR(interface, "%s: %d %d [%02x %02x %02x %02x %02x %02x %02x %02x]\n",
			can_handler.send_msg,
			txCanFrame.can_id, txCanFrame.can_dlc,
			txCanFrame.data[0], txCanFrame.data[1], txCanFrame.data[2], txCanFrame.data[3],
			txCanFrame.data[4], txCanFrame.data[5], txCanFrame.data[6], txCanFrame.data[7]);

		if(!can_handler.simulation)
		{
			rc = sendto(can_handler.socket, &txCanFrame, sizeof(struct can_frame), 0,
				    (struct sockaddr*)&can_handler.txAddress, sizeof(can_handler.txAddress));
			if (rc < 0)
			{
				AFB_ERROR(interface, "Sending CAN frame failed.");
			}
		}
	}
	else
	{
		AFB_ERROR(interface, "socket not initialized. Attempt to reopen can device socket.");
		open_can_dev();
	}
	return rc;
}

/*****************************************************************************************/
/*****************************************************************************************/
/**											**/
/**											**/
/**	   SECTION: BINDING VERBS IMPLEMENTATION					**/
/**											**/
/**											**/
/*****************************************************************************************/
/*****************************************************************************************/

/*
 * @brief Get fan speed HVAC system
 *
 * @param struct afb_req : an afb request structure
 *
 */
static void get_fanspeed(struct afb_req request)
{
	json_object *ret_json;
	uint8_t fanspeed = read_fanspeed();

	ret_json = json_object_new_object();
	json_object_object_add(ret_json, "FanSpeed", json_object_new_int(fanspeed));

	afb_req_success(request, ret_json, NULL);
}

/*
 * @brief Read Consign right zone temperature for HVAC system
 *
 * @param struct afb_req : an afb request structure
 *
 */
static void get_temp_right_zone(struct afb_req request)
{
	json_object *ret_json;
	uint8_t temp = read_temp_right_zone();

	ret_json = json_object_new_object();
	json_object_object_add(ret_json, "RightTemperature", json_object_new_int(temp));

	afb_req_success(request, ret_json, NULL);
}

/*
 * @brief Read Consign left zone temperature for HVAC system
 *
 * @param struct afb_req : an afb request structure
 *
 */
static void get_temp_left_zone(struct afb_req request)
{
	json_object *ret_json;
	uint8_t temp = read_temp_left_zone();

	ret_json = json_object_new_object();
	json_object_object_add(ret_json, "LeftTemperature", json_object_new_int(temp));

	afb_req_success(request, ret_json, NULL);
}

/*
 * @brief Read all values
 *
 * @param struct afb_req : an afb request structure
 *
 */
static void get(struct afb_req request)
{
	AFB_ERROR(interface, "Getting all values");
	json_object *ret_json;

	ret_json = json_object_new_object();
	json_object_object_add(ret_json, "LeftTemperature", json_object_new_int(read_temp_left_zone()));
	json_object_object_add(ret_json, "RightTemperature", json_object_new_int(read_temp_right_zone()));
	json_object_object_add(ret_json, "FanSpeed", json_object_new_int(read_fanspeed()));

	afb_req_success(request, ret_json, NULL);
}

/*
 * @brief Set a component value using a json object retrieved from request
 *
 * @param struct afb_req : an afb request structure
 *
 */
static void set(struct afb_req request)
{
	int i, rc, x, changed;
	double d;
	int fd;
	char buffer[30];
	struct json_object *query, *val;
	uint8_t values[sizeof hvac_values / sizeof *hvac_values];
	uint8_t saves[sizeof hvac_values / sizeof *hvac_values];

	/* records initial values */
	AFB_ERROR(interface, "Records initial values");
	i = (int)(sizeof hvac_values / sizeof *hvac_values);
	while (i) {
		i--;
		values[i] = saves[i] = hvac_values[i].value;
	}

	/* Loop getting arguments */
	query = afb_req_json(request);
	changed = 0;
	i = (int)(sizeof hvac_values / sizeof *hvac_values);
	AFB_ERROR(interface, "Looping for args. i: %d", i);
	while (i)
	{
		i--;
		AFB_ERROR(interface, "Searching... query: %s, i: %d, comp: %s", json_object_to_json_string(query), i, hvac_values[i].name);
		if (json_object_object_get_ex(query, hvac_values[i].name, &val))
		{
			AFB_ERROR(interface, "We got it. Tests if it is an int or double.");
			if (json_object_is_type(val, json_type_int)) {
				x = json_object_get_int(val);
				AFB_ERROR(interface, "We get an int: %d",x);
			}
			else if (json_object_is_type(val, json_type_double)) {
				d = json_object_get_double(val);
				x = (int)round(d);
				AFB_ERROR(interface, "We get a double: %f => %d",d,x);
			}
			else {
				afb_req_fail_f(request, "bad-request",
					"argument '%s' isn't integer or double", hvac_values[i].name);
				return;
			}
			if (x < 0 || x > 255)
			{
				afb_req_fail_f(request, "bad-request",
					"argument '%s' is out of bounds", hvac_values[i].name);
				return;
			}
			if (values[i] != x) {
				values[i] = (uint8_t)x;
				changed = 1;
				AFB_ERROR(interface,"%s changed to %d",hvac_values[i].name,x);
			}
		}
		else {
			AFB_ERROR(interface, "%s not found in query!",hvac_values[i].name);
		}
	}

	/* attemps to set new values */
	AFB_ERROR(interface, "Diff: %d", changed);
	if (changed)
	{
		i = (int)(sizeof hvac_values / sizeof *hvac_values);
		while (i) {
			i--;
			hvac_values[i].value = values[i];
		}

		/* init serial port */
		fd = init_serial();

		memset(buffer, 0, sizeof(buffer));
	
		/* data format: l:<left_temp>&r:<right_temp>&f:<fan_speed>& */

		/* copy left temp data */	
		sprintf(buffer, "l:%d&", hvac_values[0].value);
		/* copy right temp data */	
		strcat(buffer, "r:%d&", hvac_values[1].value);
		/* copy fan speed data */	
		strcat(buffer, "f:%d&", hvac_values[3].value);
		strcat(buffer, "\r\n");

		/* send data over serial */
		write(fd, (void *)buffer, sizeof(buffer));
		
		rc = write_can();
		if (rc >= 0)
			afb_req_success(request, NULL, NULL);
		else if (retry(write_can)) {
			/* restore initial values */
			i = (int)(sizeof hvac_values / sizeof *hvac_values);
			while (i) {
				i--;
				hvac_values[i].value = saves[i];
			}
			afb_req_fail(request, "error", "CAN error");
		}
	}
	else {
		afb_req_success(request, NULL, "No changes");
	}
}

// TODO: Have to change session management flag to AFB_SESSION_CHECK to use token auth
static const struct afb_verb_desc_v1 verbs[]= {
	{"get_temp_left_zone"	 , AFB_SESSION_NONE, get_temp_left_zone	, "Get the left zone temperature"},
	{"get_temp_right_zone"	 , AFB_SESSION_NONE, get_temp_right_zone	, "Get the right zone temperature"},
	{"get_fanspeed"	 , AFB_SESSION_NONE, get_fanspeed	, "Read fan speed"},
	{"get"	 , AFB_SESSION_NONE, get	, "Read all values"},
	{"set"	 , AFB_SESSION_NONE, set	, "Set a HVAC component value"},
	{NULL}
};

static const struct afb_binding binding_desc = {
	.type = AFB_BINDING_VERSION_1,
	.v1 = {
		.info = "hvac service",
		.prefix = "hvac",
		.verbs = verbs
	}
};

const struct afb_binding *afbBindingV1Register (const struct afb_binding_interface *itf)
{
	interface = itf;

	return &binding_desc;
}

int afbBindingV1ServiceInit(struct afb_service service)
{
	return open_can_dev();
}
