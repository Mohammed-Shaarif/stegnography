#ifndef COMMON_H
#define COMMON_H
#include "types.h"
/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"

OperationType check_operation_type(char c);
#endif
