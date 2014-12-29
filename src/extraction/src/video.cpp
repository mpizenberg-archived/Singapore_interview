/*
 * video.cpp
 *
 *  Created on: Dec 22, 2014
 *      Author: matthieu
 */

#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> // to check folder existence in UNIX systems

using namespace cv;
using namespace ximgproc;
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
		// Show last image in image window
		if (f == frames - 1)
			imshow(imageWindow, image);
		if (waitKey(mspf) == 'q')
			return emptyImage;
	}
	return image;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		cout << " Usage: Singapore_extraction VideoToLoadAndDisplay" << endl;
		return -1;
	}
	Mat image;
	// Read the file
	VideoCapture capture;
	capture.open(argv[1]);
	// Check if the video is loaded
	if (!capture.isOpened()) {
		cout << "Could not open or find the video" << endl;
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
	moveWindow(imageWindow, 2 * 50 + width, 100);

	// get the framerate of the video
	int fps = (int) capture.get(CAP_PROP_FPS);
	// deduce the time between 2 frames (in ms)
	double mspf = 1000 / (double) fps;

	// the vector containing 10 images
	vector<Mat> images;
	unsigned int nb_images = 10;
	unsigned int currentImage = 0;

	// Segmented image of the cat
	Mat segmentedImage;
	Ptr<SuperpixelSEEDS> seeds;
	int init = false;
	int num_superpixels = 400;
	int num_levels = 4;

	// infinite loop
	while (currentImage<nb_images) {
		image = nextImage(capture, fps, fps, mspf, videoWindow, imageWindow,
				width, height);
		if (image.empty())
			break;
		if (waitKey(10) == 'q')
			break;
		else {
			images.push_back(image.clone());
		}
		currentImage++;
	}

	// verify the number of images kept
	cout << "Number of images kept : " << to_string(images.size()) << endl;

	// Folder where to save those images
	cout << "Folder to save the images (with the / at the end) :" << endl;
	string folder;
	cin >> folder;

	// check if the folder exists
	struct stat sb;
	if (!( stat(folder.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode) )) {
	    cout << "The folder " << folder << " does not exist." << endl;
	} else {
		char folderAbsolutePath[200];
		realpath(folder.c_str(),folderAbsolutePath);
		cout << "Attempt to save the images in the folder " << folderAbsolutePath << " :" << endl;
		unsigned int i=0;
		string imageName;
		for (i = 0; i < images.size(); i++) {
			imageName = string(folderAbsolutePath) + "/" + to_string(i) + ".png";
			imwrite(imageName,images[i]);
		}
		cout << to_string(i) << " images have been saved" << endl;
	}

	return 0;
}
