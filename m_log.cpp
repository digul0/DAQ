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
