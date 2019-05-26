#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"
#include "Twiddle.h"

// for convenience
using nlohmann::json;
using std::string;

#define  MAX_COUNT 600
#define INITIAL_STEERING_P 0.7641
#define INITIAL_STEERING_I 0
#define INITIAL_STEERING_D 4.4
// Throttle 0.4
//#define INITIAL_STEERING_P 0.4
//#define INITIAL_STEERING_I 0
//#define INITIAL_STEERING_D 4.5
#define STEERING_DELTA 0.1

#define INITIAL_THROTTLE_P 0.3
#define INITIAL_THROTTLE_I 0
#define INITIAL_THROTTLE_D 3
#define THROTTLE_DELTA 0.1


// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
	auto found_null = s.find("null");
	auto b1 = s.find_first_of("[");
	auto b2 = s.find_last_of("]");
	if (found_null != string::npos) {
		return "";
	}
	else if (b1 != string::npos && b2 != string::npos) {
		return s.substr(b1, b2 - b1 + 1);
	}
	return "";
}

int main() {
	uWS::Hub h;
	PID steering_pid(0.0);
	PID throttle_pid(0.4);
	bool run_twiddle_steering = false;
	bool run_twiddle_throttle = false;
	int count = 0;
	double error = 0;

	/**
	 * TODO: Initialize the pid variable.
	 */
	Twiddle twiddle_steering(INITIAL_STEERING_P, INITIAL_STEERING_I, INITIAL_STEERING_D, STEERING_DELTA);
	vector<double> p_s = twiddle_steering.get_p();
	steering_pid.Init(p_s[0], p_s[1], p_s[2]);

	Twiddle twiddle_throttle(INITIAL_THROTTLE_P, INITIAL_THROTTLE_I, INITIAL_THROTTLE_D, THROTTLE_DELTA);
	vector<double> p_t = twiddle_throttle.get_p();
	throttle_pid.Init(p_t[0], p_t[1], p_t[2]);

	twiddle_throttle.debug_info();

	h.onMessage([&steering_pid, &throttle_pid,
				 &twiddle_throttle, &twiddle_steering,
				 &run_twiddle_throttle, &run_twiddle_steering,
				 &count, &error]
				 (uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length,
						 uWS::OpCode opCode) {
		// "42" at the start of the message means there's a websocket message event.
		// The 4 signifies a websocket message
		// The 2 signifies a websocket event
		if (length && length > 2 && data[0] == '4' && data[1] == '2') {
			auto s = hasData(string(data).substr(0, length));

			if (s != "") {
				auto j = json::parse(s);

				string event = j[0].get<string>();

				if (event == "telemetry") {
					// j[1] is the data JSON object
					double cte = std::stod(j[1]["cte"].get<string>());
					double speed = std::stod(j[1]["speed"].get<string>());
					double angle = std::stod(j[1]["steering_angle"].get<string>());

					/**
					 * TODO: Calculate steering value here, remember the steering value is
					 *   [-1, 1].
					 * NOTE: Feel free to play around with the throttle and speed.
					 *   Maybe use another PID controller to control the speed!
					 */
					steering_pid.UpdateError(cte);
					throttle_pid.UpdateError(abs(cte));

					if (run_twiddle_steering || run_twiddle_throttle) {
						if (count < MAX_COUNT) {
							error += cte * cte;
							count += 1;
						} else {
							if (run_twiddle_steering) {
								cout << "Error: " << error/MAX_COUNT; twiddle_steering.debug_info();
								twiddle_steering.update( error/MAX_COUNT );
							}
							if (run_twiddle_throttle){
								cout << "Error: " << error/MAX_COUNT; twiddle_throttle.debug_info();
								twiddle_throttle.update( error/MAX_COUNT );
							}
							if (error/MAX_COUNT < 0.01) {
								run_twiddle_steering = false;
								run_twiddle_throttle = false;
							}
							count = 0;
							error = 0.0;
							if (run_twiddle_steering) {
								vector<double> p = twiddle_steering.get_p();
								steering_pid.Init(p[0], p[1], p[2]);
								steering_pid.reset_errors();
							}
							if (run_twiddle_throttle){
								vector<double> p = twiddle_throttle.get_p();
								throttle_pid.Init(p[0], p[1], p[2]);
								throttle_pid.reset_errors();
							}
							// reset
							string msg = "42[\"reset\",{}]";
							ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
						}
					}

					double steer_value = steering_pid.TotalError();
					double throttle_value = throttle_pid.TotalError();
					//double throttle_value = 0.4;

					cout << "Steering PID: "; steering_pid.debug_info();
					cout << "Throttle PID: "; throttle_pid.debug_info();

					// DEBUG
					std::cout << "CTE: " << cte << " Steering Value: " << steer_value
							<< " Throttle Value: " << throttle_value
							<< std::endl;

					json msgJson;
					msgJson["steering_angle"] = steer_value;
					msgJson["throttle"] = throttle_value;
					auto msg = "42[\"steer\"," + msgJson.dump() + "]";
					std::cout << msg << std::endl;
					ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
				}  // end "telemetry" if
			} else {
				// Manual driving
				string msg = "42[\"manual\",{}]";
				ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
			}
		}  // end websocket message if
	}); // end h.onMessage

	h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
		std::cout << "Connected!!!" << std::endl;
	});

	h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code,
			char *message, size_t length) {
		ws.close();
		std::cout << "Disconnected" << std::endl;
	});

	int port = 4567;
	if (h.listen(port)) {
		std::cout << "Listening to port " << port << std::endl;
	} else {
		std::cerr << "Failed to listen to port" << std::endl;
		return -1;
	}

	h.run();
}
