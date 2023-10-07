
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<signal.h>
#include<fstream>
#include<errno.h>
#include<sys/stat.h>
#include <mosquitto.h>
#include <stdlib.h>
#include "util.h"
using namespace std;
pthread_mutex_t lock1;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int tai = 0;
int dem,rc;
int mqtt_value = 0;
Mat frame;
 char hello[80] = "Hello to client";;
pthread_t thread1,thread2,thread3;

void on_connect(struct mosquitto *mosq, void *obj, int rc) {
        printf("ID: %d\n", * (int *) obj);
        if(rc) {
                printf("Error with result code: %d\n", rc);
                exit(-1);
        }
        mosquitto_subscribe(mosq, NULL, "test/t1", 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
        printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload);
	mqtt_value = 1;
}

void* Socket2(void *arg)
{
    int socket_desc , new_socket , c, read_size,buffer = 0;
    struct sockaddr_in server , client;
    char *readin;
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)

    {
      printf("Could not create socket 2");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8879 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
      puts("bind failed socket 2");
      exit(1);
    }

    puts("bind done socket 2");
    //Listen
            int  id=12;

        mosquitto_lib_init();

        struct mosquitto *mosq;

        mosq = mosquitto_new("subscribe-test", true, &id);


        rc = mosquitto_connect(mosq,"119.17.253.45", 1883,10);
        printf("rc = %d",rc);
        if(rc != 0) {
                printf("Could not connect to Broker with return code %d\n", rc);
                exit(1);
        }
        mosquitto_connect_callback_set(mosq, on_connect);
        mosquitto_message_callback_set(mosq, on_message);
	mosquitto_loop_start(mosq);


    listen(socket_desc , 3); // int listen(int s, int backlog); | `backlog` (3)
    //limits the number of outstanding connections in the socket's listen
    //queue to the value specified by the backlog argument.
      c = sizeof(struct sockaddr_in);
      // waits for a connection. returns -1 on failure, a positive val on success
     while(new_socket == 0){
      if((new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c)))
      {
          puts("Connection accepted Socket 2");
          cout << "new_socket 2 = " << new_socket << "\n";
      }
      else
              cout << " huy " << endl;
      fflush(stdout);
    }
    while(new_socket > 0)
        {

		if(mqtt_value ==1)
		{
			cout << "gui tín hiệu " << endl;
            		send(new_socket, hello,80 , 0);
			mqtt_value = 0;
		}
            		
	}
        mosquitto_loop_stop(mosq, true);
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();

}
/*void send_signal(int signum)
{
	FILE *codefile;
	if(codefile=fopen("/var/www/html/live/tai.txt","r"))
	{
  	//	hello = "Hello server client";
		tai = 1;
                   fclose(codefile);
	
	   //	   remove("tai.txt");
		tai = 0;
	}
		
//	return NULL;

}*/
void receive_image2(int socket)
{ // Start function 

int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;

char imagearray[10241],verify = '1';
FILE *image;

//Find the size of the image
do{
stat = read(socket, &size, sizeof(int));
}while(stat<0);

printf("Packet received.\n");
printf("Packet size: %i\n",stat);
printf("Image size: %i\n",size);
printf(" \n");

if(size ==0)
	return;
char buffer[] = "Got it";

//Send our verification signal
do{
stat = write(socket, &buffer, sizeof(int));
}while(stat<0);

printf("Reply sent\n");
printf(" \n");

image = fopen("test.jpg", "w");

if( image == NULL) {
printf("Error has occurred. Image file could not be opened\n");
exit(1); }

//Loop while we have not received the entire file yet


int need_exit = 0;
struct timeval timeout = {10,0};

fd_set fds;
int buffer_fd, buffer_out;

while(recv_size < size) {
//while(packet_index < 2){

    FD_ZERO(&fds);
    FD_SET(socket,&fds);

    buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);

    if (buffer_fd < 0)
       printf("error: bad file descriptor set.\n");

    if (buffer_fd == 0)
       printf("error: buffer read timeout expired.\n");

    if (buffer_fd > 0)
    {
        do{
               read_size = read(socket,imagearray, 10241);
            }while(read_size <0);

            printf("Packet number received: %i\n",packet_index);
        printf("Packet size: %i\n",read_size);


        //Write the currently read data into our image file
         write_size = fwrite(imagearray,1,read_size, image);
         printf("Written image size: %i\n",write_size); 

             if(read_size !=write_size) {
                 printf("error in read write\n");    }


             //Increment the total number of bytes read
             recv_size += read_size;
             packet_index++;
             printf("Total received image size: %i\n",recv_size);
             printf(" \n");
             printf(" \n");
    }

}


  fclose(image);
  printf("Image successfully Received!\n");
  
  }


void receive_image(int socket)
{ // Start function


    int bytes = 0;
    Mat img;
    img = Mat::zeros(480 , 640, CV_8UC3);
    int  imgSize = img.total()*img.elemSize();
    uchar sockData[imgSize];
    for (int i = 0; i < imgSize; i += bytes) {
    if ((bytes = recv(socket, sockData +i, imgSize  - i, 0)) == -1)  {
       cout << " recv erro " << endl;
       exit(0);
    }

    }
	int ptr=0;
for (int i = 0;  i < img.rows; i++) {
    for (int j = 0; j < img.cols; j++) {                                     
       img.at<cv::Vec3b>(i,j) = cv::Vec3b(sockData[ptr+ 0],sockData[ptr+1],sockData[ptr+2]);
       ptr=ptr+3;
    }
}
    imwrite("test.jpg",img);
    cout << "image save succesful" << endl;
}
void* Receive(void *arg)
{

	pthread_mutex_lock(&lock1);
    int socket_desc , new_socket , c, read_size,buffer = 0;
    struct sockaddr_in server , client;
    char *readin;
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)

   {
      printf("Could not create socket");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8889 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
      puts("bind failed");
      exit(1);
    }

    puts("bind done");
    //Listen
    listen(socket_desc , 3); // int listen(int s, int backlog); | `backlog` (3)
    //limits the number of outstanding connections in the socket's listen
    //queue to the value specified by the backlog argument.
      c = sizeof(struct sockaddr_in);
      // waits for a connection. returns -1 on failure, a positive val on success
     while(new_socket == 0){
      if((new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c)))
      {
          puts("Connection accepted");
          cout << "new_socket = " << new_socket << "\n";
//        receive_image(new_socket);
      }
      else
	      cout << " huy " << endl;

      fflush(stdout);
    }
//	signal(SIGALRM,send_signal);
	
//	alarm(1);
	
     Mat myImage;
        int i = 1;	
    while(new_socket > 0)
	{
   	    if(dem == 1)
	    {
		    pthread_mutex_lock(&lock1);
		    dem = 0;
	    }
              
    //	    alarm(1);	 	  
      //Accept and incoming connection
     
	   puts("Waiting for incoming connections...");
       
	  /* if(tai != 0)
           {

        ///        write(new_socket, buffer, 80);
                send(new_socket, hello,80 , 0);
		cout << "send succesful " << endl;
		continue;
	   
	   }*/
      // receive image
   
	 receive_image2(new_socket);
	frame = imread("test.jpg");  
	   pthread_cond_signal(&cond);


  	   pthread_mutex_unlock(&lock1);
 	
	   const std::string str = "photos/test" + to_string(i) + ".jpg";
	
	imwrite(str,frame);
	i = i + 1;
//	 dem = 1;
	sleep(1);	 
	}  
    close(socket_desc);
    fflush(stdout);
  
  //  return NULL;
}
void* ReCog(void *arg)
{
	CascadeClassifier face_cascade;
    string face_cascade_name = "haarcascade_frontalface_alt2.xml";
    string fn_csv = string("face_db.csv");
    vector<string> names = {"Tan Tai", "Sundar Pichai", "Andrew Ng"};

    vector<Mat> images;
    vector<int> labels;
    Ptr<face::FaceRecognizer> model;

    // Read the csv file to get all images and its labels
    if (read_csv(fn_csv, images, labels) == -1)
        exit(1);

    // Load Haar Cascade for face detection
    if (loadCascade(face_cascade, face_cascade_name) == -1)
        exit(1);

    // Check camera availabilit
    // y
  //  if (checkCamera(cap) == -1)
    //    return -1;

    // Create LBPHFaceRecognizer with 80 of threshold
    create_model(model, 80, images, labels);

    cout << " success" << endl;
    while (1)
    {

       /*           bool bSuccess = cap.read(frame);
        if (!bSuccess)
        {
            cout << "Error reading frame from camera feed" << endl;
            break;
        }*/
	    pthread_cond_wait(&cond, &lock1);
	  pthread_mutex_lock(&lock1);
	dem = 1;
	
      //	pthread_cond_wait(&cond, &lock1);
//	dem = 1;
        
        // Flip the frame
        flip(frame, frame, 1);
 
        // Detect and predict all faces in frame

	int t = detect_and_predict(face_cascade, model, frame, names);

	if(t == 2)
		cout << " klhong co nguoi " << endl;
	else if (t ==1)
		cout << " co nguoi " << endl;
        // Show the frame
//      imshow("test", frame);

	pthread_mutex_unlock(&lock1);

	usleep(1);
    }
}

int main()
{

	
	pthread_mutex_init(&lock1, NULL);
	pthread_create(&thread1, NULL, &ReCog, NULL);

	pthread_create(&thread2, NULL, &Receive, NULL);
	pthread_create(&thread2, NULL, &Socket2, NULL);
	pthread_join(thread1, NULL);
 	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	pthread_mutex_destroy(&lock1);
	return 0;
}
