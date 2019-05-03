#pragma once

//////////////////////////////////////////////////////////////////
// DO NOT EDIT THIS FILE										//
// This file will be replaced with a new version during marking //
//////////////////////////////////////////////////////////////////

#include <fstream>
#include <vector>
#include <chrono>

class ACW_Wrapper {
	std::ofstream _fout;
	int _currentTimer;
	std::chrono::steady_clock::time_point _startTime;
	std::vector<double> _ellapsedTimes;

public:
	explicit ACW_Wrapper(const char* const filename);
	~ACW_Wrapper();

	void startTimer();
	double stopTimer();
};

#ifdef _M_IX86
#ifdef _DEBUG
#pragma comment(lib,"ACW_WrapperD.lib")
#else
#pragma comment(lib,"ACW_Wrapper.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib,"ACW_Wrapperx64D.lib")
#else
#pragma comment(lib,"ACW_Wrapperx64.lib")
#endif
#endif