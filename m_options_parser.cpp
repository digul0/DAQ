#ifndef PARSER_H
#include "m_options_parser.h"
#endif // PARSER_H


namespace Settings
{
  using namespace std;
  //vector<SettingsParser::settings_struct>

  vector<Settings::settings_struct>
  SettingsParser::get_settings_struct (const string& _portmap_ini_name, const string& _job_ini_name )
  {
    //Port Position Receptacle Temperature
    // from PortMap.ini
    string options_format = "(\\d*)\\s"  "([A-Z]\\d*-\\d)"  "\\s(\\d*)\\s(\\d)";
                             // ^^            ^^                  ^^        ^^
                             //Port        Position             Receptacle Temperature
    map<string, settings_struct > name_to_com_table;
    boost::regex r1(options_format);
    string pm_ini = SettingsReader(_portmap_ini_name).read_settings_to_whole_string();
    for( boost::sregex_iterator it (pm_ini.begin(), pm_ini.end(), r1), end_it; it!=end_it; ++it)
    {
      if (it->size()!=0){
        name_to_com_table.insert( {
                                 it->str(2) ,
                                 //
                                 settings_struct(stoi(it->str(1)),
                                                 it->str(2),
                                                 stoi(it->str(3)),
                                                 stoi(it->str(4)))
                                 } );
      }
    }
    //vector<string> job_ini_vec; // BXX-X from settings
    string block_find = "([A-Z]\\d*-\\d)"; //BXX-X
    boost::regex r2(block_find);
    string job_ini = SettingsReader(_job_ini_name).read_settings_to_whole_string();
    vector<settings_struct> nc;
    for( boost::sregex_iterator it (job_ini.begin(), job_ini.end(), r2), end_it; it!=end_it; ++it)
    {

      nc.push_back( name_to_com_table[it->str(1)]);
      //job_ini_vec.push_back(  );
    }
    return nc;
  }

}
