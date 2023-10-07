
#include "util1.h"

using namespace std;
pthread_mutex_t lock1;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int nhandang = 0;
int id_count = 1;
int rc;
int mqtt_value = 0;
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

void Receive()
{
	const int id = id_count;
	const std::string image_file = "test" + to_string(id) + ".jpg";
        int n_name = image_file.size();
	char image_file_name[n_name+1];
//      cout << "khai báo chuỗi " << endl;
        strcpy(image_file_name, image_file.c_str());
	
	const std::string image_file_predict = "test_predict" + to_string(id) + ".jpg";
	n_name = image_file_predict.size();
	char image_file_name_predict[n_name+1];
//      cout << "khai báo chuỗi " << endl;
        strcpy(image_file_name_predict, image_file_predict.c_str());	
     Mat myImage;
        int i = 1;
	CascadeClassifier face_cascade;
    string face_cascade_name = "haarcascade_frontalface_alt2.xml";
    string fn_csv = string("face_db.csv");
    vector<string> names = {"Tan Tai", "Unknow", "Unknow"};
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

	clock_t start, end; 
	double time_use;
	int n;
	Mat frame;
	int rc;
	struct mosquitto * mosq;

	int tong, nhandangduoc,khongduoc,loop = 0;
	mosquitto_lib_init();

	mosq = mosquitto_new("test", true, NULL);

	rc = mosquitto_connect(mosq,"119.17.253.45", 1883, 60);
	if(rc != 0){
		printf("Client could not connect to broker! Error Code: %d\n", rc);
		mosquitto_destroy(mosq);
		exit(0);
	}
	double trungbinh_time = 0;
	printf("We are now connected to the broker!\n");

       	while(new_socket > 0)
	{
//   	  	

     	puts("Waiting for incoming connections...");
	  
	
	receive_image2(image_file_name,&new_socket);
	 cout << "Camera gửi " << image_file_name << endl;
	 if(new_socket <= 0)
	 {
		 break;
		 cout << " out ra socket " << endl;
	}
	 cout << " nhận diện " << endl;
	frame = imread(image_file_name);  
	if (frame.empty())
	{
		printf("Hình ảnh vấn đề \n");
		continue;
	}
	//	   pthread_cond_signal(&cond);


//  	   pthread_mutex_unlock(&lock1);
 	        flip(frame, frame, 1);
		string name = detect_and_predict(face_cascade, model, frame, names,image_file_name_predict);
		cout << " detect " << endl;
        // Detect and predict all faces in frame

  //      char* name;

	//	char name[10];
//	strcpy(name, detect_and_predict(face_cascade, model, frame, names));
	 end = clock(); 
	 time_use = (double)(end - start) / CLOCKS_PER_SEC;
//	  cout<<"Thoi gian chay nhận dạng: "<<time_use << endl;
	//	cout << " detect xong " << name << endl;
	n = name.size();

//	cout << " lấy size " << n << endl;
	char t_array[n+1];
//	cout << "khai báo chuỗi " << endl;
	strcpy(t_array, name.c_str());
//	cout << "so sánh " << endl;
	if(strcmp(t_array,"Tan Tai") == 0)
	{
		nhandang = 1;
		nhandangduoc = nhandangduoc + 1;
		
	}
	else if(strcmp(t_array,"Unknow")==0)
	{
		khongduoc = khongduoc + 1;
	}
	else
		continue;
	tong = tong + 1;
	cout << " Tổng :" << tong << endl;
	trungbinh_time = trungbinh_time + time_use;
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
	      
	int socket_desc, c = 0;
        struct sockaddr_in client;
        socket_init(&socket_desc,&c);
	pid_t pid_child;
    	while(1)
    	{
      		if((new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c)))
      		{
	  		cout << endl;
	  		cout << endl;
	  		cout << " Đã có thiết bị mới kết nối" << endl;
          		puts("Connection accepted");
	  		cout << "ID Camera và đường dẫn file là : " << id_count << endl;     
      		}
      		fflush(stdout);	
      		pid_child = fork();
      		if (pid_child < 0) {
              		printf("ERROR in new process creation");
	      		exit(0);
         	}
      		if (pid_child == 0) {
            //child process
            		close(socket_desc);
	    		int mes_first[4];
	    		read(new_socket, &mes_first,sizeof(int)*4);
	    		for(int i = 0; i < 4 ; i++)
		    		cout << mes_first[i];
	    		string create_zoneminder;
	    		if(mes_first[0] == 1)
		    		create_zoneminder = "./smart.sh " + to_string(mes_first[1]) + " "+ to_string(mes_first[2]) + " " + to_string(id_count);
	    		else if(mes_first[0] ==2){
		    		write(new_socket, &id_count, sizeof(int));
		    		create_zoneminder = "./stream.sh " + to_string(mes_first[1]) + " "+ to_string(mes_first[2]) + " " + to_string(id_count);
	    		}
	    		else if(mes_first[0] ==3){
		    		write(new_socket, &id_count, sizeof(int));
                    		create_zoneminder = "./stream.sh " + to_string(mes_first[1]) + " "+ to_string(mes_first[2]) + " " + to_string(id_count);
		    		id_count = id_count + 1;
			}
	    		else 
		    	cout << "No Know Cam " << endl;
        		int n_create_zoneminder = create_zoneminder.size();
        		char create_zoneminder_char[n_create_zoneminder+1];
//      cout << "khai báo chuỗi " << endl;
        		strcpy(create_zoneminder_char, create_zoneminder.c_str());

//			system(create_zoneminder_char);
	    		Receive();
	    		close(new_socket);
	    		printf("\n");
	    		printf("Waiting for other connection \n");
	    		exit(0);
      		}
		else {
             //parent process
             		close(new_socket);
	     		id_count = id_count + 1;
          	}
    	}
    	close(socket_desc);
    	return 0;
}
