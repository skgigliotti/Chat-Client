/*
Sophia Gigliotti
CS140
Chat Client
Worked With: Kleinberg and Wilkens (connecting to the server), Ryley Oroku (select),
James Bek (nCurses)

Resources:
John Rodkey's Sample Code from Part 1, helped with malloc specifically

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


//create sub console/window to display messages from the server
//this window will be above and is colored blue
WINDOW* recWind(){
  int cols, rows;

  struct ttysize ts;
  ioctl(STDIN_FILENO, TIOCGSIZE, &ts);

  //get the number of coloumns and rows of the
  //terminal to render the boxes and size them
  //accordingly
  cols = ts.ts_cols;
  rows = ts.ts_lines;

  int h = rows/2 - 10;
  int w = cols - 40;
  int y = 10;
  int x = 20;

  //create window pointers
  //recWin is the inner white window where msgs are displayed
  //recWinContainer provides a nice colored border for the rec window
  WINDOW *recWin, *recWinContainer;

  recWin = newwin(h,w,y,x);
  recWinContainer = newwin(h+10,w+10,y-5,x-5);

  start_color();
  init_pair(1,COLOR_BLUE, COLOR_WHITE);
  init_pair(5,COLOR_BLUE, COLOR_BLUE);
  wbkgd(recWinContainer, COLOR_PAIR(5));
  wbkgd(recWin, COLOR_PAIR(1));

  box(recWinContainer,0,0);

  //refresh to render the windows
  wrefresh(recWinContainer);
  wrefresh(recWin);
  return recWin;
}

//create sub console/window to send messages to the server
//this window will be below and is colored green
WINDOW* sendWind(){
  int cols, rows;

  //get the number of coloumns and rows of the
  //terminal to render the boxes and size them
  //accordingly
  struct ttysize ts;
  ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
  cols = ts.ts_cols;
  rows = ts.ts_lines;
  int h = 2;
  int w = cols - 40;
  int y = rows/2 + 12;
  int x = 20;

  //create window pointers
  //recWin is the inner white window where msgs are displayed
  //recWinContainer provides a nice colored border for the rec window
  WINDOW *sWin, *sWinContainer;

  sWin = newwin(h,w,y,x);
  sWinContainer = newwin(h+6,w+6,y-3,x-3);
  start_color();
  init_pair(2,COLOR_GREEN, COLOR_WHITE);
  init_pair(4,COLOR_GREEN, COLOR_GREEN);
  wbkgd(sWinContainer, COLOR_PAIR(4));
  wbkgd(sWin, COLOR_PAIR(2));

  box(sWinContainer,0,0);

  //refresh to render the windows
  wrefresh(sWinContainer);
  wrefresh(sWin);
  return sWin;
}

//create the main screen to hold the other 4 windows
int mainWind(){

  //create the screen and enable cbreak
  initscr();
  cbreak();

  start_color();
  init_pair(3, COLOR_RED, COLOR_BLACK);
  attron(COLOR_PAIR(3));
  bkgd(COLOR_PAIR(3));
  refresh();


  return 0;

}

//receive messages from the server and print them to
//the console
int receiveMsg(int sock, char *buff, WINDOW *msgWind, int sendM){


  int len;
  int rec;
  int rMsg;
  int m;
  fd_set read, activeRead;
  struct timeval time;

  //allocate memory to store received messages in
  //that come from the server
  buff = malloc(len+1);

  //receive and set data in buff
  rec = recv(sock, buff, BUF_SIZE ,0);

  if( rec == -1 ){
    if( errno == EAGAIN ){

    }
  }


  else{


    //allow the screen to scroll if the
    //messages exceed the actual screen size
    scrollok(msgWind,TRUE);

    //print the messages to the screen and
    //refresh to render
    wprintw(msgWind, buff);
    wrefresh(msgWind);
  }
  //free the message from memory
  free(buff);

  return rec;
}

int sendMsg(int sock){
  char *msg;
  char *sen;
  char *buffer, *name;
  int sentMsg;
  int len;
  int s;

  fd_set write, activeWrite;
  struct timeval time;

  int quit = 0;



  mainWind();


  //render the pairs of sub windows
  WINDOW *sWind = sendWind();

  WINDOW *msgWind = recWind();

  refresh();


  //set time for select to switch after 1 second
  //so that messages can be received in real time
  time.tv_sec = 1;
  time.tv_usec = 0;

  len = BUF_SIZE;

  //allocate memory for the username
  name = malloc(len+1);

  //prompt the user to enter their username and send it to the server
  wprintw(sWind, "Please enter your username: ");
  wrefresh(sWind);
  wgetstr(sWind, name);
  wrefresh(sWind);
  strcat(name, "\n");

  //send username to the server
  int sentUser = send(sock, name, strlen(name),0);
  wclear(sWind);
  wrefresh(sWind);

  //free the memory from the name variable
  free(name);

  //loop through sending and receiving until the user sends 'quit'
  while( !quit ){

    //reset the fd select variable each iteration through the loop
    FD_ZERO(&write);
    FD_SET(0, &write);
    FD_SET(sock, &write);

    s = select(sock+1, &write, NULL, NULL, &time);

    //if the fd is set, receive the message
    if( FD_ISSET(sock, &write) ){
      receiveMsg(sock, buffer, msgWind, 0);
    }

    wrefresh(msgWind);

    //if someone is typing and 0 is set, send messages to the server
    if( FD_ISSET(0, &write) ){

      //allocate memory for sending messages
      buffer = malloc(len+1);

      //scroll if the user is writing too much
      scrollok(sWind,TRUE);

      //get the user input and send to the server
      wgetstr(sWind, buffer);

      strcat(buffer,"\n");
      wrefresh(sWind);
      sentMsg = send(sock, buffer, strlen(buffer),0);

      //check if the user entered quit
      //if so, exit the while loop and close the socket
      quit = (strcmp (buffer,"quit\n") == 0);

      wrefresh(sWind);
      wclear(sWind);
      wrefresh(sWind);

      wrefresh(msgWind);
      free(buffer);

    }


  }

  FD_CLR(0,&activeWrite);

  endwin();
  return sentMsg;
}

//create socket, connect to the server, and close the server
int connectToServer(char *argv, int argc){
  int len;
  char *name;
  struct timeval timev;

  //create a socket file descriptor
  int sock = socket(AF_INET,SOCK_STREAM,0);

  //used to pass in the IP address
  struct in_addr add;

  //allocate enough room for one server
  struct sockaddr_in saddr;

  //Set up recv timeout for .5 sec
  timev.tv_sec = 0;
  timev.tv_usec = 1000 * 500;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timev, sizeof(timev));


  if( sock ){

    //convert the IP address so that it can be passed into
    //the connection
    int inetWorked = inet_pton(AF_INET, SERVER, &add);
    if( !inetWorked ){
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

    //go to take in user name and
    //to send messages
    sendMsg(sock);

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
