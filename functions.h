#pragma once

#include <stdlib.h>
#include "global.h"

#define F_bits(byte) (0xF0 & (byte)) >> 4
#define S_bits(byte) (0x0F & (byte))
#define T_bits(b1, b2) (S_bits(b1) << 8) + (b2)

uchar memory[MEM_SIZE];
int stack[STACK_SIZE];
uchar regs[REGS_COUNT];
uchar video[VMEM_Y_SIZE][VMEM_X_SIZE];
int memreg;
int PC;
int cur_key;
int key_reg;

void ret(uchar *bytes);          //00EE
void jump(uchar *bytes);         //1NNN
void call(uchar *bytes);         //2NNN
void cond_econst(uchar *bytes);  //3XNN
void cond_uconst(uchar *bytes);  //4XNN
void cond_eregs(uchar *bytes);   //5XY0
void assign_const(uchar *bytes); //6XNN
void add_const(uchar *bytes);    //7XNN
void assign_regs(uchar *bytes);  //8XY0
void or_regs(uchar *bytes);      //8XY1
void and_regs(uchar *bytes);     //8XY2
void xor_regs(uchar *bytes);     //8XY3
void add_regs(uchar *bytes);     //8XY4
void sub_regs(uchar *bytes);     //8XY5
void rshift_regs(uchar *bytes);  //8XY6
void sub2_regs(uchar *bytes);    //8XY7
void lshift_regs(uchar *bytes);  //8XYE
void cond_uregs(uchar *bytes);   //9XY0
void assign_mem(uchar *bytes);   //ANNN
void jump_av0(uchar *bytes);     //BNNN
void randbit(uchar *bytes);      //CXNN
void load_sprite(uchar *bytes);  //DXYN
void is_ekey(uchar *bytes);      //EX9E
void is_ukey(uchar *bytes);      //EXA1
void get_delay(uchar *bytes);    //FX07
void get_key(uchar *bytes);      //FX0A
void delay_timer(uchar *bytes);  //FX15
void add_mem(uchar *bytes);      //FX1E
void get_font(uchar *bytes);     //FX29
void reg_dump(uchar *bytes);     //FX55
void reg_load(uchar *bytes);     //FX65
void stop_cycle(uchar *bytes);   //0000
