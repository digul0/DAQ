#include <iostream>
//#include <iosfwd>
#include <fstream>
#include <mutex>
#include <atomic>
#include <thread>

class m_log
{
public:
    explicit m_log(std::ostream& out_dest = std::cout ): ul(_mut, std::defer_lock), _out(out_dest)
    {
        while(!ul.try_lock())
            std::this_thread::yield();
    }
    m_log(m_log&& rlog) noexcept:
      ul ( std::move(rlog.ul) ),
      _out ( rlog._out)
      {

      }


    m_log(const m_log&)            = delete;
    m_log& operator=(const m_log&) = delete;
    template< typename T>
    friend m_log&  operator<<(m_log&&  _m_log, const T& t) noexcept;
private:
    static std::mutex _mut; //common mutex
    std::unique_lock<std::mutex> ul;
    std::ostream& _out;
public:
    static std::atomic<unsigned int> _failure_to_open_port_Counter;
    static std::atomic<unsigned int> _failure_to_read_port_Counter;
    //std::ostream& operator()(){return cout;}
};

///Definitions mutex for output and atomic variables for counters
inline std::atomic<unsigned int> m_log::_failure_to_open_port_Counter = 0; //?
inline std::atomic<unsigned int> m_log::_failure_to_read_port_Counter = 0;
inline std::mutex m_log::_mut;

//Atomic output for <<  until m_log object is live
template< typename T>
m_log& operator<<(m_log&& _m_log, const T& t ) noexcept
{
    _m_log._out << t;
    return _m_log;
}

