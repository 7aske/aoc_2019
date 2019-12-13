//
// Created by nik on 12/11/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

static int max_size = 0;

struct clist {
	void* data;
	uint size;
	uint cap;
	uint len;
};

int manhattan_dist(int sx, int sy, int dx, int dy) {
	return abs(sx - dx) + abs(sy - dy);
}

struct clist* clist_new(uint size) {
	struct clist* newlist = calloc(1, sizeof(struct clist));
	newlist->cap = 32;
	newlist->data = calloc(newlist->cap, size);
	newlist->size = size;
	newlist->len = 0;
}

void clist_add(struct clist* list, int* data) {
	if (list->len == list->cap) {
		list->cap *= 2;
		list->data = reallocarray(list->data, list->cap * list->size, list->size);
	}
	memcpy(list->data + list->len * list->size, data, list->size);
	list->len++;
}

int* clist_get(struct clist* list, uint index) {
	if (index >= list->len) {
		return NULL;
	}
	return list->data + index * list->size;
}

char* load_grid(FILE* fptr, uint* size) {
	if (fptr == NULL) {
		return NULL;
	}
	#define BSIZE 32
	char* buffer = calloc(BSIZE, sizeof(char));
	char* base = buffer;
	int c = 0;
	uint count = BSIZE;
	while (((c = fgetc(fptr)) != EOF) && c != '\n') {
		if (count == *size) {
			count *= 2;
			buffer = realloc(base, count);
			base = buffer;
			buffer += *size;
		}
		*buffer++ = (char) c;
		(*size)++;
	}
	*buffer = '\0';
	return buffer - *size;
}

void parse_dir(char* wire0, struct clist* list0) {
	char* temp = NULL;
	char* ptr = wire0;
	char dir;
	int x = 0;
	int y = 0;
	int num;
	int coord[3];
	int steps = 0;
	while (ptr != NULL && *ptr != '\0') {
		if (isupper(*ptr)) {
			dir = *ptr++;
			temp = strchr(ptr, ',');
			if (temp != NULL) {
				*temp++ = '\0';
				num = (int) strtol(ptr, NULL, 10);
				switch (dir) {
					case 'R':
						x += num;
						break;
					case 'L':
						x -= num;
						break;
					case 'U':
						y -= num;
						break;
					case 'D':
						y += num;
						break;
				}
				if (abs(x) > max_size) {
					max_size = abs(x);
				}
				if (abs(y) > max_size) {
					max_size = abs(y);
				}
				steps += num;
				coord[0] = x;
				coord[1] = y;
				coord[2] = steps;
				clist_add(list0, coord);
			}
			ptr = temp;
		}
	}
}

typedef struct Point {
	int x;
	int y;
} Point;

int max(int a, int b) {
	return a > b ? a : b;
}

int min(int a, int b) {
	return a > b ? b : a;
}

int onSegment(Point p, Point q, Point r) {
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return 1;

	return 0;
}

int orientation(Point p, Point q, Point r) {
	int val = (q.y - p.y) * (r.x - q.x) -
			  (q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;

	return (val > 0) ? 1 : 2;
}

int doIntersect(Point p1, Point q1, Point p2, Point q2) {
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	if (o1 != o2 && o3 != o4)
		return 1;

	if (o1 == 0 && onSegment(p1, p2, q1)) return 1;
	if (o2 == 0 && onSegment(p1, q2, q1)) return 1;
	if (o3 == 0 && onSegment(p2, p1, q2)) return 1;
	if (o4 == 0 && onSegment(p2, q1, q2)) return 1;

	return 0;
}

Point intersection(Point A, Point B, Point C, Point D) {
	// Line AB represented as a1x + b1y = c1
	double a1 = B.y - A.y;
	double b1 = A.x - B.x;
	double c1 = a1 * (A.x) + b1 * (A.y);

	// Line CD represented as a2x + b2y = c2
	double a2 = D.y - C.y;
	double b2 = C.x - D.x;
	double c2 = a2 * (C.x) + b2 * (C.y);

	double determinant = a1 * b2 - a2 * b1;

	if (determinant == 0) {
		// The lines are parallel. This is simplified
		// by returning a pair of FLT_MAX
		return (Point) {INT_MAX, INT_MAX};
	} else {
		double x = (b2 * c1 - b1 * c2) / determinant;
		double y = (a1 * c2 - a2 * c1) / determinant;
		return (Point) {x, y};
	}
}

int main() {
	FILE* fptr = fopen("day03/input.txt", "r");
	uint wsize0 = 0;
	char* wire1 = load_grid(fptr, &wsize0);
	wsize0 = 0;
	char* wire2 = load_grid(fptr, &wsize0);

	struct clist* list1 = clist_new(sizeof(int[3]));
	struct clist* list2 = clist_new(sizeof(int[3]));
	parse_dir(wire1, list1);
	parse_dir(wire2, list2);
	printf("%d, %d, %d\n", list1->len, max_size, max_size * max_size);
	int* e1;
	int* e2;
	int* e3;
	int* e4;
	int steps1 = 0;
	int steps2 = 0;
	int steps_total = 0;
	int steps_min = INT_MAX;
	int i, j;
	int dist;
	int min_dist = INT_MAX;
	for (i = 0; i < list1->len - 1; ++i) {
		e1 = clist_get(list1, i);
		e2 = clist_get(list1, i + 1);
		for (j = 0; j < list2->len - 1; ++j) {
			e3 = clist_get(list2, j);
			e4 = clist_get(list2, j + 1);

			Point A = {e1[0], e1[1]};
			Point B = {e2[0], e2[1]};
			Point C = {e3[0], e3[1]};
			Point D = {e4[0], e4[1]};

			if (doIntersect(A, B, C, D)) {
				printf("(%d,%d)(%d,%d)->(%d,%d)(%d,%d)\n", A.x, A.y, B.x, B.y, C.x, C.y, D.x, D.y);
				Point x = intersection(A, B, C, D);
				printf("(%d,%d)\n", x.x, x.y);
				dist = manhattan_dist(0, 0, x.x, x.y);
				printf("Dist: %d\n", dist);
				steps1 = e1[2];
				steps2 = e3[2];
				steps_total = steps1 + steps2;
				steps_total += manhattan_dist(x.x, x.y, A.x, A.y);
				steps_total += manhattan_dist(x.x, x.y, C.x, C.y);
				if (steps_total < steps_min) {
					steps_min = steps_total;
				}
				printf("Steps %d\n", steps_total);
				if (dist < min_dist) {
					min_dist = dist;
				}
			}
		}
	}
	printf("\n%d %d\n", min_dist, steps_min);
}