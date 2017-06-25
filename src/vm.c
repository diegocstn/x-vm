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
  int *code;
  int *stack;
  int *global;
  bool trace;
  int codesize;

  int ip;
  int sp;
  int fp;
} Vm;

#define STACK_SIZE 100


Vm *vm_init(int code[], int main, int datasize, int codesize) {
  Vm *machine = malloc(sizeof(Vm));
  machine->code = code;
  machine->codesize = codesize;
  machine->global = malloc(datasize * sizeof(int));
  machine->stack = malloc(STACK_SIZE * sizeof(int));
  machine->sp = -1;
  machine->ip = main;

  return machine;
}

void vm_exec(Vm *vm) { // TODO fix code data type
  int opcode;
  int value;
  int addr;

  while (vm->ip < vm->codesize) {
    // fetch
    opcode = vm->code[vm->ip];

    // trace
    if (vm->trace) {
      // TODO print stack
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

      case GSTORE:
        // store value at addr of global store
        value = vm->stack[vm->sp];
        vm->sp--;
        addr = vm->code[vm->ip];
        vm->ip++;
        vm->global[addr] = value;
        break;

      case GLOAD:
        // get value from global 
        addr = vm->code[vm->ip];
        vm->ip++;
        value = vm->global[addr];

         // store at the top  of the stack
        vm->sp++;
        vm->stack[vm->sp] = value;
        break;
      
      case HALT:
        return;
    }
  }
}

int main() {
  int program[] = {
    ICONST, 99,
    GSTORE, 0,
    GLOAD, 0,
    PRINT,
    HALT
  };

  Vm *vm = vm_init(program, 0, 1, 8);
  vm->trace = true;
  vm_exec(vm);

}

