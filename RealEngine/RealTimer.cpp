#include "RealTimer.h"

using namespace std::chrono;

RealTimer::RealTimer() noexcept {
	last = steady_clock::now();
}

float RealTimer::mark() noexcept {
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float RealTimer::get() const noexcept {
	return duration<float>(steady_clock::now() - last).count();
}