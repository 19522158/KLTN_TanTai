#include <cstring>
#include <sys/wait.h>
#include <sys/types.h>
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
int nhandang = 0;
int tai = 0;
int dem = 0;
int rc;
int mqtt_value = 0;
Mat frame;
int new_socket;
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
	if( strcmp((char *)msg->payload, "stream") == 0)
	{
	    mqtt_value =2;
	}
	else if(strcmp((char *)msg->payload, "two_mode") == 0)
		mqtt_value = 3;
	else
		mqtt_value = 1;


}

void* Socket2(void *arg)
{
    //Listen
            int  id=12;
	char* hello;
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
    while(new_socket > 0)
        {
		if(mqtt_value >0 || nhandang ==1)
		{
			cout << "mqtt_value : " << mqtt_value << endl;
			cout << "gui tín hiệu " << endl;
			if(mqtt_value ==2)
				hello =(char *)"stream";
			else if(mqtt_value ==3)
				hello =(char *)"two_mode";
			else
				hello = (char *)"Hello to client";	
			send(new_socket, hello,80 , 0);
			cout << hello << endl;
			mqtt_value = 0;
			nhandang = 0;
			sleep(2);
		}
            		
	}
        mosquitto_loop_stop(mosq, true);
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
	pthread_exit(NULL);
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
void receive_image2()
{ // Start function 

int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;

char imagearray[10241],verify = '1';
FILE *image;

//Find the size of the image
do{
stat = read(new_socket, &size, sizeof(int));
}while(stat<0);

//printf("Packet received.\n");
//printf("Packet size: %i\n",stat);
printf("Image size: %i\n",size);
printf(" \n");
if(size == 0 )
{
	printf("Thiết bị ngắt kết nối \n");

	new_socket = 0;
	return ;
}
	
char buffer[] = "Got it";

//Send our verification signal



//printf("Reply sent\n");
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
    FD_SET(new_socket,&fds);

    buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);

    if (buffer_fd < 0)
       printf("error: bad file descriptor set.\n");

    if (buffer_fd == 0)
    {
       printf("error: buffer read timeout expired.\n");
	return;
    }

    if (buffer_fd > 0)
    {
        do{
               read_size = read(new_socket,imagearray, 10241);
            }while(read_size <0);

        //    printf("Packet number received: %i\n",packet_index);


//        printf("Packet size: %i\n",read_size);


        //Write the currently read data into our image file
         write_size = fwrite(imagearray,1,read_size, image);
  //       printf("Written image size: %i\n",write_size); 

             if(read_size !=write_size) {
                 printf("error in read write\n");    }


             //Increment the total number of bytes read
             recv_size += read_size;
             packet_index++;
    //         printf("Total received image size: %i\n",recv_size);
        //     printf(" \n");
          //   printf(" \n");

    }

}


  fclose(image);
  printf("Image successfully Received!\n");
  return;
  }
/*
void func(int signum)
{
	print("Waiting other connection \n");
    wait(NULL);
}*/
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
     Mat myImage;
        int i = 1;
	CascadeClassifier face_cascade;
    string face_cascade_name = "haarcascade_frontalface_alt2.xml";
    string fn_csv = string("face_db.csv");
    vector<string> names = {"Tan Tai", "Sundar Pichai", "Andrew Ng"};
	int m = 0;
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

	int n;
	int rc;
	struct mosquitto * mosq;

	mosquitto_lib_init();

	mosq = mosquitto_new("test", true, NULL);

	rc = mosquitto_connect(mosq,"119.17.253.45", 1883, 60);
	if(rc != 0){
		printf("Client could not connect to broker! Error Code: %d\n", rc);
		mosquitto_destroy(mosq);
		exit(0);
	}
	printf("We are now connected to the broker!\n");

       	while(new_socket > 0)
	{
//   	  	

     	puts("Waiting for incoming connections...");
	  
	
	 receive_image2();
	 if(new_socket <= 0)
	 {
		 break;
		 cout << " out ra socket " << endl;
	}
	 cout << " nhận diện " << endl;
	frame = imread("test.jpg");  
	if (frame.empty())
	{
		printf("Hình ảnh vấn đề \n");
		continue;
	}
	//	   pthread_cond_signal(&cond);


//  	   pthread_mutex_unlock(&lock1);
 	        flip(frame, frame, 1);

		cout << " detect " << endl;
        // Detect and predict all faces in frame

  //      char* name;

	//	char name[10];
//	strcpy(name, detect_and_predict(face_cascade, model, frame, names));
	string name = detect_and_predict(face_cascade, model, frame, names);
	cout << " detect xong " << endl;
	 n = name.length();
	 char t_array[n+1];
	strcpy(t_array, name.c_str());
	if(strcmp(t_array,"Tan Tai") == 0)
		nhandang = 1;
	cout << name << endl;
	mosquitto_publish(mosq, NULL, "test/t2", n+1, t_array, 0, false);
	   const std::string str = "photos/test" + to_string(i) + ".jpg";
	
	imwrite(str,frame);
	i = i + 1;
//	 dem = 1;
		
	}  

    close(new_socket);
//    fflush(stdout);
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);

	mosquitto_lib_cleanup();
//	printf("CHờ thiết bị khác kết nối\n");
	pthread_exit(NULL);
 //	 exit(0);
  //  return NULL;
}
int main()
{
	      int  socket_desc, c, read_size, buffer = 0;
        struct sockaddr_in server,client;
	pid_t pid_child;
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
    while(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
      puts("bind failed");
        sleep(1);
    }

    puts("bind done");
    //Listen

    listen(socket_desc , 3); // int listen(int s, int backlog); | `backlog` (3)
    //limits the number of outstanding connections in the socket's listen
    //queue to the value specified by the backlog argument.
      c = sizeof(struct sockaddr_in);
      // waits for a connection. returns -1 on failure, a positive val on success

    while(1)
    {
      if((new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c)))
      {
          puts("Connection accepted");
          
      }
      else
              cout << " huy " << endl;
      fflush(stdout);
   	
      pid_child = fork();
      if (pid_child < 0) {
              printf("ERROR in new process creation");
	      exit(0);
         }
      if (pid_child == 0) {
            //child process
            close(socket_desc);
	    pthread_create(&thread1, NULL, &Receive, NULL);
	    pthread_create(&thread2, NULL, &Socket2, NULL);
	    pthread_join(thread1, NULL);
 	    pthread_join(thread2, NULL);
	    close(new_socket);
	    printf("\n");
	    printf("Waiting for other connection \n");
	    exit(0);
      }
	else {
             //parent process
             close(new_socket);

//	     signal(SIGCHLD, func);
          }
    }
    close(socket_desc);
    return 0;
}
