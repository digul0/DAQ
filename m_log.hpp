#ifndef M_LOG_H
#define M_LOG_H
//
#include "common_std_headers.hpp"
#include <iostream>
//#include <iosfwd>
#include <fstream>
#include <mutex>
#include <atomic>
#include <thread>


class m_log
{
public:
    explicit m_log(std::ostream& out_dest = std::cout ): _ul(_mut, std::defer_lock), _out(out_dest)
    {
        while(!_ul.try_lock())
            std::this_thread::yield();
    }
    m_log(m_log&& rlog) noexcept:
      _ul ( std::move(rlog._ul) ),
      _out ( rlog._out)
      {

      }
    m_log(const m_log&)            = delete;
    m_log& operator=(const m_log&) = delete;
    template< typename T>
    friend m_log&&  operator<<(m_log&&  _m_log, const T& t) noexcept;
private:
	  inline static std::mutex _mut; //common mutex
    std::unique_lock<std::mutex> _ul;
    std::ostream& _out;
public:
  static std::string make_name_from_time (std::chrono::system_clock::time_point tp);
	//inline static std::atomic<unsigned int> _failure_to_open_port_Counter{};
	//inline static std::atomic<unsigned int> _failure_to_read_port_Counter{};
    //std::ostream& operator()(){return cout;}
};

//Definitions mutex for output and atomic variables for counters
//inline std::atomic<unsigned int> m_log::_failure_to_open_port_Counter = 0; //?
//inline std::atomic<unsigned int> m_log::_failure_to_read_port_Counter = 0;
//inline std::mutex m_log::_mut;

//Atomic output for <<  until m_log object is live
template< typename T>
m_log&& operator<<(m_log&& _m_log, const T& t ) noexcept
{
    _m_log._out << t;
    return std::move(_m_log);
}

#endif // M_LOG_H

