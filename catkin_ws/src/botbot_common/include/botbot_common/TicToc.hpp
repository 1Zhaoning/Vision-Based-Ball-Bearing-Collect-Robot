#pragma once

/**
 @brief
    This is an easy-to-use performance timer, that supports nesting timers. eg:
        TIC() //begin timer A
            TIC() //begin timer B
            ---
            TOC("") //stop timer B
        TOC("") //stop timer A

        TIC_TOC_SUMMARY() //Print avg times for A and B

    Although runtime performance impact should be minimal, it can be completely
    disabled by not defining PROFILE or PROFILE_SUMMARY.
*/

#include <array>
#include <chrono>
#include <iomanip>
#include <iosfwd>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include <ros/console.h>

// Comment the below line to disable timing
// #define PROFILE // All individual times and summary + all times to ROS
#define PROFILE_SUMMARY // Summary only + all times to ROS

// Macro allows getting function and file names,
// and disabling timing with zero cost

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef PROFILE_SUMMARY
#define TIC(NAME)                                                                                                      \
    {                                                                                                                  \
        std::string my_tic_toc_name = mms::profiling::create_name(NAME, __FUNCTION__);                                 \
        static mms::profiling::tic_helper my_tic_helper(my_tic_toc_name);                                              \
        mms::profiling::tic(my_tic_helper.idx);                                                                        \
    }
#define TOC(NAME)                                                                                                      \
    {                                                                                                                  \
        std::string my_tic_toc_name = mms::profiling::create_name(NAME, __FUNCTION__);                                 \
        static mms::profiling::toc_helper my_toc_helper(my_tic_toc_name);                                              \
        mms::profiling::toc(my_toc_helper.idx);                                                                        \
    }
#define FPS_LOG(NAME)                                                                                                  \
    {                                                                                                                  \
        std::string my_tic_toc_name = mms::profiling::create_name(NAME, __FUNCTION__);                                 \
        static mms::profiling::tic_helper my_tic_helper(my_tic_toc_name);                                              \
        mms::profiling::fps_real(my_tic_helper.idx);                                                                   \
    }
#define LATENCY_LOG(NAME, TIME)                                                                                        \
    {                                                                                                                  \
        std::string my_tic_toc_name = mms::profiling::create_name(NAME, __FUNCTION__);                                 \
        static mms::profiling::tic_helper my_tic_helper(my_tic_toc_name);                                              \
        mms::profiling::log_latency(TIME, my_tic_helper.idx);                                                          \
    }
#define TIC_TOC_SUMMARY(NAME) mms::profiling::toc_summary(NAME);
#else
#define TIC(NAME)
#define TOC(NAME)
#define FPS_LOG(NAME)
#define LATENCY_LOG(NAME, TIME)
#define TIC_TOC_SUMMARY(NAME)
#endif

namespace mms {
namespace profiling {

/* A Brief word on std::map

    std::map is very similar to a python dictionary, except all keys and
    all values must be the same data type.

    However, if you do:
        my_map["my key"]
    and that key doesn't exist, it will simply add it to the map
    and set the value of it to 0. If you want bounds checking, use
        my_map.at("my key")

    Thus we can do:
        my_map["non-existent-key"] += A,
    and it will just set the value for that key to A!
    This is the weirdest c++ behaviour that I know of.
*/

// Defines how many minimum and maximum times to keep, which defines the
// Nth percentile range of the measurements: (which also depends on the total count).
static const uint NUM_QUANTILE = 100;

struct tic_helper {
    tic_helper(std::string name);
    size_t idx;
};

struct toc_helper {
    toc_helper(std::string name);
    size_t idx;
};
std::string create_name(const std::string &a, const std::string &b);

static std::vector<std::string> names;
static std::vector<std::chrono::time_point<std::chrono::steady_clock>> current_times;
static std::vector<double> total_time;
static std::vector<uint> total_count;
static std::vector<std::array<float, NUM_QUANTILE>> time_mins;
static std::vector<std::array<float, NUM_QUANTILE>> time_maxs;

static std::mutex mtx;

/**
 @brief Increments counter, adds to total time and adds time into
    min/max arrays of times.
*/
void add_measurement(size_t key, float millisec);

void log_latency(ros::Time time, size_t key);

void tic(size_t key);

void toc(size_t key);

void fps_real(size_t key);

void toc_summary(const std::string &name);

} // namespace profiling
} // namespace mms