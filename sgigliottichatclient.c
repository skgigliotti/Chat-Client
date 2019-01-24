#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include <string.h>

//Resources: Kleinberg and Wilkens
//https://gist.github.com/jirihnidek/388271b57003c043d322
//http://man7.org/linux/man-pages/man3/getaddrinfo.3.html
int receiveMsg(int sock){
  char rMsg[256];
  int rec = recv(sock, rMsg, sizeof(rMsg),0);
  printf("From Server: %s\n", rMsg);
  return rec;
}

int connectToServer(){
    //create a socket
    //const struct in_addr add;
    int sock = socket(AF_INET,SOCK_STREAM,0);
    int BUF_SIZE = 100;
    //used to pass in the IP address
    struct in_addr add;
    struct sockaddr_in saddr;

    if(sock){
        //convert the IP address so that it can be passed into
        //the connection
        int inetWorked = inet_pton(AF_INET, "10.115.20.250", &add);
        printf("Successful conversion %d\n",inetWorked);

        //set fields
        saddr.sin_family = AF_INET;

        //set the port number
        saddr.sin_port = htons(49153);
        saddr.sin_addr = add;

        //connect to the socket
        int connection = connect(sock, (struct sockaddr*)&saddr, sizeof(saddr));

        printf("Connection: %d \n", connection);

        //use send and receive, some sort of while loop to get data

        //ask user for name
        char user[100];
        printf("Please enter your username: ");
        fgets(user,100,stdin);

        //send username to the server
        int sentUser = send(sock, &user, sizeof(user),0);
        printf("Sent: %d\n", sentUser);

        char msg[256];
        for(int i = 0; i < sizeof(msg); i++){
          msg[i] = ' ';
        }
        while(*msg != '\n'){
          for(int i = 0; i < sizeof(msg); i++){
            msg[i] = ' ';
          }
          fgets(msg,sizeof(msg),stdin);
          int sentMsg = send(sock, &msg, sizeof(msg),0);
          receiveMsg(sock);
        }

        //printf("%s \n", msg);
        //close socket
        int closeSock = close(sock);

        return closeSock;
    }
    return sock;
}



int main(int argc, char *argv[]){
    printf("Test\n");
    int check = connectToServer();
    printf("Socket closed: %d\n",check);
}
