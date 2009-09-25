#include <iostream>
#include "device.hh"
#include "params.hh"
#include "math.hh"

extern Params* params;

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

// 8 IR proximity sensors
#define NB_DIST_SENS 8
#define PS_RIGHT_00 0
#define PS_RIGHT_45 1
#define PS_RIGHT_90 2
#define PS_RIGHT_REAR 3
#define PS_LEFT_REAR 4
#define PS_LEFT_90 5
#define PS_LEFT_45 6
#define PS_LEFT_00 7
#define NO_SIDE -1
#define LEFT 0
#define RIGHT 1

int ps_value[NB_SENSORS] = {0,0,0,0,0,0,0,0};
const int PS_OFFSET_SIMULATION[NB_DIST_SENS] = {300,300,300,300,300,300,300,300};

////////////////////////////////////////////
// OAM - Obstacle Avoidance Module (from e-puck_line controller)
//
// The OAM routine first detects obstacles in front of the robot, then records
// their side in "oam_side" and avoid the detected obstacle by 
// turning away according to very simple weighted connections between
// proximity sensors and motors. "oam_active" becomes active when as soon as 
// an object is detected and "oam_reset" inactivates the module and set 
// "oam_side" to NO_SIDE. Output speeds are in oam_speed[LEFT] and oam_speed[RIGHT].

bool oam_active;
int oam_speed[2];
int oam_side = NO_SIDE;
bool bloque;
bool demi_tour;

#define OAM_OBST_THRESHOLD 3000
#define OAM_FORWARD_SPEED 400
#define OAM_K_PS_90 0.02
#define OAM_K_PS_45 0.09
#define OAM_K_PS_00 0.12
#define OAM_K_MAX_DELTAS 600
#define OAM_BLOQUE 8000

int blocked = 0;
double last_x = 0, last_y = 0;

void RobotDevice::avoid_block () 
{
  	double x = position_.x_get ();
	double y = position_.y_get ();
  
  	if (fabs (last_x - x) < 0.002 && fabs (last_y - y) < 0.002) {
    	blocked++;
  	}
  	else {
    	blocked = blocked > 0 ? blocked - 1: 0;
  	}
  	last_x = x;
  	last_y = y;
}

bool RobotDevice::obstacle_avoidance_module(double angle)
{
  int max_ds_value, DeltaS=0, i;
  int Activation[]={0,0};

  // Determine the presence and the side of an obstacle
  max_ds_value = 0;
  for (i = PS_RIGHT_00; i <= PS_RIGHT_45; i++) {
    if (max_ds_value < ps_value[i]) max_ds_value = ps_value[i];
  	Activation[RIGHT] += ps_value[i];
  }
  Activation[RIGHT] += ps_value[PS_RIGHT_90];
  for (i = PS_LEFT_45; i <= PS_LEFT_00; i++) {
    if (max_ds_value < ps_value[i]) max_ds_value = ps_value[i];
    Activation[LEFT] += ps_value[i];
  }
  Activation[LEFT] += ps_value[PS_LEFT_90];
  if (max_ds_value > OAM_OBST_THRESHOLD) {
  	oam_active = true;
  }
  else {
  	oam_active = false;
  	bloque = false;
  }

  if (oam_active && !bloque) {
  	if (Activation[RIGHT] > OAM_BLOQUE && Activation[LEFT] > OAM_BLOQUE) {
  		bloque = true;
  		//cout << "bloque" << endl;
  	}
    if (Activation[RIGHT] > Activation[LEFT]) oam_side = RIGHT;
    else oam_side = LEFT;
  }

  // Forward speed
  oam_speed[LEFT]  = OAM_FORWARD_SPEED;
  oam_speed[RIGHT] = OAM_FORWARD_SPEED;

	// Go away from obstacle
  	if (!demi_tour && oam_active) {
    	// The rotation of the robot is determined by the location and the side of the obstacle
    	if (oam_side == LEFT) {
    		DeltaS -= (int) (OAM_K_PS_90 * ps_value[PS_LEFT_90]);
      		DeltaS -= (int) (OAM_K_PS_45 * ps_value[PS_LEFT_45]);
      		DeltaS -= (int) (OAM_K_PS_00 * ps_value[PS_LEFT_00]);
    	}
    	else { // oam_side == RIGHT
    		DeltaS += (int) (OAM_K_PS_90 * ps_value[PS_RIGHT_90]);
      		DeltaS += (int) (OAM_K_PS_45 * ps_value[PS_RIGHT_45]);
      		DeltaS += (int) (OAM_K_PS_00 * ps_value[PS_RIGHT_00]);
    	}
    	//cout << "delta1 :" << DeltaS << endl;
  	}
  
  	avoid_block ();
  	if (!bloque && blocked < 500) {
  		if (fabs(angle) < 0.1 * PI) {
  			demi_tour = false;
  			DeltaS -= (int)(2 * (float)OAM_FORWARD_SPEED / PI * -angle);
  		}
  		else if (demi_tour || fabs(angle) > 0.9 * PI) {
  			demi_tour = true;
  			//cout << "demi-tour : "<< angle << endl;
  			oam_speed[LEFT] = 0;
		    oam_speed[RIGHT] = 0;
		    DeltaS += (angle > 0?1:-1) * OAM_K_MAX_DELTAS;
  		}
  		else {
  			DeltaS -= (int)(2 * (float)OAM_FORWARD_SPEED / PI * -angle);
  		}
  		//cout << "delta2 :" << DeltaS << endl;
	}	
	if (DeltaS > OAM_K_MAX_DELTAS) DeltaS = OAM_K_MAX_DELTAS;
    if (DeltaS < -OAM_K_MAX_DELTAS) DeltaS = -OAM_K_MAX_DELTAS;

    // Set speeds
    oam_speed[LEFT] -= DeltaS;
    oam_speed[RIGHT] += DeltaS;
	//cout << "speed: " << oam_speed[LEFT] << " " << oam_speed[RIGHT] << endl; 
  	differential_wheels_set_speed (oam_speed[LEFT], oam_speed[RIGHT]);
  	return bloque;
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

void RobotDevice::init ()
{
	static const int TIME_STEP = params->get_int("TIME_STEP");
	char name[4]="ps0";
	for (int i = 0; i < NB_SENSORS; i++) {
		ps_[i] = robot_get_device (name);
		distance_sensor_enable (ps_[i], TIME_STEP);
		name[2]++;
    }
    gps_ = robot_get_device("gps");
    gps_enable (gps_, TIME_STEP);
  	//receiver_ = robot_get_device("receiver");
  	//receiver_enable(receiver_, TIME_STEP);
  	if (camera_enabled) {
  		camera_ = robot_get_device("camera");
  		camera_enable(camera_, TIME_STEP);
	  	// get image dimensions
	  	image_width = camera_get_width(camera_);
	  	image_height = camera_get_height(camera_);
	  	field_of_view = camera_get_fov(camera_);
  	}
  	robot_step(TIME_STEP);
}

void RobotDevice::synch ()
{
  	if (!synch_)
    	return;
	
	goal_reached_prec_ = goal_reached ();
  	update_obstacle_info ();
  	
  	// save old position
  	double old_x = position_.x_get ();
  	double old_y = position_.y_get ();
  	
  	// read the gps
  	const float * gps_matrix = gps_get_matrix (gps_);
  	position_.x_set (gps_position_x (gps_matrix));
  	position_.y_set (gps_position_z (gps_matrix));
  	float euler_angles[3];
  	gps_euler (gps_matrix, euler_angles);
  	orientation_ = euler_angles[1];
//  	cout << position_.x_get() << " " << position_.y_get() << " " << orientation_ << endl;
  	
    // read and process camera images
  	if (camera_enabled) {
    	unsigned char *image = camera_get_image(camera_);
    	goal_dir_ = process_camera_image(image, REF_COLOR, &num_pixels_goal_);
  	}
  	//cout << num_pixels_goal<< endl;
  	
  	double x = position_.x_get();
	double y = position_.y_get();
	
	// le robot a-t-il ete bouge par le manipulateur
	if ((old_x - x) * (old_x - x) + (old_y - y) * (old_y - y) > 0.5) {
		manually_moved_ = true;
//		cout << "bougé" << endl;
	}
	else {
		manually_moved_ = false;
	} 
	
	// les diffs sont pour obliger à circuler dans un sens du way
	double diff1 = fabs(ecart_angulaire (orientation_, 0));
	double diff2 = fabs(ecart_angulaire (orientation_, PI/2.0));
  	if (fabs(x) < 0.05 && y < -0.5 && y > -0.7 && diff1 < 0.2) {
  		//cout << "way1 OK" << endl;
		way1_visited++;
	}
	else if (x < -0.05 && y < -0.8) {
		//cout << "way2 OK" << endl;
		way2_visited++;
	}
	else if (x > 0.05 && y < -1.3 && diff2 < 0.2) {
		//cout << "way3 OK" << endl;
		way3_visited++;
	}
}

void RobotDevice::update_obstacle_info ()
{
	obstacle_left_front = distance_sensor_get_value (ps_[7]);
  	obstacle_right_front = distance_sensor_get_value (ps_[0]);
  	obstacle_left_mid = distance_sensor_get_value (ps_[6]);
  	obstacle_right_mid = distance_sensor_get_value (ps_[1]);
  	obstacle_left_ = distance_sensor_get_value (ps_[5]);
  	obstacle_right_ = distance_sensor_get_value (ps_[2]);
  	obstacle_left_back = distance_sensor_get_value (ps_[4]);
  	obstacle_right_back = distance_sensor_get_value (ps_[3]);
	for(int i=0;i<NB_DIST_SENS;i++) {
		int val = (int)distance_sensor_get_value(ps_[i]);
	  	ps_value[i] = ((val - PS_OFFSET_SIMULATION[i]) < 0)?0:(val - PS_OFFSET_SIMULATION[i]);
	}
}

bool RobotDevice::goal_reached () {
//	int retina_size = image_width * image_height;
//	// si l'image du but prend plus de 85% de la retine
//	// alors on est arrive !
//	if (num_pixels_goal_ > 0.85 * retina_size) {
//		return true;
//	}
//	return false;
	if (distance_goal_factor () < 0.07) {
		return true;
	}
	else {
		return false;	
	}
}

#define GOAL_X 0
#define GOAL_Y -1.68

double RobotDevice::distance_goal_factor () {
	// renvoie un facteur de distance qui pourrait par exemple
	// etre mesuree par l'odorat (facteur lineaire)
	double x = position_.x_get() - GOAL_X;
	double y = position_.y_get() - GOAL_Y;
	double dist = sqrt(x*x + y*y);
	return (dist > 1)?1:dist;
}


bool RobotDevice::all_ways_visited (bool reinit) {
	// indique si on a parcouru au moins une fois chaque bras
	if (reinit) {
		way1_visited = way2_visited = way3_visited = 0;
	}
	return (way1_visited == 2) && (way2_visited == 2) && (way3_visited == 2);
}
