#include <opencv2/opencv.hpp>
#include <math.h> 

const double pi = 3.14159265358979311599796346854;

int Circle() {

	cv::Mat my_image(cv::Size(800, 450), CV_8SC3);
	cv::Scalar my_color_green(0, 255, 0);

	my_image.setTo(my_color_green);

	cv::circle(my_image, cv::Point(400, 225), 200, cv::Scalar(255, 0, 0), -1);
	cv::circle(my_image, cv::Point(400, 225), 100, cv::Scalar(255, 255, 255), 10);


	cv::namedWindow("Display");
	cv::imshow("Display", my_image);
	cv::waitKey();

	return 0;
}

int grayscale_image(cv::Mat image) {
	// Make a copy the same size, but a single color channel (C1)
	cv::Mat grayscale(image.size(), CV_8UC1);

	// Basic grayscale conversion
	for (int y = 0; y < image.size().height; ++y) {
		for (int x = 0; x < image.size().width; ++x) {
			cv::Vec3b colour = image.at<cv::Vec3b>(y, x);
			grayscale.at<unsigned char>(y, x) =
				(unsigned char)((colour[0] + colour[1] + colour[2]) / 3.0 + 0.5);
		}
	}

	// Make a copy the same size, but a single color channel (C1)
	cv::Mat grayscale2(image.size(), CV_8UC1);

	// proper grayscale conversion
	for (int y = 0; y < image.size().height; ++y) {
		for (int x = 0; x < image.size().width; ++x) {
			cv::Vec3b colour = image.at<cv::Vec3b>(y, x);
			grayscale2.at<unsigned char>(y, x) =
				(unsigned char)((colour[0] * .1 + colour[1] * .6 + colour[2] * .3));
		}
	}

	cv::namedWindow("Display");
	cv::imshow("Display", grayscale);
	cv::imshow("Display2", grayscale2);
	cv::waitKey();

	// Save the results
	cv::imwrite("grey.png", grayscale);
	cv::imwrite("grey2.png", grayscale2);

	return 0;
}

cv::Mat translationMatrix(double dx, double dy) {
	cv::Mat T = cv::Mat::eye(3, 3, CV_64F);
	T.at<double>(0, 2) = dx;
	T.at<double>(1, 2) = dy;
	return T;
}

cv::Mat scaleMatrix(double s) {
	cv::Mat T = cv::Mat::eye(3, 3, CV_64F);
	T.at<double>(0, 0) = s;
	T.at<double>(1, 1) = s;
	return T;
}

cv::Mat rotationMatrix(double angle) {

	double angleRad = (angle * pi)/180;

	cv::Mat T = cv::Mat::eye(3, 3, CV_64F);
	T.at<double>(0, 0) = (double)cos(angleRad);
	T.at<double>(1, 0) = (double)-sin(angleRad);
	T.at<double>(0, 1) = (double)sin(angleRad);
	T.at<double>(1, 1) = (double)cos(angleRad);
	return T;
}

cv::Mat rotation(std::string filename) {
	cv::Mat original = cv::imread(filename);

	//what angle the picture needs to be at
	cv::Mat R = rotationMatrix(45);

	//finding the center of the image
	double x = original.size().width/2;
	double y = original.size().height/2;

	cv::Mat T1 = translationMatrix(x, y);
	cv::Mat T2 = translationMatrix(-x, -y);

	//define the corners of the image
	cv::Mat c1(3, 1, CV_64F);
	c1.at<double>(0, 0) = 0;
	c1.at<double>(1, 0) = 0;
	c1.at<double>(2, 0) = 1;

	cv::Mat c2(3, 1, CV_64F);
	c2.at<double>(0, 0) = original.size().width;
	c2.at<double>(1, 0) = 0;
	c2.at<double>(2, 0) = 1;

	cv::Mat c3(3, 1, CV_64F);
	c3.at<double>(0, 0) = original.size().width;
	c3.at<double>(1, 0) = original.size().height;
	c3.at<double>(2, 0) = 1;

	cv::Mat c4(3, 1, CV_64F);
	c4.at<double>(0, 0) = 0;
	c4.at<double>(1, 0) = original.size().height;
	c4.at<double>(2, 0) = 1;

	std::vector<cv::Mat> corners = { c1, c2, c3, c4 };
	int TwoDPoints[4][2];

	//compute new coordinates to find their location in the image
	for (int i = 0; i < corners.size(); i++) {
		corners[i] = T1 * R * T2 * corners[i];
		TwoDPoints[i][0] = corners[i].at<double>(0, 0);
		TwoDPoints[i][1] = corners[i].at<double>(1, 0);
	}

	//finding min and max points
	int minX = TwoDPoints[0][0], minY = TwoDPoints[0][1];
	int maxX = TwoDPoints[0][0], maxY = TwoDPoints[0][1];

	// Iterate through the array to find min and max values
	for (int i = 1; i < 4; i++) {
		// Update minX and maxX
		if (TwoDPoints[i][0] < minX)
			minX = TwoDPoints[i][0];
		else if (TwoDPoints[i][0] > maxX)
			maxX = TwoDPoints[i][0];

		// Update minY and maxY
		if (TwoDPoints[i][1] < minY)
			minY = TwoDPoints[i][1];
		else if (TwoDPoints[i][1] > maxY)
			maxY = TwoDPoints[i][1];
	}

	//find what size the new frame must be
	double FrameSizeX = maxX-minX;
	double FrameSizeY = maxY-minY;

	cv::Mat target(FrameSizeY, FrameSizeX, CV_8UC3);

	for (int v = minY; v < maxY; ++v) {
		for (int u = minX; u < maxX; ++u) {
			// The column vector [u, v, 1]
			cv::Mat s(3, 1, CV_64F);
			s.at<double>(0, 0) = u;
			s.at<double>(1, 0) = v;
			s.at<double>(2, 0) = 1;

			cv::Mat t1 = T1 * R * T2 * s;

			int u_ = (int)(t1.at<double>(0) / t1.at<double>(2) + 0.5);
			int v_ = (int)(t1.at<double>(1) / t1.at<double>(2) + 0.5);

			if (v_ >= 0 && v_ < original.size().height && u_ >= 0 && u_ < original.size().width) {
				target.at<cv::Vec3b>(v-minY, u-minX) = original.at<cv::Vec3b>(v_, u_);
			}
		}
	}

	return target;
}

cv::Mat translation(std::string filename, double x, double y) {
	cv::Mat original = cv::imread(filename);
	cv::Mat target(original.size(), CV_8UC3);
	cv::Mat T = translationMatrix(x, y);

	for (int v = 0; v < original.size().height; ++v) {
		for (int u = 0; u < original.size().width; ++u) {
			// The column vector [u, v, 1]
			cv::Mat s(3, 1, CV_64F);
			s.at<double>(0, 0) = u;
			s.at<double>(1, 0) = v;
			s.at<double>(2, 0) = 1;
			cv::Mat t = T.inv() * s;

			int u_ = (int)(t.at<double>(0) / t.at<double>(2) + 0.5);
			int v_ = (int)(t.at<double>(1) / t.at<double>(2) + 0.5);

			if (v_ >= 0 && v_ < original.size().height && u_ >= 0 && u_ < original.size().width) {
				target.at<cv::Vec3b>(v, u) = original.at<cv::Vec3b>(v_, u_);
			}
		}
	}

	return target;
}

int main(int argc, char* argv[]) {
	std::string filename = "test.jpg";

	cv::Mat image = cv::imread(filename); //Mat data structures are used for both images and matrixes

	if (image.empty()) {
		std::cerr << "Could not load image from " << filename << std::endl;
		return -1;
	}

	//cv::namedWindow("Display");
	//cv::imshow("Display", image);
	//cv::waitKey(); //tell open cv UI to display the image using wait key

	//Circle();

	//grayscale_image(image);

	cv::namedWindow("Display");
	cv::imshow("Display", rotation(filename));
	cv::waitKey();

	return 0;
}