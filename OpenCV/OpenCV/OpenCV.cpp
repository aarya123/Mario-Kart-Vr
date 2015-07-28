#include "stdafx.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace cv;

Scalar MIN_BLUE = Scalar(70, 0, 0);
Scalar MAX_BLUE = Scalar(180, 80, 80);

Scalar MIN_RED = Scalar(0, 0, 70);
Scalar MAX_RED = Scalar(80, 80, 180);

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "80"


Rect getLargestRect(Mat& src, Scalar min, Scalar max)
{
	Mat color;
	inRange(src, min, max, color);
	vector<vector<Point>> contours;
	findContours(color.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	vector<Point> largestContour;
	int maxSize = -1;
	for (int j = 0; j < contours.size(); j++)
	{
		int area = contourArea(contours[j]);
		if (area > maxSize)
		{
			maxSize = area;
			largestContour = contours[j];
		}
	}
	return boundingRect(largestContour);
}

SOCKET setupServer()
{
	SOCKET ListenSocket = INVALID_SOCKET;
	WSADATA wsaData;
	int iResult;
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	return ListenSocket;
}

int getCollision(Mat& src)
{
	Rect red, blue;
	red = getLargestRect(src, MIN_RED, MAX_RED);
	blue = getLargestRect(src, MIN_BLUE, MAX_BLUE);
	Rect intersect = red & blue;
	return intersect.area();
}

int _tmain()
{
	namedWindow("Camera_Output", 1);    //Create window
	VideoCapture capture(-1);  //Capture using any camera connected to your system
	while (1){ //Create infinte loop for live streaming
		Mat img;
		capture >> img; //Create image frames from capture
		imshow("Camera_Output", img);   //Show image frames on created window
		char key = cvWaitKey(10);     //Capture Keyboard stroke
		if (char(key) == 27){
			break;      //If you hit ESC key loop will break.
		}
	}
	capture.release();
	/*
	SOCKET ListenSocket = setupServer();
	while (true){
		// Accept a client socket
		SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET)
		{
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		int iSendResult = send(ClientSocket, "hi", 2, 0);
		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		printf("Bytes sent: %d\n", iSendResult);
		// shutdown the connection since we're done
		int iResult = shutdown(ClientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	}
	*/
	/*
	for (int i = 1; i < 5; i++)
	{
	string srcString = "C:\\Users\\t-anarya.REDMOND\\Desktop\\";
	srcString += to_string(i);
	srcString += ".jpg";
	Mat src = imread(srcString, CV_LOAD_IMAGE_COLOR);
	Rect red, blue;
	red = getLargestRect(src, MIN_RED, MAX_RED);
	blue = getLargestRect(src, MIN_BLUE, MAX_BLUE);
	Rect intersect = red & blue;
	cout << intersect.area() << endl;
	imshow("src", src);
	waitKey(0);
	}
	*/
	return 0;
}
