#include "TimeMeasurer.hpp"

TimeMeasurer::TimeMeasurer()
    : beginPoint(std::chrono::time_point<std::chrono::high_resolution_clock>::min())
    , endPoint(std::chrono::time_point<std::chrono::high_resolution_clock>::min())
{

}

TimeMeasurer::~TimeMeasurer()
{

}

void TimeMeasurer::start()
{
    beginPoint = std::chrono::high_resolution_clock::now();
    return;
}

void TimeMeasurer::stop()
{
    endPoint = std::chrono::high_resolution_clock::now();
    return;
}

double TimeMeasurer::getDurationMsec() const
{
    const std::chrono::duration<double, std::milli> elapsed = endPoint - beginPoint;
    const double time = elapsed.count();
    return time;
}

double TimeMeasurer::getDurationSec() const
{
    static const double milliSecToSecDenominator = 1000;
    const double time = getDurationMsec();
    return time / milliSecToSecDenominator;
}