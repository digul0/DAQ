#ifndef M_OPTIONS_PARSER_H
#define M_OPTIONS_PARSER_H

#include "common_std_headers.h"

/**
 * @brief Options files represents as a lists of text-strings due to backward compatibility requrements.
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
    /**
     * Join settings for both options file
     * @param[in]  portmap_ini_name_  file contains folowing settings:
     *    @code
     *    Port Position Receptacle Temperature
     *    81   B01-1    11         0
     *    82   B01-2    11         0
     *    83   B01-3    11         0
     *    84   B01-4    11         0
     *    ....
     *    @endcode
     * @param[in]  job_ini_name_    file contains Positions for operate.
     *    @code
     *    B01-1
     *    B01-2
     *
     *    B05-2
     *    ....
     *    @endcode
     * @return settings container.
     */
    std::vector<settings_struct>
    get_settings_struct (const std::string& portmap_ini_name_, const std::string& job_ini_name_ ) noexcept(false);
private:
    ///Read text file to string
    std::string file_to_string(const std::string& file_name,  size_t skip_n_first_lines = 0) noexcept(false);
};


} //end Settings namespace
#endif // M_OPTIONS_PARSER_H
