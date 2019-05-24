/*
 * Twiddle.h
 *
 *  Created on: May 23, 2019
 *      Author: nora
 */

#ifndef SRC_TWIDDLE_H_
#define SRC_TWIDDLE_H_

#include <vector>

using namespace std;

class Twiddle {
public:
	Twiddle();
	virtual ~Twiddle();

	bool is_done();

	void update( double error );

	vector<double> get_p();

	void debug_info();

private:
	double tolerance = 0.002;
	vector<double> p;
	vector<double> dp;
	int index;
	int run;
	double best_error = 1000000.0;
};

#endif /* SRC_TWIDDLE_H_ */
