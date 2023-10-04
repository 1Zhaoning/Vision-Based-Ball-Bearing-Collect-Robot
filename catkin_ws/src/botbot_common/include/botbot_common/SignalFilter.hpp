#pragma once

#include <boost/circular_buffer.hpp>
#include <vector>

namespace mms {

class SignalFilter
/**
    @brief Creates a low pass, high pass or band pass filter of the specified length.
    Uses a truncated sinc function to create the filter, and a hamming window
    filter to reduce artifacts.
    Refer to: https://au.mathworks.com/help/signal/ug/fir-filter-design.html?searchHighlight=firpm&s_tid=doc_srchtitle

    It makes use of the boost circular buffer: https://www.boost.org/doc/libs/1_72_0/doc/html/circular_buffer.html,
    because this is a fixed size container that will never need to allocate new memory when adding an input, or copy any
    of the current values.
*/
{
public:
    /**
        @param[in] lower_fraction_limit Lowest allowed frequency in radians per sample. (0 for LPF)
        @param[in] upper_fraction_limit Highest allowed frequency in radians per sample. (1.0 for HPF)

        @warning Large filter lengths can cause the output to not be as expected. Somewhere in the range 8-30 is a good
        length.
        @warning If the rate at which the filter receives inputs changes, then the effective cutoff frequency (in Hz)
        will also change. eg: If the computational load on the computer increases and rate of inputs halve, then the
        effective cutoff frequency will also halve.
    */
    SignalFilter(const int filter_length, const double lower_fraction_limit, const double upper_fraction_limit);

    /**
        @brief Adds an input sample/measurement and returns the output of the convolution of the filter and input
       samples
    */
    [[nodiscard]] double do_sample(double input);

    /**
        @brief Adds an input sample/measurement but does not do any filter calculations
    */
    void add_sample(double input);

    /**
        @brief Clears the filter's buffer.
    */
    void clear();

private:
    int _filter_length;
    std::vector<double> _filter_coeffs;
    boost::circular_buffer<double> _input_buffer;
};
} // namespace mms