#pragma once

#include <stdbool.h>

#define MEM_SIZE 4096
#define STACK_SIZE 64
#define REGS_COUNT 16
#define VMEM_Y_SIZE 32
#define VMEM_X_SIZE 64
#define VF 15

#define PIX_S 10
#define WIN_W VMEM_X_SIZE*PIX_S
#define WIN_H VMEM_Y_SIZE*PIX_S
#define FREQ 20

#define INTERVAL 16

typedef unsigned char uchar;

extern uchar memory[MEM_SIZE];                        //4K
extern int stack[STACK_SIZE];                         //16 nests
extern uchar regs[REGS_COUNT];                        //16 8-bits registers
extern uchar video[VMEM_Y_SIZE][VMEM_X_SIZE];         //64x32 pixels
extern int memreg;                                    //I register for memory addresses
extern int PC;                                        //Program counter
extern int SC;                                        //Stack counter
extern int timer_val;                                 //delay timer val
extern int cur_key;                                   //current pressed key
extern int key_reg;                                   //wait for a key
