/*
rbt_output.c
@Author: Mike Zou <zxhmike@gmail.com>
*/
#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>

extern t_vnet *vnets;
extern int vnets_length;

extern t_icdev *devices;
extern int devices_length;

extern t_pr_marco *marcos;
extern int marcos_length;

extern t_bb_array bb_array;

t_output_record *output_records;

typedef s_output_pin{
	int x;
	int y;
	char *x_str;
	char *y_str;
}t_output_pin;

/*
typedef output_record:
Record the device and the pins
*/
typedef struct s_output_record{
	char* device_name;
	int pin_num;
	output_pin *pins;
}t_output_record;

int
rbt_output_record_init()
{
	int i;
	if (NULL == (output_records = (rbt_output_record*) malloc (marcos_length * sizeof (rbt_output_record))))
		return -1;
	
	for (i = 0; i < marcos_length; i++){
		/* Skip if POWER or GND */
		if (
			marcos[i].type == GND ||
			marcos[i].type == POWER
			)
			continue;

		if (NULL == (output_records[i].device_name = (char*) malloc (100 * sizeof (char))))
			return -1;
		strcpy (output_records[i].device_name, marcos[i].device->name);
		output_records[i].pin_num = marcos[i].device->pin_num;
		if (NULL == (output_records[i].pins = (output_pin*) malloc (output_records[i].pin_num * sizeof (output_pin))))
			return -1;
	}
}

t_output_record*
rbt_find_output_record
(char* device_to_find)
{
	int i;
	for (i = 0; i < marcos_length; i++){
		if (!strcmp (output_records[i].device_name, device_to_find))
			return &output_records[i];
	}
	return NULL;
}


/**
 * function rbt_output
 * Output the array information to a file for Qt use
 */
int
rbt_output
(char* output_file)
{
	int x, y;

	for (x = 0;x < bb_array.height; x++)
		for (y = 0; y < bb_array.width; y++){
			/* Skip if nothing in a node */
			if (
				bb_array.bb_node.type == BLANK ||
				bb_array.bb_node.status == FREE
			)
				continue;

			/* Record the device and pins */

		}
	return 0;
}
