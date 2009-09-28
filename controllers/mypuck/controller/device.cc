#include "device.hh"
#include "params.hh"
#include "math.hh"
#include "mystr.hh"
#include <iostream>
#include <sstream>
#include <cstdlib>

RobotDevice* RobotDevice::the_robot_ = 0;

// camera
int camera_enabled = 0; // mettre à 1 pour activer la camera

RobotDevice::RobotDevice () :
	position_(0.0, -0.05), orientation_(0.0), num_pixels_goal_(0), 
	nb_trial_(0), goal_reached_(false),
	cpt_trial_(0), cpt_total_(0), sleep_(false)
{
	const int TIME_STEP = Params::get_int ("TIME_STEP");
	gps_ = getGPS ("gps");
	gps_->enable (TIME_STEP);
	compass_ = getCompass ("compass");
	compass_->enable (TIME_STEP);
//	ls_ = getLightSensor ("ls0");
//	ls_->enable (TIME_STEP);
  	receiver_ = getReceiver ("receiver");
  	receiver_->enable (TIME_STEP);
  	if (camera_enabled) {
		camera_ = getCamera ("camera");
		camera_->enable (TIME_STEP);
  	}
  	step (TIME_STEP); // pr la màj des capteurs

  	const double* pos = gps_->getValues ();
  	position_.x_set (pos[0]);
  	position_.y_set (pos[2]);
  	const double* orient = compass_->getValues ();
  	orientation_ = -atan2 (orient[0], orient[2]) + M_PI/2;
  	orientation_ = orientation_ > M_PI ? orientation_ - 2*M_PI : orientation_;
  	orientation_ = orientation_ < -M_PI ? orientation_ + 2*M_PI : orientation_;
  	
  	the_robot_ = this;
}

RobotDevice::~RobotDevice () 
{
} 

// find goal direction in camera image (from icea_sim controller)
// the image is compared to a reference color (ref_color) and thresholded,
// matching pixels are added and weighted from left to right
// returns: a number between -1.0 and +1.0
// -1.0 -> the goal is located in the leftmost part of the image
// +1.0 -> the goal is located in the rightmost part of the image
//  0.0 -> the goal is located in the exact middle of the image
//  nan -> no pixel close to ref_color is currently visible
// output: npixels indicates the count of matching pixels in the image
float RobotDevice::process_camera_image(const unsigned char *image, int *npixels) {
	int image_width = camera_->getWidth();
	int image_height = camera_->getHeight();
	// goal color: green
	const int ref_color[3] = { 0, 255, 0 };
	// compute rgb discrepancy with respect to ref_color
	int sum_pixels = 0, total = 0;
	for (int x = 0; x < image_width; x++) {
		int sum_column = 0;
		for (int y = 0; y < image_height; y++) {
			const unsigned char *pixel = image + 3 * (y * image_width + x);
			int rgb_diff = abs(pixel[0] - ref_color[0]) + abs(pixel[1] - ref_color[1]) + abs(pixel[2] - ref_color[2]);
			int bin = rgb_diff < 10 ? 1 : 0;
			sum_pixels += bin;
			sum_column += bin;
		}
		total += sum_column * x;
	}
	if (npixels) *npixels = sum_pixels;
	return ((float)total / ((float)sum_pixels * ((float)image_width - 1.0))) * 2.0 - 1.0;
}

void RobotDevice::synch ()
{
    cpt_trial_++;
    cpt_total_++;
  	
  	// save old position
  	double old_x = position_.x_get ();
  	double old_y = position_.y_get ();
  	
  	// read the gps
  	const double* pos = gps_->getValues ();
  	position_.x_set (pos[0]);
  	position_.y_set (pos[2]);
  	const double* orient = compass_->getValues ();
  	orientation_ = -atan2 (orient[0], orient[2]) + M_PI/2;
  	orientation_ = orientation_ > M_PI ? orientation_ - 2*M_PI : orientation_;
  	orientation_ = orientation_ < -M_PI ? orientation_ + 2*M_PI : orientation_;
//  	cout << position_.x_get() << " " << position_.y_get() << " " << orientation_ << endl;
  	
    // read and process camera images
  	if (camera_enabled) {
    	const unsigned char *image = camera_->getImage ();
    	process_camera_image(image, &num_pixels_goal_);
  	}
  	//cout << num_pixels_goal<< endl;
  	
  	double x = position_.x_get();
	double y = position_.y_get();
	
	// le robot a-t-il ete bouge par le manipulateur
	if ((old_x - x) * (old_x - x) + (old_y - y) * (old_y - y) > 0.5) {
		manually_moved_ = true;
		cpt_trial_ = 0;
		nb_trial_++;
	}
	else {
		manually_moved_ = false;
	} 
	
	goal_reached_ = false;
	sleep_ = false;
	while (receiver_->getQueueLength () > 0) {
		const string message = static_cast<const char*>(receiver_->getData ());
//		cout << message << endl;
		receiver_->nextPacket ();
		if (message == "goal") {
			goal_reached_ = true;
		}
		else if (message == "sleep") {
			sleep_ = true;
//			cout << "sleep !!!" << endl; 
		}
//		else {
//			dist_goal_ = atoi(message.c_str());	
//		}
	}
}

bool RobotDevice::goal_reached () const {
	// methode avec vision
//	int image_width = camera_->getWidth();
//	int image_height = camera_->getHeight();
//	int retina_size = image_width * image_height;
//	cout << num_pixels_goal_ << " " << retina_size << endl;
//	return num_pixels_goal_ > 0.25 * retina_size;
	// methode avec distance
//	return distance_goal_factor () < 0.07;
	return goal_reached_;
}

