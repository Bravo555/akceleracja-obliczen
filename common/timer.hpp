#ifndef TIMER_H
#define TIMER_H
#include <chrono>

using namespace std::chrono;

class Timer
{
private:
    steady_clock::time_point startTime;

public:
    void start()
    {
        startTime = steady_clock::now();
    }

    duration<long long, std::nano> getElapsedTime()
    {
        auto now = steady_clock::now();
        return now - startTime;
    }
  
    long long getElapsedMicroseconds()
    {
        auto elapsedTime = getElapsedTime();
        return duration_cast<microseconds>(elapsedTime).count();
    }


};

#endif