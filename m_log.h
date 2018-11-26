#ifndef M_LOG_H
#define M_LOG_H

#include "common_std_headers.h"
#include <iomanip>
#include <sstream>
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
    explicit m_log(std::ostream& out_dest = std::cout ): _out(&out_dest)
    {

    }
    // for file
    explicit m_log(std::ofstream& out_file ): _out(&out_file)
    {

    }
    //for setted file
    explicit m_log(m_log::other)
    {
        if (_p_setted_file!=nullptr)
            {
                _out = _p_setted_file;
            }
        else
            throw std::logic_error("out log file not set!");
    }
    m_log(m_log&& rlog) noexcept:
        _out ( rlog._out), _ss(std::move(rlog._ss))
    {

    }
    m_log(const m_log&)            = delete;
    m_log& operator=(const m_log&) = delete;
    ~m_log()
    {
        if (_out!=nullptr)
            write_to(*_out);
    }
    void static setLogfile(std::ofstream* out_file)
    {
        _p_setted_file = out_file;
    }
private:
    template<typename T>
    void write_to(T&)
    {
        auto& relevant_mutex = Mutex_selector<T>::get();
        std::lock_guard<std::mutex> ul(relevant_mutex);
        *_out << _ss.rdbuf() << std::flush;
    }
    template< typename T>
    friend m_log&&  operator<<(m_log&&  _m_log, const T& t) noexcept;

    //inline static std::mutex _mut_console, _mut_file;
    std::ostream* _out {nullptr};
    std::stringstream _ss;
    inline static std::ofstream* _p_setted_file {nullptr};
public:
    static const std::string make_name_from_time (const std::string name_prefix,
            std::chrono::system_clock::time_point tp,
            const std::string name_postfix
                                                 );
};

//Atomic output for <<  until m_log object is live
template< typename T>
m_log&& operator<<(m_log&& _m_log, const T& t ) noexcept
{
    _m_log._ss << t;
    return std::move(_m_log);
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
                return _mut_file;
            }
        else if (std::is_same_v<std::ostream,T>)
            {
                return _mut_console;
            }
        else
            {
                return _any_common_mut;
            }
    };
private:
    inline static std::mutex _mut_console,
                             _mut_file,
                             _any_common_mut;
};
#endif // M_LOG_H

