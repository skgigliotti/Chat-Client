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
#include<sys/socket.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<string.h>

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
  char msg[BUF_SIZE];
  int sentMsg;

  //bzero(msg,sizeof(msg));
  while(*msg != '\n'){
    bzero(msg,sizeof(msg));
    fgets(msg,sizeof(msg),stdin);
    strcat(msg, "\n");
    sentMsg = send(sock, &msg, strlen(msg),0);
    receiveMsg(sock);
  }
  return sentMsg;
}

//create socket, connect to the server, and close the server
int connectToServer(){

    //create a socket
    int sock = socket(AF_INET,SOCK_STREAM,0);
    //used to pass in the IP address
    struct in_addr add;

    //allocates enough room for one server
    struct sockaddr_in saddr;

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

        //use send and receive, some sort of while loop to get data

        //ask user for name
        char user[BUF_SIZE];
        printf("Please enter your username: ");
        fgets(user,BUF_SIZE,stdin);

        //send username to the server
        int sentUser = send(sock, &user, sizeof(user),0);
        printf("Welcome to the chat client. \n");
        printf("Enter messages to send. Send an empty message to quit. \n");

        //go to function to send messages
        sendMsg(sock);

        //close socket
        int closeSock = close(sock);

        return closeSock;
    }
    return sock;
}



int main(int argc, char *argv[]){

    int check = connectToServer();
    printf("Socket closed: %d\n",check);
}
