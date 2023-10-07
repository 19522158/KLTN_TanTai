#include "lib.hpp"

using namespace std;
using namespace cv;

const char *semName = "flag";
pthread_t thread1,thread2,thread3;
int mqtt_value, new_socket, new_socket2 = 0;
char mes_mqtt[20];
char mes_connect[4];

void on_connect(struct mosquitto *mosq1, void *obj, int rc){
    if(rc){
            printf("Erro with result code: %d\n", rc);
            exit(-1);
    }
    mosquitto_subscribe(mosq1, NULL, "test/t4",0);
}

void on_message(struct mosquitto *mosq1, void *obj, const struct mosquitto_message *msg){
    printf("New phone connection:%s\n",(char *) msg ->payload);
	strcpy(mes_connect,(char *)msg->payload);
	cout << mes_connect << endl;
}
	
void* Socket2(void *arg)
{
    cout << "thread 2" << endl;
	vector<string> names;
	get_name(names);
	int rc;
	int id = 41;
	struct mosquitto * mosq1;
    mosquitto_lib_init();
    mosq1 = mosquitto_new("test11", true, &id);
    rc = mosquitto_connect(mosq1,"119.17.253.45", 1883, 3600);
    if(rc != 0){
        printf("Client could not connect to broker! Error Code: %d\n", rc);
        mosquitto_destroy(mosq1);
        exit(0);
    }
	ofstream name_file("name.txt",ios_base::app);
    Mat frame;
	void *ptr;
	int shm_fd = shm_open("name", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, 30);
    ptr = mmap(0,30, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    sem_t *sem_id = sem_open(semName, O_CREAT, 0600, 0);
    if (sem_id == SEM_FAILED){
        perror("Child   : [sem_open] Failed\n");
    }
	string s;
	string s2;
    strcpy((char*)ptr,"1");
    while(new_socket > 0){
               // cout << (char*)ptr << endl;
		if(new_socket <= 0)
            break;
//              cout << add << endl;
        if (strcmp((char*)ptr,"1") != 0){
            write(new_socket,(char*)ptr,20);
            if (sem_wait(sem_id) < 0)
                printf("Child  : [sem_wait] Failed\n");
            send_image(&new_socket);
			s = ((char*)ptr);
            s2 = "dataset/" + s + ".jpg";
            frame = imread("name.jpg");
            imwrite(s2,frame);
			s = s + "/";
			//name_file << s << endl;
			strcat((char*)ptr,"/");
	//		cout << (char*)ptr << endl;
		//	mosquitto_publish(mosq1, NULL, "test/t3", 20, (char*)ptr, 0, false);
	/*		int confirm = receive_message_confirm(&new_socket);
                        if (confirm == 1)			
			{
				cout << " Smart Camera đã nhận thành công " << endl;
                                string s((char*)ptr);
                                string s2 = "dataset/" + s + ".jpg";
                                frame = imread("name.jpg");
                                imwrite(s2,frame);
                        }*/
  //                      strcpy((char*)ptr,"1");
			if(std::find(names.begin(), names.end(), s) != names.end()){
				cout << "Da co ten trong danh sach " << endl;
				strcpy((char*)ptr,"1");
				continue;
			}
			else{
				mosquitto_publish(mosq1, NULL, "test/t5", 20, (char*)ptr, 0, false);
				names.push_back(s);
				name_file << s << endl;
				cout << (char*)ptr << endl;
				strcpy((char*)ptr,"1");
			}
		//	strcpy((char*)ptr,"1");
        }

            sleep(1);
        }
        if (sem_close(sem_id) != 0){
                perror("Parent  : [sem_close] Failed\n");
        }

        if (sem_unlink(semName) < 0){
                printf("Parent  : [sem_unlink] Failed\n");
        }
	shm_unlink("name");
	mosquitto_destroy(mosq1);
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
	
        rc = mosquitto_connect(mosq,"119.17.253.45", 1883, 3600);
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
		void *ptr1;
		int shm_fd1 = shm_open("cam_status", O_CREAT | O_RDWR, 0666);
    	ptr1 = mmap(0,10, PROT_WRITE |PROT_READ, MAP_SHARED, shm_fd1, 0);
		memset((char*)ptr1, 0,10);
		strcpy((char*)ptr1,"Co/");
		mosquitto_publish(mosq, NULL, "test/t2", 10, (char*)ptr1, 0, false);
        while(new_socket > 0){
                puts("Waiting for incoming connections...");
				recv(new_socket, name, 20,0);
                int t = receive_image2("test.jpg",&new_socket);
                if(t ==2 || new_socket <= 0)
                        break;

                frame = imread("test.jpg");
                if (frame.empty())
                {
                        printf("Hình ảnh vấn đề \n");
                        continue;
                }
				count = count + 1;
                cout << "So luong anh nhan duoc: " << count << endl;
                flip(frame, frame, 1);
        	mosquitto_publish(mosq, NULL, "test/t2", 16, "Co hinh anh", 0, false);
        	s = name;
		s = "photos/" + s;
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
	int id3 = 49;
	void *ptr;
	int shm_fd = shm_open("cam_status", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, 10);
    ptr = mmap(0,10, PROT_WRITE |PROT_READ, MAP_SHARED, shm_fd, 0);
	memset((char*)ptr, 0,10);
	strcpy((char*)ptr,"Khong/");
//	char confirm[1] = {1};
	struct mosquitto *mosq1;
	struct mosquitto *mosq2;
	struct mosquitto *mosq3;
	mosquitto_lib_init();
	mosquitto_lib_init();
	mosquitto_lib_init();
	mosq1 = mosquitto_new("sub_sendname_t4",true, &id1);
	mosq2 = mosquitto_new("pub_sendname_t5",true, &id2);
	mosq3 = mosquitto_new("pub_sendstatus_t2",true, &id3);
	int rc1 = mosquitto_connect(mosq1,"119.17.253.45",1883,3600);
	if(rc1 != 0) {
        printf("Could not connect to Broker in sub_sendname with return code %d\n", rc1);
        mosquitto_destroy(mosq1);
		exit(1);
    }
	int rc2 = mosquitto_connect(mosq2,"119.17.253.45",1883,3600);
	if(rc2 != 0) {
        printf("Could not connect to Broker in pub_sendname with return code %d\n", rc1);
        mosquitto_destroy(mosq2);
		exit(1);
    }
	int rc3 = mosquitto_connect(mosq3,"119.17.253.45",1883,3600);
	if(rc3 != 0) {
        printf("Could not connect to Broker in pub_sendname with return code %d\n", rc1);
        mosquitto_destroy(mosq3);
		exit(1);
    }
	mosquitto_connect_callback_set(mosq1, on_connect);
	mosquitto_message_callback_set(mosq1, on_message);
	mosquitto_loop_start(mosq1);
	char name_array[100];
	while(1){
		if(strcmp(mes_connect,"123")==0)
		{
			read_name_file(name_array);
		//	cout << "gui tin hieu " << endl;
			mosquitto_publish(mosq2, NULL, "test/t5", 70, name_array, 0, false);
			cout << "gui danh sach ten thanh cong" << endl;
			mosquitto_publish(mosq2, NULL, "test/t2", 10, (char*)ptr, 0, false);
			memset(mes_connect,0,sizeof(mes_connect));
			memset(name_array,0,sizeof(name_array));
		}
	}
	mosquitto_loop_stop(mosq1, true);
	mosquitto_disconnect(mosq1);
    mosquitto_destroy(mosq1);
	mosquitto_disconnect(mosq2);
    mosquitto_destroy(mosq2);
	shm_unlink("cam_status");
	mosquitto_destroy(mosq3);
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
//        	int socket_desc2,c2 = 0;
        struct sockaddr_in client;
        socket_init(&socket_desc,&c);
//        	socket_init2(&socket_desc2,&c2);
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
//	char name_add[20];
//	void* ptr
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
//	close(socket_desc);
//	close(socket_desc2);
	return 0;
}


