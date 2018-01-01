#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <pthread.h>

void *recieveMessage(void *sock)
{
    int serverSock = *((int*)sock);
    char msg[500];
    int len;
    while((len = recv(serverSock, msg, 500, 0)) > 0)
    {
        msg[len] = '\0';
        fputs(msg,stdout);
        memset(msg, '\0', sizeof(msg));
    }
}

int main(int argc, char *argv[]) {
   
    int sock, n, len;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000], msg[500], res[500], username[50], connected[100];
    pthread_t sendt, recvt;
    char ip[INET_ADDRSTRLEN];
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8000 );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    inet_ntop(AF_INET, (struct sockaddr *)&server, ip, INET_ADDRSTRLEN);
    
    printf("Connected to %s\n", ip);

    printf("Enter your username: ");
    scanf("%s", username);
    
    pthread_create(&recvt,NULL,recieveMessage,&sock);
    
    while(fgets(msg,500,stdin) > 0)
    {
        strcpy(res,username);
        strcat(res,":");
        strcat(res,msg);
        if(write(sock, res, strlen(res)) < 0)
        {
            perror("Send Failed");
            exit(1);
        }
        memset(msg, '\0', sizeof(msg));
        memset(res, '\0', sizeof(res));
    }
    pthread_join(recvt,NULL);

    close(sock);
    return 0;
}