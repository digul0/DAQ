#include "m_log.hpp"
#include <iomanip>
#include <sstream>
//std::mutex m_log::_mut;
std::string m_log::make_name_from_time( std::chrono::system_clock::time_point tp)
{
    auto current_time_t = std::chrono::system_clock::to_time_t( tp  );
    auto current_local_time ( std::localtime(&current_time_t) );
    auto current_time_str = std::put_time(current_local_time,"%Y_%m_%d-%H%M%S");
    std::ostringstream os;
    os << current_time_str;
    const std::string log_name = std::string("Results") + os.str() + ".txt";
    return log_name;
}
