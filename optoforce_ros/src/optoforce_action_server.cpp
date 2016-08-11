/********************************************
 * This program adds an interface through topics to:
 *   enable publishing topics
 *   enable store data to file when accquisition finish
 *
 * The program inherits optoforce_node, which:
 *   reads parameters from ROS parameter server
 *   initialize  optoforce driver
 *   get periodically wrench data
 *   if configured can:
 *     publish wrench data
 *     store   wrench data
 */
#include "optoforce_action_server.h"

/*
optoforce_action_server::optoforce_action_server(std::string name) :
  as_(nh_, name, boost::bind(&optoforce_action_server::executeCB, this, _1),false),
  action_name_(name)
{
  as_.start();
}
*/
optoforce_action_server::optoforce_action_server(std::string name)
{
  as_name_ = name;
}
void optoforce_action_server::add_ros_interface()
{
  //as_ = new ActionServer(nh_, as_name_, boost::bind(&optoforce_action_server::executeCB, this, _1),false);
  as_ = new ActionServer(nh_, as_name_, boost::bind(&optoforce_action_server::run, this, _1),false);
  as_->start();
  ROS_INFO("[add_ros_interface] actinlib started");
}
optoforce_action_server::~optoforce_action_server()
{

}
void optoforce_action_server::run(const ActionServer::GoalConstPtr& goal)
{

  ROS_INFO("[optoforce_action_server::run] Enter runCB");

  int it = 0;

  ros::Rate timer(goal->publish_freq); // 1Hz timer
  geometry_msgs::WrenchStamped wrench;
  std::vector< std::vector<float> > latest_samples;

  ros::Time time_actual = ros::Time::now();

  while (it < goal->acq_duration)
  {
      std::cout << "time: "<< ros::Time::now();
      ROS_INFO("[optoforce_action_server::run] while in runCB");
      if (as_->isPreemptRequested()){
         ROS_WARN("goal cancelled!");
         result_.result = 0;
         as_->setAborted(result_); // tell the client we have given up on this goal; send the result message as well
         return; // done with callback
      }

      latest_samples.clear();
      force_acquisition_->getData(latest_samples);
      feedback_.wrench_lst.clear();

      if (connectedDAQs_ > 0 )
      {
        wrench.header.stamp = ros::Time::now();
        wrench.wrench.force.x  = latest_samples[0][0];
        wrench.wrench.force.y  = latest_samples[0][1];
        wrench.wrench.force.z  = latest_samples[0][2];
        wrench.wrench.torque.x = latest_samples[0][3];
        wrench.wrench.torque.y = latest_samples[0][4];
        wrench.wrench.torque.z = latest_samples[0][5];

        feedback_.wrench_lst.push_back(wrench);

      }
      if (connectedDAQs_ == 2)
      {
        wrench.header.stamp = ros::Time::now();
        wrench.wrench.force.x  = latest_samples[1][0];
        wrench.wrench.force.y  = latest_samples[1][1];
        wrench.wrench.force.z  = latest_samples[1][2];
        wrench.wrench.torque.x = latest_samples[1][3];
        wrench.wrench.torque.y = latest_samples[1][4];
        wrench.wrench.torque.z = latest_samples[1][5];

        feedback_.wrench_lst.push_back(wrench);

      }

      as_->publishFeedback(feedback_); // send feedback to the action client that requested this goal

      it++;
      timer.sleep();
    }
}

void optoforce_action_server::executeCB(const actionlib::SimpleActionServer<optoforce_ros::OptoForceAction>::GoalConstPtr& goal)
{
  /*
  ROS_INFO("[optoforce_action_server::executeCB] Enter executeCB");

  int it = 0;

  ros::Rate timer(goal->freq); // 1Hz timer
  geometry_msgs::WrenchStamped wrench;
  std::vector< std::vector<float> > latest_samples;

  while (it < goal->duration)
  {
    //ROS_INFO("[optoforce_action_server::executeCB] Running executeCB");
    if (as_->isPreemptRequested()){
       ROS_WARN("goal cancelled!");
       result_.result = 0;
       as_->setAborted(result_); // tell the client we have given up on this goal; send the result message as well
       return; // done with callback
    }
    std::vector<float> data;
    data.push_back(0.2);
    data.push_back(0.3);

    latest_samples.clear();
    force_acquisition_->getData(latest_samples);

    //ROS_INFO_STREAM("force: " << wrench_[0].wrench.force.z);

    feedback_.wrench = data;

    as_->publishFeedback(feedback_); // send feedback to the action client that requested this goal

    it++;
    timer.sleep();
  }
*/

}

// Inherited virtual method from optoforce_node
// Start transmision trough topics, only enable the flag
void optoforce_action_server::transmitStart()
{
  puplish_enable_ = true;
}

// Inherited virtual method from optoforce_node
// Stop transmision trough topics, only enable the flag
void optoforce_action_server::transmitStop()
{
  puplish_enable_ = false;
}




int main(int argc, char* argv[])
{

  ros::init(argc, argv, "optoforce_action_server");
  ROS_INFO_STREAM("[optoforce_action_server] Node name is:" << ros::this_node::getName());

  std::string action_name = "action_server";
  optoforce_action_server optoforce_as(action_name);

  if (optoforce_as.init() < 0)
  {
    std::cout << "optoforce_action_server could not be initialized" << std::endl;
  }
  else
  {
    std::cout << "optoforce_action_server Correctly initialized" << std::endl;

    // Add Actionlib ROS Interface
    optoforce_as.add_ros_interface();

    // Execute loop
    //optoforce_as.run();
  }
  std::cout << "exit main" << std::endl;

  return 1;

}
