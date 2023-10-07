#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fstream>
#include <errno.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <signal.h>
using namespace cv;

using namespace std;


void sigpipe_handler(int signum) {
    // Xử lý tín hiệu SIGPIPE, ví dụ đóng kết nối và thoát khỏi chương trình
    printf("Bị ngắt kết nối\n");
    exit(1);
}

void get_name(vector<string>& names)
{
	ifstream myfile("name.txt");
	string line;
	while (getline(myfile, line))
    	{
        	names.push_back(line);
    	}
    	myfile.close();
}

void send_image(int* socket_desc)
{
    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256];
    packet_index = 1;
    picture = fopen("send.jpg", "r");
    printf("Getting Picture Size\n");
    if(picture == NULL) {
         printf("Error Opening Image File"); }
    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    printf("Total Picture size: %i\n",size);
    //Send Picture Size
    printf("Sending Picture Size\n");
    write(*socket_desc, (void *)&size, sizeof(int));
    //Send Picture as Byte Array
    printf("Sending Picture as Byte Array\n");
    printf("Received data in socket\n");
    printf("Socket data: %s\n", read_buffer);
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

void socket_init(int* socket_desc){
	struct sockaddr_in server;
        *socket_desc = socket(AF_INET , SOCK_STREAM , 0);
        if (*socket_desc == -1) {
                exit(0);
        }
        memset(&server,0,sizeof(server));
        server.sin_addr.s_addr = inet_addr("119.17.253.45");
        server.sin_family = AF_INET;
        server.sin_port = htons( 8814 );
        if (connect(*socket_desc , (struct sockaddr *)&server , sizeof(server)) <0){
                 cout<<strerror(errno);
                close(*socket_desc);
                puts("Connect Error");
                exit(0);
        }	
}
	
int main() {
        Mat frame;
        Mat frame_gray;
    	Mat crop;
    	signal(SIGPIPE, sigpipe_handler);
        VideoCapture cap(0);
        int socket_desc; 
	char name[20] = " ";
//	cin.ignore();
	cout << name << endl;
        if (!cap.isOpened()) {
                cout << "cannot open camera";
                return 0;
        }
        CascadeClassifier faceCascade;
        faceCascade.load("haarcascade_frontalface_default.xml");
        if (faceCascade.empty())
        {
                cout << "XML not loaded" << endl;
        }
        vector<string> names;

        get_name(names);
        ofstream name_file("name.txt",ios_base::app);
        
        size_t ic = 0; // ic is index of current element
    	int ac = 0; // ac is area of current elemen
    	size_t ib = 0; // ib is index of biggest element
    	int ab = 0; // ab is area of biggest element
    	cv::Rect roi_b;
    	cv::Rect roi_c;
    	int flag = 1;
	socket_init(&socket_desc);
        while (socket_desc >0) {
        	if(flag ==1){
        		
        		memset(name,0,sizeof(name));
        		cout << "Nhap Ten :";
        		//cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        		cin.ignore();
        		cin.getline(name,20);
        		//strcat(name,"/");
        		if (std::find(names.begin(), names.end(), name) != names.end())
			{
				cout << "Da co ten trong danh sach, nhan nut Y neu muon cap nhat" << endl;
				char ch;
				if(scanf("%[yY]", &ch) != 1){
					continue;
 				}
  // Element in vector.	
			}
        		
        		flag = 0;	
        	 }
                cap >> frame;
                cvtColor(frame,frame_gray,COLOR_BGR2GRAY);
                equalizeHist(frame_gray,frame_gray);
                vector<Rect> faces;
                faceCascade.detectMultiScale(frame_gray, faces, 1.9, 5);

                   for (ic = 0; ic < faces.size(); ic++) // Iterate through all current elements (detected faces)

		    {
			roi_c.x = faces[ic].x;
			roi_c.y = faces[ic].y;
			roi_c.width = (faces[ic].width);
			roi_c.height = (faces[ic].height);

			ac = roi_c.width * roi_c.height; // Get the area of current element (detected face)

			roi_b.x = faces[ib].x;
			roi_b.y = faces[ib].y;
			roi_b.width = (faces[ib].width);
			roi_b.height = (faces[ib].height);

			ab = roi_b.width * roi_b.height; // Get the area of biggest element, at beginning it is same as "current" element

			if (ac > ab)
			{
			    ib = ic;
			    roi_b.x = faces[ib].x;
			    roi_b.y = faces[ib].y;
			    roi_b.width = (faces[ib].width);
			    roi_b.height = (faces[ib].height);
			}

			crop = frame(roi_b);
			resize(crop, crop, Size(200, 200), 0, 0, INTER_LINEAR); // This will be needed later while saving images
			cvtColor(crop, crop, COLOR_BGR2GRAY); // Convert cropped image to Grayscale
			imwrite("send.jpg", crop);
			names.push_back(name);
        		name_file << name << endl;
			cout << name << endl;
			send(socket_desc, &name, 19,0);
			send_image(&socket_desc);
			memset(name,0,sizeof(name));
			flag = 1;
		}
                imshow("Name",frame_gray);
                char c = (char)waitKey(25);//Allowing 25 milliseconds frame processing time and initiating break condition//
      		if (c == 27){ //If 'Esc' is entered break the loop//
         		break;
      		}

        }
        close(socket_desc);
        return 0;
}

