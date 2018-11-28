#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib_msgs/GoalStatusArray.h>
#include <string>
#include <iostream>
#include <geometry_msgs/PoseStamped.h>
#include <actionlib_msgs/GoalStatus.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

ros::Publisher goalPub;
std::string frame_id = "map";
std::vector<geometry_msgs::PoseStamped> allGoals;
actionlib_msgs::GoalStatus currentGoalStatus;
int goalCount = 0;

void statusCB(const actionlib_msgs::GoalStatusArray status){
	if(status.status_list.size() != 0){
		if(status.status_list[status.status_list.size() - 1].status == actionlib_msgs::GoalStatus::SUCCEEDED){
			currentGoalStatus = status.status_list[status.status_list.size() - 1];
			ROS_INFO("Completed goal %d", goalCount + 1);
			goalCount++;
		} else if(status.status_list[status.status_list.size() - 1].status == actionlib_msgs::GoalStatus::ACTIVE){
			currentGoalStatus = status.status_list[status.status_list.size() - 1];
			ROS_INFO("Completing goal %d", goalCount + 1);
		}
	}
}

int main(int argc, char** argv){
	ros::init(argc, argv, "move_turtle");
	ros::NodeHandle nh;
	XmlRpc::XmlRpcValue list;
	MoveBaseClient ac("move_base", true);

	nh.getParam("/neobotix_test/goals", list);
	ros::Subscriber statusSub = nh.subscribe("/move_base/status", 1000, statusCB);
	goalPub = nh.advertise<geometry_msgs::PoseStamped>("/currentGoal", 1000);

	while(!ac.waitForServer(ros::Duration(5.0))){
		ROS_INFO("Waiting for action server to come up");
	}

	ros::Time begin = ros::Time::now();
	for(int i = 0; i < list.size(); i++){
		XmlRpc::XmlRpcValue goal = list[i];
		geometry_msgs::PoseStamped psGoal;
		psGoal.header.frame_id = frame_id;
		psGoal.header.stamp = ros::Time::now();
		psGoal.pose.position.x = double(goal["x"]);
		psGoal.pose.position.y = double(goal["y"]);
		psGoal.pose.orientation.w = double(goal["w"]);
		allGoals.push_back(psGoal);
		currentGoalStatus.status = actionlib_msgs::GoalStatus::SUCCEEDED;
	}
	
	bool done = false;
	while(ros::ok() && !done){
		if(goalCount == list.size()){
			ros::Time end = ros::Time::now();
			ros::Duration t = end - begin;
			ROS_INFO("Total time was %0.2f seconds", t.toSec());
			done = true;
		} else if(currentGoalStatus.status == actionlib_msgs::GoalStatus::SUCCEEDED){
			currentGoalStatus.status = actionlib_msgs::GoalStatus::ACTIVE;
			move_base_msgs::MoveBaseGoal mbGoal;
			mbGoal.target_pose = allGoals[goalCount];
			goalPub.publish(allGoals[goalCount]);
			ac.sendGoal(mbGoal);
			ROS_INFO("Sending goal %d", goalCount + 1);
		}

		ros::spinOnce();
	}

	ROS_INFO("Done with goals");

	return 0;
}
