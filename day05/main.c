//
// Created by nik on 12/15/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

int get_input() {
	char buf[16];
	printf("$: ");
	fgets(buf, 16, stdin);
	return (int) strtol(buf, NULL, 10);
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
void run(uint* bp, uint** gpc) {
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
			reg2 = get_input();
			*(bp + reg1) = reg2;
			*gpc += 2;
			break;
		case OUT:
			reg1 = *(pc + 1);
			reg2 = *(bp + reg1);
			printf("(%d) %d\n", reg1, reg2);
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

int main() {
	uint* bp = NULL;
	uint* pc = NULL;

	struct program* prog = load_program("day05/input.txt");
	if (prog == NULL) {
		fprintf(stderr, "Unable to load program.\n");
		return 1;
	}
	pc = prog->mem;
	bp = prog->mem;
	while (running) {
		run(bp, &pc);
	}
	return 0;
}
