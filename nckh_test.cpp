#include "lib.hpp"

using namespace std;
using namespace cv;
pthread_t thread1,thread2;

int mqtt_value, new_socket = 0;
char mes_mqtt[10];

int t;
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
	strcpy(mes_mqtt, mes.c_str());	
	cout << mes_mqtt << endl;
//	cout << (char *) msg->payload	<< endl
	mqtt_value = 1;
}
void* Socket2(void *arg)
{
    //Listen
    int rc;
            
    int  id=12;
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
	cout << new_socket << endl;
    	while(new_socket > 0)
        {
                if(mqtt_value >0)
                {
                        send(new_socket,mes_mqtt,10, 0);
              // 		send(new_socket,mes,10, 0);
			cout << "gui tin hieu " << endl;	
                        mqtt_value = 0;
                        sleep(2);
                }

        }
        mosquitto_loop_stop(mosq, true);
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        pthread_exit(NULL);
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
	char name[20];
        char name_back[20] = { };
        string s;
	string str;
        while(new_socket > 0){
               puts("Waiting for incoming connections...");
	       recv(new_socket, name, 20,0);
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
        	mosquitto_publish(mosq, NULL, "test/t2", 16, "Có hình ảnh mới", 0, false);
           	
	//	const std::string str = "photos/test" + to_string(i) + ".jpg";
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
//       dem = 1;

        }

    	close(new_socket);
//    fflush(stdout);
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);

        mosquitto_lib_cleanup();
//      printf("CHờ thiết bị khác kết nối\n");
        pthread_exit(NULL);
 //      exit(0);
  //  return NULL;
}


int main()
{
	pid_t pid_child;
	int socket_desc, c = 0;
	struct sockaddr_in client;
	socket_init(&socket_desc,&c);
    	while(1)
    	{
      		if((new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c)))
          		puts("Connection accepted");

      		fflush(stdout);
      		pid_child = fork();
      		if (pid_child < 0) {
              		printf("ERROR in new process creation");
              		exit(0);
         	}
      		if (pid_child == 0) {
            //child process
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
    	return 0;
}
