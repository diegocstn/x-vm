#include <stdio.h>
#include <stdlib.h>

#include "isa.h"
#include "stack.h"

typedef enum { false, true } bool;

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

#define STACK_SIZE 10


Vm *vm_init(int code[], int main, int datasize, int codesize) {
  Vm *machine = malloc(sizeof(Vm));
  machine->code = code;
  machine->codesize = codesize;
  machine->global = calloc(datasize, sizeof(int));
  machine->stack = calloc(STACK_SIZE, sizeof(int));
  machine->sp = -1;
  machine->ip = main;

  return machine;
}

void vm_exec(Vm *vm) { // TODO fix code data type
  int opcode;
  int value;
  int value2;
  int addr;
  int nargs;

  while (vm->ip < vm->codesize) {
    // fetch
    opcode = vm->code[vm->ip];

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

         // store it at the top of the stack
        vm->sp++;
        vm->stack[vm->sp] = value;
        break;

      case BR:
        vm->ip = vm->code[vm->ip++];
        break;

      case BRT:
        addr = vm->code[vm->ip++];
        if (vm->stack[vm->sp] == true) {
         vm->ip = addr; 
        }
        break;

      case BRF:
        addr = vm->code[vm->ip++];
        if (vm->stack[vm->sp] == false) {
         vm->ip = addr; 
        }
        break;

      case ILT:
        value2 = vm->code[vm->ip++];
        value = vm->code[vm->ip++];
        if (value < value2) {
          vm->stack[vm->sp] = true;
        } else {
          vm->stack[vm->sp] = false;
        }
        break;

      case LOAD:
        // get offset
        value = vm->code[vm->ip++];
        vm->sp++;
        vm->stack[vm->sp] = vm->stack[vm->fp + value];
        break;
      
      case CALL:
        // we assume that args have been already pushed on the stack
        addr = vm->code[vm->ip++];    // target addr function
        nargs = vm->code[vm->ip++];   // how many args
        vm->stack[vm->sp++] = nargs;   // save nargs on stack
        vm->stack[vm->sp++] = vm->fp; // save current frame pointer
        vm->stack[vm->sp++] = vm->sp;  // save current ip

        vm->fp = vm->sp;              // make room for locals
        vm->ip = addr;                // jump to the function

        break;

      case RET:
        value = vm->stack[vm->sp--];    // pop return value
        vm->sp = vm->fp;                // jump over locals
        vm->ip = vm->stack[vm->sp--];   // get return address
        vm->fp = vm->stack[vm->sp--];   // restore fp
        
        nargs = vm->stack[vm->sp--];    // how many args do we need to throw away?
        vm->sp -= nargs;

        vm->sp++;
        vm->stack[vm->sp] = value;      // save return value on stack
        break;


      case HALT:
        fprintf( stderr, "%04d: %s", vm->ip, opname_from_code(opcode));
        stack_print(vm->stack, STACK_SIZE);
        fprintf(stderr, "\n");
        return;
    }
    // trace
    if (vm->trace) {
      fprintf( stderr, "%04d: %s", vm->ip, opname_from_code(opcode));
      stack_print(vm->stack, STACK_SIZE);
      fprintf(stderr, "\n");
    }
  }
}

int main() {
  // a beautiful factorial program
  int program[] = {
                    // address
    LOAD, -3,       // 0
    ICONST, 2,      // 2
    ILT,            // 4
    BRF, 10,        // 5
    ICONST, 1,      // 7
    RET,            // 9

    LOAD, -3,       // 10: load n on the stack
    LOAD, -3,       // 12: load n on the stack 
    ICONST, 1,      // 14: and subtract 1 to it
    ISUB,           // 16:
    CALL, 0, 1,     // 17: call fn and save result on the stack
    IMUL,           // 20: multiply the result with the previous n saved on the stack


    // main
    ICONST, 5,  // 22
    CALL, 0, 1, // 24
    PRINT,      // 26
    HALT        // 27
  };

  int main_ip = 21;
  int datasize = 1;
  int codesize = 28;

  Vm *vm = vm_init(program, main_ip, datasize, codesize);
  vm->trace = true;
  vm_exec(vm);

}

