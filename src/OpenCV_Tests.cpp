//OpenCV Testing
//Phil R Posillico
//5/11/2019

#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include <opencv2/core.hpp>			//Basic OpenCV
#include <opencv2/imgproc.hpp>		//Operations and filters on images
#include <opencv2/imgcodecs.hpp>	//
#include <opencv2/videoio.hpp>		//OpenCV Video
#include <opencv2/highgui.hpp>		//Render graphics on open frame

using namespace std;
using namespace cv;
//For cleaner OpenCV calls

//Vars
int gaussian_kernel = 11;			//Order of gaussian blur
char window_name[] = "Image and Video Gaussian Blur Test"; // make the other windows this way//////////////////
string img_filename = "test_img.jpg";
string vid_filename = "baseball_test_vid.mp4";			//Use something short
string vid_output_filename = "output_test.avi";				//Altered Vid file
Mat imgSource;
Mat imgNew;
Mat vidSourceFrame;
Mat vidNewFrame;
//Prototypes
int display_caption(const char* caption);
int display_imgNew();
int image_test();
int vid_test();

int main(void) {
	printf(
			"This program will test opening, closing, and modifying a sample image and"
					"\nsample video. Gaussian Blur of 11th Kernel will be the example modification.\n");
	printf("Press return throughout the program to step forward\n");

	image_test();
	vid_test();

	return 0;
}

int display_caption(const char* caption) {//creates caption screens using 'highgui.hpp'
	imgNew = Mat::zeros(imgSource.size(), imgSource.type());
	putText(imgNew, caption, Point(imgSource.cols / 10, imgSource.rows / 3),
			FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 255));
	return display_imgNew();
}

int display_imgNew() {									//displays image
	imshow(window_name, imgNew);
	return 0;
}

int image_test() {
	imgSource = imread(img_filename, IMREAD_COLOR);
	namedWindow(window_name, WINDOW_AUTOSIZE);
	if (imgSource.empty()) {
		printf("\nError Opening Image File\n");
		return -1;
	} else {
		printf("\nValid File Found");
	}
	display_caption("Original Picture");
	waitKey(0);
	imgNew = imgSource.clone();
	display_imgNew();
	waitKey(0);
	display_caption("Gaussian Blur");
	waitKey(0);

	try {		//Gaussian blur on test image
		GaussianBlur(imgSource, imgNew, Size(gaussian_kernel, gaussian_kernel),
				0, 0);
		display_imgNew();
	} catch (...) {
		printf("\n\nIteration Failure!");
		return -1;
	};
	printf("\n\nIteration Success!");
	waitKey(0);
	destroyWindow(window_name);

	return 0;
}

int vid_test() {
	namedWindow(window_name, WINDOW_AUTOSIZE);
	display_caption("Original Video");
	waitKey(0);
	destroyWindow(window_name);
	VideoCapture capVid;
	VideoWriter writeVid;
	capVid.open(vid_filename);
	int codec = writeVid.fourcc('M', 'J', 'P', 'G');//avi codec, will look into others
	int fps = capVid.get(CAP_PROP_FPS);
	int frame_delay = 1000 / fps;
	char chCheckForEscKey = 0;

	capVid.read(vidSourceFrame);
	while (capVid.isOpened() && chCheckForEscKey != 27) {//Display original, look for keypresses

		imshow("vidSourceFrame", vidSourceFrame);

		if ((capVid.get(CAP_PROP_POS_FRAMES) + 1)
				< capVid.get(CAP_PROP_FRAME_COUNT) / 3) {
			clock_t startTime = clock();//loop manages a timer to render at source fps
			capVid.read(vidSourceFrame);
			while (clock() - startTime < frame_delay) {
				waitKey(1);
			}
		} else {
			printf("\nEnd of Video");
			waitKey(0);
			destroyWindow("vidSourceFrame");
			break;
		}

		chCheckForEscKey = waitKey(1);
	}

	if (chCheckForEscKey != 27) {
		waitKey(0);
	}
	printf("\n.\n.\n.\n.");
	printf("\nWe will begin iterating the new video. Be patient.\n\n");
	system("pause");
	capVid.release();
	capVid.open(vid_filename);
	capVid.read(vidSourceFrame);
	bool isColor = (vidSourceFrame.type() == CV_8UC3);
	Size S = Size((int) capVid.get(CAP_PROP_FRAME_WIDTH),  // Acquire input size
	(int) capVid.get(CAP_PROP_FRAME_HEIGHT));
	writeVid.open(vid_output_filename, codec, fps, S, isColor);
	if (!writeVid.isOpened()) {
		printf("Could Not Open Output File");
		waitKey(0);
		return -1;
	}

	for (;;)			//Loop through all frames and performs Gaussian blur
			{
		if (!capVid.read(vidSourceFrame)) {
			printf("no more frames!\n");
			break;
		}
		try {
			GaussianBlur(vidSourceFrame, vidNewFrame,
					Size(gaussian_kernel, gaussian_kernel), 0, 0);
			float current_frame = capVid.get(CAP_PROP_POS_FRAMES);
			float total_frame = capVid.get(CAP_PROP_FRAME_COUNT);
			float progress_frame = (current_frame / total_frame) * 100;
			printf("\r          %.0f %%", progress_frame);
		} catch (...) {
			printf("\n\nIteration Failure!");
			return 0;
		};
		writeVid.write(vidNewFrame);
		if (waitKey(5) >= 0) {
			break;
		}
	}
	writeVid.release();
	printf("\n\nIteration Success!\n");
	system("pause");

	capVid.release();
	namedWindow(window_name, WINDOW_AUTOSIZE);
	display_caption("Gaussian Blur Video");
	waitKey(0);
	destroyWindow(window_name);
	capVid.open(vid_output_filename);
	capVid.read(vidSourceFrame);

	while (capVid.isOpened() && chCheckForEscKey != 27) {//Display blurred, look for keypresses

		imshow("vidSourceFrame", vidSourceFrame);

		if ((capVid.get(CAP_PROP_POS_FRAMES) + 1)
				< capVid.get(CAP_PROP_FRAME_COUNT) / 4) {
			clock_t startTime = clock();
			capVid.read(vidSourceFrame);
			while (clock() - startTime < frame_delay) {
				waitKey(1);
			}
		} else {
			printf("\nEnd of Video");
			waitKey(0);
			destroyWindow("vidSourceFrame");
			break;
		}

		chCheckForEscKey = waitKey(1);
	}

	if (chCheckForEscKey != 27) {
		waitKey(0);
	}

	return 0;
}
