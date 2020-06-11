#ifndef _UART_H_
#define _UART_H_

int init_uart(char *path, int com_speed);
void uninit_uart(int fd);

#endif
