#include <stdio.h>
#include <stdlib.h>
#include "RBT_parser.h"

int main()
{
	char *filename;
	if (NULL == (filename = malloc (1000 * sizeof(char)))){
		return -1;
	}

	if (NULL == (filename = RBT_find_device_file ("/home/mike/Projects/rabbit/parts/parts.lst", "10 Ω Resistor"))){
		printf ("Error finding device.\n");
		return -2;
	}

	printf ("Device filename: %s\n", filename);

	return 0;
}
