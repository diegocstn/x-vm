#include <stdio.h>
#include <stdlib.h>

#include "isa.h"
#include "stack.h"

typedef enum {
  false = 0,
  true = 1 } bool;

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
  machine->fp = main;

  return machine;
}

void vm_printstatus(Vm *vm, int opcode) {
  fprintf( stderr, "%04d: %s", vm->ip, opname_from_code(opcode));
  stack_print(vm->stack, STACK_SIZE);
  fprintf(stderr, "\tfp=%d", vm->fp);
  fprintf(stderr, "\tsp=%d", vm->sp);
  fprintf(stderr, "\n");
}

void vm_exec(Vm *vm) { // TODO fix code data type
  int opcode;
  int value1;
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
        value1 = vm->code[vm->ip];
        vm->ip++;
        vm->sp++;
        vm->stack[vm->sp] = value1;
        break;
      
      case PRINT:
        value1 = vm->stack[vm->sp];
        vm->sp--;
        printf("PRINT -> %d\n", value1);
        break;

      case GSTORE:
        // store value at addr of global store
        value1 = vm->stack[vm->sp];
        vm->sp--;
        addr = vm->code[vm->ip];
        vm->ip++;
        vm->global[addr] = value1;
        break;

      case GLOAD:
        // get value from global 
        addr = vm->code[vm->ip];
        vm->ip++;
        value1 = vm->global[addr];

         // store it at the top of the stack
        vm->sp++;
        vm->stack[vm->sp] = value1;
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
        value2 = vm->stack[vm->sp--];
        value1 = vm->stack[vm->sp--];
        vm->stack[++vm->sp] = value1 < value2 ? true : false;
        break;

      case IADD:
        value2 = vm->stack[vm->sp--];
        value1 = vm->stack[vm->sp--];
        vm->stack[++vm->sp] = value1 + value2;
        break;
      
      case ISUB:
        value2 = vm->stack[vm->sp--];
        value1 = vm->stack[vm->sp--];
        vm->stack[++vm->sp] = value1 - value2;
        break;

      case IMUL:
        value2 = vm->stack[vm->sp--];
        value1 = vm->stack[vm->sp--];
        vm->stack[++vm->sp] = value1 * value2;
        break;

      case LOAD:
        // get offset
        addr = vm->code[vm->ip++];
        vm->stack[++vm->sp] = vm->stack[vm->fp + addr];
        break;
      
      case CALL: {
        // we assume that args have been already pushed on the stack
        addr = vm->code[vm->ip++];      // target addr function
        nargs = vm->code[vm->ip++];     // how many args

        vm->stack[++vm->sp] = vm->fp;   // save current frame pointer
        vm->stack[++vm->sp] = vm->ip;   // save current ip

        vm->fp = vm->sp;                // make room for locals
        int first_arg = vm->sp - nargs;
        for (int i = 0; i < nargs; i++) {
          vm->stack[vm->fp + i] = vm->stack[first_arg + i]; 
        }
        
        vm->ip = addr;                  // jump to the function

        break;
      }

      case RET:
        value1 = vm->stack[vm->sp--];    // pop return value
        vm->sp = vm->fp;                // jump over locals
        vm->ip = vm->stack[vm->sp--];   // get return address
        vm->fp = vm->stack[vm->sp--];   // restore fp
        printf("RET TO: %d\n", vm->sp);
        return;
        
        nargs = vm->stack[vm->sp--];    // how many args do we need to throw away?
        vm->sp -= nargs;

        vm->sp++;
        vm->stack[vm->sp] = value1;      // save return value on stack
        break;


      case HALT:
        vm_printstatus(vm, opcode);
        return;
    }
    // trace
    if (vm->trace) {
      vm_printstatus(vm, opcode);
    }
  }
}

int main() {
  // a beautiful factorial program
  int program_fact[] = {
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
    ICONST, 5,  // 21
    CALL, 0, 1, // 23
    PRINT,      // 26
    HALT        // 27
  };
  int main_ip_fact = 21;

  int program_fun_call[] = {
    LOAD, -3,     // 0
    LOAD, -4,     // 2
    IADD,         // 4
    PRINT,        // 5
    RET,          // 6
    ICONST, 99,    // 7
    ICONST, 123,    // 9
    CALL, 0, 2,   // 10
    HALT          // 14
  };
  int main_ip_fn_call = 7;

  int datasize = 1;
  int codesize = 28;

  Vm *vm = vm_init(program_fun_call, main_ip_fn_call, datasize, codesize);
  vm->trace = true;
  vm_exec(vm);

}

