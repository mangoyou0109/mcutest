/*
 * Copyright (C) 2020 , HSAE Inc.
 * @Author: Sun Ming
 * @Date: 2020-5
 * @Version: V1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "mcuopt.h"

int main(int argc, char *argv[])
{
    if(argc < 2){
        mcu_help(argv[0]);
        return -1; 
    }else{
        if(!help_check(argc, argv)){
            return 1; 
        }
        int fd = fd_init(argc, argv);
        if(fd != -1)
        {
            if(!func_check(fd, argc, argv)){
                fd_free(fd);
                return 0; 
            }
            if(!cmd_check(fd, argc, argv))
            {
                fd_free(fd);
                return 0; 
            }
           fd_free(fd);
        }
    }
    mcu_help(argv[0]);
    return -1;
}
