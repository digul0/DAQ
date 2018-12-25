#include "m_log.h"
#include <iomanip>
#include <sstream>

const std::string m_log::make_name_from_time(const std::string name_prefix,
        std::chrono::system_clock::time_point tp,
        const std::string name_postfix
                                            )
{
    auto current_time_t        = std::chrono::system_clock::to_time_t(tp);
    auto current_local_time    (std::localtime(&current_time_t));
    auto current_time_str      = std::put_time(current_local_time,"%Y_%m_%d-%H%M%S");
    std::stringstream os;
    os << current_time_str;
    const std::string log_name = name_prefix + os.str() + name_postfix;
    return log_name;
}
m_log::m_log(std::ostream& out_dest): out_(&out_dest)
{

}
m_log::m_log(std::ofstream& out_file): out_(&out_file)
{

}
m_log::m_log(m_log::other)
{
    if (p_setted_file_!=nullptr)
        {
            out_ = p_setted_file_;
        }
    else
        throw std::logic_error("out log file not set!");
}
m_log::m_log(m_log&& rlog) noexcept:
    out_ ( rlog.out_), ss_(std::move(rlog.ss_))
{

}
m_log::~m_log()
{
    if (out_!=nullptr)
        write_to(*out_);
}
void m_log::setLogfile(std::ofstream* out_file)
{
    p_setted_file_ = out_file;
}
template<typename T>
void m_log::write_to(T&)
{
    auto& relevant_mutex = Mutex_selector<T>::get();
    std::lock_guard<std::mutex> ul(relevant_mutex);
    *out_ << ss_.rdbuf() << std::flush;
}

template <typename T>
std::mutex& Mutex_selector<T>::get()
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
