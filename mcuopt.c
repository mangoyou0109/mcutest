/*
 * Copyright (C) 2020 , HSAE Inc.
 * @Author: Sun Ming
 * @Date: 2020-5
 * @Version: V1.0
 */
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
#include "uart.h"
#include "mcuopt.h"

char mcu_reset[]        = {0xE0, 0xD4, 0x01, 0x03, 0x00};
char mcu_audio_open[]   = {0XE0, 0X01, 0X00, 0X00};
char mcu_audio_add[]    = {0XE0, 0X31, 0X01, 0X01, 0X00};
char mcu_audio_sub[]    = {0XE0, 0X31, 0X01, 0X00, 0X00};
char mcu_audio_fm1[]    = {0XE0, 0X20, 0X01, 0X00, 0X00};
char mcu_audio_fm2[]    = {0XE0, 0X20, 0X01, 0X01, 0X00};
char mcu_audio_fm3[]    = {0XE0, 0X20, 0X01, 0X02, 0X00};
char mcu_audio_fm4[]    = {0XE0, 0X20, 0X01, 0X03, 0X00};
char mcu_audio_am1[]    = {0XE0, 0X20, 0X01, 0X04, 0X00};
char mcu_audio_am2[]    = {0XE0, 0X20, 0X01, 0X05, 0X00};
char mcu_audio_am3[]    = {0XE0, 0X20, 0X01, 0X06, 0X00};
char mcu_audio_aux[]    = {0XE0, 0X20, 0X01, 0X09, 0X00};
char mcu_audio_usb[]    = {0XE0, 0X20, 0X01, 0X0A, 0X00};
char mcu_audio_video[]  = {0XE0, 0X20, 0X01, 0X0B, 0X00};
char mcu_audio_carplay[] = {0XE0, 0X20, 0X01, 0X0C, 0X00};
char mcu_audio_hfp[]    = {0XE0, 0X20, 0X01, 0X0D, 0X00};
char mcu_audio_a2dp[]   = {0XE0, 0X20, 0X01, 0X0E, 0X00};
char mcu_audio_ipod[]   = {0XE0, 0X20, 0X01, 0X10, 0X00};

void print_func_list()
{
    printf("Please select:\n");
    printf("\t0: Reset power\n");
    printf("\t1: Audio open\n");
    printf("\t2: Audio ++\n");
    printf("\t3: Audio --\n");
    printf("\t4: To USB audio\n");
    printf("\t5: To HFP audio\n");
    printf("\t6: To A2DP audio\n");
}

void mcu_help(char *name)
{
    printf("Usage: %s [[par] <value>] ...\n", name);
    printf(" [par]: <value>\n");
    printf("    -h: help. Eg. mcutest -h\n");
    printf("    -d: uart node. Eg. mcutest -d /dev/ttymxc1\n");
    printf("    -f: function list. Eg. mcutest -f\n");
    printf("    -c: cmds. Eg. mcutest -c 0xE0 0xD4 0x01 0x03 0x00\n");
}

//字符串转16进制数
unsigned long strtohex(char* str)
{
    unsigned long var = 0;
    if(!str){
        return 0; 
    }
    if(*str =='0'){
        str++;
        if((*str == 'X')||(*str == 'x')){
            str++;
        }
    }
    for (; *str; str++){
        var <<= 4;
        if(*str>='A' && *str <='F'){
            var |= *str-55;
        }else if(*str>='a' && *str <='f'){
            var |= *str-87;
        }else if(*str>='0' && *str <='9'){
            var |= *str-48;
        }else{
            break;
        }
    }
    return var;
}


//判断是否为自然数
int is_natural_number(char *str)
{
    int flg = 1;
     for (; *str; str++){
        if(*str < '0' || *str > '9'){
            flg = 0;
            break;
        }
    }
    return flg;
}

//判断是否为16进制数
int is_hex_number(char *str)
{
    int flg = 1;

    if(!str){
        return 0; 
    }
    if(*str =='0'){
        str++;
        if((*str == 'X')||(*str == 'x')){
            str++;
        }else{
            flg = 0;  
            return flg;
        }
    }
    for (; *str; str++){
        if((*str>='A' && *str <='F')
        ||(*str>='a' && *str <='f')
        ||(*str>='0' && *str <='9')){
            flg = 1;
        }else{
            flg = 0;
            break;
        }
    }
    return flg;
}


int mcu_send(int fd, char *msg, int size)
{
#if 1
    printf("To: ");
    for(int i = 0; i < size; i++){
        printf("0X%02X ", msg[i]);  
    }
    printf("\n");
#endif
    return write(fd, msg, size);
}


//参数解析
int par_parse(char *par)
{
    if((*par == '/') || (*par == 't')){
        return MCU_DEV;
    }else if(!strncasecmp(par, "0X", 2)){
        return MCU_CMD;
    }else if(*par == '-' || (*par == '_')){
        par++;
        if(!strncasecmp(par, "H", 1)){
            return MCU_PAR_HELP;
        }else if(!strncasecmp(par, "F", 1)){
            return MCU_PAR_FUNC;
        }else if(!strncasecmp(par, "D", 1)){
            return MCU_PAR_DEV;
        }else if(!strncasecmp(par, "C", 1)){
            return MCU_PAR_CMD;
        }
    }
    return MCU_NODE;
}

//校验位计算
unsigned char mcu_sum(unsigned char * str, int size)
{
    for(int i = 1; i < size-1; i++){
        if(i == 1){
            str[size-1] = str[i]; 
        }else{
            str[size-1] ^= str[i]; 
        }
    }
    return str[size-1];
}

void run_func_list(int fd, char *argv)
{
    int cmd = atoi(argv);
    switch(cmd){
        case 0: {
            int len =sizeof(mcu_reset)/sizeof(mcu_reset[0]); 
            mcu_sum((unsigned char *)mcu_reset, len);
            mcu_send(fd, (char *)mcu_reset, len);   
        }break;
        case 1: {
            int len =sizeof(mcu_audio_open)/sizeof(mcu_audio_open[0]); 
            mcu_sum((unsigned char *)mcu_audio_open, len);
            mcu_send(fd, (char *)mcu_audio_open, len);   
        }break;
        case 2:{
            int len =sizeof(mcu_audio_add)/sizeof(mcu_audio_add[0]); 
            mcu_sum((unsigned char *)mcu_audio_add, len);
            mcu_send(fd, (char *)mcu_audio_add, len);   
        }break;
        case 3:{
            int len =sizeof(mcu_audio_sub)/sizeof(mcu_audio_sub[0]); 
            mcu_sum((unsigned char *)mcu_audio_sub, len);
            mcu_send(fd, (char *)mcu_audio_sub, len);   
        }break;
        case 4:{
            int len =sizeof(mcu_audio_usb)/sizeof(mcu_audio_usb[0]); 
            mcu_sum((unsigned char *)mcu_audio_usb, len);
            mcu_send(fd, (char *)mcu_audio_usb, len);   
        }break;
        case 5:{
            int len =sizeof(mcu_audio_hfp)/sizeof(mcu_audio_hfp[0]); 
            mcu_sum((unsigned char *)mcu_audio_hfp, len);
            mcu_send(fd, (char *)mcu_audio_hfp, len);   
        }break;
        case 6:{
            int len =sizeof(mcu_audio_a2dp)/sizeof(mcu_audio_a2dp[0]); 
            mcu_sum((unsigned char *)mcu_audio_a2dp, len);
            mcu_send(fd, (char *)mcu_audio_a2dp, len);   
        }break;
        default:break;
    }
}

//串口初始化
int fd_init(int argc, char *argv[])
{
    int fd = -1;
    int i = 0;
    for(i = 1; i < argc; i++){
        if(par_parse(argv[i]) == MCU_PAR_DEV){
            if(i+1 < argc){
                if(par_parse(argv[i+1]) == MCU_DEV){
                    fd = init_uart(argv[i+1], DEV_SERIAL_SPEED);
                    goto re_fd;
                }else{
                    fd =init_uart(DEV_SERIAL_PORT, DEV_SERIAL_SPEED);
                    goto re_fd;
                }
            }else{
                fd =init_uart(DEV_SERIAL_PORT, DEV_SERIAL_SPEED);
                goto re_fd;
            }
        }else if(par_parse(argv[i]) == MCU_DEV){
            fd = init_uart(argv[i], DEV_SERIAL_SPEED);
            goto re_fd;
        }
    }
    if(i == argc){
        fd =init_uart(DEV_SERIAL_PORT, DEV_SERIAL_SPEED);
    }
re_fd:
    return fd;
}
//串口释放
void fd_free(int fd)
{
    uninit_uart(fd);
}

//检查帮助项
int help_check(int argc, char *argv[])
{
    for(int i=1; i < argc; i++){
        if(par_parse(argv[i]) == MCU_PAR_HELP){
            mcu_help(argv[0]);
            return 0;
        }
    }
    return -1;
}



//查检功能项
int func_check(int fd, int argc, char *argv[])
{
    if(fd == -1){
        return -1;
    }
    for(int i = 1; i < argc; i++){
        if(par_parse(argv[i]) == MCU_PAR_FUNC){
            if(i+1 < argc){
                if(is_natural_number(argv[i+1])){
                    run_func_list(fd, argv[i+1]); 
                    return 0;
                }
            }
            char buff[255];
            print_func_list();
            fgets(buff, 255, stdin);
            if (isdigit(buff[0])){
                run_func_list(fd, buff);
                return 0;
            }
        }
    }
    return -1;
}


//执行命令
int run_cmd(int fd, int start, int argc, char *argv[])
{
    int len = strtohex(argv[start+2]);
    unsigned char *packet = (unsigned char *)malloc(len+4);
    packet[0] = strtohex(argv[start]);      //start bit
    packet[1] = strtohex(argv[start+1]);    //cmd-id
    packet[2] = len;                        //length
    packet[3+len] = packet[1]^packet[2];    //check bit
    for(int i=0; i<len; i++)
    {
        packet[3+i] = strtohex(argv[start+3+i]);
        packet[3+len] ^= packet[3+i];
    }
    mcu_send(fd, (char *)packet, len+4);
    free(packet);
    return 0;
}

//检查命令项
int cmd_check(int fd, int argc, char *argv[])
{
    for(int i = 1; i < argc; i++){
        if(par_parse(argv[i]) == MCU_PAR_CMD){
            i++;
            run_cmd(fd, i, argc, argv);
            return 0; 
        }else if(par_parse(argv[i]) == MCU_CMD){
            run_cmd(fd, i, argc, argv);
            return 0; 
        }
    }
    return -1;
}


