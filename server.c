#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

//the thread function
void *connection_handler(void *);

struct clientInfo {
    int sockNo;
    char ip[INET_ADDRSTRLEN];
    char username[50];
};
int clients[32];
int n = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void sendToAll(char *msg, int curr)
{
    int i;
    pthread_mutex_lock(&mutex);
    for(i = 0; i < n; i++){
        if(clients[i] != curr){
            if(send(clients[i],msg,strlen(msg),0) < 0){
                perror("sending failure");
                continue;
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *recieveMessage(void *sock)
{
    struct clientInfo cI = *((struct clientInfo *)sock);
    char msg[500], connected[500];
    int len;
    int i;
    int j;
    while((len = recv(cI.sockNo,msg,500,0)) > 0){
        msg[len] = '\0';
        for(int k = 0; k < strlen(msg); k++){
            if(msg[k] == ':'){
                if(msg[k+2] == '\0'){
                    strncpy(connected, msg, k);
                    strcpy(cI.username,connected);
                    strcat(connected, " has joined the group chat!\n");
                    sendToAll(connected,cI.sockNo);
                }
                else{
                    sendToAll(msg,cI.sockNo);
                }
            }
        }
        printf("Client[%d]: %s", cI.sockNo, msg);
        memset(msg,'\0', sizeof(msg));
        memset(connected,'\0', sizeof(connected));
    }
    pthread_mutex_lock(&mutex);
    printf("%s disconnected\n",cI.ip);
    for(i = 0; i < n; i++){
        if(clients[i] == cI.sockNo){
            j = i;
            while(j < n-1){
                clients[j] = clients[j+1];
                j++;
            }
        }
    }
    n--;
    pthread_mutex_unlock(&mutex);
}
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c, *new_sock;
    struct sockaddr_in server , client;
    pthread_t sendt, recvt;
    char ip[INET_ADDRSTRLEN];
    struct clientInfo cI;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8000 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        exit(1);
    }
    puts("bind done");
     
    //Listen
    if(listen(socket_desc , 5) != 0)
    {
        perror("listening unsuccessful");
        exit(1);
    }     
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	
	
    while(1)
    {
        if((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) < 0){
            perror("Accept Unsuccessful");
            exit(1);
        }
        pthread_mutex_lock(&mutex);
        inet_ntop(AF_INET, (struct sockaddr *)&client, ip, INET_ADDRSTRLEN);
        printf("%s connected\n",ip);
        cI.sockNo = client_sock;
        strcpy(cI.ip,ip);
        clients[n] = client_sock;
        n++;
        pthread_create(&recvt,NULL,recieveMessage,&cI);
        pthread_mutex_unlock(&mutex);
    }
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size, num;
    char *message , client_message[2000];
    char welcome[1000] = "Welcome to our chat app!\n";

    num = sock;
    num--;

    write(sock, welcome, strlen(welcome));

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        printf("Client[%d] > %s", sock, client_message);
        while(num > 3)
        {
            if(num != sock)
            {
                write(num , client_message , strlen(client_message));
                num--;
            }
        }  
        memset(client_message,0,sizeof client_message);
    }
    
     
    if(read_size == 0)
    {
        printf("Client[%d] disconnected\n", sock);
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
} 