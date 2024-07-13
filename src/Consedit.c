#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

#define CTRL_KEY(k) ((k) & 0x1f)

struct termios orig_termios;

char editorReadkey(){
    int nread;
    char c;
    while((nread=read(STDIN_FILENO,&c,1)) != 1){
        if(nread == -1 && errno != EAGAIN)
            die("read");
    }
    return c;
}

void die (const char* s){
    perror(s);
    exit(1);
}

void editorProcessKeypress(){
    char c = editorReadkey();

    switch (c)
    {
    case CTRL_KEY('q'):
        write(STDOUT_FILENO,"\x1b[2J",4);
        write(STDOUT_FILENO,"xib[h",3);
        exit(0);
        break;
    }
}

void editorrefreshscreen(){
    write(STDOUT_FILENO,"\x1b[2J",4);
    write(STDOUT_FILENO,"xib[h",3);
}

void disableRawmode(){
    if(tcsetattr(STDIN_FILENO,TCSAFLUSH, &orig_termios)==-1)
        die("tcsetatte");
}


void enableRawmode(){
    if(tcgetattr(STDIN_FILENO,&orig_termios)==-1) die("tcgetattr");
    atexit(disableRawmode);

    struct termios raw = orig_termios;


    raw.c_iflag &= ~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO,TCSAFLUSH, &raw)==-1) die("tcsetattr");
}

int main()
{
    enableRawmode();

    while(1){
        editorrefreshscreen();
        editorProcessKeypress();
    }
    return 0;
}