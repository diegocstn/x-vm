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
  LOAD = 10,    // load from current frame
  GLOAD = 11,   // load from global
  STORE = 12,   // store in local
  GSTORE = 13,  // store in global
  PRINT = 14,   // print stack top
  POP = 15,     // throw away top of the stack
  CALL = 16,
  RET = 17,
  HALT = 18
} is;

char *opname_from_code(int code);