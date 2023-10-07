#include "lib.hpp"

using namespace std;
using namespace cv;

const char *semName = "flag";
pthread_t thread1,thread2;
int mqtt_value, new_socket, new_socket2 = 0;
char mes_mqtt[20];

void* Socket2(void *arg)
{
        cout << "thread 2" << endl;
	int rc;
	struct mosquitto * mosq1;
        mosquitto_lib_init();
        mosq1 = mosquitto_new("test11", true, NULL);
        rc = mosquitto_connect(mosq1,"119.17.253.45", 1883, 60);
        if(rc != 0){
                printf("Client could not connect to broker! Error Code: %d\n", rc);
                mosquitto_destroy(mosq1);
                exit(0);
        }
	ofstream name_file("name.txt",ios_base::app);
        void *ptr;
	Mat frame;
	int shm_fd = shm_open("name", O_CREAT | O_RDWR, 0666);
        ftruncate(shm_fd, 30);
        ptr = mmap(0,30, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
        sem_t *sem_id = sem_open(semName, O_CREAT, 0600, 0);
        if (sem_id == SEM_FAILED){
                perror("Child   : [sem_open] Failed\n");
        }
        strcpy((char*)ptr,"1");
        while(new_socket > 0){
                cout << (char*)ptr << endl;
		if(new_socket <= 0)
                        break;
//              cout << add << endl;
                if (strcmp((char*)ptr,"1") != 0){
                        write(new_socket,(char*)ptr,20);
                        if (sem_wait(sem_id) < 0)
                                printf("Child  : [sem_wait] Failed\n");
                        send_image(&new_socket);
			string s((char*)ptr);
                        string s2 = "dataset/" + s + ".jpg";
                        frame = imread("name.jpg");
                        imwrite(s2,frame);
			name_file << s << endl;
			strcat((char*)ptr,"/");
			cout << (char*)ptr << endl;
			mosquitto_publish(mosq1, NULL, "test/t3", 20, (char*)ptr, 0, false);
	/*		int confirm = receive_message_confirm(&new_socket);
                        if (confirm == 1)			
			{
				cout << " Smart Camera đã nhận thành công " << endl;
                                string s((char*)ptr);
                                string s2 = "dataset/" + s + ".jpg";
                                frame = imread("name.jpg");
                                imwrite(s2,frame);
                        }*/
                        strcpy((char*)ptr,"1");
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
        struct mosquitto * mosq;

        mosquitto_lib_init();

        mosq = mosquitto_new("test", true, NULL);

        rc = mosquitto_connect(mosq,"119.17.253.45", 1883, 60);
        if(rc != 0){
                printf("Client could not connect to broker! Error Code: %d\n", rc);
                mosquitto_destroy(mosq);
                exit(0);
        }
        printf("We are now connected to the broker!\n");

        while(new_socket > 0){
                puts("Waiting for incoming connections...");
                receive_image2("test.jpg",&new_socket);
                if(new_socket <= 0)
                        break;
                frame = imread("test.jpg");
                if (frame.empty())
                {
                        printf("Hình ảnh vấn đề \n");
                        continue;
                }
                flip(frame, frame, 1);
        	mosquitto_publish(mosq, NULL, "test/t2", 16, "Co hinh anh", 0, false);
        	const std::string str = "photos/test" + to_string(i) + ".jpg";
        	imwrite(str,frame);
        	i = i + 1;
        }
        close(new_socket);
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
  	pthread_exit(NULL);
}

int main()
{
	pid_t pid_child1,pid_child2,pid_child3;
        int socket_desc, c = 0;
	int socket_desc2,c2 = 0;
        struct sockaddr_in client;
        socket_init(&socket_desc,&c);
	socket_init2(&socket_desc2,&c2);
	pid_child1 = fork();
        if (pid_child1 < 0) {
        	printf("ERROR in new process1 creation");
                exit(0);
        }
	else if (pid_child1 == 0) {
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
		close(new_socket);
		exit(0);
	}
	else{	
//	char name_add[20];
//	void* ptr;

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
		close(new_socket2);
		exit(0);

	}
	return 0;
}


