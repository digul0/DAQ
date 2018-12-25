#ifndef M_LOG_H
#define M_LOG_H

#include <iomanip>
#include <sstream>

#include "common_std_headers.h"

template<typename T>
class Mutex_selector;

/**
* @brief
* Provides multi-threaded console and file output.
* @warning Do not use endl, ends. Instead of ones use '\n'
*/
class m_log
{

public:
    enum class other { to_setted_log_file };

    // for stdout
    explicit m_log(std::ostream& out_dest = std::cout );

    // for file
    explicit m_log(std::ofstream& out_file );

    //for setted file
    explicit m_log(m_log::other);

    m_log(m_log&& rlog) noexcept;

    m_log(const m_log&)            = delete;

    m_log& operator=(const m_log&) = delete;

    //output perform in destructor.
    ~m_log();

    void static setLogfile(std::ofstream* out_file);
private:
    template<typename T>
    void write_to(T&);

    template< typename T>
    friend m_log&&  operator<<(m_log&&  m_log_, T&& t) noexcept;

    std::ostream* out_ {nullptr};
    std::stringstream ss_;
    inline static std::ofstream* p_setted_file_ {nullptr};
public:
    static const std::string
    make_name_from_time (const std::string name_prefix,
                         std::chrono::system_clock::time_point tp,
                         const std::string name_postfix);
};

//choose relevant static mutex
template<typename T>
class Mutex_selector
{
public:
    static std::mutex& get();
private:
    inline static std::mutex mut_console_,
           mut_file_,
           any_common_mut_;
};

//Atomic output for <<  until m_log object is live
template< typename T>
static m_log&& operator<<(m_log&& m_log_, T&& t ) noexcept
{
    m_log_.ss_ << std::forward<decltype(t)>(t);
    return std::move(m_log_);
};
#endif // M_LOG_H

