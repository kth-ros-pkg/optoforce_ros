#include <optoforce/optoforce_acquisition.hpp>

#include <iostream>

#include <ros/ros.h>
#include <geometry_msgs/WrenchStamped.h>

class optoforce_node {
  public:
    //! Constructor
    optoforce_node();

    //! Destructor
    ~optoforce_node();

    //! init
    int init();

    //! configure
    int configure();

    //! run
    int run();


  private:
    // finish node
    void finish();

    // ROS node handler
    ros::NodeHandle nh_;

    OptoforceAcquisition * force_acquisition_;

    // Wrench Publisher
    ros::Publisher wrench_pub_[2];
    std::vector<geometry_msgs::WrenchStamped> wrench_;

    // Frequency in which the program will read sensor data
    int acquisition_rate_;

    // Publish frequency
    int loop_rate_;

    // Number of devices connected
    int connectedDAQs_;

    // Senor's transmission frequency
    int transmission_speed_;

    // Sensor Filter
    int filter_;

    // File where data will be stored
    std::string filename_;

    // Samples to be stored
    int num_samples_;

    std::vector<std::string> ldevice_;
    std::vector<std::vector<float> > lcalib_;
    std::vector<int> lspeed_;

};
