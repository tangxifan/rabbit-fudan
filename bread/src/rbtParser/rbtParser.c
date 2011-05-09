/*
rbtPaser.c

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

#include "rbtParser.h"

/*
 * XML Unitlity: getdoc
 * From official LibXML tutorial
 */
xmlDocPtr
getdoc (char *docname)
{
	xmlDocPtr doc;
	doc = xmlParseFile(docname);
	if (doc == NULL )
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
 * function rbtParseNetlist
 * Parse the Fritzing netlist xml
 */
int
rbtParseNetlist (char* docname)
{
	xmlDocPtr doc;
	xmlChar *xpath = (xmlChar*) "//net";
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	int i;
		
	if (NULL == doc = getdoc(docname))
		return -1;

	result = getnodeset (doc, xpath);
	if (result == NULL)
		return -2;
	nodeset = result->nodesetval;

	for (i=0; i < nodeset->nodeNr; i++) {
		printf("%s\n",
			xmlGetProp(nodeset->nodeTab[i],(xmlChar*) "id"));
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

	if ('\n' != str_in[strlen (str_in) -2]){
		return -2;
	}

	str_in[strlen (str_in) -2] = '\0';
	return 0;
}

/**
 * Find the xml part describtion file accoring to the device name
 */
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
			return file;
		}
	}

	free (buf);
	fclose (input_file);
	return NULL;
}
