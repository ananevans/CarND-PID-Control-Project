#include "PID.h"

#include <iostream>

/**
 *
 * TODO: Complete the PID class. You may add any additional desired functions.
 */

PID::PID() {
	reset_errors();
}

PID::~PID() {}

void PID::Init(double Kp_, double Ki_, double Kd_) {
  /**
   * TODO: Initialize PID coefficients (and errors, if needed)
   */
	this->Kp = Kp_;
	this->Ki = Ki_;
	this->Kd = Kd_;
}

void PID::UpdateError(double cte) {
  /**
   * Update PID errors based on cte.
   */
	d_error = cte - p_error;
	i_error = i_error + cte;
	p_error = cte;
}

double PID::TotalError() {
  /**
   * TODO: Calculate and return the total error
   */
  return - Kp * p_error - Kd * d_error - Ki * i_error;
}

void PID::debug_info() {
	std::cout << "kp " << Kp << " " << "ki " << Kd << " " << "ki " << Ki << " "
			<< "p_error " << p_error << " " << "i_error " << i_error << " "
			<< "d_error " << d_error << "\n";
}

void PID::reset_errors() {
	this->d_error = 0.0;
	this->p_error = 0.0;
	this->i_error = 0.0;
}
