/*
rbt_paser.h

Mike Zou <zxhmike@gmail.com>

Header file for rbt_parser.c

Copyright (C) 2011

Rabbit Project of Fudan University

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "rabbit_type.h"
#include "place_route.h"
#include "device.h"

/* Read the device list "device.lst" to find out
   which file is to describe the certain part. */
char* rbt_find_device_file (char*, char*);

/* Parse the electronic parts in the netlist. */
int rbt_parse_netlist (char*, char*, char*);
