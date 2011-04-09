/*
hash_table.h: The header file of hash table
Copyright (C) <2011> Rabbit Project of Fudan University
Author: Mike Zou <zxhmike@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdlib.h>
#define HASH_PRIME 1 
#define hash_table hash_node*

typedef enum hash_node_type_e{
	HASH_NODE_TYPE_INT = 0,
	HASH_NODE_TYPE_STR
}hash_node_type;

typedef struct hash_node_s{
	char* key;
	hash_node_type type;
	
	union{
		int val_int;
		char* val_str;
	}val;

	struct hash_node_s *next;
}hash_node;

hash_table* hash_create ();
void hash_free (hash_table*);
hash_node* hash_ins_str (hash_table*, const char*, const char*);
hash_node* hash_ins_int (hash_table*, const char*, const int);
int hash_rm (hash_table*, const char*);
hash_node* hash_find (hash_table*, const char*);
int hash_get_str (hash_table*, const char*, char*);
int hash_get_int (hash_table*, const char*, int*);
