/*
 * Copyright (C) 2020 , HSAE Inc.
 * @Author: Sun Ming
 * @Date: 2020-5
 * @Version: V1.0
 */
#ifndef __H_MCU_OPT__
#define __H_MCU_OPT__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <ctype.h>

#define DEV_SERIAL_PORT     "/dev/ttymxc1"
#define DEV_SERIAL_SPEED    38400

typedef enum _eMCUParType{
    MCU_NODE = 0,
    MCU_PAR_HELP = 1,
    MCU_PAR_FUNC,
    MCU_FUNC,
    MCU_PAR_CMD,
    MCU_CMD,
    MCU_PAR_DEV,
    MCU_DEV,
}eMCUParType;

int fd_init(int argc, char *argv[]);
void fd_free(int fd);
void mcu_help(char *name);
int help_check(int argc, char *argv[]);
int func_check(int fd, int argc, char *argv[]);
int run_cmd(int fd, int start, int argc, char *argv[]);
int cmd_check(int fd, int argc, char *argv[]);

#endif /* __H_MCU_OPT__ */
