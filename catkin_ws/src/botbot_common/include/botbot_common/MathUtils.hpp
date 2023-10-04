#pragma once

#include <utility>
#include <cmath>

namespace mms {
namespace MathUtils {
constexpr double PI = 3.14159265359;

[[nodiscard]] double gaussian_noise(const double stddev, const double mean = 0.0);

[[nodiscard]] double uniform_random(const double min = 0.0, const double max = 1.0);

[[nodiscard]] double abs_angle_difference(double angle_1, double angle_2);

[[nodiscard]] constexpr double rad2deg(double rads) { return rads * 180 / PI; }

[[nodiscard]] constexpr double deg2rad(double degs) { return (degs / 180) * PI; }

[[nodiscard]] std::pair<double, double> unwrap_state_yaw(double yaw, double newYaw, int loopCount);

[[nodiscard]] int update_loop_count(double yaw, double newYaw);

/*
 * @brief Returns its arguement squared, really a helper function for variadic templates
 */
template <typename T>[[nodiscard]] T constexpr sqr(T v) { return v * v; }

/*
 * @brief Returns the sum of its arguements squared
 */
template <typename T, typename... Args>[[nodiscard]] constexpr T sqr(T first, Args... args)
{
    return first * first + sqr(args...);
}

/*
 * @brief Variadic template function for norm of N variables
 */
template <typename T, typename... Args>[[nodiscard]] constexpr T norm(T first, Args... args)
{
    return std::sqrt(sqr(first, args...));
}

/**
 * @brief Perpendicular distance between a line and a point.
 * @details num is twice the area of the triangle of the three points {num = 2*area = base*height}
 * dem is distance between (line_x0, line_y0) and (line_x1, line_y1) {dem = base}
 * return the negtive value to indicate direction of distance where negative is to the left on the line
 * and positive is to the right, when looking from point 0 to 1
 * {-num/dem = -2*area/base = -height}
 * @param [ref_x, ref_y] is the point to which the distance is being measured
 * @param [line_x0, line_y0] are the coordinates of a point the line passes through
 * @param [line_x1, line_y1] are the coordinates of a point the line passes through
 * 
 * @return perpendicular distance between line and point (ref_x, ref_y)
 */
template <typename T>[[nodiscard]] T constexpr distance_from_line_to_point(T line_x0, T line_y0, T line_x1, T line_y1, T ref_x, T ref_y) 
{
    T num = (line_y1 - line_y0) * ref_x - (line_x1 - line_x0) * ref_y + line_x1 * line_y0 - line_y1 * line_x0;
    T dem = mms::MathUtils::norm(line_y1 - line_y0, line_x1 - line_x0);
    return -num / dem;
}

} // namespace MathUtils
} // namespace mms