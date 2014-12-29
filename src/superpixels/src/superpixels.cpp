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
 * The returned vector contains the matrices :
 * 0. labels
 * 1. borders
 */
vector<Mat> segmentation(Ptr<SuperpixelSEEDS>& seeds, int *init, int width,
		int height, Mat& image, int num_superpixels, int num_levels) {

	vector<Mat> segmentedImages;

	// initialization of SEEDS superpixels
	if (!(*init)) {
		seeds = createSuperpixelSEEDS(width, height, image.channels(),
				num_superpixels, num_levels, 1, 5, true);
		cout << "Number of superpixels created : "
				<< to_string(seeds->getNumberOfSuperpixels()) << endl;
		*init = true;
	}

	// superpixels iterations
	Mat converted;
	cvtColor(image, converted, COLOR_BGR2Lab);
	seeds->iterate(converted);
	cvtColor(converted, converted, COLOR_Lab2BGR);

	// retrieve the segmentation result
	Mat labels;
	seeds->getLabels(labels);
	segmentedImages.push_back(labels.clone());

	// get the contours for displaying
	Mat mask;
	seeds->getLabelContourMask(mask, false);
	segmentedImages.push_back(mask.clone());

	return segmentedImages;
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

		// Create windows for images display.
		int width = 800, widthBorder = 2;
		int height = 450, heightBorder = 40;
		// original images
		string imageWindow = "Image window";
		namedWindow(imageWindow, WINDOW_NORMAL);
		resizeWindow(imageWindow, width, height);
		moveWindow(imageWindow, 50, 50);
		// borders
		string bordersWindow = "Borders window";
		namedWindow(bordersWindow, WINDOW_NORMAL);
		resizeWindow(bordersWindow, width, height);
		moveWindow(bordersWindow, 50 + width + widthBorder, 50);
		// labels
		string labelsWindow = "Labels window";
		namedWindow(labelsWindow, WINDOW_NORMAL);
		resizeWindow(labelsWindow, width, height);
		moveWindow(labelsWindow, 50 + width + widthBorder, 50 + height + heightBorder);
		// segmented
		string segmentedWindow = "Segmented window";
		namedWindow(segmentedWindow, WINDOW_NORMAL);
		resizeWindow(segmentedWindow, width, height);
		moveWindow(segmentedWindow, 50, 50 + height + heightBorder);

		// Configuration of SEEDS superpixels
		Ptr<SuperpixelSEEDS> seeds;
		int init = false;
		int num_superpixels = 100;
		int num_levels = 4;

		// infinite loop
		Mat image, borders, labels, segmented;
		vector<Mat> segmentedImages;
		unsigned int currentImage = 0;
		while (true) {
			sequence >> image;
			if (image.empty())
				break;
			imshow(imageWindow, image);
			waitKey(10);

			// segmentation of the image
			segmentedImages = segmentation(seeds, &init, width, height, image,
					num_superpixels, num_levels);
			currentImage++;

			// show the borders
			borders = segmentedImages[1].clone();
			imshow(bordersWindow, 255*borders);
			if (waitKey(10) == 'q') {
				break;
			}

			// show the labels
			labels = segmentedImages[0].clone();
			const int num_label_bits = 3;
			labels &= (1 << num_label_bits) - 1;
			labels *= 1 << (16 - num_label_bits);
			imshow(labelsWindow, labels);
			if (waitKey(10) == 'q') {
				break;
			}

			// show the segmented image
			segmented = image.clone();
			segmented.setTo(Scalar(0, 0, 255), borders);
			imshow(segmentedWindow, segmented);
			if (waitKey(0) == 'q') {
				break;
			}

		}
		cout << to_string(currentImage) << " images have been segmented"
				<< endl;
	}
	return 0;
}
