#include "Progress.h"

void Progress::start(uint64_t max)
{
    this->max = max;
    if (started != nullptr)
        started();
    if (changed != nullptr)
        changed(lastPercentage = 0.0);
}

void Progress::finish()
{
    if (changed != nullptr)
        changed(lastPercentage = 100.0);
    if (finished != nullptr)
        finished();
}

void Progress::set(int current)
{
    if (changed != nullptr)
    {
        if (max == 0)
            changed(0.0);
        else
        {
            double percentage = current * 100.0 / static_cast<double>(max);
            double delta = lastPercentage - percentage;
            if (delta > 0.1 || delta < -0.1)
            {
                changed(percentage);
                lastPercentage = percentage;
            }
        }
    }
}
