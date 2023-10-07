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
#include <sys/mman.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h> 
#include <dirent.h>
#include <string>
#include <signal.h>
#include <algorithm>
#include <vector>
#include <jsoncpp/json/json.h>
#include <chrono>
#include <ctime>   

int receive_image2(char image_file_name[],int* new_socket);
void socket_init2(int* socket_desc2, int* c2);
void socket_init(int* socket_desc, int* c);
void send_image(int* socket_desc);
void send_image2(int* socket_desc);
int receive_message_confirm(int* new_socket);
void receive_new_image(int* new_socket2, const char* semName);
int count_files(std::string& dir_path);
void read_name_file(char* name_array);
void sigpipe_handler(int signum);
void get_name(std::vector<std::string>& names);
const std::string get_time();
std::vector<std::string> split (const std::string &s, char delim);
void write_noti(int type,char* name);
void write_to_json();
void write_name_to_json();
