/*
Sophia Gigliotti
CS140
Chat Client
Worked With: Kleinberg and Wilkens (connecting to the server)
Resources:
John Rodkey's Sample Code from Part 1
https://gist.github.com/jirihnidek/388271b57003c043d322
http://man7.org/linux/man-pages/man3/getaddrinfo.3.html
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

#define BUF_SIZE 1024
#define SERVER "10.115.20.250"
#define PORT 49153

//receive messages from the server and print them to
//the console
int receiveMsg(int sock){
  char rMsg[BUF_SIZE];
  bzero(rMsg,sizeof(rMsg));
  int rec = recv(sock, rMsg, sizeof(rMsg),0);
  printf("%s\n", rMsg);
  return rec;
}

int sendMsg(int sock){
  char *msg;
  char *sen;
  char *buffer, *originalbuffer;
  int sentMsg;
  int len;


  int quit = 0;
  //bzero(msg,sizeof(msg));
  while( !quit ){
    receiveMsg(sock);
    len = BUF_SIZE;
    buffer = malloc(len+1);
    originalbuffer = buffer;

    if( getline(&buffer, (size_t *) &len, stdin)>1){
      sentMsg = send(sock, buffer, strlen(msg),0);
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

        name = &argv[1];
        len = strlen(name);
        name[len] = '\n';
        name[len+1] = '\0';
        int sentUser = send(sock, name, sizeof(name),0);

        //use send and receive, some sort of while loop to get data

        //ask user for name
        /*char user[BUF_SIZE];
        printf("Please enter your username: ");
        fgets(user,BUF_SIZE,stdin);

        //send username to the server
        int sentUser = send(sock, &user, sizeof(user),0);
        printf("Welcome to the chat client. \n");
        printf("Enter messages to send. Send an empty message to quit. \n");
        */
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
