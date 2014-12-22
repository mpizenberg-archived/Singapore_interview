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

/**
 * Compute the SEEDS superpixels algorithm to segment the cat.
 */
Mat segmentation(Ptr<SuperpixelSEEDS>& seeds, int *init, int width, int height,
		Mat& image, int num_superpixels, int num_levels) {

	// initialization of SEEDS superpixels
	if (!(*init)) {
		seeds = createSuperpixelSEEDS(width, height, image.channels(),
				num_superpixels, num_levels, 2, 5, false);
		*init = true;
	}

	// superpixels iterations
	Mat converted;
	cvtColor(image, converted, COLOR_BGR2HSV);
	seeds->iterate(converted);

	// retrieve the segmentation result
	Mat labels;
	seeds->getLabels(labels);
	const int num_label_bits = 2;
	labels &= (1 << num_label_bits) - 1;
	labels *= 1 << (16 - num_label_bits);

	// get the contours for displaying
	Mat mask;
	seeds->getLabelContourMask(mask, false);
	converted.setTo(Scalar(0, 0, 255), mask);

	return mask;
	//return labels;
	//return converted;
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
	string segmentedWindow = "Segmented window";
	namedWindow(segmentedWindow, WINDOW_NORMAL);
	resizeWindow(segmentedWindow, width, height);
	moveWindow(segmentedWindow, 2 * 50 + width, 2 * 100 + height);

	// get the framerate of the video
	int fps = (int) capture.get(CAP_PROP_FPS);
	// deduce the time between 2 frames (in ms)
	double mspf = 1000 / (double) fps;

	// the vector containing 10 images
	vector<Mat> images;
	int currentImage = 0;

	// Segmented image of the cat
	Mat segmentedImage;
	Ptr<SuperpixelSEEDS> seeds;
	int init = false;
	int num_superpixels = 400;
	int num_levels = 4;

	// infinite loop
	while (1) {
		image = nextImage(capture, fps, fps, mspf, videoWindow, imageWindow,
				width, height);
		if (image.empty())
			break;
		// segmentation of the image
		segmentedImage = segmentation(seeds, &init, width, height, image,
				num_superpixels, num_levels);
		imshow(segmentedWindow, segmentedImage);
		if (waitKey(10) == 'q')
			break;
		else if (currentImage < 10) {
			images.push_back(image.clone());
		}
		currentImage++;
	}
	//cout << "Number of images kept : " << to_string(images.size()) << endl;

	return 0;
}
