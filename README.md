# Neobotix Problem Set (Multigoal Driver)

Author: Lilia Heinold

This node was written for the neobotix problem set 1.

Requirements:
* A running navigation stack (the action server is available and there is a '/map' frame)
* There is a robot listening to the /cmd_vel topic

This node was tested on ROS Melodic using the turtlebot3_fake node and the turtlebot3_navigation node.

The YAML file that contains the goals is called "goals.yaml" (can be changed in the launch file) and is in the base directory of the node (can be changed in the launch file).

The format is as follows:
goals: [{x: (double), y: (double), w: (double)}, ... ,{x: (double), y: (double), z: (double)}]

The file is read into the ros param neobotix_test/goals and is a list of structs containing x, y and w (x y position and w orientation - the w piece of the quaternion).

The goals are read in order and are sent when the past goal has been completed. The program listens to the move_base/status topic and will print out the progress of the current goal. The currrent goal is published as a PoseStamped on the topic /currentGoal.

Once all goals have been completed the program will print the total time taken by the robot to complete the path and exit. Note that on ROS Melodic there is a mutex error from actionlib when the program exits. This is a ROS/actionlib bug and can be ignored.

