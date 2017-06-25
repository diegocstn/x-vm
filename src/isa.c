#include "isa.h"

char *opname_from_code(int code) {
  switch(code) {
  case IADD:
    return "IADD";
  case ISUB:
    return "ISUB";
  case IMUL:
    return "IMUL";
  case ILT:
    return "ILT";
  case IEQ:
    return "IEQ";
  case BR:
    return "BR";
  case BRT:
    return "BRT";
  case BRF:
    return "BRF";
  case ICONST:
    return "ICONST";
  case LOAD:
    return "LOAD";
  case GLOAD: 
    return "GLOAD:";
  case STORE:
    return "STORE";
  case GSTORE: 
    return "GSTORE:";
  case PRINT:
    return "PRINT";
  case POP:
    return "POP";
  case CALL: 
    return "CALL";
  case RET:
    return "RET";
  case HALT:
    return "HALT";
  }

  return "UNKNOWN OP";
}