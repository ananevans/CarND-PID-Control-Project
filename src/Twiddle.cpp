/*
 * Twiddle.cpp
 *
 *  Created on: May 23, 2019
 *      Author: nora
 */

#include "Twiddle.h"

#include <iostream>

Twiddle::Twiddle() {
	run = 0;
	index = 0;
	p.push_back(1.0);
	p.push_back(0.0);
	p.push_back(0.0);
	dp.push_back(1.0);
	dp.push_back(1.0);
	dp.push_back(1.0);
}

Twiddle::~Twiddle() {
	// TODO Auto-generated destructor stub
}

bool Twiddle::is_done() {
	return dp[0] + dp[1] + dp[2] <= tolerance;
}

void Twiddle::update( double error ) {
	if (run == 0) {
		if ( error < best_error ) {
//			cout << "First then taken\n";
			best_error = error;
			dp[index] *= 1.1;
			// setup for next run
			index = (index+1)%3;
			p[index] += dp[index];
		} else {
//			cout << "First else taken\n";
//			cout << "Decreasing p " << index;
			p[index] -= 2 * dp[index];
			run = 1;
		}
	} else {
		if ( error < best_error ) {
//			cout << "Second then taken\n";
			best_error = error;
			dp[index] *= 1.1;
		} else {
//			cout << "Second else taken\n";
			p[index] += dp[index];
			dp[index] *= 0.9;
		}
		run = 0;
		index = (index+1)%3;
		p[index] += dp[index];
	}
}

vector<double> Twiddle::get_p() {
	return p;
}

void Twiddle::debug_info() {
	cout << "p: " << p[0] << " " << p[1] << " " << p[2] << "\n";
	cout << "dp: " << dp[0] << " " << dp[1] << " " << dp[2] << "\n";
}
