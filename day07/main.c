//
// Created by nik on 12/16/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>

#define SBSIZE 32

enum opcodes {
	HLT = 99,
	NOP = 0,
	ADD = 1, // num1 num2 dest
	MUL = 2, // num1 num2 dest
	INP = 3, // dest
	OUT = 4, // dest
	JT = 5, // cond dest
	JF = 6, // cond dest
	LT = 7, // num1 num2 dest
	EQ = 8, // num1 num2 dest
};

enum opmode {
	POS = 0,
	IMM = 1,
};

struct opcode {
	enum opmode modes[3];
	enum opcodes code;
};

int running = 1;

struct program {
	uint* mem;
	uint size;
};

int get_int(void* map) {
	int x = *(int*) map;
	memmove(map, map + sizeof(int), SBSIZE - sizeof(int));
	return x;
}

void parse_opcode(struct opcode* opcode, uint const* pc) {
	uint num = *pc;
	uint x;
	uint count = 0;
	while (num != 0) {
		x = num % 10;
		num /= 10;
		if (count < 2) {
			opcode->code += x * (uint) pow(10, count);
		} else {
			opcode->modes[2 - (count - 2)] = (x == 1 ? IMM : POS);
		}
		count++;
	}
}

uint load_register(uint const* bp, uint const* pc, uint offset, enum opmode mode) {
	if (mode == POS) {
		return *(bp + *(pc + offset));
	} else {
		return *(pc + offset);
	}
}

/**
 * @param bp Pointer to the beginning of the code
 * @param gpc Global program counter pointer
 */
void run(uint* bp, uint** gpc, void* iptr, void* optr) {
	uint* pc = *gpc;
	struct opcode opcode = {POS, POS, POS, NOP};
	parse_opcode(&opcode, *gpc);
	uint reg1, reg2, reg3;
	switch (opcode.code) {
		case HLT:
			running = 0;
			break;
		case ADD:
			reg1 = load_register(bp, pc, 1, opcode.modes[2]);
			reg2 = load_register(bp, pc, 2, opcode.modes[1]);
			reg3 = *(pc + 3);
			*(bp + reg3) = reg1 + reg2;
			*gpc += 4;
			break;
		case MUL:
			reg1 = load_register(bp, pc, 1, opcode.modes[2]);
			reg2 = load_register(bp, pc, 2, opcode.modes[1]);
			reg3 = *(pc + 3);
			*(bp + reg3) = reg1 * reg2;
			*gpc += 4;
			break;
		case INP:
			reg1 = *(pc + 1);
			reg2 = get_int(iptr);
			*(bp + reg1) = reg2;
			printf("$%d\n", reg2);
			*gpc += 2;
			break;
		case OUT:
			reg1 = *(pc + 1);
			reg2 = *(bp + reg1);
			printf("%d\n", reg2);
			memcpy(optr, &reg2, sizeof(uint));
			*gpc += 2;
			break;
		case JT:
			reg1 = load_register(bp, pc, 1, opcode.modes[2]);
			reg2 = load_register(bp, pc, 2, opcode.modes[1]);
			if (reg1) {
				*gpc = bp + reg2;
				break;
			}
			*gpc += 3;
			break;
		case JF:
			reg1 = load_register(bp, pc, 1, opcode.modes[2]);
			reg2 = load_register(bp, pc, 2, opcode.modes[1]);
			if (!reg1) {
				*gpc = bp + reg2;
				break;
			}
			*gpc += 3;
			break;
		case LT:
			reg1 = load_register(bp, pc, 1, opcode.modes[2]);
			reg2 = load_register(bp, pc, 2, opcode.modes[1]);
			reg3 = *(pc + 3);
			*(bp + reg3) = reg1 < reg2;
			*gpc += 4;
			break;
		case EQ:
			reg1 = load_register(bp, pc, 1, opcode.modes[2]);
			reg2 = load_register(bp, pc, 2, opcode.modes[1]);
			reg3 = *(pc + 3);
			*(bp + reg3) = reg1 == reg2;
			*gpc += 4;
			break;
		default:
			fprintf(stderr, "Unrecognized opcode '%d'@'%p'(%lu)\n", opcode.code, *gpc, *gpc - bp);
			running = 0;

			break;
	}
}

struct program* load_program(char const* filepath) {
	FILE* fptr = fopen(filepath, "r");
	if (fptr == NULL) {
		fprintf(stderr, "Failed to open file.\n");
		return NULL;
	}
	unsigned char buf[6];
	unsigned char curr = 0;
	int c;
	uint x;
	uint ctotal = 32;
	uint total = 0;
	struct program* out = malloc(sizeof(struct program));
	uint* program = calloc(ctotal, sizeof(uint));
	do {
		c = (char) fgetc(fptr);
		if (c == ',' || c == EOF) {

			buf[curr] = '\0';
			curr = 0;
			x = (uint) strtol(buf, NULL, 10);
			memcpy(program + total, &x, sizeof(uint));
			total++;
			if (total == ctotal) {
				ctotal += 32;
				program = reallocarray(program, ctotal, sizeof(uint));
			}
			if (c == EOF) {
				break;
			}
		} else {
			buf[curr++] = c;
		}

	} while (1);
	fclose(fptr);
	out->mem = program;
	out->size = total;
	return out;
}

void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

int fact(int x) {
	int i;
	int f = 1;
	for (i = x; i > 1; --i) {
		f *= i;
	}
}

void permute(int* a, int l, int r, int* dest, int* index) {
	if (l == r) {
		memcpy(&dest[*index * 5], a, sizeof(int) * 5);
		(*index)--;
	} else {
		for (int i = l; i <= r; i++) {
			swap(&a[l], &a[i]);
			permute(a, l + 1, r, dest, index);
			swap(&a[l], &a[i]);
		}
	}
}

int* get_permutations(int* nums, int size, int* count) {
	*count = fact(size);
	// printf("%d\n", *count);
	int index = *count - 1;
	int* combos = calloc(*count, sizeof(int[size]));
	permute(nums, 0, size - 1, combos, &index);
	// exit(1);
	return combos;
}

void load_amp_prog(uint** amp_prog, struct program* prog) {
	int i;
	for (i = 0; i < 5; ++i) {
		if (amp_prog[i] != NULL) {
			free(amp_prog[i]);
			amp_prog[i] = NULL;
		}
		amp_prog[i] = calloc(prog->size, sizeof(uint));
		memcpy(amp_prog[i], prog->mem, prog->size * sizeof(uint));
	}
}

int thruster_power(struct program* prog) {
	uint* active;
	uint* bp = NULL;
	uint* pc = NULL;
	int i;

	uint* amp_prog[5] = {NULL};
	load_amp_prog(&(amp_prog[0]), prog);

	// 1 phase setting
	// 2 input signal (starts as 0)
	int last_num = 0;
	int max_num = 0;

	int count = 0;
	int count2 = 0;
	int nums[5] = {0, 1, 2, 3, 4};
	int nums2[5] = {5, 6, 7, 8, 9};
	int* combos = get_permutations(nums, 5, &count);
	// int* combos2 = get_permutations(nums2, 5, &count2);

	void* shared_out = malloc(SBSIZE);
	void* shared_in = malloc(SBSIZE);

	for (int j = 0; j < count; ++j) {
		int* curr = &combos[j * sizeof(nums) / sizeof(nums[0])];
		last_num = 0;
		for (i = 0; i < sizeof(nums) / sizeof(nums[0]); ++i) {

			memcpy(shared_in, curr + i, sizeof(int));
			memcpy(shared_in + sizeof(int), &last_num, sizeof(int));

			active = amp_prog[i];
			pc = active;
			bp = active;
			while (running) {
				run(bp, &pc, shared_in, shared_out);
			}
			running = 1;
			last_num = get_int(shared_out);
			if (last_num > max_num) {
				max_num = last_num;
			}
			memset(shared_in, 0, SBSIZE);
			memset(shared_out, 0, SBSIZE);
		}
	}
	return max_num;
}

int main() {

	struct program* prog = load_program("day07/input.txt");
	if (prog == NULL) {
		fprintf(stderr, "Unable to load program.\n");
		return 1;
	}
	struct program* test1 = load_program("day07/test1.txt");

	printf("Part 1: %d\n", thruster_power(test1));
	return 0;
}