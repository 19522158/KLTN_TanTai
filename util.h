#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>


using namespace std;
using namespace cv;
using namespace face;
using std::string; using std::reverse;
int loadCascade(CascadeClassifier& face_cascade, string face_cascade_name);
int checkCamera(VideoCapture cap);

string detect_and_predict(CascadeClassifier face_cascade, Ptr<FaceRecognizer> model, Mat& frame, vector<string> names);
int read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';');
void create_model(Ptr<FaceRecognizer>& model, double threshold, vector<Mat> images, vector<int> labels);

#endif // UTIL_H_INCLUDED
