#include "../include/botbot_common/TicToc.hpp"

#include <algorithm>
#include <iostream>
using namespace mms;

profiling::tic_helper::tic_helper(std::string name)
{
    std::unique_lock<std::mutex> guard{profiling::mtx};
    idx = profiling::names.size();
    names.emplace_back(name);
    current_times.emplace_back();
    total_time.emplace_back(0);
    total_count.emplace_back(0);
    time_mins.emplace_back();
    time_maxs.emplace_back();
}

profiling::toc_helper::toc_helper(std::string name)
{
    std::unique_lock<std::mutex> guard{profiling::mtx};
    auto iter = std::find(profiling::names.begin(), profiling::names.end(), name);
    if (iter == profiling::names.end()) { throw std::logic_error("The TICTOC name: " + name + " was not found"); }
    idx = iter - profiling::names.begin();
}

std::string profiling::create_name(const std::string &a, const std::string &b) { return a + "--" + b; }

void profiling::add_measurement(size_t key, float millisec)
{
    if (profiling::total_count[key] < profiling::NUM_QUANTILE)
    {
        time_mins[key][profiling::total_count[key]] = millisec;
        time_maxs[key][profiling::total_count[key]] = millisec;
    } else
    {
        float *max_elem_ptr = std::max_element(time_mins[key].begin(), time_mins[key].end());
        float max_elem      = *max_elem_ptr;
        if (millisec < max_elem) { *max_elem_ptr = millisec; }

        float *min_elem_ptr = std::min_element(time_maxs[key].begin(), time_maxs[key].end());
        float min_elem      = *min_elem_ptr;
        if (millisec > min_elem) { *min_elem_ptr = millisec; }
    }

    profiling::total_time[key] += millisec;
    profiling::total_count[key] += 1;
}

void profiling::tic(size_t key) { profiling::current_times[key] = std::chrono::steady_clock::now(); }

void profiling::toc(size_t key)
{
    const auto time_end = std::chrono::steady_clock::now();

    std::chrono::time_point<std::chrono::steady_clock> time_begin = profiling::current_times.at(key);

    const auto time_delta = time_end - time_begin;

    float time_elapsed = 1000 * std::chrono::duration_cast<std::chrono::duration<float>>(time_delta).count();

    std::ostringstream msg;
    msg << std::setw(20) << profiling::names[key] << ": " << std::fixed << std::setw(10) << std::setprecision(3)
        << time_elapsed << " ms elapsed";
    ROS_DEBUG_STREAM_THROTTLE(0.1, msg.str());

    profiling::add_measurement(key, time_elapsed);
}

void profiling::log_latency(ros::Time time, size_t key)
{
    float time_elapsed = 1000.0f * static_cast<float>((ros::Time::now() - time).toSec());

    std::ostringstream msg;
    msg << std::setw(20) << profiling::names[key] << ": " << std::fixed << std::setw(10) << std::setprecision(3)
        << time_elapsed << " ms latency";
    ROS_DEBUG_STREAM_THROTTLE(0.1, msg.str());

    profiling::add_measurement(key, time_elapsed);
}

void profiling::fps_real(size_t key)
{
    const auto time_end = std::chrono::steady_clock::now();

    if (profiling::current_times[key] == std::chrono::time_point<std::chrono::steady_clock>{})
    {
        profiling::current_times[key] = time_end;
        return;
    }
    std::chrono::time_point<std::chrono::steady_clock> time_begin = profiling::current_times.at(key);

    profiling::current_times[key] = time_end;

    const auto time_delta = time_end - time_begin;

    float time_elapsed = 1000 * std::chrono::duration_cast<std::chrono::duration<float>>(time_delta).count();

    std::ostringstream msg;
    msg << std::setw(20) << profiling::names[key] << ": " << std::fixed << std::setw(10) << std::setprecision(3)
        << time_elapsed << " ms (1/Hz)";
    ROS_DEBUG_STREAM_THROTTLE(0.1, msg.str());

    profiling::add_measurement(key, time_elapsed);
}

void profiling::toc_summary(const std::string &name = "")
{
    std::cout << "\n---------------------------" << name << " TIMING SUMMARY---------------------------\n";

    for (size_t key = 0; key < profiling::names.size(); ++key)
    {
        const double avg_time = profiling::total_time[key] / profiling::total_count[key];
        const float max_time =
            *std::min_element(profiling::time_maxs.at(key).begin(), profiling::time_maxs.at(key).end());
        const float min_time =
            *std::max_element(profiling::time_mins.at(key).begin(), profiling::time_mins.at(key).end());

        const float percentile =
            (1.0f - profiling::NUM_QUANTILE / static_cast<float>(profiling::total_count[key])) * 100.0f;

        std::ostringstream msg;
        msg << std::setw(50) << profiling::names[key] << ": " << std::fixed << std::setw(10) << std::setprecision(2)
            << avg_time << " ms avg, "
            << "(" << min_time << ", " << max_time << ") " << std::setprecision(1) << percentile << "th percentile\n";

        std::cout << msg.str() << "\n";

        ROS_INFO_STREAM(name + "|" + msg.str());
    }
    std::cout << "\n-------------------------------------------------------------------------\n";
}
