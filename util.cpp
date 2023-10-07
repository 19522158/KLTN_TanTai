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

// Detect and recognize all detected faces
string detect_and_predict(CascadeClassifier face_cascade, Ptr<FaceRecognizer> model, Mat& frame, vector<string> names)
{
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    //-- Detect faces
    vector<Rect> faces;
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 5);
  
   if (faces.size() == 0)
                return "Khong co ai";
   else{ 
    for (size_t i = 0; i < faces.size(); i++)
    {
        Rect roi;
        Mat face, face_rs;
        Point p1, p2, pText;

//	string predicted_name = "unknown";
	 string predicted_name = "Unknow";
        
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
    	cout << " Ten:" << predicted_name << endl;
        return predicted_name;	
              

    }
           // if (faces.size() == 0)
         //       return "Không có ai";
       // else
  }   
	   //	return predicted_name;
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
