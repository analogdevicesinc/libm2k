#if !defined(_WIN32) && !defined(__CYGWIN__)

#include "conio.h"
#include <termios.h>
#include <stdio.h>
#include <unistd.h>

int _getch()
{
	struct termios oldattr, newattr;
	int ch;
	tcgetattr( STDIN_FILENO, &oldattr );
	newattr = oldattr;
	newattr.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
	return ch;
}

#endif
