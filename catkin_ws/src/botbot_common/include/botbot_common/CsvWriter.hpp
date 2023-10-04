#pragma once

#include "ThreadPool.hpp"

#include <fstream>

namespace mms {
template <typename... Args> class CsvWriter;

namespace _csvWriterHelpers {
template <typename T> std::ostream &operator<<(std::ostream &os, const std::optional<T> &opt)
// Write nothing out for empty optionals
{
    if (opt) { return os << *opt; }
    return os;
}

// Magic to turn <int, 3> into <int, int, int>
template <typename Expander, std::size_t Index> using ExpandHelper = Expander;
template <typename T, std::size_t N, typename Indices = std::make_index_sequence<N>> struct CsvVectorWriterHelper;
template <typename T, std::size_t N, std::size_t... Indices>
struct CsvVectorWriterHelper<T, N, std::index_sequence<Indices...>> {
    using type = CsvWriter<ExpandHelper<T, Indices>...>;
};

// Magic to check all types of parameter pack are same
namespace Detail {
enum class enabler {};
}
template <bool Condition> using EnableIf = typename std::enable_if<Condition, Detail::enabler>::type;
template <typename... Conds> struct And : std::true_type {
};
template <typename Cond, typename... Conds> struct And<Cond, Conds...>
    : std::conditional<Cond::value, And<Conds...>, std::false_type>::type {
};
template <typename Target, typename... Ts> using are_all_T = And<std::is_same<Ts, Target>...>;

// Magic to get type of first arg in parameter pack
template <typename ...Args> using FirstArgType = std::tuple_element_t<0, std::tuple<Args...>>;
} // namespace _csvWriterHelpers

template <typename T, size_t N> using CsvVectorWriter = typename _csvWriterHelpers::CsvVectorWriterHelper<T, N>::type;

template <typename... Args> class CsvWriter
{
public:
    /**
     * @brief A low latency csv writing class that pushes io and formatting to another thread. Note this will leave a
     * trailing "," for every row. Args is the type of each of the columns. If you continuously try to write log rows
     * faster than the disk can write you will cause a memory leak.
     *
     * @param colnames Names of each of the columns
     * @param fname_format A csv filename that can include data/time formatters as per:
     * https://en.cppreference.com/w/cpp/chrono/c/strftime
     */
    explicit CsvWriter(const std::string (&colnames)[sizeof...(Args)],
                       const std::string &fname_format = "%Y.%m.%d_%H.%M.csv")
    {
        std::time_t t = std::time(nullptr);
        char fname[100];
        std::strftime(fname, sizeof(fname), fname_format.c_str(), std::localtime(&t));

        _file.open(fname, std::ios::out);
        if (!_file.is_open()) { throw std::logic_error("Unable to open file: " + std::string{fname}); }
        for (const auto &name : colnames)
        {
            if (name.empty()) { throw std::logic_error("You did not specify enough column names"); }
            _file << name << ",";
        }
        _file << "\n";
    }

    ~CsvWriter()
    {
        _pool.wait_for_completion();
        _file.close();
    }

    /**
     * @param args One row of data points to log
     */
    void log_row(Args... args) { _pool.add_job(&CsvWriter::log_row_async, this, std::forward<Args>(args)...); }

    using FirstArgT = _csvWriterHelpers::FirstArgType<Args...>;
    /**
     * @param container A std::vector to log
     */
    void log_vector_row(std::vector<FirstArgT> &&container)
    {
        static_assert(_csvWriterHelpers::are_all_T<FirstArgT, Args...>::value, "You can only use log_vector_row when all types are the same");
        assert(container.size() == sizeof...(Args) && "Wrong number of elems for row");
        _pool.add_job(&CsvWriter::log_vector_row_async, this, std::forward<std::vector<FirstArgT>>(container));
    }

private:
    void log_row_async(Args... args)
    {
        using namespace _csvWriterHelpers; // For operator << of optional
        ((_file << std::forward<Args>(args) << ","), ...);
        _file << "\n";
        maybe_flush();
    }

    void log_vector_row_async(const std::vector<FirstArgT> &container)
    {
        using namespace _csvWriterHelpers; // For operator << of optional
        for (const auto item : container)
        {
            _file << item << ", ";
        }
        _file << "\n";
        maybe_flush();
    }

    void maybe_flush()
    {
        if (++_line_count == _flush_interval)
        {
            _file.flush();
            _line_count = 0;
        }
    }

    std::ofstream _file;
    mms::ThreadPool _pool;
    const size_t _flush_interval = 10;
    size_t _line_count           = 0;
};
} // namespace mms
