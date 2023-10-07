#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <cstring>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <sys/wait.h>
#include <sys/types.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<signal.h>
#include<fstream>
#include<errno.h>
#include<sys/stat.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <time.h>


using namespace std;
using namespace cv;
using namespace face;
using std::string; using std::reverse;
int loadCascade(CascadeClassifier& face_cascade, string face_cascade_name);
int checkCamera(VideoCapture cap);
void socket_init(int* socket_desc, int* c);
void receive_image2(char image_file_name[], int* new_socket);
string detect_and_predict(CascadeClassifier face_cascade, Ptr<FaceRecognizer> model, Mat& frame, vector<string> names,char image_file_name[]);
int read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';');
void create_model(Ptr<FaceRecognizer>& model, double threshold, vector<Mat> images, vector<int> labels);

#endif // UTIL_H_INCLUDED
