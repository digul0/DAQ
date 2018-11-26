#include <map>
#include "m_options_parser.h"

#ifdef HAS_BOOST
using boost::regex, boost::sregex_iterator, boost::regex;
#else
using std::regex, std::sregex_iterator, std::regex;
#endif // HAS_BOOST

using std::string, std::vector, std::map, std::stringstream, std::numeric_limits, std::logic_error;
namespace Settings
{
vector<settings_struct>
SettingsParser::get_settings_struct(const string & _portmap_ini_name,
                                    const string & _job_ini_name)
{
    //Port Position Receptacle Temperature
    // from PortMap.ini
    const string options_format =
        "(\\d+)\\s"  "([A-Z]\\d+-\\d)"  "\\s(\\d+)\\s(\\d)";
    // ^^            ^^                  ^^        ^^
    //Port        Position             Receptacle Temperature
    regex options_regex(options_format);

    //get string with content of settings file
    string portmap_ini_string = SettingsReader(_portmap_ini_name).read_settings_to_whole_string();
    string job_ini_string = SettingsReader(_job_ini_name).read_settings_to_whole_string();

    map<string, settings_struct> name_to_com_table;
    for (sregex_iterator it(portmap_ini_string.begin(), portmap_ini_string.end(), options_regex), end_it;
            it != end_it; ++it)
        {
            if (it->size() != 0)
                {
                    settings_struct s 	{stoi((*it).str(1)),
                                         (*it).str(2),
                                         stoi((*it).str(3)),
                                         stoi((*it).str(4))};
                    name_to_com_table.emplace( (*it).str(2), std::move(s));
                }
        }
    //Port Position Port
    //vector<string> job_ini_vec; // BXX-X from settings
    string position_search_pattern =
        "([A-Z]\\d+-\\d)";
    //BXX-X
    regex r2(position_search_pattern);


    vector<settings_struct> nc;

    //parse string with content of settings file
    for (sregex_iterator it(job_ini_string.begin(), job_ini_string.end(), r2), end_it;
            it != end_it; ++it)
        {
            if (it->size() != 0)
                {
                    nc.push_back(name_to_com_table[(*it)[1].str()]);
                }
        }
    return nc;
}

string SettingsReader::read_settings_to_whole_string(have_header h) const
{
    if (!_f.is_open())
        throw logic_error(std::string(_filename) + " file not exist!");
    stringstream s;
    //skip settings header
    if ( h == have_header::have )
        _f.ignore(numeric_limits<std::streamsize>::max(),'\n');
    s << _f.rdbuf();
    return s.str();
}
} //Settings end
