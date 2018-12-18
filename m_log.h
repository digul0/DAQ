#ifndef M_LOG_H
#define M_LOG_H

#include <iomanip>
#include <sstream>

#include "common_std_headers.h"

template<typename T>
class Mutex_selector;

class m_log
{
    /**
    * @brief
    * Provides multi-threaded console and file output.
    */
public:
    enum class other { to_setted_log_file };

    // for stdout
    explicit m_log(std::ostream& out_dest = std::cout ): out_(&out_dest)
    {

    }
    // for file
    explicit m_log(std::ofstream& out_file ): out_(&out_file)
    {

    }
    //for setted file
    explicit m_log(m_log::other)
    {
        if (p_setted_file_!=nullptr)
            {
                out_ = p_setted_file_;
            }
        else
            throw std::logic_error("out log file not set!");
    }
    m_log(m_log&& rlog) noexcept:
        out_ ( rlog.out_), ss_(std::move(rlog.ss_))
    {

    }
    m_log(const m_log&)            = delete;
    m_log& operator=(const m_log&) = delete;
    ~m_log()
    {
        if (out_!=nullptr)
            write_to(*out_);
    }
    void static setLogfile(std::ofstream* out_file)
    {
        p_setted_file_ = out_file;
    }
private:
    template<typename T>
    void write_to(T&)
    {
        auto& relevant_mutex = Mutex_selector<T>::get();
        std::lock_guard<std::mutex> ul(relevant_mutex);
        *out_ << ss_.rdbuf() << std::flush;
    }
    template< typename T>
    friend m_log&&  operator<<(m_log&&  m_log_, const T& t) noexcept;

    //inline static std::mutex mut_console_, mut_file_;
    std::ostream* out_ {nullptr};
    std::stringstream ss_;
    inline static std::ofstream* p_setted_file_ {nullptr};
public:
    static const std::string
    make_name_from_time (const std::string name_prefix,
                         std::chrono::system_clock::time_point tp,
                         const std::string name_postfix);
};

//Atomic output for <<  until m_log object is live
template< typename T>
m_log&& operator<<(m_log&& m_log_, const T& t ) noexcept
{
    m_log_.ss_ << t;
    return std::move(m_log_);
}

//choose relevant static mutex
template<typename T>
class Mutex_selector
{
public:
    static std::mutex& get()
    {
        if (std::is_same_v<std::ofstream,T> )
            {
                return mut_file_;
            }
        else if (std::is_same_v<std::ostream,T>)
            {
                return mut_console_;
            }
        else
            {
                return any_common_mut_;
            }
    };
private:
    inline static std::mutex mut_console_,
           mut_file_,
           any_common_mut_;
};
#endif // M_LOG_H

