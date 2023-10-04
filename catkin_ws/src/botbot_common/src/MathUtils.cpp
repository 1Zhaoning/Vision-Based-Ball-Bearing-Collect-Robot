#include <random>

#include "../include/botbot_common/MathUtils.hpp"

constexpr double PI = 3.14159265359;

[[nodiscard]] double mms::MathUtils::gaussian_noise(const double stddev, const double mean)
{
    static std::default_random_engine generator;
    std::normal_distribution<double> distribution(mean, stddev);
    return distribution(generator);
}

[[nodiscard]] double mms::MathUtils::uniform_random(const double min, const double max)
{
    static std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

[[nodiscard]] double mms::MathUtils::abs_angle_difference(double angle_1, double angle_2)
{
    double unwrapped_difference = std::abs(angle_1 - angle_2);
    return std::min(unwrapped_difference, 2 * PI - unwrapped_difference);
}

[[nodiscard]] std::pair<double, double> mms::MathUtils::unwrap_state_yaw(double yaw, double newYaw, int loopCount)
{
    double tempYaw = yaw - 2 * PI * loopCount;
    loopCount += mms::MathUtils::update_loop_count(tempYaw, newYaw);

    return std::make_pair(loopCount, newYaw + 2 * PI * loopCount);
}

[[nodiscard]] int mms::MathUtils::update_loop_count(double yaw, double newYaw)
{
    if ((yaw < PI) && (yaw > PI / 2) && (newYaw > -PI) && (newYaw < -PI / 2))
    { // If yaw was is 2nd quadrant and has moved into the 3rd quadrant
        return 1;
    }
    if ((yaw > -PI) && (yaw < -PI / 2) && (newYaw < PI) && (newYaw > PI / 2))
    { // If yaw was in 3rd quadrant and has moved into the 2nd quadrant
        return -1;
    }
    return 0;
}
