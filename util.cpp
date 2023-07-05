#include "util.h"

// Load Face Cascade for face detection
int loadCascade(CascadeClassifier& face_cascade, string face_cascade_name)
{
    if(!face_cascade.load( face_cascade_name ))
    {
        cout << "[ERROR] Error loading " + face_cascade_name + "\n";
        return -1;
    }
    else
    {
        cout << "[INFO] Successfully loaded face cascade\n";
        return 0;
    }
}

// Check camera availability
int checkCamera(VideoCapture cap)
{
    if (!cap.isOpened())
    {
        cout << "[ERROR] Error initializing video camera!\n" << endl;
        return -1;
    }
    else
    {
        cout << "[INFO] Starting camera...\n";
        cout << "[INFO] Press \'Esc\' to close the program\n";
        return 0;
    }
}

int receive_image2(char* image_file_name, int* new_socket)
{ // Start function

        int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
        char imagearray[10241],verify = '1';
        FILE *image;
//Find the size of the image
        do{
                stat = read(*new_socket, &size, sizeof(int));
        }while(stat<0);

        printf("Image size: %i\n",size);
        printf(" \n");
        if(size == 0 )
        {
            printf("Thiết bị ngắt kết nối \n");
            *new_socket = 0;
            return 0;
        }
        char buffer[] = "Got it";
        printf(" \n");
        image = fopen(image_file_name, "w");
        if( image == NULL) {
                printf("Error has occurred. Image file could not be opened\n");
                exit(1);
        }
//Loop while we have not received the entire file yet
        int need_exit = 0;
        struct timeval timeout = {10,0};
        fd_set fds;
        int buffer_fd, buffer_out;
        while(recv_size < size) {
//while(packet_index < 2){
                FD_ZERO(&fds);
                FD_SET(*new_socket,&fds);
                buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);
                if (buffer_fd < 0)
                        printf("error: bad file descriptor set.\n");

                if (buffer_fd == 0)
                {
                        printf("error: buffer read timeout expired.\n");
                        return 0;
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


void socket_init(int* socket_desc){
        struct sockaddr_in server;
        *socket_desc = socket(AF_INET , SOCK_STREAM , 0);
        if (*socket_desc == -1) {
                exit(0);
        }
        memset(&server,0,sizeof(server));
        server.sin_addr.s_addr = inet_addr("119.17.253.45");
        server.sin_family = AF_INET;
        server.sin_port = htons( 8889 );
//        fcntl(*socket_desc, F_SETFL, SOCK_NONBLOCK);
        if (connect(*socket_desc , (struct sockaddr *)&server , sizeof(server)) <0){
                 cout<<strerror(errno);
                close(*socket_desc);
                puts("Connect Error");
                exit(0);
        }
}

void send_image(int* socket_desc)
{
    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256];
    packet_index = 1;
    picture = fopen("/home/hoang/Desktop/KLTN/name.jpg", "r");
  //  printf("Getting Picture Size\n");
    if(picture == NULL) {
         printf("Error Opening Image File"); }
    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    printf("Total Picture size: %i\n",size);
    //Send Picture Size
//    printf("Sending Picture Size\n");
 //   write(*socket_desc, (void *)&size, sizeof(int));
    send(*socket_desc, &size, sizeof(int), 0);
    //Send Picture as Byte Array
//    printf("Sending Picture as Byte Array\n");
//    printf("Received data in socket\n");
//    printf("Socket data: %s\n", read_buffer);
    while(!feof(picture)) {
       //Read from the file into our send buffer
       read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);
       //Send data through our socket
       do{
         stat = write(*socket_desc, send_buffer, read_size);
       }while (stat < 0);
    //   printf("Packet Number: %i\n",packet_index);
    //   printf("Packet Size Sent: %i\n",read_size);
    //   printf(" \n");
    //   printf(" \n");
       packet_index++;
//      /Zero out our send buffer
       bzero(send_buffer, sizeof(send_buffer));
        }
    printf("Sending Picture Successlully\n");
}

// Detect and recognize all detected faces
int detect_and_predict(CascadeClassifier face_cascade, Ptr<FaceRecognizer> model, Mat& frame, vector<string> names,char* name)
{
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    //-- Detect faces
    vector<Rect> faces;
    int flag = 0;
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 5);
    for (size_t i = 0; i < faces.size(); i++)
    {
        Rect roi;
        Mat face, face_rs;
        Point p1, p2, pText;
        string predicted_name = "unknown";

        // Create roi to crop the face image
        roi.x = faces[i].x;
        roi.y = faces[i].y;
        roi.width = (faces[i].width);
        roi.height = (faces[i].height);

        // Create Point to draw rectangle
        p1.x = faces[i].x;
        p1.y = faces[i].y;
        p2.x = faces[i].x + faces[i].width;
        p2.y = faces[i].y + faces[i].height;

        // Create Point to PutText
        pText.x = faces[i].x;
        pText.y = faces[i].y - 10;

        // Draw rectangle for detected face
        rectangle(frame, p1, p2, Scalar(0, 0, 255), 1);

        // Crop face and resize to (200, 200) of size
        face = frame_gray(roi);
        resize(face, face_rs, Size(200, 200), 0, 0, INTER_LINEAR);

        // Predict the face using LBPH Face Recognizer
        int predicted_label = model->predict(face_rs);

        // If the predict return a value (non -1) then set the text as the predicted class
        if (predicted_label != -1)
            predicted_name = names[predicted_label];

        // PutText with the person name
        putText(frame, predicted_name, pText, 3, 0.5, Scalar(0, 0, 255), 2);
        if (strcmp(name,"Total")==0 && predicted_name.compare("unknow") != 0)
        {
           flag = 1;
        }
        else if(strcmp(name,predicted_name.c_str())==0)
        {
            flag = 1;
            break;
        }

    }
    if(flag ==1)
    	return 1;

    return 0;
}

// Read csv file to get all face datasets and its labels
int read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator)
{
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file)
    {
        cout << "[ERROR] No valid input file was given, please check the given filename." << endl;
        return -1;
    }
    else
    {
        cout << "[INFO] Successfully loaded face csv\n";
    }
    string line, path, classlabel;
    while (getline(file, line))
    {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty())
        {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
    return 0;
}

// Create LBPH Face Recognizer model
void create_model(Ptr<FaceRecognizer>& model, double threshold, vector<Mat> images, vector<int> labels)
{
    model =  LBPHFaceRecognizer::create(1, 8, 8, 8, threshold);
    model->train(images, labels);
    cout << "[INFO] LBPH-Face Recognition Model created" << endl;
}

void get_name(vector<string>& names)
{
	ifstream myfile("/home/hoang/Desktop/KLTN/name.txt");
	string line;
	while (getline(myfile, line))
    	{
        	names.push_back(line);
    	}
    	myfile.close();
}

const std::string get_time(){
	    auto start = std::chrono::system_clock::now();
	        auto end = std::chrono::system_clock::now();
		 
		    std::chrono::duration<double> elapsed_seconds = end-start;
		        std::time_t end_time = std::chrono::system_clock::to_time_t(end);  
			    return ctime(&end_time);
}
