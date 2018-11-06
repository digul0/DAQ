#ifndef M_LOG_H
#define M_LOG_H

#include "common_std_headers.h"

class m_log
{
    /**
    * @brief
    * ќбеспечивает многопоточный вывод в консоль
    * Provides multi-threaded console output
    */
public:
    explicit m_log(std::ostream& out_dest = std::cout ): _out(out_dest)
    {

    }
    // for file
    explicit m_log(std::ofstream& out_file ): _out(out_file)
    {

    }
    m_log(m_log&& rlog) noexcept:
        _out ( rlog._out), _ss(std::move(rlog._ss))
    {

    }
    m_log(const m_log&)            = delete;
    m_log& operator=(const m_log&) = delete;
    ~m_log()
    {
      write_to(_out);
    }

    void write_to(std::ostream& out_dest)
    {
      std::lock_guard<std::mutex> ul(_mut_console);
      _out << _ss.rdbuf();
    }
    void write_to(std::ofstream& out_file)
    {
      std::lock_guard<std::mutex> ul(_mut_file);
      _out << _ss.rdbuf();
    }
    template< typename T>
    friend m_log&&  operator<<(m_log&&  _m_log, const T& t) noexcept;
private:
    inline static std::mutex _mut_console; //common mutex
    inline static std::mutex _mut_file; //common mutex
    //inline static std::atomic<bool> flag{false};
    //inline static bool flag{false};
    //std::unique_lock<std::mutex> _ul;
    std::ostream& _out;
public:
    static std::string make_name_from_time (std::chrono::system_clock::time_point tp);
};

//Definitions mutex for output and atomic variables for counters


//Atomic output for <<  until m_log object is live
template< typename T>
m_log&& operator<<(m_log&& _m_log, const T& t ) noexcept
{
    _m_log._out << t;
    return std::move(_m_log);
}

#endif // M_LOG_H

