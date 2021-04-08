#pragma once
#include <chrono>

class TimeMeasurer final
{
public:
    explicit TimeMeasurer();
    ~TimeMeasurer();
    TimeMeasurer(const TimeMeasurer&) = delete;
    TimeMeasurer& operator=(const TimeMeasurer&) = delete;

    void start();
    void stop();

    double getDurationMsec() const;
    double getDurationSec() const;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> beginPoint;
    std::chrono::time_point<std::chrono::high_resolution_clock> endPoint;
};

