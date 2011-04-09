#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"

int
main()
{
	hash_table *table;
	hash_node *node;
	char *name, *school;
	int age, tall;

	if (NULL == (table = hash_create ())){
		printf ("Error creating hash table.\n");
		return -1;
	};

	node = hash_ins_str (table, "AD", "Mike");
	hash_ins_str (table, "BC", "Fudan");
	hash_ins_int (table, "Age", 23);
	hash_ins_int (table, "Tall", 176);
	if (NULL ==  hash_ins_int (table, "Tall", 176))
		printf("Re-insert key \"Tall\" unsuccessful.\n");
	if (NULL ==  hash_ins_int (table, "AD", 176))
		printf("Re-insert key \"AD\" unsuccessful.\n");

	name = malloc (sizeof (char) * 10);
	school = malloc (sizeof (char) * 10);
	hash_get_str (table, "AD", name);
	hash_get_str (table, "BC", school);
	hash_get_int (table, "Age", &age);
	hash_get_int (table, "Tall", &tall);

	printf ("Hello! My name is %s.\nI am %d years old.\nI am a student in %s.\nI am %dcm tall.\n",
		name, age, school, tall
	);

	hash_rm (table, "BC");
	hash_rm (table, "Tall");
	if (0 != hash_get_str (table, "Name", name))
		printf ("Hash value \"School\" not found.\n");
	if (0 != hash_get_int (table, "Age", &age))
		printf ("Hash value \"Tall\" not found.\n");
	
	hash_free (table);

	printf("**********\nThe system should print garbage\nbecause the hash table has been freed already,\nso the keys cannot be found.\n**********\n");

	printf("%s\n", node->val.val_str);
	return 0;
}
