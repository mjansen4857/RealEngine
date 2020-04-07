#pragma once
#include "RealWin.h"
#include <vector>
#include <string>

class DxgiInfoManager {
public:
	DxgiInfoManager();
	~DxgiInfoManager();
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator = (const DxgiInfoManager&) = delete;
	void set() noexcept;
	std::vector<std::string> getMessages() const;
private:
	unsigned long long next = 0u;
	struct IDXGIInfoQueue* dxgiInfoQueue = nullptr;
};

