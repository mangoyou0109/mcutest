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

#include "uart.h"

static int set_speed(int fd, int speed)
{
	struct termios tio;

	if (0 != tcgetattr(fd, &tio))
	{
     //printf("set_speed : tcgetattr call fail.\n");
		return -1;
	}

	tcflush(fd, TCIOFLUSH);

	switch (speed)
	{
	case 115200 :
		cfsetispeed(&tio, B115200);
	    cfsetospeed(&tio, B115200);
	    break;
	case 57600 :
		cfsetispeed(&tio, B57600);
	    cfsetospeed(&tio, B57600);
	    break;
	case 38400 :
		cfsetispeed(&tio, B38400);
	    cfsetospeed(&tio, B38400);
		break;
	case 9600 :
		cfsetispeed(&tio, B9600);
	    cfsetospeed(&tio, B9600);
		break;
	default:
		cfsetispeed(&tio, B19200);
	    cfsetospeed(&tio, B19200);
	}

	if  (0 != tcsetattr(fd, TCSANOW, &tio))
	{
      //printf("set_speed : tcsetattr call fail.\n");
		return -1;
	}

	tcflush(fd, TCIOFLUSH);

	return 0;
}


static int set_parity(int fd, int databits, int stopbits, int parity)
{
	struct termios tio;

	if (0 != tcgetattr(fd, &tio))
	{
       printf("set_parity : tcgetattr call fail.\n");
		return -1;
	}

	tio.c_cflag &= ~CSIZE;

	switch (databits)
	{
	case 7:
		tio.c_cflag |= CS7;
		break;
	case 8:
		tio.c_cflag |= CS8;
		break;
	default:
		printf("set_parity : Unsupported data size.\n");
		return -1;
	}

	switch (parity)
	{
	case 'n':
	case 'N':
		tio.c_cflag &= ~PARENB;    /* Clear parity enable */
		tio.c_iflag &= ~INPCK;     /* Clear parity checking */
		break;
	case 'o':
	case 'O':
		tio.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
		tio.c_iflag |= INPCK;             /* Disnable parity checking */
		break;
	case 'e':
	case 'E':
		tio.c_cflag |= PARENB;     /* Enable parity */
		tio.c_cflag &= ~PARODD;    /* 转换为偶效验*/
		tio.c_iflag |= INPCK;      /* Disnable parity checking */
		break;
	case 'S':
	case 's':  /*as no parity*/
		tio.c_cflag &= ~PARENB;
		tio.c_cflag &= ~CSTOPB;
		break;
	default:
		printf("set_parity : Unsupported parity.\n");
		return -1;
	}

	switch (stopbits)
	{
	case 1:
		tio.c_cflag &= ~CSTOPB;
		break;
	case 2:
		tio.c_cflag |= CSTOPB;
		break;
	default:
		printf("set_parity : Unsupported stop bits.\n");
		return -1;
	}

	// Raw Mode
	tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //input
	tio.c_oflag &= ~OPOST;//output

    tio.c_cflag |= (CLOCAL | CREAD);

	tio.c_iflag &= ~(ICRNL | PARMRK | IGNCR | IUCLC);// don't map CR to NL

	tio.c_iflag &= ~(IXON | IXOFF | IXANY);// cacnel flow control

	/* Set input parity option */
	if (parity != 'n')
		tio.c_iflag |= INPCK;

	tcflush(fd, TCIOFLUSH);

	tio.c_cc[VTIME] = 0;

	tio.c_cc[VMIN] = 0;

	if (0 != tcsetattr(fd, TCSANOW, &tio))
	{
//		printf("set_Parity : tcsetattr call fail.\n");

		return -1;
	}

	return 0;
}

int init_uart(char *path, int com_speed)
{
    int tc_fd;

    tc_fd = open(path, O_RDWR | O_NOCTTY);// | O_NDELAY);

    if (tc_fd < 0)
    {
        perror(path);
        printf("com_init : open %s error.\n",path);
        return -1;
    }

	if (-1 == set_speed(tc_fd, com_speed))
	{
       printf("com_init : set speed error.\n");
		return -1;
	}

	if (-1 == set_parity(tc_fd, 8, 1, 'N')) //
	{
        printf("com_init : set parity error.\n");
		return -1;
	}

	return tc_fd;
}



void uninit_uart(int fd)
{
    close(fd);
}
