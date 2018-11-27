/**
 * @brief Options files is a list of text-strings due to backward compatibility requrements.
 */
#ifndef M_OPTIONS_PARSER_H
#define M_OPTIONS_PARSER_H

#include "common_std_headers.h"

namespace Settings
{
/**
* @brief
* Provides parsing of text settings files (Portmap.ini, Job.ini),
* sampling (aka SQL "SELECT [Portmap.ini].* FROM [Job.ini], [Portmap.ini]
*                              WHERE [Portmap.ini] .Position = [Job.ini].Position;")
* and transfering the result to the user.
*/


struct settings_struct
{
  /**
   * @brief
   * port             - com port number
   * position         - device name according to enternal naming
   * receptacle       - not used
   * temperature_mode - describe nessesary to switch device to ambient
   *                    temperature before start measurements.
   */
    int port;
    std::string position;
    int receptacle;
    int temperature_mode;
};

class SettingsReader
{
public:
    explicit SettingsReader( const std::string& ini_name ) : _filename(ini_name), _f(ini_name) {}
    SettingsReader( const SettingsReader& )  =  delete;
    enum class have_header
    {    have,    no_have  };
    std::string read_settings_to_whole_string(have_header h = have_header::no_have) const;
private:
    const std::string _filename;
    mutable std::ifstream _f;
};


class SettingsParser
{
public:
    std::vector<settings_struct>
    get_settings_struct (const std::string& _portmap_ini_name, const std::string& _job_ini_name );

};
} //end Settings namespace
#endif // M_OPTIONS_PARSER_H
