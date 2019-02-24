/*
Sophia Gigliotti
CS140
Chat Client
Worked With: Kleinberg and Wilkens (connecting to the server), Ryley Oroku (select),
James Bek (nCurses)
Resources:
John Rodkey's Sample Code from Part 1, helped with malloc
https://gist.github.com/jirihnidek/388271b57003c043d322
http://man7.org/linux/man-pages/man3/getaddrinfo.3.html

To help with nCurses
https://github.com/AngusG/tictactoe-ncurses-reinforcement-learning/blob/master/tictactoefunctions.c
https://www.youtube.com/watch?v=mK7KqX6IQfk
https://www.youtube.com/watch?v=TDVhJ0dkVo8

To help with Select
https://www.youtube.com/watch?v=qyFwGyTYe-M
https://www.gnu.org/software/libc/manual/html_node/Server-Example.html (specifically FD_CLR)
*/

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<time.h>
#include<errno.h>
#include<ncurses.h>
#include<sys/time.h>
#include<sys/select.h>
#include<sys/ioctl.h>

#define BUF_SIZE 1024
#define SERVER "10.115.20.250"
#define PORT 49153



int printToScreen(){
  return 0;
}
//create sub console/window to display messages

WINDOW* recWind(){
  int cols, rows;

  struct ttysize ts;
  ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
  cols = ts.ts_cols;
  rows = ts.ts_lines;

  int h = rows/2 - 10;
  int w = cols - 40;
  int y = 10;
  int x = 20;
  WINDOW *recWin, *recWinContainer;



  recWin = newwin(h,w,y,x);
  recWinContainer = newwin(h+10,w+10,y-5,x-5);
  start_color();
  init_pair(1,COLOR_BLUE, COLOR_WHITE);
  init_pair(5,COLOR_BLUE, COLOR_BLUE);
  wbkgd(recWinContainer, COLOR_PAIR(5));
  wbkgd(recWin, COLOR_PAIR(1));

  box(recWinContainer,0,0);
  wrefresh(recWinContainer);
  wrefresh(recWin);
  return recWin;
}

WINDOW* sendWind(){
  int cols, rows;

  struct ttysize ts;
  ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
  cols = ts.ts_cols;
  rows = ts.ts_lines;

  int h = (rows/2 - 15) / 2;
  int w = cols - 40;
  int y = rows/2 + 12;
  int x = 20;
  WINDOW *sWin, *sWinContainer;

  sWin = newwin(h,w,y,x);
  sWinContainer = newwin(h+10,w+10,y-5,x-5);
  start_color();
  init_pair(2,COLOR_GREEN, COLOR_WHITE);
  init_pair(4,COLOR_GREEN, COLOR_GREEN);
  wbkgd(sWinContainer, COLOR_PAIR(4));
  wbkgd(sWin, COLOR_PAIR(2));

  box(sWinContainer,0,0);
  wrefresh(sWinContainer);
  wrefresh(sWin);
  return sWin;
}

int mainWind(){

  int h = 10;
  int w = 80;
  int y = 30;
  int x = 0;
  initscr();
  cbreak(); //maybe change to raw

  start_color();
  init_pair(3, COLOR_RED, COLOR_BLACK);
  attron(COLOR_PAIR(3));
  bkgd(COLOR_PAIR(3));
  refresh();


  return 0;

}

//receive messages from the server and print them to
//the console
int receiveMsg(int sock, char *buff, WINDOW *msgWind){


  int len;
  int rec;
  int rMsg;
  fd_set read, activeRead;
  struct timeval time;

  len = BUF_SIZE;
  FD_ZERO(&read);
  FD_SET(sock, &read);
  time.tv_sec = 2;
  time.tv_usec = 0;
  //FD_ISSET

  while(1){

    //wrefresh(msgWind);
    read = activeRead;

    rMsg = select(2, &read, NULL, NULL, &time);

    buff = malloc(len+1);

    if(rMsg < 0){
      printf("timeoutR \n");
      return 0;
    }

    else{
      if(rec == -1){
        if(errno == EAGAIN){
          break;
        }
    }
      rec = recv(sock, buff, BUF_SIZE ,0);
      scrollok(msgWind,TRUE);
      wprintw(msgWind, buff);
      wrefresh(msgWind);
      free(buff);
    }
    FD_CLR(0,&activeRead);


}
  return rec;
}

int sendMsg(int sock){
  char *msg;
  char *sen;
  char *buffer, *originalbuffer;
  int sentMsg;
  int len;
  int wMsg;

  fd_set write, activeWrite;
  struct timeval time;

  int quit = 0;



  mainWind();

  WINDOW *sWind = sendWind();

  WINDOW *msgWind = recWind();

  refresh();

  FD_ZERO(&write);
  FD_SET(sock, &write);

  time.tv_sec = 2;
  time.tv_usec = 0;
  len = BUF_SIZE;

  while( !quit ){


    receiveMsg(sock, buffer, msgWind);

    write = activeWrite;

    wMsg = select(2, &write, NULL, NULL, &time);

    buffer = malloc(len+1);

    originalbuffer = buffer;

    //check if server is writing to socket descriptor, and we are trying to read

    //wrefresh(sWind);
    if( wMsg < 0 ){
      printf("timeoutW\n");
      break;
    }
    else{

        wgetstr(sWind, originalbuffer);
        wrefresh(sWind);
        quit = (strcmp (buffer,"quit\n") == 0);

        sentMsg = send(sock, buffer, strlen(buffer),0);
        //char * yn = (char) sentMsg;
        //wprintw(sWind, buffer);
        wclear(sWind);
        wrefresh(sWind);



    }
    FD_CLR(0,&activeWrite);
    quit = (strcmp (buffer,"quit\n") == 0);
    free(originalbuffer);

  }
  endwin();
  return sentMsg;
}

//create socket, connect to the server, and close the server
int connectToServer(char *argv, int argc){
    int len;
    char *name;
    struct timeval timev;
    //create a socket
    int sock = socket(AF_INET,SOCK_STREAM,0);
    //used to pass in the IP address
    struct in_addr add;

    //allocates enough room for one server
    struct sockaddr_in saddr;

    /* Set up recv timeout for .5 sec */
    timev.tv_sec = 0;
    timev.tv_usec = 1000 * 500;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timev, sizeof(timev));


    if(sock){

        //convert the IP address so that it can be passed into
        //the connection
        int inetWorked = inet_pton(AF_INET, SERVER, &add);
        if(!inetWorked){
          printf("Cannot convert IP address.");
        }
        //set fields
        saddr.sin_family = AF_INET;

        //set the port number
        saddr.sin_port = htons(PORT);
        saddr.sin_addr = add;

        //connect to the socket
        int connection = connect(sock, (struct sockaddr*)&saddr, sizeof(saddr));
        if( connection != 0 ){
          printf("Error: Cannot connect to the server.");
        }

        if( argc < 2 ){
          printf("Usage: chat-client <screenname>\n");
          exit(1);
        }

        name = &argv[1];
        len = strlen(name);
        name[len] = '\n';
        name[len+1] = '\0';
        int sentUser = send(sock, name, sizeof(name),0);
        printf("sent user\n");
        printf("ok\n" );
        //send username to the server
        printf("%d\n", sentUser);


        //go to function to send messages
        //sendMsg(sock);

        //endwin(); //frees memory from initscr and closes the class

        //close socket
        int closeSock = close(sock);

        return closeSock;
    }
    return sock;
}



int main(int argc, char *argv[]){

    int check = connectToServer(*argv,argc);
    printf("Socket closed: %d\n",check);

}
