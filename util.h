#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <vector>
#include <algorithm>

using namespace std;
using namespace cv;
using namespace face;

int loadCascade(CascadeClassifier& face_cascade, string face_cascade_name);
int checkCamera(VideoCapture cap);
int detect_and_predict(CascadeClassifier face_cascade, Ptr<FaceRecognizer> model, Mat& frame, vector<string> names, char image_file_name[]);
int read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';');
void create_model(Ptr<FaceRecognizer>& model, double threshold, vector<Mat> images, vector<int> labels);
void socket_init(int* socket_desc);
void send_image(int* socket_desc);
int receive_image2(char* image_file_name,int* new_socket);
void get_name(vector<string>& names);
#endif // UTIL_H_INCLUDED
