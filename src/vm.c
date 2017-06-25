#include <stdio.h>
#include <stdlib.h>

typedef enum { false, true } bool;

typedef enum {
  IADD = 1,     // int add
  ISUB = 2,     // int sub
  IMUL = 3,     // int mul
  ILT = 4,      // int less than
  IEQ = 5,      // int equal
  BR = 6,       // branch
  BRT = 7,      // branch if true
  BRF = 8,      // branch if false
  ICONST = 9,   // push const int
  LOAD = 10,    // load from local
  GLOAD = 11,   // load from global
  STORE = 12,   // store in local
  GSTORE = 13,  // store in global
  PRINT = 14,   // print stack top
  POP = 15,     // throw away top of the stack
  HALT = 16
} Bytecode;

typedef struct {
  int *data;
  Bytecode *code;
  int *stack;
  bool trace;

  int ip;
  int sp;
  int fp;
} Vm;

#define STACK_SIZE 100

Vm *vm_init(Bytecode code[], int main, int datasize) {
  Vm *machine = malloc(sizeof(Vm));
  machine->code = code;
  machine->data = malloc(datasize * sizeof(int));
  machine->stack = malloc(STACK_SIZE * sizeof(int));
  machine->sp = -1;
  machine->ip = main;

  return machine;
}

void vm_exec(Vm *vm) {
  int opcode;
  int value;
  int code_size = sizeof(vm->code) / sizeof(short); // TODO fix code data type
  while (vm->ip < code_size) {
    // fetch
    opcode = vm->code[vm->ip];

    // trace
    if (vm->trace) {
      fprintf( stderr, "%04d: %d\n", vm->ip, opcode);
    }

    vm->ip++;
    switch (opcode) {
      case ICONST:
        // get the value and push it onto the stack
        value = vm->code[vm->ip];
        vm->ip++;
        vm->sp++;
        vm->stack[vm->sp] = value;
        break;
      
      case PRINT:
        value = vm->stack[vm->sp];
        vm->sp--;
        printf("%d\n", value);
        break;
      
      case HALT:
        return;
    }
  }
}

int main() {
  Bytecode program[] = {
    ICONST, 99,
    PRINT,
    HALT
  };

  Vm *vm = vm_init(program, 0, 0);
  vm->trace = true;
  vm_exec(vm);

}

