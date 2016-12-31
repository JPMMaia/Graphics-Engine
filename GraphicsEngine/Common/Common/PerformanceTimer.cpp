#include "PerformanceTimer.h"

using namespace Common;
using namespace std;
using namespace std::chrono;

void PerformanceTimer::Start()
{
	m_start = high_resolution_clock::now();
}

void PerformanceTimer::End()
{
	m_end = high_resolution_clock::now();
}