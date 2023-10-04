#include "../include/botbot_common/SignalFilter.hpp"

#include <assert.h>
#include <cmath>
#include <numeric>

constexpr const double PI = 3.14159265359;

mms::SignalFilter::SignalFilter(const int filter_length, const double lower_fraction_limit,
                                const double upper_fraction_limit)
{
    assert(filter_length > 0 && filter_length < 500);
    assert(lower_fraction_limit >= 0.0 && lower_fraction_limit < upper_fraction_limit);
    assert(upper_fraction_limit <= 1.0);

    const double lower_rads_sample = lower_fraction_limit * PI;
    const double upper_rads_sample = upper_fraction_limit * PI;

    _input_buffer = boost::circular_buffer<double>(filter_length);
    _filter_coeffs.reserve(filter_length);

    // Design the filter
    for (int i = 0; i < filter_length; ++i)
    {
        _input_buffer.push_back(0);

        const double magnitude = i - (filter_length - 1.0) / 2.0;

        const double hamming_filter = 0.54 - 0.46 * std::cos(2 * PI * i / (filter_length - 1));
        double sinc_filter;

        if (magnitude == 0.0)
        {
            sinc_filter = (upper_rads_sample - lower_rads_sample) / PI;

        } else
        {
            sinc_filter =
                (std::sin(magnitude * upper_rads_sample) - std::sin(magnitude * lower_rads_sample)) / (magnitude * PI);
        }
        _filter_coeffs.push_back(sinc_filter * hamming_filter);
    }
}

[[nodiscard]] double mms::SignalFilter::do_sample(double input)
{
    _input_buffer.push_back(input);
    return std::inner_product(_filter_coeffs.begin(), _filter_coeffs.end(), _input_buffer.begin(), 0.0);

    // Only available from GCC-9 onwards (which is not installed on the xavier)
    // return std::transform_reduce(std::execution::par, _filter_coeffs.begin(), _filter_coeffs.end(),
    //                              _input_buffer.begin(), 0.0);
}

void mms::SignalFilter::add_sample(double input) { _input_buffer.push_back(input); }

void mms::SignalFilter::clear() { _input_buffer.insert(_input_buffer.begin(), _input_buffer.size(), 0); }
