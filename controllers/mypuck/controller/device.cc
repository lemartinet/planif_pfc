#include "device.hh"
#include "params.hh"
#include "math.hh"
#include "mystr.hh"
#include <iostream>
#include <sstream>
#include <cstdlib>
//#include <webots/Camera.hpp>
//#include <webots/GPS.hpp>
//#include <webots/Compass.hpp>

// position du but
//#define GOAL_X 0
//#define GOAL_Y -1.68

// indique si on a parcouru au moins une fois chaque bras
bool way1_visited = 0;
bool way2_visited = 0;
bool way3_visited = 0;

// goal color: green
const int REF_COLOR[3] = { 0, 255, 0 };

// camera
int camera_enabled = 0; // mettre à 1 pour activer la camera
int image_width = 0, image_height = 0;
float field_of_view = 0.0;

RobotDevice::RobotDevice () :
	position_(0.0, -0.05), orientation_(0.0), num_pixels_goal_(0), 
	nb_trial_(0), goal_reached_(false),
	cpt_trial_(0), cpt_total_(0)
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
	  	// get image dimensions
	  	image_width = camera_->getWidth();
	  	image_height = camera_->getHeight();
	  	field_of_view = camera_->getFov();
  	}
  	step (TIME_STEP); // pr la màj des capteurs

  	const double* pos = gps_->getValues ();
  	position_.x_set (pos[0]);
  	position_.y_set (pos[2]);
  	const double* orient = compass_->getValues ();
  	orientation_ = -atan2 (orient[0], orient[2]) + M_PI/2;
  	orientation_ = orientation_ > M_PI ? orientation_ - 2*M_PI : orientation_;
  	orientation_ = orientation_ < -M_PI ? orientation_ + 2*M_PI : orientation_;

  	string filename = Params::get_path ();
	filename += "output_mypuck.txt";
  	output_mypuck_ = new ofstream (filename.c_str ());
	filename = Params::get_path ();
	filename += "output_neurosolver.txt";
  	output_neuro_ = new ofstream (filename.c_str ());
}

RobotDevice::~RobotDevice () 
{
	output_mypuck ("Mypuck: die ok");
	delete output_mypuck_;
	output_neuro ("Neurosolver: die ok");
	delete output_neuro_;
} 

int rgb_diff(const unsigned char a[3], const int b[3]) {
  return abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2]);
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
float process_camera_image(const unsigned char *image, const int ref_color[3], int *npixels) {

  // compute rgb discrepancy with respect to ref_color
  int x, sum_pixels = 0, total = 0;
  for (x = 0; x < image_width; x++) {
    int y, sum_column = 0;
    for (y = 0; y < image_height; y++) {
      const unsigned char *pixel = image + 3 * (y * image_width + x);
      int bin = rgb_diff(pixel, ref_color) < 10 ? 1 : 0;
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
    	unsigned char *image = camera_->getImage ();
    	goal_dir_ = process_camera_image(image, REF_COLOR, &num_pixels_goal_);
  	}
  	//cout << num_pixels_goal<< endl;
  	
  	double x = position_.x_get();
	double y = position_.y_get();
	
	// le robot a-t-il ete bouge par le manipulateur
	if ((old_x - x) * (old_x - x) + (old_y - y) * (old_y - y) > 0.5) {
		manually_moved_ = true;
		cpt_trial_ = 0;
		output_mypuck ("bougé");
		nb_trial_++;
	}
	else {
		manually_moved_ = false;
	} 
	
	goal_reached_ = false;
	while (receiver_->getQueueLength () > 0) {
		const string message = static_cast<const char*>(receiver_->getData ());
//		cout << message << endl;
		receiver_->nextPacket ();
		
		if (message == "way1") {
			way1_visited++;
		}
		else if (message == "way2") {
			way2_visited++;
		}
		else if (message == "way3") {
			way3_visited++;
		}
		else if (message == "goal") {
			goal_reached_ = true;
			output_mypuck ("goal found");
		}
	}
}

bool RobotDevice::goal_reached () const {
//	int retina_size = image_width * image_height;
//	// si l'image du but prend plus de 85% de la retine
//	// alors on est arrive !
//	cout << num_pixels_goal_ << " " << retina_size << endl;
//	if (num_pixels_goal_ > 0.25 * retina_size) {
//		cout << "trouve!!!" << endl;
//		return true;
//	}
//	return false;

//	if (distance_goal_factor () < 0.07) {
//		return true;
//	}
//	else {
//		return false;	
//	}
	return goal_reached_;
}

//double RobotDevice::distance_goal_factor () const {
//	// renvoie un facteur de distance qui pourrait par exemple
//	// etre mesuree par l'odorat (facteur lineaire)
//	double x = position_.x_get() - GOAL_X;
//	double y = position_.y_get() - GOAL_Y;
//	double dist = sqrt(x*x + y*y);
//	return (dist > 1)?1:dist;
//}

//bool RobotDevice::all_ways_visited (bool reinit) {
//	// indique si on a parcouru au moins une fois chaque bras
//	if (reinit) {
//		way1_visited = way2_visited = way3_visited = 0;
//	}
//	return (way1_visited == 2) && (way2_visited == 2) && (way3_visited == 2);
//}

void RobotDevice::write_message (ofstream& file, const string& message) const
{
	file << time_get (false) << ": " << message << endl;
}

string RobotDevice::time_get (bool total) const
{
	static const int TIME_STEP = Params::get_int ("TIME_STEP");
	ostringstream time;
	if (total) {
		time << "Step " << cpt_total_ << " ";
	}
	time << "Day " << day_get () << " Trial " << trial_get () 
		 << " Time " << cpt_trial_ * TIME_STEP / 1000.0 << "s";
	return time.str ();
}

