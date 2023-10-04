#include <ostream>

#include <ros/ros.h>

#include "AlgorithmNode.hpp"



int main(int argc, char **argv)
{
    ros::init(argc, argv, "botbot_sim");

    auto my_node = AlgorithmNode();
    (void) my_node;
    
    ros::spin();
}