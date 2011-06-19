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

#include "rabbit_type.h"
#include "place_route.h"
#include "device.h"

/* for external use*/
t_vnet *vnets;
int vnets_length;

t_icdev *devices;
int devices_length;

t_pr_marco *marcos;
int marcos_length;

/* Rabbit arrays */
t_vnet **rbt_vnets;
int rbt_vnets_length;

t_icdev **rbt_devices;
int rbt_devices_length;

t_pr_marco **rbt_marcos;
int rbt_marcos_length;

char *part_list;
char *part_dir;

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

/**
 * connector10 => 10
 */
int
rbt_convert_connector_to_int
(char* connector_id)
{
	//DEBUG
	printf ("convert0: %s\n", &connector_id[0]);
	printf ("convert1: %s\n", &connector_id[9]);
	printf ("convert2: %d\n", atoi(&connector_id[9]));
	return atoi(&connector_id[9]);
}
/*
 * function rbt_convert_pin_to_int
 * remove string "pin" of a pin_id and convert the pin number to int
 */
int
rbt_convert_pin_to_int (char* pin_id)
{
	char *id_string;
	int id_int;

	if (NULL == (id_string = malloc (10 * sizeof (char))))
		return -1;
	
	strcpy (id_string, pin_id);
	strtok (id_string, " ");
	id_string = strtok (NULL, " ");

	if (NULL == id_string)
		return -1;

	id_int = atoi (id_string);
	return id_int;
}

/*
 * function rbt_find_marco
 * find a marco in a t_pr_marco array
 */
t_pr_marco*
rbt_find_marco (int name)
{
	int count;

	for (count = 0; count < rbt_marcos_length; count++){
		if (rbt_marcos[count]->name == name)
			return rbt_marcos[count];
	}

	return NULL;
}


/*
 * function rbt_parse_connector
 * parse the <connector> </connector> tag within <net> </net>
 */
int
rbt_parse_connector (xmlNodePtr connector, xmlChar *pin_id, int pins_count, t_vnet *vnet_cur)
{
	xmlNodePtr cur;
	xmlChar *label;
	xmlChar	*title;
	xmlChar *name;
	xmlChar *id;
	xmlChar *connector_id;
	t_pr_pin *pin_cur;
	int pin_num;
	char *docname;
	xmlXPathObjectPtr result;
	xmlDocPtr doc;
	xmlNodeSetPtr nodeset;
	int i;
	char *filename;
	if (NULL == (filename = (char*) malloc (MAX_FILENAME_LENGTH * sizeof(char))))
		return -1;

	if (NULL == (docname = (char*)malloc (sizeof (char) * MAX_FILENAME_LENGTH)))
		return -1;
	
	if (NULL == (pin_cur = (t_pr_pin*)malloc (sizeof (t_pr_pin))))
		return -1;

	for (cur = connector->xmlChildrenNode; cur != NULL; cur = cur->next){
		if ( !xmlStrcmp ((cur->name), (const xmlChar*) "part" )){
			label = xmlGetProp (cur, (const xmlChar*) "label");
			title = xmlGetProp (cur, (const xmlChar*) "title");
			id = xmlGetProp (cur, (const xmlChar*) "id");

			/* Main Parse: fill in other data */
			if (NULL == (pin_cur->parent = rbt_find_marco (atoi ((char*)id)))){
				return -2;
			}

			pin_cur->numnet = 1;
			pin_cur->nets = vnet_cur;
			strcpy (pin_cur->parent->label, (char*)label);

			/* Fill loc & offset */
			/* If marco, do extra convert work */
			if (pin_cur->parent->type == ICBLOCK){
				if (NULL == (filename = rbt_find_device_file (part_list, pin_cur->parent->device->name)))
					return -2;
				strcpy (docname, part_dir);
				strcat (docname, filename);

				/* Open the fzp file and parse, fill in struct icdev */
				if (NULL == (doc = getdoc(docname)))
					return -1;

				result = getnodeset (doc, (xmlChar*) "//connector");
				if (result) {
					nodeset = result->nodesetval;
					for (i=0; i < nodeset->nodeNr; i++) {
						connector_id = xmlGetProp (nodeset->nodeTab[i], (const xmlChar*) "id");
						name = xmlGetProp (nodeset->nodeTab[i], (const xmlChar*) "name");
						printf ("connector_id = %s name = %s\n", connector_id, name);
						if (!strcmp ((char*)pin_id,(char*) name))
							pin_num = rbt_convert_connector_to_int ((char*)connector_id);
					}
				}
			}else{
				pin_num = rbt_convert_pin_to_int ((char*)pin_id);
			}

			if (pin_num != -1){
			printf ("%s pin no %d added.\n", pin_cur->parent->device->name, pin_num);
				pin_cur->parent->pins[pin_num] = pin_cur;

				pin_cur->loc = pin_cur->parent->device->pinls[pin_num].loc;
				pin_cur->offset = pin_cur->parent->device->pinls[pin_num].offset;
				pin_cur->pin_no = pin_num;
			}else{
			}

			vnet_cur->pins[pins_count] = pin_cur;

			xmlFree (id);
			xmlFree (title);
			xmlFree (label);
		}
	}
	return 0;

	xmlXPathFreeObject (result);
	xmlFreeDoc(doc);
	xmlCleanupParser();

}

/**
 * function rbt_ins_vnet
 * Insert a vnet, which is a node in the schematic
 */
t_vnet*
rbt_ins_vnet ()
{
	t_vnet *cur;

	if (NULL == (cur = (t_vnet*) malloc (sizeof (t_vnet))))
		return NULL;
	
	rbt_vnets[rbt_vnets_length] = cur;

	/* Set type to SPECIAL initially. */
	/* If IC blocks found in <connector>, set it to NORMAL then. */
	cur->type = SPECIAL;

	rbt_vnets_length++;
	return cur;
}

/**
 * function rbt_ins_marco
 */
t_pr_marco*
rbt_ins_marco (int name)
{
	t_pr_marco *cur;

	if (NULL == (cur = (t_pr_marco*) malloc (sizeof (t_pr_marco))))
		return NULL;

	rbt_marcos[rbt_marcos_length] = cur;
	cur->name = name;

	/* Initialize the label */
	if (NULL == (cur->label = (char*) malloc (sizeof (char) * MAX_LABEL_LENGTH)))
		return NULL;

	rbt_marcos_length++;
	return cur;
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
	t_vnet *vnet_cur;
	int pins_count;

	/* First parse: initialize the vnet*/
	/* fill in numpin and create memory for pins. */
	pins_count = 0;
	for (cur = net->xmlChildrenNode; cur != NULL; cur = cur->next){
		if ( !xmlStrcmp ((cur->name), (const xmlChar*) "connector"))
			pins_count++;
	}

	
	if (NULL == (vnet_cur = rbt_ins_vnet ()))
		return -2;

	vnet_cur->numpin = pins_count;

	if (NULL == (vnet_cur->pins = (t_pr_pin**) malloc (pins_count * sizeof (t_pr_pin*))))
		return -1;
	
	/* Second parse: fill in the marcos */
	for (pins_count = 0, cur = net->xmlChildrenNode; cur != NULL; cur = cur->next){
		if ( !xmlStrcmp ((cur->name), (const xmlChar*) "connector")){
			pin_id = xmlGetProp (cur, (const xmlChar*) "name");
			rbt_parse_connector (cur, pin_id, pins_count, vnet_cur);	
			pins_count++;
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
rbt_find_device (char* device_name)
{
	int count;

	for (count = 0; count < rbt_devices_length; count++){
		if ( !strcmp (rbt_devices[count]->name, device_name) )
			return rbt_devices[count];
	}

	return NULL;
}

/* function rbt_config_ic
 */
int
rbt_config_ic (t_icdev* cur, xmlDocPtr doc)
{
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	int i;

	result = getnodeset (doc, (xmlChar*)"//connector");
	if (!result) 
		return -2;
	
	nodeset = result->nodesetval;

	cur->height = 2;
	cur->width = nodeset->nodeNr / 2;
	cur->area = cur->height * cur->width;
	cur->pin_num = nodeset->nodeNr;

	/* Deal with pins */
	if (NULL == (cur->pinls = (t_dev_pin*)malloc (cur->pin_num * sizeof (t_dev_pin))))
		return -1;

	for (i=0; i < cur->pin_num; i++){
		if (i < cur->pin_num / 2){ /* lower level */
			cur->pinls[i].loc = BOTTOM;
			cur->pinls[i].offset = i;
			cur->pinls[i].index = i;
		}else{ /* upper level */
			cur->pinls[i].loc = TOP;
			cur->pinls[i].offset = i - (cur->pin_num / 2);
			cur->pinls[i].index = i;
		}
	}

	return 0;	
}

/*
 * function rbt_config_scalable
 */
int
rbt_config_scalable (t_icdev* cur, char* type)
{
	int i;

	cur->pin_num = 2;
	/* Deal with pins */
	if (NULL == (cur->pinls = (t_dev_pin*)malloc (2 * sizeof (t_dev_pin))))
		return -1;
	for (i = 0; i < 2; i++)
			cur->pinls[i].index = i;

	if (!strcmp (type, "Resistor")){
		cur->max_length = RESISTOR_MAX_LENGTH;
		cur->min_length = RESISTOR_MIN_LENGTH;

	}else if (!strcmp (type, "Capacitor")){
		cur->max_length = CAPASITOR_MAX_LENGTH;
		cur->min_length = CAPASITOR_MIN_LENGTH;
	}
	return 0;
}

/*
 * function rbt_config_device
 * fill the parameters in the t_icdev
 */
int
rbt_config_device (t_icdev* cur, char *device_name)
{
	char *docname, *filename;
	xmlDocPtr doc;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	int i;
	xmlChar *keyword;

	cur->name = malloc (sizeof (char) * ( strlen (device_name) +1));
	if (cur->name == NULL) return -1;
	strcpy (cur->name, device_name);
	
	if (NULL == (filename = (char*) malloc (MAX_FILENAME_LENGTH * sizeof(char))))
		return -1;
	if (NULL == (docname = (char*) malloc (MAX_FILENAME_LENGTH * sizeof(char))))
		return -1;
	
	if (NULL == (filename = rbt_find_device_file (part_list, device_name)))
		return -2;
	strcpy (docname, part_dir);
	strcat (docname, filename);

	/* Open the fzp file and parse, fill in struct icdev */

	if (NULL == (doc = getdoc(docname)))
		return -1;

	result = getnodeset (doc, (xmlChar*) "//tag");
	if (result) {
		nodeset = result->nodesetval;
		for (i=0; i < nodeset->nodeNr; i++) {
			keyword = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			if (!strcmp ((char*)keyword, "Resistor") ||
				!strcmp ((char*)keyword, "Capacitor")){
			/* Resistors and Capacitors */
				rbt_config_scalable (cur, (char*) keyword);
				xmlFree (keyword);
				break;
			}else{
				rbt_config_ic (cur, doc);
				xmlFree (keyword);
				break;
			}
		}
		xmlXPathFreeObject (result);
	}

	free (docname);
	xmlFreeDoc(doc);
	xmlCleanupParser();

	return 0;
}

/*
 * function rbt_ins_device
 * insert a device into a t_icdev array
 */
int
rbt_ins_device (char *device_name)
{
	t_icdev *cur;
	cur = (t_icdev*) malloc (sizeof (t_icdev));
	if (cur == NULL) return -1;

	rbt_devices[rbt_devices_length] = cur;
	rbt_devices_length++;
	rbt_config_device (cur, device_name);

	return 0;
}

/*
 * function rbt_set_marco_type
 * Set whether the marco is a VCC or GND or IC or scalable
 * according to <tag></tag>
 */
int
rbt_set_marco_type (t_pr_marco *marco_cur, char *device_name)
{
	char *docname, *filename;
	xmlDocPtr doc;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	int i;
	xmlChar *keyword;

	/* Power and GND */
	if (!strcmp (device_name, "Power")){
		marco_cur->type = VDD;
		marco_cur->pinnum = 0;
		return 0;
	}else if (!strcmp (device_name, "Ground")){
		marco_cur->type = GND;
		marco_cur->pinnum = 0;
		return 0;
	}

	if (NULL == (filename = (char*) malloc (MAX_FILENAME_LENGTH * sizeof(char))))
		return -1;
	if (NULL == (docname = (char*) malloc (MAX_FILENAME_LENGTH * sizeof(char))))
		return -1;
	
	if (NULL == (filename = rbt_find_device_file (part_list, device_name)))
		return -2;
	strcpy (docname, part_dir);
	strcat (docname, filename);

	/* Open the fzp file and parse, fill in struct icdev */

	if (NULL == (doc = getdoc(docname)))
		return -1;

	result = getnodeset (doc, (xmlChar*) "//tag");
	if (result) {
		nodeset = result->nodesetval;
		for (i=0; i < nodeset->nodeNr; i++) {
			keyword = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			if (!strcmp ((char*)keyword, "Resistor") ||
					  !strcmp ((char*)keyword, "Capacitor")){
			/* Resistors and Capacitors */
				marco_cur->type = RCD;
				free (docname);
				xmlFreeDoc(doc);
				xmlCleanupParser();
				return 0;
			}
		}
		xmlXPathFreeObject (result);
	}
	marco_cur->type = ICBLOCK;

	free (docname);
	xmlFreeDoc(doc);
	xmlCleanupParser();

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
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	int device_count;
	int marco_count;
	ENTRY entry0;
	char *temp_string, *temp_string2;
	t_pr_marco *marco_cur;
	t_icdev *dev_cur;
	
	int i;
		
	if (NULL == (doc = getdoc(docname)))
		return -1;

	/* First parse: count device number */
	result = getnodeset (doc, (xmlChar*) "//part");
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

	/* Init rbt_devices */
	rbt_devices = (t_icdev**) malloc(device_count * sizeof(t_icdev*));
	if (rbt_devices == NULL)
		return -4;

	/* Second parse, create devices array*/
	rbt_devices_length = 0;

	for (i=0; i < nodeset->nodeNr; i++) {
		temp_string = (char*) xmlGetProp (nodeset->nodeTab[i], (xmlChar*) "title");
		if (NULL == rbt_find_device ( temp_string)){
			rbt_ins_device (temp_string);
		}
		xmlFree (temp_string);
	}

	/* Initialize rbt_vnets */
	rbt_vnets = (t_vnet**) malloc(nodeset->nodeNr * sizeof(t_vnet*));
	if (rbt_vnets == NULL)
		return -5;

	/* Destroy hash */
	hdestroy();

	/* Third parse. count how many marcos. */
	/* Create a new hash */
	if (!hcreate(53))
		return -3;

	result = getnodeset (doc, (xmlChar*) "//part");
	if (result == NULL)
		return -2;
	nodeset = result->nodesetval;

	marco_count = 0;
	for (i=0; i < nodeset->nodeNr; i++) {
		/* hash them and count how many marco */
		/* write to marco_count */
		entry0.key = (char*) xmlGetProp (nodeset->nodeTab[i], (xmlChar*) "id");
		if (NULL == hsearch (entry0, FIND)){
			hsearch (entry0, ENTER);
			marco_count++;
		}
		xmlFree (entry0.key);
	}

	/* Initialize rbt_marcos */
	if (NULL == (rbt_marcos = (t_pr_marco**) malloc (sizeof (t_pr_marco*) * marco_count)))
		return -1;
	
	/* Fourth Parse: insert rbt_marcos */
	rbt_marcos_length= 0;

	for (i=0; i < nodeset->nodeNr; i++) {
		temp_string = (char*) xmlGetProp (nodeset->nodeTab[i], (xmlChar*) "id");
		temp_string2 = (char*) xmlGetProp (nodeset->nodeTab[i], (xmlChar*) "title");
		if (NULL == rbt_find_marco (atoi (temp_string))){
			marco_cur = rbt_ins_marco (atoi (temp_string));

			if ((dev_cur = rbt_find_device (temp_string2)) == NULL)
				return -5;
			marco_cur->pinnum = dev_cur->pin_num;

			marco_cur->device = dev_cur;

			rbt_set_marco_type (marco_cur, (char*) temp_string2);

			/* Initialize marco pins to NULL */
			if (marco_cur->pinnum > 0){
				if (NULL == (marco_cur->pins = (t_pr_pin**) malloc (sizeof(t_pr_pin*) * marco_cur->pinnum)))
					return -1;
			}
			int j;
			for (j = 0; j < marco_cur->pinnum; j++)
				marco_cur->pins[j] = NULL;
		}
		xmlFree (temp_string);
		xmlFree (temp_string2);
	}

	/* Destroy hash */
	hdestroy();

	xmlXPathFreeObject (result);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	
	return 0;
}

/**
 * function rbt_gen_arrays
 * Generate the arrays for external use.
 */
int
rbt_gen_arrays()
{
	int i, j, k;

	vnets_length = rbt_vnets_length;
	devices_length = rbt_devices_length;
	marcos_length = rbt_marcos_length;

	if (NULL == (vnets = (t_vnet*)malloc (vnets_length * sizeof (t_vnet))))
		return -1;
	if (NULL == (devices = (t_icdev*)malloc (devices_length * sizeof (t_icdev))))
		return -1;
	if (NULL == (marcos = (t_pr_marco*)malloc (marcos_length * sizeof (t_pr_marco))))
		return -1;

	for (i = 0; i < vnets_length; i++){
		memcpy (&vnets[i], rbt_vnets[i], sizeof (t_vnet));
	}

	for (i = 0; i < devices_length; i++)
		memcpy (&devices[i], rbt_devices[i], sizeof (t_icdev));

	for (i = 0; i < marcos_length; i++){
		memcpy (&marcos[i], rbt_marcos[i], sizeof (t_pr_marco));
		
		/* Change the vnets */
		for (j = 0; j < marcos[i].pinnum; j++){
			for (k = 0; k < rbt_vnets_length; k++){
				if (marcos[i].pins[j] == NULL) continue;
				if (rbt_vnets[k] == marcos[i].pins[j]->nets){
					marcos[i].pins[j]->nets = &vnets[k];
				}
			}
		}
	}

	return 0;
}

/**
 * function rbt_parse_netlist
 * Parse the Fritzing netlist xml
 */
int
rbt_parse_netlist
(char* docname, char* in_part_list, char* in_part_dir)
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

	if (NULL == (part_list = (char*) malloc (sizeof(char) * MAX_FILENAME_LENGTH)))
		return -1;

	if (NULL == (part_dir = (char*) malloc (sizeof(char) * MAX_FILENAME_LENGTH)))
		return -1;

	strcpy (part_list, in_part_list);
	strcpy (part_dir, in_part_dir);

	if (rbt_parse_init(docname)) return -3;

	/* Fifth parse. Fill in rbt_vnets */
	for (i=0; i < nodeset->nodeNr; i++) {
		// DEBUG
		printf ("Now parsing net #%d.\n", i);
		rbt_parse_net (nodeset->nodeTab[i]);
	}

	/* Generate the arrays for output */
	rbt_gen_arrays();

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
