#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ROS_INFO_STREAM("Sending motor commands");

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv)) {
        ROS_ERROR("Failed to call service ball_chaser/command_robot");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    bool chasing = false;

    for (int i = 0; i < img.height * img.step; i++) {
	    if (img.data[i] == white_pixel) {
		    // cause camera have 3 colors RGB
		    chasing = true;
		    int col = i / 3 % img.width;
		    if (col >= 0 && col <= 200) {
			// turn left
			drive_robot(0.0, 0.5);
		    }else if(col > 200 && col < 600) {
			// move forward
			drive_robot(0.5, 0.0);
		    } else if (col >= 600 && col <= 800){
			// turn right
			drive_robot(0.0, -0.5);
		    }
		    break;
	    } 
    }
    if(!chasing) {
	    // stop robot
	    drive_robot(0.0, 0.0);
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
