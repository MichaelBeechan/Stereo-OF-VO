# OF-VO：Robust and Efficient Stereo Visual Odometry Using Points and Feature Optical Flow
# MyLibviso2
Containing a wrapper for libviso2, a visual odometry library. 

This is the main task of my postgraduate stage, code sharing, after the graduation thesis is submitted

The project about Optical flow and ORB and Libviso = visual odometry

## ABSTRACT

Stereo visual odometry is a critical component for mobile robot navigation and safety. It estimates the ego-motion using stereo images frame by frame. In this paper, we demonstrate an approach of calculating visual odometry for indoor or outdoor robots(UGV) equipped with a stereo rig. Differ from others visual odometry, we use an improved stereo-tracking method that combines information from optical flow and stereo to estimate and control the current position of Unmanned Ground Vehicle. For feature matching, we employ the circle matching strategy in VISO-2. A high-accuracy navigation system is used to evaluate our results on challenging real-world video sequences. The experiment result indicates our approach is more accurate than other visual odometry method in accuracy and run-time.

## Index Terms
Stereo vision, optical flow, circle matching, tracking


In the past few decades, the area of mobile robotics and autonomous systems has attracted substantial attention from researchers all over the world, resulting in major advances and breakthroughs. Currently, for applications which mobile robots are expected to perform complicated tasks that require navigation and localization in complex and dynamic indoor and outdoor environments without any human input. As a result, the localization problem has been studied in detail and various techniques have been proposed to solve the localization problem. The simplest form of localization is to use wheel odometry methods that rely upon wheel encoders to measure the amount of rotation of robots wheels. In those methods, wheel rotation measurements are incrementally used in conjunction with the robot’s motion model to find the robot’s current location with respect to a global reference coordinate system. The wheel odometry method has some major limitations. Firstly, it is limited to wheeled ground vehicles and secondly, since the localization is incremental (based on the previous estimated location), measurement errors are accumulated over time and cause the estimated robot pose to drift from its actual location. There are a number of error sources in wheel odometry methods, the most significant being wheel slippage in uneven terrain or slippery floors.
