/*
Sophia Gigliotti
CS140
Chat Client
Worked With: Kleinberg and Wilkens (connecting to the server)
Resources:
John Rodkey's Sample Code from Part 1, helped with malloc
https://gist.github.com/jirihnidek/388271b57003c043d322
http://man7.org/linux/man-pages/man3/getaddrinfo.3.html

To help with nCurses
https://github.com/AngusG/tictactoe-ncurses-reinforcement-learning/blob/master/tictactoefunctions.c
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
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 1024
#define SERVER "10.115.20.250"
#define PORT 49153




//create sub console/window to display messages
int secondWind(){
  initscr();

  int x;
  int y;

  for( y=0 ; y <=10; y++){
    mvaddch(y, 0, '|');
    mvaddch(y,12, '|');
  }


  return 1;

}

//receive messages from the server and print them to
//the console
int receiveMsg(int sock, char *buff){


  int len;
  int rec;
  int rMsg;
  fd_set read;
  struct timeval time;

  len = BUF_SIZE;

  while(1){
    FD_ZERO(&read);
    FD_SET(sock, &read);

    time.tv_sec = 5;
    time.tv_usec = 0;

    buff = malloc(len+1);

    rec = recv(sock, buff, sizeof(buff),0);

    if(rec == -1){
      if(errno == EAGAIN){
        break;
      }

  }

    rMsg = select(2, &read, NULL, NULL, &time);

    if(rMsg == 0){
      printf("timeout \n");
      return 0;
    }

    else{
      printf("%s", buff);
    }

    free(buff);
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

  fd_set write;
  struct timeval time;


  int quit = 0;

  while( !quit ){
    receiveMsg(sock, buffer);
    len = BUF_SIZE;

    buffer = malloc(len+1);

    originalbuffer = buffer;

    FD_ZERO(&write);
    FD_SET(sock, &write);

    time.tv_sec = 5;
    time.tv_usec = 0;

    wMsg = select(2, NULL, &write, NULL, &time);
    if(wMsg == 0){
      printf("Timeout \n");
    }
    else{
      if( getline(&buffer, (size_t *) &len, stdin)>1 ){

        sentMsg = send(sock, buffer, strlen(buffer),0);

      }
    }

    quit = (strcmp (buffer,"quit\n") == 0);
    free(originalbuffer);

  }
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

        name = &argv[8];
        len = strlen(name);
        name[len] = '\n';
        name[len+1] = '\0';
        int sentUser = send(sock, name, sizeof(name),0);

        //send username to the server
        printf("%d\n", sentUser);


        //go to function to send messages
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
