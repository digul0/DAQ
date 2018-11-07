#ifndef M_LOG_H
#define M_LOG_H

#include "common_std_headers.h"

class m_log
{
    /**
    * @brief
    * ������������ ������������� ����� � ������� ��� ����.
    * Provides multi-threaded console and file output.
    */
public:
    // for stdout
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
private:
    void write_to(std::ostream&)
    {
        std::lock_guard<std::mutex> ul(_mut_console);
        _out << _ss.rdbuf();
    }
    void write_to(std::ofstream&)
    {
        std::lock_guard<std::mutex> ul(_mut_file);
        _out << _ss.rdbuf();
    }
    template< typename T>
    friend m_log&&  operator<<(m_log&&  _m_log, const T& t) noexcept;

    inline static std::mutex _mut_console, _mut_file;
    std::ostream& _out;
    std::stringstream _ss;
public:
    static std::string make_name_from_time (std::chrono::system_clock::time_point tp);
};

//Atomic output for <<  until m_log object is live
template< typename T>
m_log&& operator<<(m_log&& _m_log, const T& t ) noexcept
{
    _m_log._ss << t;
    return std::move(_m_log);
}

#endif // M_LOG_H

