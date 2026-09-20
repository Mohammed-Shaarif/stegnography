#include "common.h"

OperationType check_operation_type(char c){
    if (c == 'e')
        return e_encode;
    else if (c == 'd')
        return e_decode;
    else
        return e_unsupported;
}