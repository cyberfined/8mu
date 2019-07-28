#include "functions.h"

void ret(uchar *bytes) {
    if(SC > 0) SC--;
    PC = stack[SC];
}

void jump(uchar *bytes) {
    PC = T_bits(bytes[0], bytes[1]);
}

void call(uchar *bytes) {
    stack[SC] = PC;
    PC = T_bits(bytes[0], bytes[1]);
    if(SC < STACK_SIZE-1) SC++;
}

void cond_econst(uchar *bytes) {
    int r = S_bits(bytes[0]);
    if(regs[r] == bytes[1]) PC += 2;
}

void cond_uconst(uchar *bytes) {
    int r = S_bits(bytes[0]);
    if(regs[r] != bytes[1]) PC += 2;
}

void cond_eregs(uchar *bytes) {
    int r1 = S_bits(bytes[0]);
    int r2 = F_bits(bytes[1]);
    if(regs[r1] == regs[r2]) PC += 2;
}

void assign_const(uchar *bytes) {
    int r = S_bits(bytes[0]);
    regs[r] = bytes[1];
}

void add_const(uchar *bytes) {
    int r = S_bits(bytes[0]);
    regs[r] += bytes[1];
}

void assign_regs(uchar *bytes) {
    int r1 = S_bits(bytes[0]);
    int r2 = F_bits(bytes[1]);
    regs[r1] = regs[r2];
}

void or_regs(uchar *bytes) {
    int r1 = S_bits(bytes[0]);
    int r2 = F_bits(bytes[1]);
    regs[r1] = regs[r1] | regs[r2];
}

void and_regs(uchar *bytes) {
    int r1 = S_bits(bytes[0]);
    int r2 = F_bits(bytes[1]);
    regs[r1] = regs[r1] & regs[r2];
}

void xor_regs(uchar *bytes) {
    int r1 = S_bits(bytes[0]);
    int r2 = F_bits(bytes[1]);
    regs[r1] = regs[r1] ^ regs[r2];
}

void add_regs(uchar *bytes) {
    int r1 = S_bits(bytes[0]);
    int r2 = F_bits(bytes[1]);

    uchar tmp = regs[r1];
    regs[r1] += regs[r2];

    if(regs[r1] < tmp) regs[VF] = 1; //overflow detect
    else regs[VF] = 0;
}

void sub_regs(uchar *bytes) {
    int r1 = S_bits(bytes[0]);
    int r2 = F_bits(bytes[1]);

    uchar tmp = regs[r1];
    regs[r1] -= regs[r2];
 
    if(regs[r1] > tmp) regs[VF] = 0; //borrow detect
    else regs[VF] = 1;
}

void rshift_regs(uchar *bytes) {
    int r1 = S_bits(bytes[0]);
    int r2 = F_bits(bytes[1]);

    regs[VF] = regs[r2] & 1;
    regs[r1] = regs[r2] = regs[r2] >> 1;
}

void sub2_regs(uchar *bytes) {
    int r1 = S_bits(bytes[0]);
    int r2 = F_bits(bytes[1]);
    
    regs[r1] = regs[r2] - regs[r1];

    if(regs[r1] > regs[r2]) regs[VF] = 0; //borrow detect
    else regs[VF] = 1;
}

void lshift_regs(uchar *bytes) {
    int r1 = S_bits(bytes[0]);
    int r2 = F_bits(bytes[1]);

    regs[VF] = regs[r2] & (1 << 7);
    regs[r1] = regs[r2] = regs[r2] << 1;
}

void cond_uregs(uchar *bytes) {
    int r1 = S_bits(bytes[0]);
    int r2 = F_bits(bytes[1]);
    if(regs[r1] != regs[r2]) PC += 2;
}

void assign_mem(uchar *bytes) {
    memreg = T_bits(bytes[0], bytes[1]);
}

void jump_av0(uchar *bytes) {
    PC = regs[0] + T_bits(bytes[0], bytes[1]);
}

void randbit(uchar *bytes) {
    int r = S_bits(bytes[0]);
    regs[r] = rand()&bytes[1];
}

void load_sprite(uchar *bytes) {
    int r1 = S_bits(bytes[0]);
    int r2 = F_bits(bytes[1]);
    char height = S_bits(bytes[1]);
    
    int x = regs[r1], y = regs[r2];
    int i, j;

    regs[VF] = 0;
    for(i = memreg; i < MEM_SIZE && i < memreg + height && y < VMEM_Y_SIZE; i++, y++) {
        for(j = 7; j >= 0; j--) {
            int nx = x + 7 - j;
            if(nx >= VMEM_X_SIZE) break;

            char tmp = video[y][nx];
            video[y][nx] ^= 1 && (memory[i] & (1 << j));
            regs[VF] |= tmp & !video[y][nx];
        }
    }
}

void is_ekey(uchar *bytes) {
    int r = S_bits(bytes[0]);
    int code = regs[r];

    if(code == cur_key) PC += 2;
}

void is_ukey(uchar *bytes) {
    int r = S_bits(bytes[0]);
    int code = regs[r];

    if(code != cur_key)  PC += 2;
}


void get_delay(uchar *bytes) {
    int r = S_bits(bytes[0]);
    regs[r] = timer_val;
}

void get_key(uchar *bytes) {
    int r = S_bits(bytes[0]);
    key_reg = r;
}

void delay_timer(uchar *bytes) {
    int r = S_bits(bytes[0]);
    timer_val = regs[r];
}

void add_mem(uchar *bytes) {
    int r = S_bits(bytes[0]);
    memreg += regs[r];
}

void get_font(uchar *bytes) {
    int r = S_bits(bytes[0]);
    memreg = regs[r] * 5;
}

void reg_dump(uchar *bytes) {
    int r = S_bits(bytes[0]);
    int i;

    for(i = 0; i <= r && memreg < MEM_SIZE; i++, memreg++) {
        memory[memreg] = regs[i];
    }
}

void reg_load(uchar *bytes) {
    int r = S_bits(bytes[0]);
    int i;

    for(i = 0; i <= r && memreg < MEM_SIZE; i++, memreg++) {
        regs[i] = memory[memreg];
    }
}

void stop_cycle(uchar *bytes) {
    PC = MEM_SIZE;
}
