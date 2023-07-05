#include "lib.hpp"

using namespace std;


void socket_init(int* socket_desc, int* c){
    struct sockaddr_in server,client;
    //Create socket
    *socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (*socket_desc == -1)
    {
    	printf("Could not create socket");
    }
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8889 );
    //Bind
    while(bind(*socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
      cout << "bind failed" << endl;
      exit(1);
    }

    cout << "bind done" << endl;
    //Listen

    listen(*socket_desc , 3); // int listen(int s, int backlog); | `backlog` (3)
    //limits the number of outstanding connections in the socket's listen
    //queue to the value specified by the backlog argument.
    *c = sizeof(struct sockaddr_in);
}

void socket_init2(int* socket_desc2, int* c2){
    struct sockaddr_in server,client;
    //Create socket
    *socket_desc2 = socket(AF_INET , SOCK_STREAM , 0);
    if (*socket_desc2 == -1)
    {
        printf("Could not create socket 2");
    }
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8879 );
    //Bind
    while(bind(*socket_desc2,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
      cout << "bind failed" << endl;
       exit(1);
    }

    cout << "bind done" << endl;
    //Listen

    listen(*socket_desc2 , 3); // int listen(int s, int backlog); | `backlog` (3)
    //limits the number of outstanding connections in the socket's listen
    //queue to the value specified by the backlog argument.
    *c2 = sizeof(struct sockaddr_in);
}

void send_image(int* socket_desc)
{
    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256];
    packet_index = 1;
    picture = fopen("/var/www/html/live/name.jpg", "r");
    printf("Getting Picture Size\n");
    if(picture == NULL) {
         printf("Error Opening Image File"); }
    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    printf("Total Picture size: %i\n",size);
    //Send Picture Size
    printf("Sending Picture Size\n");
    write(*socket_desc, (void *)&size, sizeof(int));
    //Send Picture as Byte Array
    printf("Sending Picture as Byte Array\n");
    printf("Received data in socket\n");
    printf("Socket data: %s\n", read_buffer);
    while(!feof(picture)) {
       //Read from the file into our send buffer
       read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);
       //Send data through our socket
       do{
         stat = write(*socket_desc, send_buffer, read_size);
       }while (stat < 0);
       printf("Packet Number: %i\n",packet_index);
       printf("Packet Size Sent: %i\n",read_size);
       printf(" \n");
       printf(" \n");
       packet_index++;
//      /Zero out our send buffer
       bzero(send_buffer, sizeof(send_buffer));
        }
}

int count_files(string& dir_path){
	int count = 0;
	struct dirent* entry;
	DIR* dir = opendir(dir_path.c_str()); // open directory
	if(dir){
    		while ((entry = readdir(dir)) != NULL) { // read directory entries
        		if (entry->d_type == DT_REG) { // if it is a regular file
            			count++;
        		}
    		}
		closedir(dir);
	}
	else if (ENOENT == errno) {
		int status = mkdir(dir_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        	if (status != 0) {
            		std::cout << "Failed to create the folder." << std::endl;
        	} 
		else {
            		std::cout << "The folder has been created." << std::endl;
        	}
    	} 
	else {
        	std::cout << "Failed to open the folder." << std::endl;
    	}
//    	closedir(dir); // close directory
    	return count;
}

int receive_image2(char image_file_name[], int* new_socket)
{ // Start function

        int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
        char imagearray[10241],verify = '1';
        FILE *image;
//Find the size of the image
      
      //	stat = read(*new_socket, &size, sizeof(int));
	int t = recv(*new_socket, &size, sizeof(int),0);
	if(t <= 0)
        {
                *new_socket = 0;
                return 2;
        }
        
        printf("Image size: %i\n",size);
        printf(" \n");
        if(size == 0 )
        {
                printf("Thiết bị ngắt kết nối \n");
                *new_socket = 0;
                return 2;
        }
        char buffer[] = "Got it";
        printf(" \n");
        image = fopen(image_file_name, "w");
        if( image == NULL) {
                printf("Error has occurred. Image file could not be opened\n");
                return 2;
        }
//Loop while we have not received the entire file yet
        int need_exit = 0;
        struct timeval timeout = {3,0};
        fd_set fds;
        int buffer_fd, buffer_out;
        while(recv_size < size) {
//while(packet_index < 2){
                FD_ZERO(&fds);
                FD_SET(*new_socket,&fds);
                buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);
                if (buffer_fd < 0){
                        printf("error: bad file descriptor set.\n");
			return 2;
		}
                if (buffer_fd == 0)
                {
                        printf("error: buffer read timeout expired.\n");
                        return 2;
                }

                if (buffer_fd > 0)
                {
                        do{
                                read_size = read(*new_socket,imagearray, 10241);
                        }while(read_size <0);

        //Write the currently read data into our image file
                        write_size = fwrite(imagearray,1,read_size, image);
                        if(read_size !=write_size) {
                                printf("error in read write\n");    }
             //Increment the total number of bytes read
                        recv_size += read_size;
                        packet_index++;

                }
        }
//      return image;
        fclose(image);
        printf("Image successfully Received!\n");
        return 1;
}

int receive_message_confirm(int* new_socket)
{
	char recv_data[4];
	fd_set fds;
	struct timeval timeout = {10,0};
	FD_ZERO(&fds);
        FD_SET(*new_socket,&fds);
	int buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);
	if (buffer_fd < 0){
        	printf("error: bad file descriptor set.\n");
                return 0;
        }
        if (buffer_fd == 0)
        {
        	printf("error: buffer read timeout expired.\n");
                return 0;
        }
	if (buffer_fd > 0)
	{
		read(*new_socket, recv_data, 4);
		return 1;
	}
	return 1;
}

void receive_new_image(int* new_socket2, const char* semName)
{
	char name_add[20];
        void* ptr;
//	signal(SIGPIPE, sigpipe_handler);
	int shm_fd = shm_open("name", O_RDWR , 0666);
        ptr = mmap(0,30, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
        sem_t *sem_id = sem_open(semName, O_CREAT, 0600, 0);
        if (sem_id == SEM_FAILED){
        	perror("Parent  : [sem_open] Failed\n");

         }
	cout << "Loop socket 2 " << endl;
	int t;
         while(*new_socket2 > 0)
         {
         //	read(*new_socket2, name_add,sizeof(char)*19);
           	t = recv(*new_socket2, name_add, 19,0);
		cout << t << endl;
		if(t<=0)
		{
			*new_socket2 = 0;
		}
		if(*new_socket2 <= 0)
		{
			close(*new_socket2);
                        break;
		}
		cout << name_add << endl;
		cout << " luu anh " << endl;
	    	strcpy((char*)ptr,name_add);
		cout << (char*)ptr << endl;
                int recv_imagenew = receive_image2("/var/www/html/live/name.jpg",new_socket2);
                cout << " nhan anh thanh cong " << endl;
		if (recv_imagenew == 1)
                {
                 	if (sem_post(sem_id) < 0)
                        	printf("Parent   : [sem_post] Failed \n");
                }
		else if(recv_imagenew = 2)
		{
			*new_socket2 = 0;
			break;
		}	
                cout << (char*)ptr << endl;
//                                      send(new_socket, &name_add,20 , 0);
         }

//	 close(*new_socket2);
  //       shm_unlink("name");
         printf("Waiting for other connect 2     \n");
}

void read_name_file(char* name_array){
	ifstream myfile("/var/www/html/live/name.txt");
	string line;
	while(getline(myfile, line)){
		strcat(name_array, line.c_str());
	}
	myfile.close();
}

void get_name(vector<string>& names)
{
	ifstream myfile("/var/www/html/live/name.txt");
	string line;
	while (getline(myfile, line))
    	{
        	names.push_back(line);
    	}
    	myfile.close();
}

void sigpipe_handler(int signum) {
    // Xử lý tín hiệu SIGPIPE, ví dụ đóng kết nối và thoát khỏi chương trình
    printf("Bị ngắt kết nối\n");
    exit(1);
}

const std::string get_time(){
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
 
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);  
    return ctime(&end_time);
}

void write_noti(int type,char* name){
        ofstream noti("/var/www/html/live/noti.txt",ios_base::app);
        string line(name);
        if(type == 1){
                line = "Nguoi dung yeu cau/" + line + "/" + get_time();  
        }
        else if(type == 2){
                line = "Co hinh anh moi/" + line + "/" + get_time();  
        }
        else if(type == 3){
                line = "Them doi tuong moi/" + line + "/" + get_time();
        }
        else if(type == 4){
                line = "Cap nhat lai doi tuong/" + line + "/" + get_time();
        }
        noti << line;
        noti.close();
}

std::vector<std::string> split (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

void write_to_json(){
        ofstream noti("/var/www/html/live/noti.json");
        ifstream noti_txt("/var/www/html/live/noti.txt");
        Json::Value event;
	Json::Value vec(Json::arrayValue);
        string line;
	int i = 9999;
        vector<string> v;
        Json::StyledWriter styledWriter;
        while(getline(noti_txt,line)){
		
                v = split(line,'/');
                event[to_string(i)]["type"] = v[0];
		event[to_string(i)]["name"] = v[1];
		event[to_string(i)]["time"] = v[2];
		i = i - 1;
        }
        noti << styledWriter.write(event);
        noti.close();
        noti_txt.close();
}
