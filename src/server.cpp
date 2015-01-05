#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "stddef.h"
#include "opencv2\core\core.hpp"
#using <System.dll>
#include <windows.h> <WINDOWS.H>
#include <strsafe.h> <STRSAFE.H>
#include <stdio.h><STDIO.H>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
using namespace cv;
using namespace std;
using namespace System;
using namespace System::IO::Ports;
using namespace System::Threading;
using System::Runtime::InteropServices::Marshal;
//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 179;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

int xfront=0, yfront=0;
	int xrear=0,yrear=0;
	int xcenter=0,ycenter=0;

int cur_node_count=0;
int cur_angle=0;
int desired_angle=0;
int prev_angle=0;
int rotation=0;
int node_count=0;
int orientation=0;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";

int path[40][3];

void calibrate()
{
	printf("Started calibration...\n");
	WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
    }
	//printf("here2\n");
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
    }
	printf("here3\n");
    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
    }

	printf("here4\n");
    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);

    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
    }

	printf("here5\n");
	printf("Waiting for a client to connect\n");
    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
    }
	printf("here7\n");
    // No longer need server socket
    closesocket(ListenSocket);
	printf("here8\n");
	int fwdcount=0;
	double fwdavg;
	int fwdsum=0;
	double rightsum=0;
	double leftsum=0;
	double leftavg=0;
	double rightavg=0;
	int rightcount=0;
	int leftcount=0;
	int turn=1;
    // Receive until the peer shuts down the connection
    do 
	{
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) 
		{
			string received(recvbuf);
			int pos=received.find("\n");
			string final=received.substr(0,pos);
			cout<<final<<endl;
			if(final.compare("rpi")==0)
			{
				printf("Raspberry pi connected..\n");
				send( ClientSocket,"Connected to server.",20, 0 );
			}
			else if(final.compare("waiting")==0)
			{
				if(turn!=3)
				{
				if(turn%3==1)
				{
					if(fwdcount!=1)
					{
					fwdcount++;
					double oldxcenter=xcenter;
					double oldycenter=ycenter;
					send( ClientSocket,"fw",2, 0 );
					iResult=recv(ClientSocket, recvbuf, recvbuflen, 0);

					double newxcenter=xcenter;
					double newycenter=ycenter;
					fwdsum=fwdsum+sqrt(pow((newxcenter-oldxcenter),2)+pow((newycenter-oldycenter),2));
					fwdavg=(double)fwdsum/(double)fwdcount;
					printf("fwdavg:%f\n"
					}
				}
				if(turn%3==2)
				{
					if(rightcount!=1)
					{
					rightcount++;
					double oldxcenter=orientation;
					send( ClientSocket,"rt",2, 0 );
					double newxcenter=orientation;
					rightsum=rightsum+abs(newxcenter-oldxcenter);
					rightavg=(double)rightsum/(double)rightcount;
					}
				}
				if(turn%3==0)
				{
					if(leftcount!=1)
					{
					leftcount++;
					double oldxcenter=orientation;
					send( ClientSocket,"rt",2, 0 );
					double newxcenter=orientation;
					leftsum=leftsum+abs(newxcenter-oldxcenter);
					leftavg=(double)leftsum/(double)leftcount;
					}
				}
				turn++;
				}
				else
				{
					printf("Completed calibration\n");
					printf("Fwd speed :%f pixles/s\n ",fwdavg);
					printf("Right speed :%f pixles/s\n ",rightavg);
					printf("Left speed :%f pixles/s\n ",leftavg);
					scanf("%d");
				}
			}
		}
	else if (iResult == 0)
            printf("Connection closing...\n");
        else  
		{
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
			}

    } while (1);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) 
	{
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
	scanf("%d");
}

public ref class ArduinoCom
{
private:
    static bool _continue;
    static SerialPort^ _serialPort;

public:
static DWORD WINAPI get_orientation(LPVOID lpParameter)
    {
        
        System::String^ message;
        //StringComparer^ stringComparer = StringComparer::OrdinalIgnoreCase;
        
		//Thread^ readThread = gcnew Thread(gcnew ThreadStart(ArduinoCom::Read));

        // Create a new SerialPort object with default settings.
        _serialPort = gcnew SerialPort();

       // These are the Arduino com defaults.
        _serialPort->PortName = "COM24";
        _serialPort->BaudRate = 9600;
        _serialPort->Parity = Parity::None;
        _serialPort->DataBits = 8;
        _serialPort->StopBits = StopBits::One;
        _serialPort->Handshake = Handshake::None;

        // Set the read/write timeouts
        _serialPort->ReadTimeout = 500;
        _serialPort->WriteTimeout = 500;

        _serialPort->Open();
        _continue = true;
        //readThread->Start();

        //Console::WriteLine("Type BYE to exit");

        /*while (_continue)
        {
            message = Console::ReadLine();

            if (stringComparer->Equals("bye", message))
            {
                _continue = false;
            }
            else
            {
                _serialPort->WriteLine(
                    String::Format("{0}",message) );
            }
        }

        //readThread->Join();
        //_serialPort->Close();*/
		while (_continue)
        {
            try
            {
                message = _serialPort->ReadLine();
                //Console::WriteLine(message);
				std::string mesg;
		const char * stringPointer = (const char *) Marshal::StringToHGlobalAnsi(message).ToPointer(); 
		mesg = stringPointer; 
		Marshal::FreeHGlobal(IntPtr((void *)stringPointer));
		
		cout << mesg << endl;
		int front;
		int rear;
		int count=0;
		for(int i=0;i<mesg.length();i++)
		{
			if(mesg.at(i)==',')
			{
				count++;
			}
			if(mesg.at(i)==',' && count==2)
			{
				front=i;
			}
			if(mesg.at(i)==',' && count==3)
			{
				rear=i;
				break;
			}
		}
		//cout<<front<<","<<rear<<endl;
		string a=mesg.substr(0,rear);
		string res=a.substr(front+1);
		//cout<<res<<endl;
		orientation=stof(res);
            }
            catch (TimeoutException ^) { }
        }
		
		
		_serialPort->Close();
		return 0;
		//Console::WriteLine(String::Format("{0}",message));
    }
};
DWORD WINAPI angle_func(LPVOID lpParameter) {
    ArduinoCom::get_orientation(lpParameter);
	return 0;
}
void on_trackbar( int, void* )
{//This function gets called whenever a
	// trackbar position is changed





}
string intToString(int number){


	std::stringstream ss;
	ss << number;
	return ss.str();
}
void createTrackbars(){
	//create window for trackbars


    namedWindow(trackbarWindowName,0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
    createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
    createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
    createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
    createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
    createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
    createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );


}
void drawObject(int x, int y,Mat &frame,int front){

	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

    //UPDATE:JUNE 18TH, 2013
    //added 'if' and 'else' statements to prevent
    //memory errors from writing off the screen (ie. (-25,-25) is not within the window!)
	if(front==1)
	{
		//drawing front
	circle(frame,Point(x,y),5,Scalar(255,0,0),2);
    if(y-25>0)
    line(frame,Point(x,y),Point(x,y-25),Scalar(255,0,0),2);
    else line(frame,Point(x,y),Point(x,0),Scalar(255,0,0),2);
    if(y+25<FRAME_HEIGHT)
    line(frame,Point(x,y),Point(x,y+25),Scalar(255,0,0),2);
    else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(255,0,0),2);
    if(x-25>0)
    line(frame,Point(x,y),Point(x-25,y),Scalar(255,0,0),2);
    else line(frame,Point(x,y),Point(0,y),Scalar(255,0,0),2);
    if(x+25<FRAME_WIDTH)
    line(frame,Point(x,y),Point(x+25,y),Scalar(255,0,0),2);
    else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(255,0,0),2);

	putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(255,0,0),2);
	}
	else
	{
	//drawing rear
	circle(frame,Point(x,y),5,Scalar(0,0,255),2);
    if(y-25>0)
    line(frame,Point(x,y),Point(x,y-25),Scalar(0,0,255),2);
    else line(frame,Point(x,y),Point(x,0),Scalar(0,0,255),2);
    if(y+25<FRAME_HEIGHT)
    line(frame,Point(x,y),Point(x,y+25),Scalar(0,0,255),2);
    else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,0,255),2);
    if(x-25>0)
    line(frame,Point(x,y),Point(x-25,y),Scalar(0,0,255),2);
    else line(frame,Point(x,y),Point(0,y),Scalar(0,0,255),2);
    if(x+25<FRAME_WIDTH)
    line(frame,Point(x,y),Point(x+25,y),Scalar(0,0,255),2);
    else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,0,255),2);

	putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,0,255),2);
	}
}
void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
    //dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);


	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
	


}
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed,int front){

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
					refArea = area;
				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				if(front==1)
				{
				putText(cameraFeed,"Front",Point(0,50),2,1,Scalar(255,0,0),2);
				drawObject(x,y,cameraFeed,1);
				}
				else
				{
				putText(cameraFeed,"Rear",Point(0,100),2,1,Scalar(0,0,255),2);	
				drawObject(x,y,cameraFeed,0);
				}
				//draw object location on screen
				}

		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}
DWORD WINAPI  video_feed(LPVOID lpParam)
{
	//some boolean variables for different functionality within this
	//program
    bool trackObjects = true;
    bool useMorphOps = false;
	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	//matrix storage for HSV image
	Mat HSV;
	//matrix storage for binary threshold image
	Mat threshold_front;
	//x and y values for the location of the front

	Mat threshold_rear;
	//x and y values for the location of the end

	

	//create slider bars for HSV filtering
	//createTrackbars();
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open(3);
	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	printf("%dx%d",FRAME_WIDTH,FRAME_HEIGHT);

	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while(1){
		//store image to matrix
		capture.read(cameraFeed);
		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
		//filter HSV image between values and store filtered image to
		//threshold matrix
		inRange(HSV,Scalar(150,58,179),Scalar(170,184,256),threshold_front);
		inRange(HSV,Scalar(0,81,216),Scalar(23,256,256),threshold_rear);
		//inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold_front);
		//inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold_rear);
		//perform morphological operations on thresholded image to eliminate noise
		//and emphasize the filtered object(s)
		if(useMorphOps)
		morphOps(threshold_front);
		morphOps(threshold_rear);
		//pass in thresholded frame to our object tracking function
		//this function will return the x and y coordinates of the
		//filtered object
		if(trackObjects)
		trackFilteredObject(xfront,yfront,threshold_front,cameraFeed,1);
		trackFilteredObject(xrear,yrear,threshold_rear,cameraFeed,0);
		//printf("%d,%d\n",x,y);
		//show frames 
		//imshow(windowName2,threshold_front);
		//imshow(windowName2,threshold_rear);
		xcenter=(xfront+xrear)/2;
		ycenter=(yfront+yrear)/2;
		circle(cameraFeed,Point(xcenter,ycenter),8,Scalar(0,255,0),2,8,0);
		circle(cameraFeed,Point(xcenter,ycenter),4,Scalar(0,255,0),-1,8,0);
		imshow(windowName,cameraFeed);
		//imshow(windowName1,HSV);
		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}
	return 0;
}
DWORD WINAPI coords_func(LPVOID lpParameter) {
    video_feed(lpParameter);
	return 0;
}

void start_navigation()
{
	printf("Started navigation server...\n");
	WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
    }
	//printf("here2\n");
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
    }
	printf("here3\n");
    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
    }

	printf("here4\n");
    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);

    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
    }

	printf("here5\n");
	printf("Waiting for a client to connect\n");
    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
    }
	printf("here7\n");
    // No longer need server socket
    closesocket(ListenSocket);
	printf("here8\n");
    // Receive until the peer shuts down the connection
    do 
	{
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            //printf("Bytes received: %d\n", iResult);
			//printf("%s\n",recvbuf);
			//int ends=recvbuf.find("\n");
			string received(recvbuf);
			int pos=received.find("\n");
			string final=received.substr(0,pos);
			cout<<final<<endl;
			if(final.compare("rpi")==0)
			{
				printf("Raspberry pi connected..\n");
				send( ClientSocket,"Connected to server.",20, 0 );
			}
			else if(final.compare("waiting")==0)
			{
				int led_flag=0;
				int stop_flag=0;
				if(path[cur_node_count+1][2]==8)
				{
					//next node is blue
					led_flag=1;
				}
				if(path[cur_node_count+1][2]==7)
				{
					//next node is white
					stop_flag=1;
				}
				if(cur_node_count!=node_count-1)
				{
				if(path[cur_node_count][0]-path[cur_node_count+1][0]>10)
				{
					//next node is to the left
					//go left
					//printf("Sending left\n");
					printf("Current Angle :%d\n",cur_angle);
					printf("Prev Angle :%d\n",prev_angle);
					desired_angle=-90;
					cur_angle=desired_angle-cur_angle+prev_angle;
					rotation=cur_angle-prev_angle;
					printf("Rotation Angle :%d\n",rotation);
						if(rotation==-90 || rotation==270)
						{
							//left+forward
							send( ClientSocket,"lf",2, 0 );
							printf("sent lf");
						}
						else if(rotation==0 || rotation==360)
						{
							//forward
							send( ClientSocket,"fw",2, 0 );
							printf("sent fw");
						}
						else if(rotation==90 || rotation==-270)
						{
							//right+forward
							send( ClientSocket,"rf",2, 0 );
							printf("sent rf");
						}
						else if((rotation==0 || rotation==360) && led_flag==1)
						{
							//forward
							send( ClientSocket,"gf",2, 0 );
							printf("sent gf");
						}
						else if((rotation==-90 || rotation==270) && led_flag==1)
						{
							send( ClientSocket,"lg",2, 0 );
							printf("sent lg");
						}
						else if((rotation==90 || rotation==-270) && led_flag==1)
						{
							send( ClientSocket,"rg",2, 0 );
							printf("sent rg");
						}
						else if((rotation==0 || rotation==360) && stop_flag==1)
						{
							//forward
							send( ClientSocket,"sf",2, 0 );
							printf("sent sf");
						}
						else if((rotation==-90 || rotation==270) && stop_flag==1)
						{
							send( ClientSocket,"sl",2, 0 );
							printf("sent sl");
						}
						else if((rotation==90 || rotation==-270) && stop_flag==1)
						{
							send( ClientSocket,"sr",2, 0 );
							printf("sent sr");
						}
				}
				if(path[cur_node_count+1][0]-path[cur_node_count][0]>10)
				{
					//printf("Sending right\n");
					//next node is to the right
					//go right
					printf("Current Angle :%d\n",cur_angle);
					printf("Prev Angle :%d\n",prev_angle);
					desired_angle=90;
					cur_angle=desired_angle-cur_angle+prev_angle;
					rotation=cur_angle-prev_angle;
					printf("Rotation Angle :%d\n",rotation);
						if(rotation==-90 || rotation==270)
						{
							//left+forward
							send( ClientSocket,"lf",2, 0 );
							printf("sent lf");
						}
						else if(rotation==0 || rotation==360)
						{
							//forward
							send( ClientSocket,"fw",2, 0 );
							printf("sent fw");
						}
						else if(rotation==90 || rotation==-270)
						{
							//right+forward
							send( ClientSocket,"rf",2, 0 );
							printf("sent rf");
						}
						else if((rotation==0 || rotation==360) && led_flag==1)
						{
							//forward
							send( ClientSocket,"gf",2, 0 );
							printf("sent gf");
						}
						else if((rotation==-90 || rotation==270) && led_flag==1)
						{
							send( ClientSocket,"lg",2, 0 );
							printf("sent lg");
						}
						else if((rotation==90 || rotation==-270) && led_flag==1)
						{
							send( ClientSocket,"rg",2, 0 );
							printf("sent rg");
						}
						else if((rotation==0 || rotation==360) && stop_flag==1)
						{
							//forward
							send( ClientSocket,"sf",2, 0 );
							printf("sent sf");
						}
						else if((rotation==-90 || rotation==270) && stop_flag==1)
						{
							send( ClientSocket,"sl",2, 0 );
							printf("sent sl");
						}
						else if((rotation==90 || rotation==-270) && stop_flag==1)
						{
							send( ClientSocket,"sr",2, 0 );
							printf("sent sr");
						}
				}
				if(path[cur_node_count+1][1]-path[cur_node_count][1]>10)
				{
					//next node is below
					//go below
					printf("Current Angle :%d\n",cur_angle);
					printf("Prev Angle :%d\n",prev_angle);
					//printf("Sending below\n");
					desired_angle=180;
					cur_angle=desired_angle-cur_angle+prev_angle;
					rotation=cur_angle-prev_angle;
					printf("Rotation Angle :%d\n",rotation);
						if(rotation==-90 || rotation==270)
						{
							//left+forward
							send( ClientSocket,"lf",2, 0 );
							printf("sent lf");
						}
						else if(rotation==0 || rotation==360)
						{
							//forward
							send( ClientSocket,"fw",2, 0 );
							printf("sent fw");
						}
						else if(rotation==90 || rotation==-270)
						{
							//right+forward
							send( ClientSocket,"rf",2, 0 );
							printf("sent rf");
						}
						else if((rotation==0 || rotation==360) && led_flag==1)
						{
							//forward
							send( ClientSocket,"gf",2, 0 );
							printf("sent gf");
						}
						else if((rotation==-90 || rotation==270) && led_flag==1)
						{
							send( ClientSocket,"lg",2, 0 );
							printf("sent lg");
						}
						else if((rotation==90 || rotation==-270) && led_flag==1)
						{
							send( ClientSocket,"rg",2, 0 );
							printf("sent rg");
						}
						else if((rotation==0 || rotation==360) && stop_flag==1)
						{
							//forward
							send( ClientSocket,"sf",2, 0 );
							printf("sent sf");
						}
						else if((rotation==-90 || rotation==270) && stop_flag==1)
						{
							send( ClientSocket,"sl",2, 0 );
							printf("sent sl");
						}
						else if((rotation==90 || rotation==-270) && stop_flag==1)
						{
							send( ClientSocket,"sr",2, 0 );
							printf("sent sr");
						}
				}
				if(path[cur_node_count][1]-path[cur_node_count+1][1]>10)
				{
					//next node is above
					//go up
					printf("Current Angle :%d\n",cur_angle);
					printf("Prev Angle :%d\n",prev_angle);
					//printf("Sending up\n");
					desired_angle=0;
					cur_angle=desired_angle-cur_angle+prev_angle;
					rotation=cur_angle-prev_angle;
					printf("Rotation Angle :%d\n",rotation);
						if(rotation==-90 || rotation==270)
						{
							//left+forward
							send( ClientSocket,"lf",2, 0 );
							printf("sent lf");
						}
						else if(rotation==0 || rotation==360)
						{
							//forward
							send( ClientSocket,"fw",2, 0 );
							printf("sent fw");
						}
						else if(rotation==90 || rotation==-270)
						{
							//right+forward
							send( ClientSocket,"rf",2, 0 );
							printf("sent rf");
						}
						else if((rotation==0 || rotation==360) && led_flag==1)
						{
							//forward
							send( ClientSocket,"gf",2, 0 );
							printf("sent gf");
						}
						else if((rotation==-90 || rotation==270) && led_flag==1)
						{
							send( ClientSocket,"lg",2, 0 );
							printf("sent lg");
						}
						else if((rotation==90 || rotation==-270) && led_flag==1)
						{
							send( ClientSocket,"rg",2, 0 );
							printf("sent rg");
						}
						else if((rotation==0 || rotation==360) && stop_flag==1)
						{
							//forward
							send( ClientSocket,"sf",2, 0 );
							printf("sent sf");
						}
						else if((rotation==-90 || rotation==270) && stop_flag==1)
						{
							send( ClientSocket,"sl",2, 0 );
							printf("sent sl");
						}
						else if((rotation==90 || rotation==-270) && stop_flag==1)
						{
							send( ClientSocket,"sr",2, 0 );
							printf("sent sr");
						}
				}
				//char cmd[1];
				//printf("Enter a command\n");
				//gets(cmd);
				//send( ClientSocket,cmd,1, 0 );
				prev_angle=cur_angle;
				cur_node_count++;
				}
				else
				{
					printf("Robot reached end\n");
				}
			}
        // Echo the buffer back to the sender
            //iS endResult = send( ClientSocket, recvbuf, iResult, 0 );
            /*if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
            }*/
            //printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else  {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
			}

    } while (1);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) 
	{
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
	scanf("%d");
}

int __cdecl main(void) 
{
	
	//video_feed();
	//ArduinoCom::get_orientation();
	/*int Data_of_coords =1;
	int Data_of_angle =2;
	HANDLE Handle_Of_coords = 0;
	 Handle_Of_coords= CreateThread( NULL, 0, 
           coords_func, &Data_of_coords, 0, NULL);  
    if ( Handle_Of_coords == NULL)
        ExitProcess(Data_of_coords);
    
    // variable to hold handle of Thread 1 
    HANDLE Handle_Of_angle = 0;
	 Handle_Of_angle = CreateThread( NULL, 0, 
           angle_func, &Data_of_angle, 0, NULL);  
    if (Handle_Of_angle == NULL)
        ExitProcess(Data_of_angle);
	*/
    //calibrate();
	FILE * rFile;
	rFile=fopen("C:/Users/Jay/Documents/ready.txt","r");
	char buf[2];
	bool ready_flag=false;
	printf("Waiting for path detection...\n");
	while(!ready_flag)
	{
	while (fgets(buf, sizeof buf, rFile) != NULL) 
	{
    // process line here
		string line(buf);
			int pos=line.find("\n");
			string final=line.substr(0,pos);
		if(final.compare("1")==0)
		{
			ready_flag=true;
		}
	}
	}
	fclose(rFile);
	printf("Ready flag %d\n",ready_flag);

	
	FILE * xFile;
	xFile=fopen("C:/Users/Jay/Documents/x.txt","r");
	char buf2[5];
	int x_count=0;
	while (fgets(buf2, sizeof buf2, xFile) != NULL) 
	{
    // process line here
		string line(buf2);
			int pos=line.find("\n");
			string final=line.substr(0,pos);
			//cout<<final<<endl;
			path[x_count][0]=stoi(final,nullptr,10);
			printf("%d\n",path[x_count][0]);
			x_count++;
	}

	FILE * yFile;
	yFile=fopen("C:/Users/Jay/Documents/y.txt","r");
	char buf3[5];
	int y_count=0;

	while (fgets(buf3, sizeof buf3, yFile) != NULL) 
	{
    // process line here
		string line(buf3);
			int pos=line.find("\n");
			string final=line.substr(0,pos);
			//cout<<final<<endl;
			path[y_count][1]=stoi(final,nullptr,10);
			printf("%d\n",path[y_count][1]);
			y_count++;
	}

	FILE * tFile;
	tFile=fopen("C:/Users/Jay/Documents/t.txt","r");
	char buf4[5];
	int t_count=0;
	while (fgets(buf4, sizeof buf4, tFile) != NULL) 
	{
    // process line here
		string line(buf4);
			int pos=line.find("\n");
			string final=line.substr(0,pos);
			//cout<<final<<endl;
			path[t_count][2]=stoi(final,nullptr,10);
			printf("%d\n",path[t_count][2]);
			t_count++;
	}

	node_count=t_count;
	start_navigation();
	scanf("%d");
	return 0;
}
