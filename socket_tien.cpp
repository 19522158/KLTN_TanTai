#include <netinet/in.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8878

using namespace std;

int receive_image2(char image_file_name[], int* new_socket)
{ // Start function

        int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
        char verify = '1';
        FILE *image;
//Find the size of the image
      
      //	stat = read(*new_socket, &size, sizeof(int));
	int t = recv(*new_socket, &size, sizeof(int),0);
	if(t <= 0)
        {
                *new_socket = 0;
                return 2;
        }
        
        printf("Image size: %i\n",size);
        printf(" \n");
        if(size == 0 )
        {
                printf("Thiết bị ngắt kết nối \n");
                *new_socket = 0;
                return 2;
        }
        char imagearray[10241];
        char buffer[] = "Got it";
        printf(" \n");
        image = fopen(image_file_name, "w");
        if( image == NULL) {
                printf("Error has occurred. Image file could not be opened\n");
                return 2;
        }
//Loop while we have not received the entire file yet
        int need_exit = 0;
        struct timeval timeout = {3,0};
        fd_set fds;
        int buffer_fd, buffer_out;
        
       // IP: 119.17.253.45
       // Port:8889
//while(packet_index < 2){
        while(recv_size < size){
//while(packet_index < 2){
                FD_ZERO(&fds);
                FD_SET(*new_socket,&fds);
                buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);
                if (buffer_fd < 0){
                        printf("error: bad file descriptor set.\n");
			return 2;
		}
                if (buffer_fd == 0)
                {
                        printf("error: buffer read timeout expired.\n");
                        return 2;
                }

                if (buffer_fd > 0)
                {
                        do{
                                read_size = read(*new_socket,imagearray, 10241);
                        }while(read_size <0);

        //Write the currently read data into our image file
                        write_size = fwrite(imagearray,1,read_size, image);
                        if(read_size !=write_size) {
                                printf("error in read write\n");    }
             //Increment the total number of bytes read
                        recv_size += read_size;
                        packet_index++;

                }
        }
//      return image;
        fclose(image);
        printf("Image successfully Received!\n");
        return 1;
}

int receive_image_string(char image_file_name[], int* new_socket)
{ // Start function

        int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
        char verify = '1';
        FILE *image;
//Find the size of the image
      
      //	stat = read(*new_socket, &size, sizeof(int));
        char size_str[10];
	int t = recv(*new_socket, size_str, 10,0);
	if(t <= 0)
        {
                *new_socket = 0;
                return 2;
        }
        string size_string(size_str);
        long size_int = atoi(size_string.c_str());
        printf("Image size: %ld\n",size_int);
        printf(" \n");
        if(size_int == 0 )
        {
                printf("Thiết bị ngắt kết nối \n");
                *new_socket = 0;
                return 2;
        }
        char imagearray[10241];
        char buffer[] = "Got it";
        printf(" \n");
        image = fopen(image_file_name, "w");
        if( image == NULL) {
                printf("Error has occurred. Image file could not be opened\n");
                return 2;
        }
//Loop while we have not received the entire file yet
        int need_exit = 0;
        struct timeval timeout = {3,0};
        fd_set fds;
        int buffer_fd, buffer_out;
        
       // IP: 119.17.253.45
       // Port:8889
//while(packet_index < 2){
        while(recv_size < size_int) {
//while(packet_index < 2){
                FD_ZERO(&fds);
                FD_SET(*new_socket,&fds);
                buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);
                if (buffer_fd < 0){
                        printf("error: bad file descriptor set.\n");
			return 2;
		}
                if (buffer_fd == 0)
                {
                        printf("error: buffer read timeout expired.\n");
                        return 2;
                }

                if (buffer_fd > 0)
                {
                        do{
                                read_size = read(*new_socket,imagearray, 10241);
                        }while(read_size <0);

        //Write the currently read data into our image file
                        write_size = fwrite(imagearray,1,read_size, image);
                        if(read_size !=write_size) {
                                printf("error in read write\n");    }
             //Increment the total number of bytes read
                        recv_size += read_size;
                        packet_index++;

                }
        }
//      return image;
        fclose(image);
        printf("Image successfully Received!\n");
        return 1;
}

int main(int argc, char const* argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
  
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
  
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8878);
  
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("blind done \n" );
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address,
                  (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    while(new_socket > 0){
 //       int t = receive_image2("/var/www/html/live/Tien.jpg",&new_socket);   
        int t = receive_image_string("/var/www/html/live/Tien.jpg",&new_socket);  
        if (t==2){
            printf("Nhan hinh anh that bai \n");
        }
        cout << "Done" << endl;
        break;

    }
    // closing the connected socket
    close(new_socket);
    printf("Socket Close, Terminate Program  \n");
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}