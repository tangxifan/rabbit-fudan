/*
RBT_paser.c

Mike Zou <zxhmike@gmail.com>

Parse the electronics parts files in xml format.

Copyright (C) 2011

Rabbit Project of Fudan University

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <stdio.h>
#include <string.h>

#include "RBT_parser.h"

#define MAX_DEVICE_LENGTH 100
#define MAX_FILENAME_LENGTH 1000
#define BUF_SIZE 200

char*
RBT_find_device_file (char* list_file, char* device_name)
{
	FILE* input_file;
	char *device;
	char *file;
	char *buf;

	if (NULL == (input_file = fopen(list_file, "r"))){
		return NULL;
	}

	if (NULL == (device = malloc (sizeof (char) * MAX_DEVICE_LENGTH))){
		free (device);
		fclose (input_file);
		return NULL;
	}

	if (NULL == (file = malloc (sizeof (char) * MAX_FILENAME_LENGTH))){
		free (device);
		fclose (input_file);
		return NULL;
	}

	if (NULL == (buf = malloc (sizeof (char) * MAX_FILENAME_LENGTH))){
		free (device);
		free (buf);
		fclose (input_file);
		return NULL;
	}

	while (NULL != fgets (buf, BUF_SIZE, input_file)){
		printf ("%s", buf);
		device = strtok (buf, "*");
		file = strtok (NULL, "*");
		printf ("%s!!!%s\n", device, file);
		if (!strcmp (device_name, device)){
			free (device);
			free (buf);
			fclose (input_file);
			return file;
		}
	}

	free (device);
	free (file);
	free (buf);
	fclose (input_file);
	return NULL;
}

t_net_info*
RBT_parse_netlist (char* filename){
	return NULL;	
}
