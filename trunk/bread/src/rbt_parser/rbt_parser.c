/*
rbt_paser.c

Mike Zou <zxhmike@gmail.com>

Parse the electronics parts files in xml format.

Copyright (C) 2011

Rabbit Project of Fudan University

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <string.h>
#include <search.h>

#include "rbt_parser.h"
#define MAX_DEVICE_NUMBER 100
#define MAX_DEVICE_NAME 50


t_vnet **rbt_vnets;
int rbt_vnets_length;

t_icdev **rbt_devices;
int rbt_devices_length;

/*
 * XML Unitlity: getdoc
 * From official LibXML tutorial
 */
xmlDocPtr
getdoc (char *docname)
{
	xmlDocPtr doc;
	doc = xmlParseFile(docname);
	if (doc == NULL)
		return NULL;
	return doc;
}

/*
 * XML Utility: getnode
 * From official LibXML tutorial
 */
xmlXPathObjectPtr
getnodeset (xmlDocPtr doc, xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(doc);
	if (context == NULL)
		return NULL;

	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL) 
		return NULL;

	if (xmlXPathNodeSetIsEmpty (result->nodesetval)){
		xmlXPathFreeObject(result);
		return NULL;
	}

	return result;
}

/*
 * function rbt_parse_connector
 * parse the <connector> </connector> tag within <net> </net>
 */
int
rbt_parse_connector (xmlNodePtr connector, xmlChar *pin_id)
{
	xmlNodePtr cur;
	xmlChar *label;
	xmlChar *title;
	xmlChar *id;

	for (cur = connector->xmlChildrenNode; cur != NULL; cur = cur->next){
		if ( !xmlStrcmp ((cur->name), (const xmlChar*) "part" )){
			label = xmlGetProp (cur, (const xmlChar*) "label");
			title = xmlGetProp (cur, (const xmlChar*) "title");
			id = xmlGetProp (cur, (const xmlChar*) "id");
			/* DEBUG */
			printf ("%s %s %s %s\n", label, title, id, pin_id);
			xmlFree (label);
			xmlFree (title);
			xmlFree (id);
		}
	}
	
	return 0;

}

/**
 * function rbt_parse_net
 * parse the <net> </net> tag in a netlist
 */
int
rbt_parse_net (xmlNodePtr net)
{
	xmlNodePtr cur;
	xmlChar* pin_id;

	for (cur = net->xmlChildrenNode; cur != NULL; cur = cur->next){
		if ( !xmlStrcmp ((cur->name), (const xmlChar*) "connector")){
			pin_id = xmlGetProp (cur, (const xmlChar*) "name");
			rbt_parse_connector (cur, pin_id);	
			xmlFree (pin_id);
		}
	}
	return 0;
}

/**
 * function rbt_find_device
 * find a device in a t_icdev array
 */
t_icdev*
rbt_find_device (t_icdev** devs, int length, char* device_name)
{
	int count;

	for (count = 0; count < length; count++){
		if ( !strcmp (devs[count]->name, device_name) )
			return devs[count];
	}

	return NULL;
}

/*
 * function rbt_config_device
 * fill the parameters in the t_icdev
 */
int
rbt_config_device (t_icdev* cur, char *device_name)
{
	/* DEBUG: not fully implemented */
	cur->name = malloc (sizeof (char) * ( strlen (device_name) +1));
	if (cur->name == NULL) return -1;
	strcpy (cur->name, device_name);

	return 0;
}

/*
 * function rbt_ins_device
 * insert a device into a t_icdev array
 */
int
rbt_ins_device (t_icdev** dev, int *length, char *device_name)
{
	t_icdev *cur;
	cur = (t_icdev*) malloc (sizeof (t_icdev));
	if (cur == NULL) return -1;

	dev[*length] = cur;
	(*length) = (*length) + 1;
	rbt_config_device (cur, device_name);

	printf ("Device inserted: %s\n", device_name);
	return 0;
}

/**
 * function rbt_parse_init
 * count the device number and init the devices array
 * init the vnets array
 */
int
rbt_parse_init(char *docname)
{	
	xmlDocPtr doc;
	xmlChar *xpath = (xmlChar*) "//part";
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	int device_count;
	ENTRY entry0;
	char *temp_string;
	
	int i;
		
	if (NULL == (doc = getdoc(docname)))
		return -1;

	result = getnodeset (doc, xpath);
	if (result == NULL)
		return -2;

	nodeset = result->nodesetval;

	if (!hcreate(53))
		return -3;

	device_count = 0;
	for (i=0; i < nodeset->nodeNr; i++) {
		/* hash them and count how many devices */
		/* write to device_count */
		entry0.key = (char*) xmlGetProp (nodeset->nodeTab[i], (xmlChar*) "title");
		if (NULL == hsearch (entry0, FIND)){
			hsearch (entry0, ENTER);
			device_count++;
		}
		xmlFree (entry0.key);
	}

	printf ("device count: %d\n", device_count);

	/* Init rbt_devices */
	rbt_devices = (t_icdev**) malloc(device_count * sizeof(t_icdev*));
	if (rbt_devices == NULL)
		return -4;

	/* Second parse, create devices array*/
	rbt_devices_length = 0;

	for (i=0; i < nodeset->nodeNr; i++) {
		temp_string = (char*) xmlGetProp (nodeset->nodeTab[i], (xmlChar*) "title");
		if (NULL == rbt_find_device (rbt_devices, rbt_devices_length, temp_string)){
			rbt_ins_device (rbt_devices, &rbt_devices_length, temp_string);
		}
		xmlFree (temp_string);
	}

	/* For things in hash, build devices */

	/* Initialize rbt_vnets */
	rbt_vnets = (t_vnet**) malloc(nodeset->nodeNr * sizeof(t_vnet*));
	if (rbt_vnets == NULL)
		return -5;

	/* Third parse. Fill in rbt_vnets */
	for (i=0; i < nodeset->nodeNr; i++) {
			
	}

	xmlXPathFreeObject (result);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	

	/* Destroy hash */
	hdestroy();
	return 0;
}

/**
 * function rbt_parse_netlist
 * Parse the Fritzing netlist xml
 */
int
rbt_parse_netlist (char* docname)
{
	xmlDocPtr doc;
	xmlChar *xpath = (xmlChar*) "//net";
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	int i;
		
	if (NULL == (doc = getdoc(docname)))
		return -1;

	result = getnodeset (doc, xpath);
	if (result == NULL)
		return -2;

	nodeset = result->nodesetval;

	if (rbt_parse_init(docname)) return -3;

	for (i=0; i < nodeset->nodeNr; i++) {
		rbt_parse_net (nodeset->nodeTab[i]);
	}

	xmlXPathFreeObject (result);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return (1);
}

/*
 * chop of '\n' at the end of a string
 * return 0 if '\n' found and chopped,
 * -1 if not found.
 */
int
chomp (char* str_in)
{
	if (strlen (str_in) < 2)
		return -1;

	if ('\n' != str_in[strlen (str_in) -1]){
		return -2;
	}

	str_in[strlen (str_in) -1] = '\0';
	return 0;
}

/**
 * Find the xml part describtion file accoring to the device name
 */
char*
rbt_find_device_file (char* list_file, char* device_name)
{
	FILE* input_file;
	char *device;
	char *file;
	char *buf;

	if (NULL == (input_file = fopen(list_file, "r"))){
		return NULL;
	}

	if (NULL == (buf = malloc (sizeof (char) * MAX_FILENAME_LENGTH))){
		free (buf);
		fclose (input_file);
		return NULL;
	}

	while (NULL != fgets (buf, BUF_SIZE, input_file)){
		device = strtok (buf, "*");
		file = strtok (NULL, "*");
		if (!strcmp (device_name, device)){
			fclose (input_file);
			chomp (file);
			return file;
		}
	}

	free (buf);
	fclose (input_file);
	return NULL;
}
