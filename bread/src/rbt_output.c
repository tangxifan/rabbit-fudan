/*
rbt_output.c
@Author: Mike Zou <zxhmike@gmail.com>
*/
#ifndef __RBT_OUTPUT_H__
#define __RBT_OUTPUT_H__

#include <stdio.h>
#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>
#include <string.h>
#include <rbt_parser.h>
#include <stdlib.h>

extern t_vnet *vnets;
extern int vnets_length;

extern t_icdev *devices;
extern int devices_length;

extern t_pr_marco *marcos;
extern int marcos_length;

extern t_bb_array bb_array;

typedef struct s_output_pin{
	int x;
	int y;
	char *pin_str;
}t_output_pin, *t_output_pin_ptr;


/*
typedef output_record:
Record the device and the pins
*/
typedef struct s_output_record{
	char* device_name;
	int pin_num;
	t_output_pin_ptr pins;
}t_output_record, *t_output_record_ptr;

t_output_record_ptr output_records;

int
rbt_output_record_init()
{
	int i, j;
	if (NULL == (output_records = (t_output_record*) malloc (marcos_length * sizeof (t_output_record))))
		return -1;
	
	for (i = 0; i < marcos_length; i++){
		/* Skip if POWER or GND */
		if (
			marcos[i].type == GND ||
			marcos[i].type == VDD 
			)
			continue;

		if (NULL == (output_records[i].device_name = (char*) malloc (100 * sizeof (char))))
			return -1;
		strcpy (output_records[i].device_name, marcos[i].device->name);
		output_records[i].pin_num = marcos[i].device->pin_num;
		if (NULL == (output_records[i].pins = (t_output_pin*) malloc (output_records[i].pin_num * sizeof (t_output_pin))))
			return -1;
		
		for (j = 0; j < output_records[i].pin_num; j++){
			if (NULL == (output_records[i].pins[j].pin_str = (char*)malloc (10 * sizeof (char))))
				return -1;
        }
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
 * function rbt_gen_pin_str
 * Generate the pin expression that Qt can understand
 */
int
rbt_gen_pin_str
(char* pin_str, int x, int y)
{
	char* line_nos = "XY**ABCDE**FGHIJ**VW";

	sprintf (pin_str, "%d%c%d", y/18, line_nos[y%18], y);
	return 0;
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
	int i, j;
	FILE *fp;

	t_output_record *record_cur;
	t_output_pin_ptr pin_cur;

	for (x = 0;x < bb_array.height; x++)
		for (y = 0; y < bb_array.width; y++){
			/* Skip if nothing in a node */
			if (
				bb_array.bb_node[x][y].type == BLANK ||
				bb_array.bb_node[x][y].status == FREE
			)
				continue;

			/* Record the device and pins */
			if (NULL == (record_cur = rbt_find_output_record (bb_array.bb_node[x][y].pin->parent->device->name))){
				return -2;
			}

			pin_cur = &record_cur->pins[bb_array.bb_node[x][y].pin->pin_no];
			pin_cur->x = x;
			pin_cur->y = y;
			rbt_gen_pin_str(pin_cur->pin_str, x, y);

		}


	/* Output */
	if (NULL == (fp = fopen (output_file, "w")))
		return -2;

	for (i = 0; i < marcos_length; i++){
		fprintf (fp, "%s", output_records[i].device_name);
		for (j = 0; j < output_records[i].pin_num; j++)
			fprintf(fp, " %s", output_records[i].pins[j].pin_str);
		fprintf(fp, "\n");
	}
	close (fp);
	return 0;
}

#endif
