#ifndef M_OPTIONS_PARSER_H
#define M_OPTIONS_PARSER_H

#include "common_std_headers.h"

/**
 * @brief Options files is a list of text-strings due to backward compatibility requrements.
 */
namespace settings
{

/**
 * @brief Settings container element
 */
struct settings_struct
{
    /// Com port number.
    int port;
    /// Device name according to enternal naming convention.
    std::string position;
    /// Not used.
    int receptacle;
    /// Describe nessesary to switch device to ambient temperature before start measurements.
    int temperature_mode;
};

/**
* @brief
* Provides parsing of text settings files (Portmap.ini, Job.ini),
* sampling (aka SQL "SELECT [Portmap.ini].* FROM [Job.ini], [Portmap.ini]
*                              WHERE [Portmap.ini] .Position = [Job.ini].Position;")
* and transfering the result to the user.
*/
class SettingsParser
{

public:
    /// @return Settings container.
    std::vector<settings_struct>
    get_settings_struct (const std::string& portmap_ini_name_, const std::string& job_ini_name_ );
private:
    std::string file_to_string(const std::string& file_name,  size_t skip_n_first_lines = 0);
};


} //end Settings namespace
#endif // M_OPTIONS_PARSER_H
