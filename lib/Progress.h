#ifndef PROGRESS_H
#define PROGRESS_H
#include <functional>

class Progress
{
    public:
        std::function<void(double percentage)> changed;
        std::function<void()> started;
        std::function<void()> finished;

    public:
        void start(uint64_t max);
        void set(int step);
        void finish();

    private:
        double lastPercentage;
        uint64_t max;
};

#endif
