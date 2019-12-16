//
// Created by nik on 12/16/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct list {
	void* data;
	uint size;
	uint cap;
	uint len;
} list_t;


typedef struct gnode {
	char name[4];
	struct gnode* parent;
	list_t* childlist;
} gnode_t;

struct list* list_new(uint size) {
	struct list* newlist = calloc(1, sizeof(struct list));
	newlist->cap = 32;
	newlist->data = calloc(newlist->cap, size);
	newlist->size = size;
	newlist->len = 0;
}

uint list_size(list_t* list) {
	return list->len;
}

void list_add(struct list* list, void* data) {
	if (list->len == list->cap) {
		list->cap *= 2;
		list->data = reallocarray(list->data, list->cap * list->size, list->size);
	}
	memcpy(list->data + list->len * list->size, data, list->size);
	list->len++;
}

void* list_get(struct list* list, uint index) {
	if (index >= list->len) {
		return NULL;
	}
	return list->data + index * list->size;
}

void* list_get_name(list_t* list, char const* name) {
	gnode_t** elem;
	for (int i = 0; i < list_size(list); ++i) {
		elem = list_get(list, i);
		if (strcmp(name, (*elem)->name) == 0) {
			return elem;
		}
	}
	return NULL;
}

int list_exists(list_t* list, char const* name) {
	int i;
	gnode_t** curr;
	for (i = 0; i < list_size(list); ++i) {
		curr = list_get(list, i);
		if (strcmp(name, (*curr)->name) == 0) {
			return i;
		}

	}
	return -1;
}


gnode_t* gnode_new(char const* name, gnode_t* parent) {
	gnode_t* out = calloc(1, sizeof(gnode_t));
	strncpy(out->name, name, 4);
	out->parent = parent;
	out->childlist = list_new(sizeof(gnode_t*));
	return out;
}
// test case (test.txt)
//		   G - H       J - K - L - YOU
//		  /           /
// COM - B - C - D - E - F
// 				  \
//                 I - sAN

gnode_t* find_in_childlist(char const* name, list_t* list) {
	for (int i = 0; i < list_size(list); ++i) {
		gnode_t** e2 = list_get(list, i);
		if (strcmp(name, (*e2)->name) != 0) {
			for (int j = 0; j < list_size((*e2)->childlist); ++j) {
				gnode_t** e3 = list_get((*e2)->childlist, j);
				if (strcmp(name, (*e3)->name) == 0) {
					return *e2;
				}
			}
		}
	}
	return NULL;
}

int main(int argc, char** argv) {
	FILE* fptr = fopen("day06/input.txt", "r");
	list_t* nodelist = list_new(sizeof(gnode_t*));
	char buf[16];
	char* parent;
	char* child;
	char delim[3] = ")\n";
	gnode_t** elem;
	gnode_t* cnode;
	gnode_t* pnode;
	gnode_t** pnodeptr;
	gnode_t** cnodeptr;

	gnode_t* COM;
	gnode_t* SAN;
	gnode_t* YOU;

	while (fgets(buf, 16, fptr) != NULL) {
		parent = child = buf;
		strtok(child, delim);
		child = strtok(NULL, delim);

		if (list_exists(nodelist, parent) == -1) {
			pnode = gnode_new(parent, find_in_childlist(parent, nodelist));
			cnode = gnode_new(child, pnode);
			list_add(pnode->childlist, &cnode);
			list_add(nodelist, &pnode);
		} else {
			pnodeptr = list_get_name(nodelist, parent);
			if (list_exists(nodelist, child) == -1) {
				cnode = gnode_new(child, *pnodeptr);
				list_add((*pnodeptr)->childlist, &cnode);
			} else {
				cnodeptr = list_get_name(nodelist, child);
				(*cnodeptr)->parent = *pnodeptr;
				list_add((*pnodeptr)->childlist, cnodeptr);
			}
		}
	}

	// fix missing parents
	for (int k = 0; k < list_size(nodelist); ++k) {
		elem = list_get(nodelist, k);
		if (strcmp((*elem)->name, "COM") == 0) {
			COM = *elem;
		}
		if ((*elem)->parent == NULL) {
			(*elem)->parent = find_in_childlist((*elem)->name, nodelist);
		}
	}
	SAN = find_in_childlist("SAN", nodelist);
	YOU = find_in_childlist("YOU", nodelist);

	// print the whole thing
	for (int i = 0; i < list_size(nodelist); ++i) {
		gnode_t** e = list_get(nodelist, i);
		printf("N: %s P: %s C: ", (*e)->name, (*e)->parent ? (*e)->parent->name : "nil");
		for (int j = 0; j < list_size((*e)->childlist); ++j) {
			gnode_t** c = list_get((*e)->childlist, j);
			printf("%s ", (*c)->name);
		}
		printf("\n");
	}

	// calculate
	int total = 0;
	for (int i = 0; i < list_size(nodelist); ++i) {
		gnode_t** e = list_get(nodelist, i);
		for (int j = 0; j < list_size((*e)->childlist); ++j) {
			gnode_t** e2 = list_get((*e)->childlist, j);
			gnode_t* curr = (*e2);
			total--;
			while (curr != NULL) {
				total++;
				curr = curr->parent;
			}
		}

	}
	printf("Part 1: %d\n", total);
	list_t* path_to_san = list_new(sizeof(gnode_t*));
	gnode_t* curr = SAN;
	while (curr != COM) {
		list_add(path_to_san, &curr);
		curr = curr->parent;
	}

	list_add(path_to_san, &COM);
	curr = YOU;
	int count = 0;
	while (list_exists(path_to_san, curr->name) == -1) {
		count++;
		list_add(path_to_san, &curr);
		curr = curr->parent;
	}
	count += list_exists(path_to_san, curr->name);
	printf("Part 2: %d\n", count);
}