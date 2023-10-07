#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8878

using namespace std;


void send_image(int* socket_desc)
{
        FILE *picture;
        int size, read_size, stat, packet_index;
        char read_buffer[256];
        packet_index = 1;
        picture = fopen("/var/www/html/live/SD.jpg", "r");
        printf("Getting Picture Size\n");
        if(picture == NULL) {
                printf("Error Opening Image File"); }
        fseek(picture, 0, SEEK_END);
        size = ftell(picture);
        fseek(picture, 0, SEEK_SET);
        printf("Total Picture size: %i\n",size);
        char send_buffer[10240];
        //Send Picture Size
        while(!feof(picture)) {
            //Read from the file into our send buffer
            read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);
            //Send data through our socket
            do{
                    stat = write(*socket_desc, send_buffer, read_size);
            }while (stat < 0);
            printf("Packet Number: %i\n",packet_index);
            printf("Packet Size Sent: %i\n",read_size);
            printf(" \n");
            printf(" \n");
            packet_index++;
            //      /Zero out our send buffer
            bzero(send_buffer, sizeof(send_buffer));
        }   
}

void send_image_string(int* socket_desc)
{
        FILE *picture;
        int size, read_size, stat, packet_index;
        char read_buffer[256];
        packet_index = 1;
        picture = fopen("/var/www/html/live/SD.jpg", "r");
        printf("Getting Picture Size\n");
        if(picture == NULL) {
                printf("Error Opening Image File"); }
        fseek(picture, 0, SEEK_END);
        size = ftell(picture);
        fseek(picture, 0, SEEK_SET);
        printf("Total Picture size: %i\n",size);
        string size_string = to_string(size);
        char send_buffer[10241];
        //Send Picture Size
        printf("Sending Picture Size\n");
        write(*socket_desc, size_string.c_str(), 10);
        //Send Picture as Byte Array
        //Read from the file into our send buffer
        while(!feof(picture)) {
            //Read from the file into our send buffer
            read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);
            //Send data through our socket
            do{
                    stat = write(*socket_desc, send_buffer, read_size);
            }while (stat < 0);
            printf("Packet Number: %i\n",packet_index);
            printf("Packet Size Sent: %i\n",read_size);
            printf(" \n");
            printf(" \n");
            packet_index++;
            //      /Zero out our send buffer
            bzero(send_buffer, sizeof(send_buffer));
        }  
        
}
  
int main(int argc, char const* argv[])
{
    int socket_desc;
    struct sockaddr_in server;
    int c;
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1) {
        exit(0);
    }
    memset(&server,0,sizeof(server));
    server.sin_addr.s_addr = inet_addr("119.17.253.45");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8878 );
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) <0){
        cout<<strerror(errno);
        close(socket_desc);
        puts("Connect Error");
        exit(0);
    }	
    //send_image(&socket_desc);
    send_image_string(&socket_desc);
    printf("Hello message sent\n");
  
    // closing the connected socket
    close(socket_desc);
    return 0;
}