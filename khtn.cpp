#include "lib.hpp"


using namespace std;
using namespace cv;

const char *semName = "flag";
pthread_t thread1,thread2,thread3;
int mqtt_value, new_socket, new_socket2 = 0;
char mes_name[20] = "0";
char mes_connect[4];

void on_connect(struct mosquitto *mosq, void *obj, int rc){
    if(rc){
            printf("Erro with result code: %d\n", rc);
            exit(-1);
    }
    mosquitto_subscribe(mosq, NULL, "test/t4",0);
    mosquitto_subscribe(mosq, NULL, "test/t1",0);
}

void on_message(struct mosquitto *mosq1, void *obj, const struct mosquitto_message *msg){
 //   printf("New phone connection:%s\n",(char *) msg ->payload);
	if(strcmp(msg->topic, "test/t1") == 0 ){
		strcpy(mes_name,(char *)msg->payload);
	//	printf("Topic is test/t1\n");
	}
	else if(strcmp(msg->topic, "test/t4") == 0 )
	{
		strcpy(mes_connect,(char *)msg->payload);
	//	cout << mes_connect << endl;
	//	printf("Topic is test/t4\n");
	}
	else
		printf("nothing");
}
	
void* Socket2(void *arg)
{
    cout << "thread 2" << endl;
	vector<string> names;
	get_name(names);
	//int rc;
	int id = 41;

	ofstream name_file("/var/www/html/live/name.txt",ios_base::app);
    Mat frame;
	struct mosquitto * mosq1;
	string s;
	string s2;
    while(new_socket > 0){
		if(new_socket <= 0)
            break;
		string name_receive = checkfile("/home/tien");
        if (name_receive != ""){
			cout << "Name Receive: " << name_receive << endl;
			s = name_receive;
//			std::string name_receive_file = std::string("/home/newftpuser/") + s + std::string(".jpg");
			if(!crop_face(std::string("/home/tien/") + s + std::string(".jpg"))){
				cout << "Khong nhan dang duoc nguoi " << endl;
				remove((std::string("/home/tien/") + s + std::string(".jpg")).c_str());
				continue;
			}
			cout << "Nhan dang va gui anh " << endl;
			write(new_socket, name_receive.c_str(),20);
            send_image(&new_socket);
            s2 = "/var/www/html/live/dataset/" + s + ".jpg";
            frame = imread("name.jpg");
            imwrite(s2,frame);
			remove((std::string("/home/tien/") + s + std::string(".jpg")).c_str());
			s = s + "/";
			if(std::find(names.begin(), names.end(), s) != names.end()){
				cout << "Da co ten trong danh sach " << endl;
				write_noti(4,name_receive.c_str());
				write_to_json();
				continue;
			}
			else{
				write_noti(3,name_receive.c_str());
				write_to_json();
				strcat((char *)name_receive.c_str(),"/");
				mosquitto_publish(mosq1, NULL, "test/t5", 20, name_receive.c_str(), 0, false);
				
				names.push_back(s);
				name_file << s << endl;
				write_name_to_json();
				cout << name_receive << endl;
				
			}
			
			
        }

            sleep(1);
        }

	name_file.close();
	mosquitto_destroy(mosq1);
	pthread_exit(NULL);
}

void* Receive(void *arg)
{
        Mat frame;
        int i = 0;
        int rc;
		int id = 73;
        struct mosquitto * mosq;

        mosquitto_lib_init();
	
        mosq = mosquitto_new("test", true, &id);
	
        rc = mosquitto_connect_async(mosq,"119.17.253.45", 1883, 43000);
        if(rc != 0){
                printf("Client could not connect to broker! Error Code: %d\n", rc);
                mosquitto_destroy(mosq);
                exit(0);
        }
        printf("We are now connected to the broker!\n");
		char name[20];
        char name_back[20] = { };
        string s;
        string str;
		int count = 0;
		Json::Value event;
		Json::Value vec(Json::arrayValue);
		void *ptr1;
		int shm_fd1 = shm_open("cam_status", O_CREAT | O_RDWR, 0666);
    	ptr1 = mmap(0,10, PROT_WRITE |PROT_READ, MAP_SHARED, shm_fd1, 0);
		memset((char*)ptr1, 0,10);
		strcpy((char*)ptr1,"Co/");
		mosquitto_publish(mosq, NULL, "test/t2", 10, (char*)ptr1, 0, false);
        while(new_socket > 0){
                puts("Waiting for incoming connections...");
				int t1 = recv(new_socket, name, 20,0);
				cout << "Name: " << name << endl;
				if(t1<=0){
					new_socket = 0;
					break;
				}
                int t = receive_image2("/var/www/html/live/test.jpg",&new_socket);
                
				if (t==2){
					printf("Nhan hinh anh that bai \n");
					continue;
				}
				if(new_socket <= 0)
                    break;
                frame = imread("/var/www/html/live/test.jpg");
                if (frame.empty())
                {
                        printf("Hình ảnh vấn đề \n");
                        continue;
                }
				count = count + 1;
                cout << "So luong anh nhan duoc: " << count << endl;
                flip(frame, frame, 1);
        		mosquitto_publish(mosq, NULL, "test/t2", 16, "Tim thay nguoi/", 0, false);
        		write_noti(2,name);
				write_to_json();
				s = name;
				s = "/var/www/html/live/photos/" + s;
                if(strcmp(name,name_back) != 0)
                {
                        i = count_files(s);
                        i = i + 1;
                }
                str = s +"/" + to_string(i) + ".jpg";
                imwrite(str,frame);
                i = i + 1;
                strncpy(name_back,name,20);
        }
        close(new_socket);
		memset((char*)ptr1, 0,10);
		strcpy((char*)ptr1,"Khong/");
		mosquitto_publish(mosq, NULL, "test/t2", 10, (char*)ptr1, 0, false);
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
  	pthread_exit(NULL);
}

void* DataName(void* arg){
	int id1 = 22;
	int id2 = 53;
//	int id3 = 49;
	int id4 = 58;
	void *ptr;
	int shm_fd = shm_open("cam_status", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, 10);
    ptr = mmap(0,10, PROT_WRITE |PROT_READ, MAP_SHARED, shm_fd, 0);
	memset((char*)ptr, 0,10);
	strcpy((char*)ptr,"Khong/");
	struct mosquitto *mosq1;
	struct mosquitto *mosq2;
	struct mosquitto *mosq4;
	mosquitto_lib_init();
	mosquitto_lib_init();
	mosquitto_lib_init();
	mosq1 = mosquitto_new("sub_sendname_t4",true, &id1);
	mosq2 = mosquitto_new("pub_sendname_t5",true, &id2);
	mosq4 = mosquitto_new("sub_sendnoti_t1",true, &id4);
	int*rc1 = new (int);
	*rc1 = mosquitto_connect_async(mosq1,"119.17.253.45",1883,43000);
	if(*rc1 != 0) {
        printf("Could not connect to Broker in sub_sendname with return code %d\n", *rc1);
        mosquitto_destroy(mosq1);
		exit(1);
    	}
	delete(rc1);	
	int* rc2 = new (int);
	*rc2 = mosquitto_connect_async(mosq2,"119.17.253.45",1883,43000);
	if(*rc2 != 0) {
        printf("Could not connect to Broker in pub_sendname with return code %d\n", *rc2);
        mosquitto_destroy(mosq2);
		exit(1);
    	}	  
	delete(rc2);
	int* rc4 = new (int);
	*rc4 = mosquitto_connect_async(mosq4,"119.17.253.45",1883,43000);
	if(*rc4 != 0){
		printf("Could not connect to rc4 \n");
		exit(1);
	}
	delete(rc4);
	
	mosquitto_connect_callback_set(mosq1, on_connect);
	mosquitto_message_callback_set(mosq1, on_message);
	mosquitto_message_callback_set(mosq4, on_message);
	mosquitto_loop_start(mosq1);
	mosquitto_loop_start(mosq4);
	char name_array[100];
	while(1){
		if(strcmp(mes_connect,"123")==0)
		{
			read_name_file(name_array);
		//	cout << "gui tin hieu " << endl;
			write_name_to_json();
			mosquitto_publish(mosq2, NULL, "test/t5", 70, name_array, 0, false);
	//		cout << "gui danh sach ten thanh cong" << endl;
			mosquitto_publish(mosq2, NULL, "test/t2", 10, (char*)ptr, 0, false);
			memset(mes_connect,0,sizeof(mes_connect));
			memset(name_array,0,sizeof(name_array));
		}
		if(strcmp(mes_name,"0") !=0){
			write_noti(1,mes_name);
			write_to_json();
			strcpy(mes_name,"0");
		}
	}
	mosquitto_loop_stop(mosq1, true);
	mosquitto_loop_stop(mosq4, true);
	mosquitto_disconnect(mosq1);
    mosquitto_destroy(mosq1);
	mosquitto_disconnect(mosq2);
    mosquitto_destroy(mosq2);
	shm_unlink("cam_status");
	mosquitto_destroy(mosq4);
    mosquitto_lib_cleanup();
	pthread_exit(NULL);

}

int main()
{
	pid_t pid_child1,pid_child2,pid_child3;
	pthread_create(&thread3, NULL, &DataName, NULL);
	pid_child1 = fork();
        if (pid_child1 < 0) {
        	printf("ERROR in new process1 creation");
                exit(0);
        }
	else if (pid_child1 == 0) {
		int socket_desc, c = 0;
        struct sockaddr_in client;
        socket_init(&socket_desc,&c);
		while(1){
			if((new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c)))
                        	puts("Connection accepted");
			fflush(stdout);
			pid_child2 = fork();
			if (pid_child2 < 0) {
                printf("ERROR in new process2 creation");
                exit(0);
            }
			if (pid_child2 == 0) {
				close(socket_desc);

				struct timeval tv;
				tv.tv_sec = 50000;             
   				tv.tv_usec = 0;
				if (setsockopt (new_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv) < 0)
       				printf("setsockopt failed\n");

    			if (setsockopt (new_socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof tv) < 0)
        			printf("setsockopt failed\n");
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
                	}
		}
		close(socket_desc);
		exit(0);
	}
	else{	
        	int socket_desc2,c2 = 0;
        	struct sockaddr_in client;
        	socket_init2(&socket_desc2,&c2);
		while(1){
			if((new_socket2 = accept(socket_desc2, (struct sockaddr *)&client,(socklen_t*)&c2)))
				puts("Connection accepted socket 2");
            fflush(stdout);
			pid_child3 = fork();
			if (pid_child3 < 0) {
                    printf("ERROR in new process2 creation");
                    exit(0);
            }
			if(pid_child3 == 0){
				close(socket_desc2);
				receive_new_image(&new_socket2,semName); 
			   	close(new_socket2);	
          			cout << "Socket 2 done" << endl;
				exit(0);
			}      
			else
				close(new_socket2);
		}
		close(socket_desc2);
		exit(0);
	}
	pthread_join(thread3, NULL);
	return 0;
}


