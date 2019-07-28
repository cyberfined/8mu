#pragma once

#include <stdbool.h>
#include "global.h"

typedef void (*op_function) (uchar*);

#define ANY 0xFF
typedef struct {
    op_function func;
    uchar pat1, pat2, pat3, pat4;
}OP;

bool check_opcode(uchar *bytes, OP *op);
void exec_command(uchar *bytes, OP *ops, int ops_count);
