#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "global.h"
#include "opcode.h"
#include "functions.h"
#include "load.h"

static OP ops[] = {
    {.pat1 = 0x00, .pat2 = 0x00, .pat3 = 0xE0, .pat4 = 0x0E, .func = ret},
    {.pat1 = 0x10, .pat2 = ANY,  .pat3 = ANY,  .pat4 = ANY,  .func = jump},
    {.pat1 = 0x20, .pat2 = ANY,  .pat3 = ANY,  .pat4 = ANY,  .func = call},
    {.pat1 = 0x30, .pat2 = ANY,  .pat3 = ANY,  .pat4 = ANY,  .func = cond_econst},
    {.pat1 = 0x40, .pat2 = ANY,  .pat3 = ANY,  .pat4 = ANY,  .func = cond_uconst},
    {.pat1 = 0x50, .pat2 = ANY,  .pat3 = ANY,  .pat4 = 0x00, .func = cond_eregs},
    {.pat1 = 0x60, .pat2 = ANY,  .pat3 = ANY,  .pat4 = ANY,  .func = assign_const},
    {.pat1 = 0x70, .pat2 = ANY,  .pat3 = ANY,  .pat4 = ANY,  .func = add_const},
    {.pat1 = 0x80, .pat2 = ANY,  .pat3 = ANY,  .pat4 = 0x00, .func = assign_regs},
    {.pat1 = 0x80, .pat2 = ANY,  .pat3 = ANY,  .pat4 = 0x01, .func = or_regs},
    {.pat1 = 0x80, .pat2 = ANY,  .pat3 = ANY,  .pat4 = 0x02, .func = and_regs},
    {.pat1 = 0x80, .pat2 = ANY,  .pat3 = ANY,  .pat4 = 0x03, .func = xor_regs},
    {.pat1 = 0x80, .pat2 = ANY,  .pat3 = ANY,  .pat4 = 0x04, .func = add_regs},
    {.pat1 = 0x80, .pat2 = ANY,  .pat3 = ANY,  .pat4 = 0x05, .func = sub_regs},
    {.pat1 = 0x80, .pat2 = ANY,  .pat3 = ANY,  .pat4 = 0x06, .func = rshift_regs},
    {.pat1 = 0x80, .pat2 = ANY,  .pat3 = ANY,  .pat4 = 0x07, .func = sub2_regs},
    {.pat1 = 0x80, .pat2 = ANY,  .pat3 = ANY,  .pat4 = 0x0E, .func = lshift_regs},
    {.pat1 = 0x90, .pat2 = ANY,  .pat3 = ANY,  .pat4 = 0x00, .func = cond_uregs},
    {.pat1 = 0xA0, .pat2 = ANY,  .pat3 = ANY,  .pat4 = ANY,  .func = assign_mem},
    {.pat1 = 0xB0, .pat2 = ANY,  .pat3 = ANY,  .pat4 = ANY,  .func = jump_av0},
    {.pat1 = 0xC0, .pat2 = ANY,  .pat3 = ANY,  .pat4 = ANY,  .func = randbit},
    {.pat1 = 0xD0, .pat2 = ANY,  .pat3 = ANY,  .pat4 = ANY,  .func = load_sprite},
    {.pat1 = 0xE0, .pat2 = ANY,  .pat3 = 0x90, .pat4 = 0x0E, .func = is_ekey},
    {.pat1 = 0xE0, .pat2 = ANY,  .pat3 = 0xA0, .pat4 = 0x01, .func = is_ukey},
    {.pat1 = 0xF0, .pat2 = ANY,  .pat3 = 0x00, .pat4 = 0x07, .func = get_delay},
    {.pat1 = 0xF0, .pat2 = ANY,  .pat3 = 0x00, .pat4 = 0x0A, .func = get_key},
    {.pat1 = 0xF0, .pat2 = ANY,  .pat3 = 0x10, .pat4 = 0x05, .func = delay_timer},
    {.pat1 = 0xF0, .pat2 = ANY,  .pat3 = 0x10, .pat4 = 0x0E, .func = add_mem},
    {.pat1 = 0xF0, .pat2 = ANY,  .pat3 = 0x20, .pat4 = 0x09, .func = get_font},
    {.pat1 = 0xF0, .pat2 = ANY,  .pat3 = 0x50, .pat4 = 0x05, .func = reg_dump},
    {.pat1 = 0xF0, .pat2 = ANY,  .pat3 = 0x60, .pat4 = 0x05, .func = reg_load},
    {.pat1 = 0x00, .pat2 = 0x00, .pat3 = 0x00, .pat4 = 0x00, .func = stop_cycle}
};

uchar memory[MEM_SIZE];               //4K
int stack[STACK_SIZE];                //16 nests
uchar regs[REGS_COUNT];               //16 8-bits registers
uchar video[VMEM_Y_SIZE][VMEM_X_SIZE]; //64x32 pixels
int memreg;                           //I register for memory addresses
int PC;                               //Program counter
int SC;                               //Stack counter
int timer_val;                        //delay timer val
int cur_key;                          //current pressed key
int key_reg;                          //wait for a key

int keys[] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'a',
    'b',
    'c',
    'd',
    'e',
    'f',
};

Uint32 timer_callback(Uint32 interval, void *param);

SDL_Window *window;
SDL_Renderer *renderer;
bool is_run = true;

#define Error(msg) do { fprintf(stderr, "%s: %s", msg, SDL_GetError()); exit(EXIT_FAILURE); } while (0)

void init() {
    char font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x40, 0xC0, 0x40, 0x40, 0xE0, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0x10, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
        0xF0, 0x10, 0x20, 0x40, 0x40, //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
        0xF0, 0x80, 0xF0, 0x80, 0x80  //F
    };
    int i, j;
    for(i = 0; i < sizeof(font); i++) memory[i] = font[i];
    for(i = sizeof(font); i < MEM_SIZE; i++) memory[i] = 0;
    for(i = 0; i < VMEM_Y_SIZE; i++) {
        for(j = 0; j < VMEM_X_SIZE; j++)
            video[i][j] = 0;
    }

    memreg = 0;
    PC = 0x200;
    SC = 0;
    timer_val = 0;
    cur_key = -1;
    key_reg = -1;
    srand(time(NULL));

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
        Error("SDL_Init");

    window = SDL_CreateWindow("8mu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H, 0);
    if(!window)
        Error("SDL_CreateWindow");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
        Error("SDL_CreateRenderer");

    SDL_TimerID tid = SDL_AddTimer(INTERVAL, timer_callback, NULL);
    if(!tid)
        Error("SDL_AddTimer");

}

Uint32 timer_callback(Uint32 interval, void *param) {
    if(timer_val > 0) timer_val--;
    return interval;
}

void event_handler(SDL_Event *event) {
    int i;

    if(event->type == SDL_QUIT) is_run = false;
    else if(event->type == SDL_KEYDOWN) {
        for(i = 0; i < 16; i++) {
            if(event->key.keysym.sym == keys[i]) {
                cur_key = i;
                break;
            }
        }
    } else if(event->type == SDL_KEYUP) {
        if(key_reg >= 0 && cur_key >= 0) {
            regs[key_reg] = cur_key;
            key_reg = -1;
        }
        cur_key = -1;
    }
}

void draw() {
    int i, j;
    uchar cell;
    SDL_Rect rect = {0,0,PIX_S,PIX_S};

    for(i = 0; i < VMEM_Y_SIZE; i++) {
        for(j = 0; j < VMEM_X_SIZE; j++) {
            cell = video[i][j]*255;
            SDL_SetRenderDrawColor(renderer, cell, cell, cell, 255);

            rect.y = PIX_S * i;
            rect.x = PIX_S * j;
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_RenderPresent(renderer);
}

Uint32 cmd_loop(Uint32 interval, void *param) {
    static uchar bytes[2];
    if(key_reg >= 0)  return interval;
    if(PC >= MEM_SIZE-1) return 0;

    bytes[0] = memory[PC];
    bytes[1] = memory[PC+1];
    PC+=2;

    exec_command(bytes, ops, sizeof(ops)/sizeof(OP));

    return interval;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fputs("Usage: 8mu <filename>", stderr);
        exit(EXIT_FAILURE);
    }

    init();

    char *msg = load_prog(argv[1]);
    if(msg) {
        fputs(msg, stderr);
        exit(EXIT_FAILURE);
    }

    SDL_TimerID clid = SDL_AddTimer(FREQ, cmd_loop, NULL); //cmd loop id
    if(!clid)
        Error("SDL_AddTimer");

    SDL_Event event;
    while(is_run) {
        while(SDL_PollEvent(&event)) {
            event_handler(&event);
        }
        draw();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
