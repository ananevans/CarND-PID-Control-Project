# CarND-Controls-PID
Self-Driving Car Engineer Nanodegree Program

---

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1(mac, linux), 3.81(Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `./install-mac.sh` or `./install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets 
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.
* Simulator. You can download these from the [project intro page](https://github.com/udacity/self-driving-car-sim/releases) in the classroom.

Fellow students have put together a guide to Windows set-up for the project [here](https://s3-us-west-1.amazonaws.com/udacity-selfdrivingcar/files/Kidnapped_Vehicle_Windows_Setup.pdf) if the environment you have set up for the Sensor Fusion projects does not work for this project. There's also an experimental patch for windows in this [PR](https://github.com/udacity/CarND-PID-Control-Project/pull/3).

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 

Tips for setting up your environment can be found [here](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/0949fca6-b379-42af-a919-ee50aa304e6a/lessons/f758c44c-5e40-4e01-93b5-1a82aa4e044f/concepts/23d376c7-0195-4276-bdf0-e02f1f3c665d)

## PID Controller

A PID controller is a control loop feedback for systems with a target output value, called a set point, and a way of measuring the error between the target value and the process variable. The set point is denoted by *SP* and the process variable by *y(t)*. The error is also a function of time, and it is denoted by *e(t)*.

The formula used to calculate the process variable is:

![PID COntroller Equation](http://latex.codecogs.com/gif.latex?y%28t%29%20%3D%20SP%20-%20Kp%20*%20e%28t%29%20-%20Ki%20*%20%5Cint%20e%28t%29%20dt%20-%20Kd%20*%20%5Cfrac%7Bde%28t%29%29%7D%7Bdt%7D)


The correction term has three compenents:

* *P*: proportional to the error term
* *I*: proportional to the integral of the error
* *D*: proportinal to the derivative of error

The *P* term is larger as the error gets larger. The correction of the *P* term can sometimes be a little to large, and the output will oscillate aound the set point *SP*. The *D* component reduces the amplitude of the oscilation. The *I* component corrects a bias in the error measurement.

## PID Controller for Self Driving Cars

The PID controller may be used to determine the steering and the throttle variables. The erorr is provided at each step by the simulator.

Assuming that the error and process variables are measures at fixed time intervals, we can use the following formula:

![Discrete Time Formula](http://latex.codecogs.com/gif.latex?s%20%3D%20-Kp%20*%20CTE_i%20-%20Ki%20%5Csum_%7Bk%7D%5E%7Bi%7D%20CTI_k%20-%20Kd%20%28CTE_i%20-%20CTE_%28i-1%29%29)

For steering, the set point is 0.0. 

For the throttle, I use the set point of 0.4 and for the error term the absolute value of the error provided by the simulator.

![Throttle PID](http://latex.codecogs.com/gif.latex?s%20%3D%200.4%20-Kp%20*%20%5Cleft%20%7C%20CTE_i%20%5Cright%20%7C%20-%20Ki%20%5Csum_%7Bk%7D%5E%7Bi%7D%20%5Cleft%20%7C%20CTI_k%20%5Cright%20%7C%20-%20Kd%20%28%5Cleft%20%7C%20CTE_i%20%5Cright%20%7C%20-%20%5Cleft%20%7C%20CTE_%28i-1%29%20%5Cright%20%7C%29)

The reason for using the absolute value of the eror is that the vehicle should slow down whether is left or right of center.

## The Process of Determining the Constant Values

I started by using a 0.1 value for throttle and 1, 0, 0 for the proportional, intergral and derivative constants. The car stayed on the road, but swerved a lot. With the values 1, 0, 0.5 the car has a reasonable trajectory. Next, I increased the throttle to 0.2 and the car started to swerve a lot more. I increased the derivative component step by step to 4, and the lowered the proportinal one to 0.4. Next, I increased the throttle to 0.4 and experimentally I determined an acceptable set of constants: 0.5, 0, 4.2. I tried to find a set of constants for throttle 0.4, but could not find one.

With the values 0.5, 0, 4.2 for the proportinal, integral and derivative constants for the steering PID controller, I determined the following set of contants for the PID controller for throttle: 0.3, 0, 3.

Finally, I fixed the values for the throttle controller and used the twiddle algorithm (implemented in Twiddle.cpp).
