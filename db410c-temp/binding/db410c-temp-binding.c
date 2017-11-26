#define _GNU_SOURCE
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <json-c/json.h>

#include <afb/afb-binding.h>

#define ADDR 0x41

int fd;
char *i2c_bus = "/dev/i2c-0";

const struct afb_binding_interface *interface;

static void get_temp(struct afb_req request)
{
	char reg[1] = {0x03};
	char data[2] = {0};
	char temp_data[5];
	int tmp;
	float val = 0.0;

	/* set temp register address */
	write(fd, reg, 1);

	/* read raw temp data */
	if(read(fd, data, 2) != 2) {
		AFB_ERROR(interface, "read failed");
		afb_req_fail(request, "failed", "Read failed");
	} else {
		/* convert the data to 14-bits */
		tmp = (data[0] * 256 + (data[1] & 0xFC)) / 4;
		if(tmp > 8191)
		{
			tmp -= 16384;
		}
	
		/* convert to celsius */
		val = tmp * 0.03125;

		/* convert temp data to readable format and return it */
		snprintf(temp_data, 4, "%f", val);
		afb_req_success(request, NULL, temp_data);
	}
}

static const struct afb_verb_desc_v1 verbs[]= {
  {"temp", AFB_SESSION_NONE, get_temp , "returns the temperature data"},
  {NULL}
};

static const struct afb_binding binding_desc = {
	.type = AFB_BINDING_VERSION_1,
	.v1 = {
		.info = "db410c temperature service",
		.prefix = "db410c",
		.verbs = verbs
	}
};

static int temp_init(void)
{
	char config[3] = {0};
	
	if ((fd = open(i2c_bus, O_RDWR)) < 0) { 
		AFB_ERROR(interface, "Failed to open the bus\n");
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE, ADDR) < 0) {
		AFB_ERROR(interface, "ioctl failed");
		return -1;
	}

	/* 
	 * Select configuration register(0x02)
	 * Set Continuous conversion, comparator mode(0x15, 0x40)
	 *
	 */
	config[0] = 0x02;
	config[1] = 0x15;
	config[2] = 0x40;
	
	write(fd, config, 3);
	
	return 0;
}

const struct afb_binding *afbBindingV1Register (const struct afb_binding_interface *itf)
{
	        interface = itf;

		return &binding_desc;
}

int afbBindingV1ServiceInit(struct afb_service service)
{
	        return temp_init();
}

