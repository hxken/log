/*
 * serial_control.c
 *
 *  Created on: May 3, 2017
 *      Author: ken
 */
#include <termios.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "serial_control.h"

static int serial_speed[] = {B0, B50,B75,B110,B134, B150, B200, B300, B600,
		B1200, B1800, B2400, B4800, B9600, B19200, B38400//, B57600,  B115200, B230400
};


static struct termios old_option;
static int _fd = 0;
//--------------------------------------
// static function
static int set(int fd, int speed, int size, int stop, int parity, int cflow );


//-----------------------------------------
//------------ API -------------------------

/*Init("/dev/ttyS0", B_115200, 8, 1, 'N')*/

int serial_init(const char *_dev, SPEED _speed, int _size, int _stop,
		int _parity, int _cflow)
{
	_fd = open(_dev, O_RDWR);
	if (_fd < 0)
	{
		return -1;//open error
	}
	return set(_fd, _speed, _size, _stop, _parity, _cflow);
}

int serial_read(uint8_t *_msg, int _len)
{
	return (int)read(_fd, _msg, _len);
}

int serial_write(const uint8_t *_msg, int _len)
{
	return (int)write(_fd, _msg, _len);
}

void serial_close(void)
{
	tcsetattr(_fd, TCSANOW, &old_option);
	close(_fd);
}




static int set(int fd, int speed, int size, int stop, int parity, int cflow )
{
	struct termios term;

	if (tcgetattr(fd, &old_option) != 0) {
		return -1;
	}
	memcpy(&term, &old_option, sizeof(term));
	cfmakeraw(&term);

	//set baud rate
	cfsetispeed(&term, serial_speed[speed]);
	cfsetospeed(&term, serial_speed[speed]);

	//set data bit
	term.c_cflag &= ~CSIZE;
	switch (size)
  {
		case 5:
			term.c_cflag |= CS5;
			break;
		case 6:
			term.c_cflag |= CS6;
			break;
		case 7:
			term.c_cflag |= CS7;
			break;
		case 8:
			term.c_cflag |= CS8;
			break;
		default:
			return -1;
	}

	//set stop bit
	switch (stop) {
		case 1:
			term.c_cflag &= ~CSTOPB;
			break;
		case 2:
			term.c_cflag |= CSTOPB;
			break;
		default:
			return -1;
	}

	//set parity
	switch (parity)
  {
		//no parity
		case 'n':
		case 'N':
			term.c_cflag &= ~PARENB;
			term.c_iflag &= ~INPCK;
			term.c_cflag |= PARODD;
			break;
			//ODD parity
		case 'o':
		case 'O':
			term.c_cflag |= PARENB;
			term.c_cflag |= PARODD;
			term.c_iflag |= INPCK;
			break;
			//EVEN parity
		case 'e':
		case 'E':
			term.c_cflag |= PARENB;
			term.c_cflag &= ~PARODD;
			term.c_iflag |= INPCK;
			break;
			//space parity
		case 'S':
		case 's':
			term.c_cflag |= (PARENB | CMSPAR |PARODD);
			//term.c_cflag &= ~PARODD;
			term.c_iflag |= INPCK;
			break;
			//mark parity
		case 'M':
		case 'm':
			term.c_cflag |= (PARENB | CMSPAR);
			term.c_cflag &= ~PARODD;
			term.c_iflag |= INPCK;
			break;
		default:
			return -1;
	}

	//flow control
	switch (cflow)
  {
		//no flow
		case 'n':
		case 'N':
			term.c_cflag &= ~CRTSCTS;
			term.c_iflag &= ~(IXON | IXOFF | IXANY );
			break;
			//Software Flow
		case 's':
		case 'S':
			term.c_cflag &= ~CRTSCTS;
			term.c_iflag |= (IXON | IXOFF | IXANY );
			break;
			//Hardware Flow
		case 'h':
		case 'H':
			term.c_iflag &= ~(IXON | IXOFF | IXANY );
			term.c_cflag |= CRTSCTS;
			break;
		case 'a':
		case 'A':
			term.c_cflag |= CRTSCTS;
			term.c_iflag |= (IXON | IXOFF | IXANY );
			break;
		default:
			return -1;
	}
	//term.c_cflag |= (CLOCAL | CREAD);
	tcflush(fd, TCIFLUSH);
	term.c_cc[VTIME] = 0;
	term.c_cc[VMIN] = 1;

	term.c_lflag &= ~(ECHO|ICANON|ISIG);
	term.c_iflag &= ~(BRKINT|ICRNL);
	term.c_oflag &= ~OPOST;

	term.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
		| INLCR | IGNCR | ICRNL | IXON);
	term.c_oflag &= ~OPOST;
	term.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

	//set attr
	if (tcsetattr(fd, TCSANOW, &term) != 0)
  {
		return -1;
	}
	return 0;
}
