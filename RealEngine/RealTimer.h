#pragma once
#include <chrono>

class RealTimer {
public:
	RealTimer() noexcept;
	float mark() noexcept;
	float get() const noexcept;
private:
	std::chrono::steady_clock::time_point last;
};

