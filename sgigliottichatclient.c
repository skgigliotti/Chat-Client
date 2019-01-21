#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>

//Resources: Kleinberg and Wilkins
//https://gist.github.com/jirihnidek/388271b57003c043d322
//http://man7.org/linux/man-pages/man3/getaddrinfo.3.html

int connectToServer(){
    //create a socket
    //const struct in_addr add;
    int sock = socket(AF_INET,SOCK_STREAM,0);
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
        //close socket
        int closeSock = close(sock);

        return closeSock;
    }
    return sock;
}

int main(int argc, char *argv[]){
    printf("Test\n");
    int check = connectToServer();
    printf("Socket closed %d\n",check);
}
