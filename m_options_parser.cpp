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
SettingsParser::get_settings_struct(const string& portmap_ini_name_,
                                    const string& job_ini_name_)
{
    // Port Position Receptacle Temperature
    // from PortMap.ini.
    const string options_format =
        "(\\d+)\\s"  "([A-Z]\\d+-\\d+)"  "\\s(\\d+)\\s(\\d)";
    // ^^            ^^                  ^^        ^^
    //Port        Position             Receptacle Temperature
    const regex options_regex1(options_format);

    //get string with content of settings file
    const string portmap_ini_string = file_to_string(portmap_ini_name_);
    const string job_ini_string     = file_to_string(job_ini_name_);

    map<string, settings_struct> name_to_com_table; //map(posit_string, settings)
    for (sregex_iterator it(portmap_ini_string.begin(),
                            portmap_ini_string.end(),
                            options_regex1),
            end_it;
            it != end_it; ++it)
        {
            if (it->size()!= 0)
                {
                    settings_struct s;
                    s.port             = stoi(it->str(1));
                    s.position         = it->str(2);
                    s.receptacle       = stoi(it->str(3));
                    s.temperature_mode = stoi(it->str(4));
                    name_to_com_table.emplace( s.position, s );
                }
        }
    //Port Position Port
    //vector<string> job_ini_vec; // BXX-X from settings
    const string position_search_pattern =
        "[A-Z]\\d+-\\d+";
    const regex options_regex2(position_search_pattern);
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
                    auto map_it = name_to_com_table.find(it->str(0));
                    if (map_it!=name_to_com_table.end())
                        {
                            result.push_back(
                                             map_it->second
                                             );
                        }
                    else
                        {
                            throw logic_error(
                                              string("Error mapping posit: ") + it->str(0)
                                              );
                        }
                }

        }
    return result;
}

string SettingsParser::file_to_string( const string& file_name, size_t skip_n_first_lines )
{
    ifstream f_ {file_name};
    if (!f_.is_open())
        throw logic_error(std::string(file_name) + " file not exist!");
    stringstream s;
    //skip n first file lines if it necessary
    if ( skip_n_first_lines!= 0)
        {
            while( skip_n_first_lines-- )
                {
                    f_.ignore(numeric_limits<std::streamsize>::max(),'\n');
                }
        }
    s << f_.rdbuf();
    return s.str();
}

} //settings
