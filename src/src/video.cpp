/*
 * video.cpp
 *
 *  Created on: Dec 22, 2014
 *      Author: matthieu
 */

#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

/**
 * Continue playing the video and extract next image in a certain number of frames
 */
Mat nextImage(VideoCapture& capture, int frames, int fps, double mspf,
		string videoWindow, string imageWindow, int width, int height) {
	Mat emptyImage;
	Mat imageFull;
	Mat image;
	for (int f = 0; f < frames; f++) {
		capture >> imageFull;
		// check if there are still frames
		if (imageFull.empty())
			return emptyImage;
		// resize the image
		resize(imageFull, image, Size(width, height), 0, 0, INTER_LINEAR);
		// Show our image inside the video window.
		imshow(videoWindow, image);
		if (f == frames-1)
			imshow(imageWindow, image);
		if (waitKey(mspf) == 'q')
			return emptyImage;
	}
	return image;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		cout << " Usage: Singapore VideoToLoadAndDisplay" << endl;
		return -1;
	}
	Mat image;
	// Read the file
	VideoCapture capture;
	capture.open(argv[1]);
	// Check if the video is loaded
	if (!capture.isOpened()) {
		cout << "Could not open or find the video" << std::endl;
		return EXIT_FAILURE;
	}

	// Create a window for video display and image display.
	int width = 800;
	int height = 450;
	string videoWindow = "Video window";
	namedWindow(videoWindow, WINDOW_NORMAL);
	resizeWindow(videoWindow, width, height);
	moveWindow(videoWindow, 50, 100);
	string imageWindow = "Image window";
	namedWindow(imageWindow, WINDOW_NORMAL);
	resizeWindow(imageWindow, width, height);
	moveWindow(imageWindow, 2*50 + width, 100);

	// get the framerate of the video
	int fps = (int) capture.get(CAP_PROP_FPS);
	// deduce the time between 2 frames (in ms)
	double mspf = 1000 / (double) fps;

	// the vector containing 10 images
	vector<Mat> images;
	int currentImage = 0;
	// infinite loop
	while (1) {
		image = nextImage(capture, fps, fps, mspf, videoWindow, imageWindow, width,
				height);
		if (image.empty())
			break;
		else if (currentImage < 10)
			images.push_back(image.clone());
		currentImage++;
	}
	// cout << "nombre d'images récupérées : " << to_string(images.size()) << endl;

	return 0;
}
