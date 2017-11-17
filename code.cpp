#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<algorithm>

using namespace std;
using namespace cv;

int threshold_val = 127;

//for sorting vector of points
bool cwComp(Point p1, Point p2) {
	return p1.x < p2.x;
}

//Displays the result onto the frame
void displayGesture(Mat frame, int feature[]) {
	char result[2] = "?";
	int b0 = feature[0];
	int b1 = feature[1];
	int b2 = feature[2];
	int b3 = feature[3];
	int b4 = feature[4];
	if (b0 == 0 && b1 == 0 && b2 == 0 && b3 == 0 && b4 == 1)
		result[0] = 'A';
	else if (b0 == 0 && b1 == 0 && b2 == 1 && b3 == 0 && b4 == 0)
		result[0] = 'B';
	else if (b0 == 0 && b1 == 0 && b2 == 1 && b3 == 1 && b4 == 0)
		result[0] = 'C';
	else if (b0 == 1 && b1 == 0 && b2 == 0 && b3 == 1 && b4 == 0)
		result[0] = 'D';
	else if (b0 == 1 && b1 == 0 && b2 == 0 && b3 == 1 && b4 == 1)
		result[0] = 'E';
	else if (b0 == 0 && b1 == 1 && b2 == 1 && b3 == 0 && b4 == 0)
		result[0] = 'F';
	else if (b0 == 0 && b1 == 1 && b2 == 0 && b3 == 1 && b4 == 0)
		result[0] = 'G';
	else if (b0 == 1 && b1 == 0 && b2 == 1 && b3 == 0 && b4 == 0)
		result[0] = 'H';
	else if (b0 == 1 && b1 == 1 && b2 == 0 && b3 == 0 && b4 == 0)
		result[0] = 'I';
	else if (b0 == 0 && b1 == 1 && b2 == 1 && b3 == 1 && b4 == 0)
		result[0] = 'J';
	else if (b0 == 1 && b1 == 1 && b2 == 1 && b3 == 0 && b4 == 0)
		result[0] = 'K';
	else if (b0 == 1 && b1 == 1 && b2 == 0 && b3 == 1 && b4 == 0)
		result[0] = 'L';
	else if (b0 == 1 && b1 == 0 && b2 == 1 && b3 == 1 && b4 == 0)
		result[0] = 'M';
	else if (b0 == 1 && b1 == 1 && b2 == 1 && b3 == 1 && b4 == 0)
		result[0] = 'N';
	else if (b0 == 0 && b1 == 0 && b2 == 0 && b3 == 1 && b4 == 1)
		result[0] = 'O';
	else if (b0 == 0 && b1 == 0 && b2 == 1 && b3 == 1 && b4 == 1)
		result[0] = 'P';
	else if (b0 == 0 && b1 == 1 && b2 == 1 && b3 == 1 && b4 == 1)
		result[0] = 'Q';
	else if (b0 == 1 && b1 == 1 && b2 == 1 && b3 == 1 && b4 == 1)
		result[0] = 'R';
	else if (b0 == 1 && b1 == 1 && b2 == 0 && b3 == 1 && b4 == 1)
		result[0] = 'S';
	else if (b0 == 1 && b1 == 0 && b2 == 1 && b3 == 1 && b4 == 1)
		result[0] = 'T';
	else if (b0 == 1 && b1 == 1 && b2 == 1 && b3 == 0 && b4 == 1)
		result[0] = 'U';
	else if (b0 == 1 && b1 == 1 && b2 == 0 && b3 == 0 && b4 == 1)
		result[0] = 'V';
	else if (b0 == 1 && b1 == 0 && b2 == 1 && b3 == 0 && b4 == 1)
		result[0] = 'W';
	else if (b0 == 0 && b1 == 0 && b2 == 1 && b3 == 0 && b4 == 1)
		result[0] = 'X';
	else if (b0 == 0 && b1 == 0 && b2 == 0 && b3 == 1 && b4 == 0)
		result[0] = '?';
	//similarly for other results...

	putText(frame, result, Point(frame.cols - 100, 80), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 4);
}

int main()
{
	VideoCapture cap(0);
	Mat frame;
	cap >> frame; //initial zero frame
	namedWindow("live feed", CV_WINDOW_AUTOSIZE);
	createTrackbar("Set Threshold Value", "live feed", &threshold_val, 255);

	while (true) {
		cap >> frame;

		/*-define a rectangular roi
		-apply Gaussian Blur to remove noise
		-perform thresholding to segment palm (define a trackbar to adjust the threshold)
		-dilate to remove noise
		*/
		Rect roi_ = Rect(50, 50, 250, 250);
		Mat roi = frame(roi_);
		cvtColor(roi, roi, CV_BGR2GRAY);
		GaussianBlur(roi, roi, Size(5, 5), 5, 5);
		threshold(roi, roi, threshold_val, 255, CV_THRESH_BINARY_INV);
		Mat se = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
		dilate(roi, roi, se);

		/*-detect orientation
		-calc centroid of the palm
		*/
		int orientation = 0;
		for (int i = 0; i<roi.cols; i++) {
			if (roi.at<uchar>(roi.rows - 1, i) > 0) {
				orientation = 0;
				break;
			}
			else if (roi.at<uchar>(i, roi.cols - 1) > 0) {
				orientation = 1;
				break;
			}
		}

		float cx = 0.0, cy = 0.0;
		float sumi = 1.0;
		for (int i = 0; i < roi.rows; i++) {
			for (int j = 0; j<roi.cols; j++) {
				cy += roi.at<uchar>(i, j) * i;
				cx += roi.at<uchar>(i, j) * j;
				sumi += roi.at<uchar>(i, j);
			}
		}
		cx /= sumi;
		cy /= sumi;

		/*Detection of finger tips
		-find the significant boundary of the palm from vector of boundaries
		-apply polygonal approximation to the above boundary
		-detect peaks (higher y cordinated from two neighboring pts)
		-check for redundant peaks
		*/
		vector<vector <Point> > contours;
		Mat temp = roi.clone();
		vector<Vec4i> heiarchy;
		findContours(temp, contours, heiarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
		int c_size = 0, c_idx = 0;
		for (int i = 0; i<contours.size(); i++) {
			if (contours.size()>c_size) {
				c_idx = i;
				c_size = contours.size();
			}
		}

		vector<Point> boundary0;
		vector<Point> peaks_;
		if (!contours.empty()) {
			boundary0 = contours[c_idx];
		}

		vector<Point> boundary;
		if (!boundary0.empty()) {
			approxPolyDP(boundary0, boundary, 2, false);

			for (int i = 0; i<boundary.size(); i++) {
				circle(frame, boundary[i] + Point(roi_.x, roi_.y), 5, Scalar(100, 200, 100));
			}
		}

		//TO DO: similarly for horizontal orientation (numerals signs)

		if (orientation == 0) {
			for (int i = 0; (i + 1)<boundary.size(); i++) {
				Point p = boundary[i];
				if (cy > p.y) {
					bool isPeak = true;
					int indxl = (i - 1) % boundary.size();
					int indxr = (i + 1) % boundary.size();
					if (boundary[indxl].y < p.y || boundary[indxr].y < p.y) {
						isPeak = false;
					}
					if (isPeak) {
						peaks_.push_back(p);
						//circle(frame, p+Point(roi_.x, roi_.y), 20, Scalar(255, 0, 0));
					}
				}
			}
			//filter out redundant peaks
			vector<Point> peaks0;
			for (int i = 0; i<peaks_.size(); i++) {
				int nexti = (i + 1) % peaks_.size();
				if (abs(peaks_[i].y - peaks_[nexti].y)<15 && abs(peaks_[i].x - peaks_[nexti].x)<15) {
					continue;
				}
				peaks0.push_back(peaks_[i]);
				circle(frame, peaks_[i] + Point(roi_.x, roi_.y), 20, Scalar(0, 0, 255));
			}
			//cout<<"peaks: "<<peaks0.size()<<'\n';
			int feature[5];
			bool detected = false;

			//convert peaks into feature vector of raised and folded fingers
			sort(peaks0.begin(), peaks0.end(), cwComp); //arrange peaks in clockwise manner;
			if (peaks0.size() == 4) {
				//detect thumb by measuring the compactness of the thumb portion of the palm
				Rect thumbRect(0, cy, roi_.width, roi_.height - cy);
				Mat thumbRoi = roi(thumbRect);
				vector< vector<Point> > thumbContours;
				vector<Vec4i> tHeirachy;
				temp = thumbRoi.clone();
				findContours(temp, thumbContours, tHeirachy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
				int tc_size = 0, tc_idx = 0;
				for (int i = 0; i<thumbContours.size(); i++) {
					if (thumbContours.size()>tc_size) {
						tc_idx = i;
						tc_size = thumbContours.size();
					}
				}

				vector<Point> tboundary0;
				if (!thumbContours.empty()) {
					tboundary0 = thumbContours[tc_idx];
				}
				double perimeter = arcLength(tboundary0, true);
				double area = contourArea(tboundary0);
				double compactness = perimeter*perimeter / (4 * 3.14*area);
				double distArray[4];
				//cout<<"[ ";
				for (int i = 0; i<peaks0.size(); i++) {
					//	cout<<peaks0[i].y<<" ";
					distArray[i] = sqrt((cy - peaks0[i].y)*(cy - peaks0[i].y) + (cx - peaks0[i].x)*(cx - peaks0[i].x));
				}
				//cout<<"]==>[ ";
				double max = 0.0;
				for (int i = 0; i<4; i++) {
					if (distArray[i] > max)
						max = distArray[i];
				}
				feature[4] = compactness>1.8 ? 1 : 0;
				for (int i = 0; i<4; i++) {
					if (distArray[i] > 0.75*max)
						feature[i] = 1;
					else
						feature[i] = 0;
				}
				for (int i = 0; i<5; i++) {
					//	cout<<feature[i]<<" ";
				}
				//cout<<"]\n";
				detected = true;
			}
			if (detected) {
				displayGesture(frame, feature);
			}
			else {
				putText(frame, "Waiting for sign...", Point(frame.cols - 200, 80), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));
			}
			line(frame, Point(roi_.x, cy + roi_.y), Point(roi_.x + roi_.width, cy + roi_.y), Scalar(0, 255, 0), 2); // horizontal line
		} // end for vertical orientation

		line(frame, Point(cx + roi_.x, cy + roi_.y), Point(cx + roi_.x, cy + roi_.y), Scalar(0, 0, 255), 5); //centroid
		rectangle(frame, roi_, Scalar(255, 0, 0), 2); //roi of the palm
		imshow("live feed", frame);
		imshow("roi", roi);
		cvWaitKey(10);
	}

	cvWaitKey(0);
	return 0;
}