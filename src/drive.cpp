
#include "ros/ros.h"
#include <chrono>

#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"

#include "sensor_msgs/LaserScan.h"


uint64_t getMilliseconds(){
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

ros::Publisher *pub;

sensor_msgs::LaserScan ld;
geometry_msgs::Twist dv;

void desHandler(const geometry_msgs::Twist& m){
  dv = m;
}

void laserHandler(const sensor_msgs::LaserScan& m){
  ld = m;
}

int main(int argc, char **argv){

  ros::init(argc, argv, "lab_4_robot");

  ros::NodeHandle n;

  ros::Publisher cmd_vel = n.advertise<geometry_msgs::Twist>("cmd_vel", 10);

  ros::Subscriber des_vel_sub = n.subscribe("des_vel", 1000, desHandler);
  ros::Subscriber laser_sub   = n.subscribe("laser_1", 10, laserHandler);

  pub = &cmd_vel;

  ros::Rate loop_rate(100);

  uint64_t hold_until = getMilliseconds();
  int count = 0;
  while (ros::ok()){
    ros::spinOnce();

    if (hold_until > getMilliseconds()){
      loop_rate.sleep();
      ++count;
      continue;
    }
    geometry_msgs::Twist message = dv;

    int m_ind = 134;
    int n_ind = 135;

    //If the robot isn't moving we don't care.
    message.angular.z = std::min(message.angular.z, 1.4);
    message.linear.x = std::max(message.linear.x, 0.0);

    if (message.linear.x < 0.02){
      pub->publish(message);
      continue;
    }

    //If the robot can continue straight right now we don't care.
    if (ld.ranges[m_ind - 20] > .5 && ld.ranges[n_ind + 20] > .5){
      pub->publish(message);
      continue;
    }

    int alg_dir = 0;
    float alg_ang = 0;
    float alg_dist = 0;

    float min_ang = 0;
    float min_dist = 1000;

    for (int i = 20; i<=134;i++){

      ///Check left side lidar at this angle from center.
      float ldist = std::min((float) ld.ranges[m_ind - i], (float) 4.0);
      if (ldist > alg_dist){
        alg_dir = -1;
        alg_ang = 0.017518 * i;
        alg_dist = ldist;
      }

      if (ldist < min_dist){
        min_dist = ldist;
        min_ang = 0.017518 * i;
      }

      //Check right side lidar at this angle from center.
      float rdist = std::min((float) ld.ranges[n_ind + i], (float) 4.0);
      if (rdist > alg_dist){
        alg_dir = 1;
        alg_ang = 0.017518 * i;
        alg_dist = rdist;
      }

      if (rdist < min_dist){
        min_dist = rdist;
        min_ang = 0.017518 * i;
      }
    }

    //If the lidar is completely engulfed... turn right 0.2 rads.
    if (alg_dir == 0){
      alg_dir = 1;
      alg_ang = 0.2;
    }

    ROS_INFO("Algorithm output: [Direction & Distance: %s]", alg_dir == -1 ? "RIGHT" : "LEFT");
    //ROS_INFO("Algorithm output: [Linear Intensity: %f]", 1 - intensity);

    message.linear.x = 0;
    message.angular.z = (float) alg_dir;
    uint64_t now = getMilliseconds();
    ROS_INFO("MIN DIST :%f, MIN ANG: %f.", min_dist, min_ang);
    ROS_INFO("ALG ANGLE:%f, ALG SECONDS: %ld. ITS %ld - HOLDING UNTIL %ld.  ", alg_ang, (uint64_t) (alg_ang * 1000), now, now + (uint64_t) (alg_ang * 1000));
    //If there's a wall to the side of our bot.
    if (min_dist < 0.5 && min_ang > 0.4){
      ROS_INFO("SPECIAL HIT");
      alg_ang += 0.8;
    }
    hold_until = now + (uint64_t) (alg_ang * 1000);

    pub->publish(message);

    loop_rate.sleep();
    ++count;
  }


  return 0;
}
