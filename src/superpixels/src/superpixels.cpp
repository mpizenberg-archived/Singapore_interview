/*
 * superpixels.cpp
 *
 *  Created on: Dec 27, 2014
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
		cout << " Usage: Singapore_superpixels folder_containing_images"
				<< endl;
		return -1;
	}

	// check if the folder exists
	struct stat sb;
	string folder = string(argv[1]);
	if (!(stat(folder.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))) {
		cout << "The folder " << folder << " does not exist." << endl;
	} else {
		char folderAbsolutePath[200];
		realpath(folder.c_str(), folderAbsolutePath);
		cout << "Attempt to load the images in the folder "
				<< folderAbsolutePath << " :" << endl;

		// get the sequence of images
		VideoCapture sequence;
		sequence.open(string(folderAbsolutePath) + "/%01d.png");

		// Check if the sequence is loaded
		if (!sequence.isOpened()) {
			cout << "Could not open or find the images" << endl;
			return EXIT_FAILURE;
		}

		// Create a window for video display and image display.
		int width = 800;
		int height = 450;
		string imageWindow = "Image window";
		namedWindow(imageWindow, WINDOW_NORMAL);
		resizeWindow(imageWindow, width, height);
		moveWindow(imageWindow, 50, 100);
		string segmentedWindow = "Segmented window";
		namedWindow(segmentedWindow, WINDOW_NORMAL);
		resizeWindow(segmentedWindow, width, height);
		moveWindow(segmentedWindow, 2 * 50 + width, 100);

		// Configuration of SEEDS superpixels
		Ptr<SuperpixelSEEDS> seeds;
		int init = false;
		int num_superpixels = 400;
		int num_levels = 4;

		// infinite loop
		Mat image, segmentedImage;
		unsigned int currentImage = 0;
		while (true) {
			sequence >> image;
			if (image.empty())
				break;
			imshow(imageWindow, image);
			waitKey(10);
			// segmentation of the image
			segmentedImage = segmentation(seeds, &init, width, height, image,
					num_superpixels, num_levels);
			imshow(segmentedWindow, segmentedImage);
			if (waitKey(0) == 'q') {
				break;
			}
			currentImage++;
		}
	}
	return 0;
}
