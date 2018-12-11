#include <map>
#include "m_options_parser.h"

#ifdef HAS_BOOST
using boost::regex, boost::sregex_iterator, boost::regex;
#else
using std::regex, std::sregex_iterator, std::regex;
#endif // HAS_BOOST

using std::string, std::vector, std::map, std::stringstream, std::ifstream,
      std::numeric_limits, std::logic_error;
namespace settings
{
vector<settings_struct>
SettingsParser::get_settings_struct(const string & _portmap_ini_name,
                                    const string & _job_ini_name)
{
    // Port Position Receptacle Temperature
    // from PortMap.ini.
    const string options_format =
        "(\\d+)\\s"  "([A-Z]\\d+-\\d)"  "\\s(\\d+)\\s(\\d)";
    // ^^            ^^                  ^^        ^^
    //Port        Position             Receptacle Temperature
    regex options_regex1(options_format);

    //get string with content of settings file
    string portmap_ini_string = settings::file_to_string(_portmap_ini_name);
    string job_ini_string = settings::file_to_string(_job_ini_name);

    map<string, settings_struct> name_to_com_table; //map(posit_string, settings)
    for (sregex_iterator it(portmap_ini_string.begin(),
                            portmap_ini_string.end(),
                            options_regex1),
         end_it;
         it != end_it; ++it)
        {
            if (it->size() != 0)
                {
                    settings_struct s;
                    s.port             = stoi((*it).str(1));
                    s.position         =      (*it).str(2);
                    s.receptacle       = stoi((*it).str(3));
                    s.temperature_mode = stoi((*it).str(4));
                    name_to_com_table.emplace( s.position, s );
                }
        }
    //Port Position Port
    //vector<string> job_ini_vec; // BXX-X from settings
    string position_search_pattern =
        "([A-Z]\\d+-\\d)";
    //BXX-X
    regex options_regex2(position_search_pattern);
    vector<settings_struct> result;
    //parse string with content of settings file
    for (sregex_iterator it(job_ini_string.begin(),
                            job_ini_string.end(),
                            options_regex2),
         end_it;
         it != end_it; ++it)
        {
            if (it->size() != 0)
                {
                    //may not find
                    result.push_back(
                                     name_to_com_table[it->str(1)]
                                     );
                }
        }
    return result;
}

string file_to_string( const string& file_name, settings::have_header h )
{
  ifstream _f {file_name};
    if (!_f.is_open())
        throw logic_error(std::string(file_name) + " file not exist!");
    stringstream s;
    //skip first file line if it necessary
    if ( h == have_header::have )
        _f.ignore(numeric_limits<std::streamsize>::max(),'\n');
    s << _f.rdbuf();
    return s.str();
}
} //settings
