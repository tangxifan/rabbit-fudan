#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbt_parser.h"

int main()
{
	char *filename;
	char *temp;
	char *author;

	if (NULL == (filename = malloc (MAX_FILENAME_LENGTH * sizeof(char)))){
		return -1;
	}

	if (NULL == (author = malloc (BUF_SIZE * sizeof(char)))){
		return -1;
	}

	if (NULL == (temp = malloc (BUF_SIZE * sizeof(char)))){
		return -1;
	}

	if (NULL == (filename = rbt_find_device_file ("../../../../parts/parts.lst", "10 Ω Resistor"))){
		printf ("Error finding device.\n");
		return -2;
	}
	
	printf ("Filename found: %s\n", filename);

	strcpy (temp, "../../../../../parts/parts/");
	strcat (temp, filename);
	strcpy (filename, temp);

	rbt_parse_netlist("simple_netlist.xml");	
	return 0;
}
