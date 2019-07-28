#include "opcode.h"

#define F_bits(byte) (0xF0 & (byte))
#define S_bits(byte) (0x0F & (byte))

bool check_opcode(unsigned char *bytes, OP *op) {
    bool res = false;

    if((op->pat1 == ANY || F_bits(bytes[0]) == op->pat1) &&
       (op->pat2 == ANY || S_bits(bytes[0]) == op->pat2) &&
       (op->pat3 == ANY || F_bits(bytes[1]) == op->pat3) &&
       (op->pat4 == ANY || S_bits(bytes[1]) == op->pat4)) {
        op->func(bytes);
        res = true;
    }

    return res;
}

void exec_command(unsigned char *bytes, OP *ops, int ops_count) {
    int i;
    for(i = 0; i < ops_count; i++) {
        if(check_opcode(bytes, &ops[i])) break;
    }
}
