/*
hash_table.c: The utility of hash table 
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

#include <stdio.h>
#include <string.h>
#include "hash_table.h"

/* internal functions */
unsigned int 
hash_get_hash (const char* key)
{
	int sum = 0;
	const char *ptr;
	
	for (ptr = key; (*ptr) != '\0'; ptr++)
		sum += (int)(*ptr);
	
	return (sum % HASH_PRIME);
}

void
hash_ins_node (hash_table *table, hash_node* node)
{
	int index;

	index = hash_get_hash (node->key);
	node->next = table[index]->next;
	table[index]->next = node;
}

hash_node*
hash_find_prev (hash_table* table, const char* key)
{
	int index;
	hash_node *ptr;

	index = hash_get_hash (key);
	for (ptr = table[index]; ptr->next != NULL; ptr = ptr->next){
		if (!strcmp (key, ptr->next->key))
			return ptr;
	}

	return NULL;
}

/* external functions */
hash_node*
hash_find (hash_table* table, const char* key)
{
	int index;
	hash_node *ptr;

	index = hash_get_hash (key);
	for (ptr = table[index]->next; ptr != NULL; ptr = ptr->next){
		if (!strcmp (key, ptr->key))
			return ptr;
	}

	return NULL;
}

hash_table*
hash_create ()
{
	hash_table* table;
	int i;

	table = (hash_node**)malloc (HASH_PRIME * sizeof (hash_node*));
	for (i=0; i < HASH_PRIME; i++){
		if (NULL == (table[i] = malloc (sizeof (hash_node))))
			return NULL;
		table[i]->next = NULL;
	}

	return table;
}

void
hash_free (hash_table* table)
{
	hash_node *ptr, *to_del;
	int i;

	for (i = 0; i < HASH_PRIME; i++)
		for (ptr = table[i]; ptr != NULL; ){
			to_del = ptr;
			ptr = ptr->next;

			if (HASH_NODE_TYPE_STR == to_del->type)
				free (to_del->val.val_str);
			free (to_del->key);
			free (to_del);
		}
	free (table);
}

hash_node*
hash_ins_str (hash_table* table, const char* key, const char* ins_str)
{
	hash_node *node;
	if (NULL != hash_find (table, key))
		return NULL;

	if (NULL == (node = malloc (sizeof (hash_node))))
		return NULL;

	if (NULL == (node->key = malloc (sizeof(char) * (strlen (key) +1))))
		return NULL;

	if (NULL == (node->val.val_str = malloc (sizeof(char) * (strlen (ins_str) + 1))))
		return NULL;

	strcpy (node->key, key);
	strcpy (node->val.val_str, ins_str);
	node->type = HASH_NODE_TYPE_STR;
	hash_ins_node (table, node);
	return node;
}

hash_node*
hash_ins_int (hash_table* table, const char* key, const int ins_int)
{
	hash_node *node;
	if (NULL != hash_find (table, key))
		return NULL;

	if (NULL == (node = malloc (sizeof (hash_node))))
		return NULL;

	if (NULL == (node->key = malloc (sizeof(char) * (strlen (key) + 1))))
		return NULL;

	strcpy (node->key, key);
	node->val.val_int = ins_int; 
	node->type = HASH_NODE_TYPE_INT;
	hash_ins_node (table, node);
	return node;
}

int
hash_rm (hash_table* table, const char* key)
{
	hash_node *node, *prev_node;
	if (NULL != (prev_node = hash_find_prev (table, key))){
		
		node = prev_node->next;
		prev_node->next = node->next;

		if (HASH_NODE_TYPE_STR == node->type)
			free (node->val.val_str);
		free (node->key);
		free (node);
		return 0;
	}else{
		return -1;
	}
}

int
hash_get_str (hash_table* table, const char* key, char* val)
{
	hash_node *node;
	if (NULL == (node = hash_find (table, key)))
		return -1;

	strcpy (val, node->val.val_str);
	return 0;
}

int
hash_get_int (hash_table* table, const char* key, int *val)
{
	hash_node *node;
	if (NULL == (node = hash_find (table, key)))
		return -1;

	*val	= node->val.val_int;
	return 0;
}
