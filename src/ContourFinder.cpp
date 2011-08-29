#include "ContourFinder.h"

namespace ofxCv {
	using namespace cv;
	
	ContourFinder::ContourFinder()
	:autoThreshold(true)
	,thresholdValue(128.)
	,invert(false)
	,simplify(true) {
		resetMinArea();
		resetMaxArea();
	}
	
	void ContourFinder::findContours(Mat img) {
		if(img.channels() == 1) {
			thresh = img.clone();
		} else if(img.channels() == 3) {
			cvtColor(img, thresh, CV_RGB2GRAY);
		} else if(img.channels() == 4) {
			cvtColor(img, thresh, CV_RGBA2GRAY);
		}
		if(autoThreshold) {
			threshold(thresh, thresholdValue, invert);
		}
		
		vector<vector<cv::Point> > allContours;
		int simplifyMode = simplify ? CV_CHAIN_APPROX_SIMPLE : CV_CHAIN_APPROX_NONE;
		cv::findContours(thresh, allContours, CV_RETR_EXTERNAL, simplifyMode);
		
		bool needMinFilter = (minArea > 0);
		bool needMaxFilter = maxAreaNorm ? (maxArea < 1) : (maxArea < numeric_limits<float>::infinity());
		if(needMinFilter || needMaxFilter) {
			contours.clear();
			double imgArea = img.rows * img.cols;
			double imgMinArea = minAreaNorm ? (minArea * imgArea) : minArea;
			double imgMaxArea = maxAreaNorm ? (maxArea * imgArea) : maxArea;
			for(int i = 0; i < allContours.size(); i++) {
				double curArea = contourArea(Mat(allContours[i]));
				if((!needMinFilter || curArea >= imgMinArea) &&
					(!needMaxFilter || curArea <= imgMaxArea)) {
					contours.push_back(allContours[i]);
				}
			}
		} else {
			contours = allContours;
		}
		
		polylines.clear();
		for(int i = 0; i < contours.size(); i++) {
			polylines.push_back(toOf(contours[i]));
		}
	}

	vector<vector<cv::Point> >& ContourFinder::getContours() {
		return contours;
	}
	
	vector<ofPolyline>& ContourFinder::getPolylines() {
		return polylines;
	}
	
	unsigned int ContourFinder::size() const {
		return contours.size();
	}
	
	vector<cv::Point>& ContourFinder::getContour(unsigned int i) {
		return contours[i];
	}
	
	ofPolyline& ContourFinder::getPolyline(unsigned int i) {
		return polylines[i];
	}
	
	cv::Rect ContourFinder::getBoundingRect(unsigned int i) const {
		return boundingRect(Mat(contours[i]));
	}
	
	cv::Point2f ContourFinder::getCenter(unsigned int i) const {
		cv::Rect box = getBoundingRect(i);
		return cv::Point2f(box.x + box.width / 2, box.y + box.height / 2);
	}
	
	cv::Point2f ContourFinder::getCentroid(unsigned int i) const {
		Moments m = moments(Mat(contours[i]));
		return cv::Point2f(m.m10 / m.m00, m.m01 / m.m00);
	}
	
	cv::Point2f ContourFinder::getAverage(unsigned int i) const {
		Scalar average = mean(Mat(contours[i]));
		return cv::Point2f(average[0], average[1]);
	}
	
	cv::Vec2f ContourFinder::getBalance(unsigned int i) const {
		return cv::Vec2f(getCentroid(i) - getCenter(i));
	}
	
	double ContourFinder::getContourArea(unsigned int i) const {
		return contourArea(Mat(contours[i]));
	}
	
	double ContourFinder::getArcLength(unsigned int i) const {
		return arcLength(Mat(contours[i]), true);
	}
	
	vector<cv::Point> ContourFinder::getConvexHull(unsigned int i) const {
		vector<cv::Point> hull;
		convexHull(Mat(contours[i]), hull);
		return hull;
	}
	
	cv::RotatedRect ContourFinder::getMinAreaRect(unsigned int i) const {
		return minAreaRect(Mat(contours[i]));
	}
	
	cv::RotatedRect ContourFinder::getFitEllipse(unsigned int i) const {
		return fitEllipse(Mat(contours[i]));
	}
	
	void ContourFinder::setAutoThreshold(bool autoThreshold) {
		this->autoThreshold = autoThreshold;
	}
	
	void ContourFinder::setThreshold(float thresholdValue) {
		this->thresholdValue = thresholdValue;
	}
	
	void ContourFinder::setInvert(bool invert) {
		this->invert = invert;
	}
	
	void ContourFinder::setSimplify(bool simplify) {
		this->simplify = simplify;
	}
	
	void ContourFinder::draw() {
		for(int i = 0; i < polylines.size(); i++) {
			polylines[i].draw();
		}
	}
	
	void ContourFinder::resetMinArea() {
		setMinArea(0);
	}
	
	void ContourFinder::resetMaxArea() {
		setMaxArea(numeric_limits<float>::infinity());
	}
	
	void ContourFinder::setMinArea(float minArea) {
		this->minArea = minArea;
		minAreaNorm = false;
	}
	
	void ContourFinder::setMaxArea(float maxArea) {
		this->maxArea = maxArea;
		maxAreaNorm = false;
	}
	
	void ContourFinder::setMinAreaRadius(float minAreaRadius) {
		minArea = PI * minAreaRadius * minAreaRadius;
		minAreaNorm = false;
	}
	
	void ContourFinder::setMaxAreaRadius(float maxAreaRadius) {
		maxArea = PI * maxAreaRadius * maxAreaRadius;
		maxAreaNorm = false;
	}
	
	void ContourFinder::setMinAreaNorm(float minAreaNorm) {
		minArea = minAreaNorm;
		this->minAreaNorm = true;
	}
	
	void ContourFinder::setMaxAreaNorm(float maxAreaNorm) {
		maxArea = maxAreaNorm;
		this->maxAreaNorm = true;
	}
	
}