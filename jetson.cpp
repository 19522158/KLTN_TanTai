#include "util.h"
#include <pthread.h>
#include <mosquitto.h>

using namespace cv;
using namespace std;
pthread_t thread1,thread2;
int socket_desc;
int flag_dataset = 0;
vector<string> names;
char find_name[20] = " ";
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
	int id = 12;
	ofstream name_file("name.txt",ios_base::app);
	ofstream image_file("face_db.csv",ios_base::app);
	// MQTT Config
	mosquitto_lib_init();
	struct mosquitto *mosq;
        mosq = mosquitto_new("sub233323", true, &id);
	int rc = mosquitto_connect(mosq,"119.17.253.45", 1883,10);
	if(rc != 0) {
                printf("Could not connect to Broker with return code %d\n", rc);
                exit(1);
        }
        int f = 0;
        mosquitto_connect_callback_set(mosq, on_connect);
        mosquitto_message_callback_set(mosq, on_message);
        mosquitto_loop_start(mosq);
        string s;
        string s2;
	while(socket_desc > 0){
		read(socket_desc, message, 20);
                s = message;
                int i = names.size() - 1;
		s2 = "dataset/" + s + ".jpg";
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
        int width = 1280;
        int height = 720;
  //      cap.set(CAP_PROP_FRAME_WIDTH, 1280);//Setting the width of the video
    //    cap.set(CAP_PROP_FRAME_HEIGHT, 720);
        Mat frame;
        int hd = 0;
//    char* window_name = "Face Recognition Application";
        string face_cascade_name = "haarcascade_frontalface_alt2.xml";
        string fn_csv = string("face_db.csv");

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
        std::string ffmpeg_cmd = std::string("ffmpeg -y -f rawvideo -r ") + std::to_string(20) + " -video_size " + std::to_string(width) + "x" +  std::to_string(height) + " -pixel_format bgr24 -i pipe:  -crf 24 -pix_fmt yuv420p -f rtsp -rtsp_transport tcp rtsp://119.17.253.45:8556/mystream1";
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
                       imshow("test", frame);
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
                            ffmpeg_cmd = std::string("ffmpeg -y -f rawvideo -r ") + std::to_string(30) + " -video_size " + std::to_string(width) + "x" +  std::to_string(height) + " -pixel_format bgr24 -i pipe:  -crf 24 -pix_fmt yuv420p -f rtsp -rtsp_transport tcp rtsp://119.17.253.45:8556/mystream1";
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
                            ffmpeg_cmd = std::string("ffmpeg -y -f rawvideo -r ") + std::to_string(20) + " -video_size " + std::to_string(width) + "x" +  std::to_string(height) + " -pixel_format bgr24 -i pipe:  -crf 24 -pix_fmt yuv420p -f rtsp -rtsp_transport tcp rtsp://119.17.253.45:8556/mystream1";
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
				imwrite("name.jpg",frame);
                                write(socket_desc, find_name,20);
				send_image(&socket_desc);
			}
		// Show the frame
		        imshow("test", frame);
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
                            ffmpeg_cmd = std::string("ffmpeg -y -f rawvideo -r ") + std::to_string(20) + " -video_size " + std::to_string(width) + "x" +  std::to_string(height) + " -pixel_format bgr24 -i pipe:  -crf 24 -pix_fmt yuv420p -f rtsp -rtsp_transport tcp rtsp://119.17.253.45:8556/mystream1";
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
                            ffmpeg_cmd = std::string("ffmpeg -y -f rawvideo -r ") + std::to_string(30) + " -video_size " + std::to_string(width) + "x" +  std::to_string(height) + " -pixel_format bgr24 -i pipe:  -crf 24 -pix_fmt yuv420p -f rtsp -rtsp_transport tcp rtsp://119.17.253.45:8556/mystream1";
                            hd = 1;
                        }
		}
        }
        pthread_exit(NULL);
}

int main(){
	socket_init(&socket_desc);
	get_name(names);
	pthread_create(&thread1, NULL, &CaptureFunction, NULL);
	pthread_create(&thread2, NULL, &Receive, NULL);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
}
