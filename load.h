#pragma once

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "global.h"

uchar memory[MEM_SIZE];

char* load_prog(char *filename); //return error
