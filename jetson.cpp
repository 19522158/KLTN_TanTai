#include "util.h"
#include <pthread.h>
#include <mosquitto.h>
#include <time.h>

#define END_PROGRAM 1000

using namespace cv;
using namespace std;
pthread_t thread1,thread2, thread3;
int socket_desc;
int flag_dataset = 0;
vector<string> names;
char find_name[20] = "Stream";
char flag[3] = " ";

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
        string mes((char *) msg->payload);
        if(strcmp(mes.c_str(), "HD") == 0 || strcmp(mes.c_str(),"SD") == 0)
            strcpy(flag, mes.c_str());
        else
            strcpy(find_name, mes.c_str());
        cout << find_name << endl;
        cout << flag << endl;
//      cout << (char *) msg->payload   << endl
}

void* Receive(void *arg){
	char message[20] = {};
 //       char confirm[4] = "yes";
        int id = 39;
	ofstream name_file("/home/hoang/Desktop/KLTN/name.txt",ios_base::app);
	ofstream image_file("/home/hoang/Desktop/KLTN/face_db.csv",ios_base::app);
	// MQTT Config
	mosquitto_lib_init();
	struct mosquitto *mosq;
        mosq = mosquitto_new("sub233323", true, &id);
        int* rc = new (int);
	*rc = mosquitto_connect_async(mosq,"119.17.253.45", 1883,30000);
	if(*rc != 0) {
                printf("Could not connect to Broker with return code %d\n", *rc);
                exit(1);
        }
	delete(rc);
        int f = 0;
        mosquitto_connect_callback_set(mosq, on_connect);
        mosquitto_message_callback_set(mosq, on_message);
        mosquitto_loop_start(mosq);
        string s;
        string s2;

	while(socket_desc > 0){
		int t = recv(socket_desc, message, 20,0);
                if(t <= 0)
                        break;
                s = message;
                int i = names.size() - 1;
		s2 = "/home/hoang/Desktop/KLTN/dataset/" + s + ".jpg";
		char image_file_name[s2.size() +1];
		strcpy(image_file_name, s2.c_str());
                f = receive_image2(image_file_name,&socket_desc);
		if (f == 1){
                    if (find(names.begin(), names.end(),s) != names.end()){
                        flag_dataset =1;
                        cout << s << endl;
                        continue;
                    }
                    cout << "themn ten" << endl;
                    name_file << s <<endl;
                    names.push_back(s);
                    image_file << s2 << ";" << i+1 <<endl;
 //                       write(socket_desc, confirm, 4);
                    flag_dataset = 1;
			
		}
		cout << i << endl;
                cout << s << endl;
	}
	cout << "End Thread 2" << endl;
	mosquitto_loop_stop(mosq, true);
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
	name_file.close();
	image_file.close();
	pthread_exit(NULL);
}

void* CaptureFunction(void *arg){
	CascadeClassifier face_cascade;
        VideoCapture cap(0);
        int width = 640;
        int height = 480;
//	cap.set(CAP_PROP_FPS, 30);
        clock_t begin, end;
        double time_spent = 0.0;
        double sum_time = 0;
        int count = 0;
        cap.set(CAP_PROP_FRAME_WIDTH, width);//Setting the width of the video
        cap.set(CAP_PROP_FRAME_HEIGHT, height);
        Mat frame;
        int hd = 1;
//    char* window_name = "Face Recognition Application";
        string face_cascade_name = "/home/hoang/Desktop/KLTN/haarcascade_frontalface_alt2.xml";
        string fn_csv = string("/home/hoang/Desktop/KLTN/face_db.csv");

        vector<Mat> images;
        vector<int> labels;
        Ptr<face::FaceRecognizer> model;

    // Read the csv file to get all images and its labels
        if (read_csv(fn_csv, images, labels) == -1)
                exit(0);

    // Load Haar Cascade for face detection
        if (loadCascade(face_cascade, face_cascade_name) == -1)
                exit(0);

    // Check camera availability
        if (checkCamera(cap) == -1)
                exit(0);

    // Create LBPHFaceRecognizer with 80 of threshold
        create_model(model, 80, images, labels);
        FILE *pipeout;
        std::string ffmpeg_cmd = std::string("ffmpeg -f rawvideo -r ") + std::to_string(18) + " -framerate 18 -video_size " + std::to_string(width) + "x" +  std::to_string(height) + " -pixel_format bgr24 -i pipe: -crf 24 -pixel_format yuv420p -f rtsp -rtsp_transport tcp rtsp://119.17.253.45:8556/mystream480";
	while(socket_desc > 0){
                pipeout = popen(ffmpeg_cmd.c_str(), "w");
		while (strcmp(find_name, "Stream") == 0 && socket_desc > 0){
                        /*bool bSuccess = cap.read(frame);
		        if (!bSuccess)
		        {
		                cout << "Error reading frame from camera feed" << endl;
                                continue;
                        }*/
                        cap >> frame;
		// Flip the frame
		        flip(frame, frame, 1);
                       fwrite(frame.data, 1, width*height*3, pipeout);
	//	       imshow("test", frame);
		        switch (waitKey(30))
		        {
		                case 27:
		                exit(0);
		        }
                        if(strcmp(flag,"SD") == 0 && hd == 0)
                        {
                            cap.release();
                            fflush(pipeout);
                            pclose(pipeout);
                            width = 640;
                            height = 480;
                            cap = VideoCapture(0);
                            cap.set(CAP_PROP_FRAME_WIDTH, width);//Setting the width of the video
                            cap.set(CAP_PROP_FRAME_HEIGHT, height);
//			    cap.set(CAP_PROP_FPS, 30);
                            ffmpeg_cmd = std::string("ffmpeg -f rawvideo -r ") + std::to_string(18) + " -framerate 18 -video_size " + std::to_string(width) + "x" +  std::to_string(height) + " -pixel_format bgr24 -i pipe: -crf 24 -pixel_format yuv420p  -f rtsp  -rtsp_transport tcp rtsp://119.17.253.45:8556/mystream480";
                            pipeout = popen(ffmpeg_cmd.c_str(), "w");
                            hd = 1;

                        }
                        if(strcmp(flag,"HD") == 0 && hd == 1)
                        {
                            cap.release();
                            fflush(pipeout);
                            pclose(pipeout);
                            width = 1280;
                            height = 720;
                            cap = VideoCapture(0);
                            cap.set(CAP_PROP_FRAME_WIDTH, width);//Setting the width of the video
                            cap.set(CAP_PROP_FRAME_HEIGHT, height);
//			    cap.set(CAP_PROP_FPS, 30);
                            ffmpeg_cmd = std::string("ffmpeg -y -f rawvideo -r ") + std::to_string(4) + " -video_size " + std::to_string(width) + "x" +  std::to_string(height) + " -pixel_format bgr24 -i pipe: -crf 24 -pix_fmt yuv420p -f rtsp  -rtsp_transport tcp rtsp://119.17.253.45:8556/mystream720";
                            pipeout = popen(ffmpeg_cmd.c_str(), "w");
                            hd = 0;
                        }

		}

		fflush(pipeout);
                pclose(pipeout);
		while (strcmp(find_name, "Stream") != 0 && socket_desc > 0){
                        //bool bSuccess = cap.read(frame);
                        cap >> frame;
                        if(flag_dataset==1){
                            if (read_csv(fn_csv, images, labels) == -1)
                                    exit(0);
                            create_model(model, 80, images, labels);
                            flag_dataset = 0;
                        }
                        /*
		        if (!bSuccess)
		        {
		                cout << "Error reading frame from camera feed" << endl;
                                continue;
                        }*/
		// Flip the frame
		        flip(frame, frame, 1);
		// Detect and predict all faces in frame
		        int t = detect_and_predict(face_cascade, model, frame, names,find_name);
			if(t==1)
			{
				imwrite("/home/hoang/Desktop/KLTN/name.jpg",frame);
                                begin = clock();
                           //     write(socket_desc, find_name,20);
                                send(socket_desc, find_name, 20, 0);
                                cout << "Send Name Successfully" << endl;
				send_image(&socket_desc);
                                end = clock();
                                time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				cout << "Time for Sending Picture: " << time_spent << endl;
                                count = count + 1;
                                sum_time = sum_time + time_spent;         
                                cout << "Count recognition: " << count << endl;
				cout << "Time Current: " << get_time() << endl;
                                cout << endl;
                                if(count == 100){
                                    sum_time = sum_time /100;
                                    cout << "Tong so anh gui : 100 " << endl;
                                    cout << "Trung binh thoi gian gui 480p 4G: " << sum_time << endl;
                                    exit(0);
                                }
                                sleep(1);
			}
         //             imshow("test", frame);
		        switch (waitKey(30))
		        {
		                case 27:
		                exit(0);
		        }
                        if(strcmp(flag,"HD") == 0 && hd == 1)
                        {
                            cap.release();
                            width = 1280;
                            height = 720;
                            cap = VideoCapture(0);
                            cap.set(CAP_PROP_FRAME_WIDTH, width);//Setting the width of the video
                            cap.set(CAP_PROP_FRAME_HEIGHT, height);
//			    cap.set(CAP_PROP_FPS, 30);
                            ffmpeg_cmd = std::string("ffmpeg -f rawvideo -r ") + std::to_string(4) + " -video_size " + std::to_string(width) + "x" +  std::to_string(height) + " -pixel_format bgr24 -i pipe: -crf 24 -pix_fmt yuv420p -f rtsp -rtsp_transport tcp rtsp://119.17.253.45:8556/mystream720";
                            hd = 0;
                        }
                        if(strcmp(flag,"SD") == 0 && hd == 0)
                        {
                            cap.release();
                            width = 640;
                            height = 480;
                            cap = VideoCapture(0);
                            cap.set(CAP_PROP_FRAME_WIDTH, width);//Setting the width of the video
                            cap.set(CAP_PROP_FRAME_HEIGHT, height);
//			    cap.set(CAP_PROP_FPS, 30);
                            ffmpeg_cmd = std::string("ffmpeg -f rawvideo -r ") + std::to_string(18) + " -framerate 18 -video_size " + std::to_string(width) + "x" +  std::to_string(height) + " -pixel_format bgr24 -i pipe: -crf 24 -pixel_format yuv420p  -f rtsp -rtsp_transport tcp rtsp://119.17.253.45:8556/mystream480";
                            hd = 1;
                        }
		}
        }
	cout << "End Thread 1" << endl;
        exit(0);
        pthread_exit(NULL);
}

void* CountTime(void* arg){
       int count = 0;
       while(socket_desc > 0){
                count = count + 1;
                if(count == END_PROGRAM)
                        break;
                sleep(1);
       } 
        socket_desc = 0;
        close(socket_desc);
        exit(0);
        pthread_exit(NULL);
}

int main(){
	socket_init(&socket_desc);
	get_name(names);
        struct timeval tv;
	tv.tv_sec = 50000;             
   	tv.tv_usec = 0;
	if (setsockopt (socket_desc, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv) < 0)
       		printf("setsockopt failed\n");

    	if (setsockopt (socket_desc, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof tv) < 0)
        	printf("setsockopt failed\n");
	
	pthread_create(&thread1, NULL, &CaptureFunction, NULL);
	pthread_create(&thread2, NULL, &Receive, NULL);
      //  pthread_create(&thread3, NULL, &CountTime, NULL);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
       // pthread_join(thread3, NULL);
}

