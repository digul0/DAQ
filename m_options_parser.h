/**@file Хедеры для набора классов для парсинга опций из обычных текстовых файлов.
 *@brief Файл с опциями из-за требований обратной совместимости
 * представляет собой список строк с '\n' на конце.
 * Два файла "
 */
#ifndef M_OPTIONS_PARSER_H
#define M_OPTIONS_PARSER_H

#include "common_std_headers.h"

#pragma once
//#include "boost/regex.hpp"
//mb SQLite and "SELECT "Portmap.ini".position FROM "Job.ini" etc.
namespace Settings
{
using namespace std;

struct settings_struct
{
    int port;
    string position;
    int receptacle;
    int temperature_mode;
};

class SettingsReader
{
private:
    mutable ifstream _f;
public:
    explicit SettingsReader( const string& ini_name ) : _f(ini_name) {}
    SettingsReader( const SettingsReader& )  =  delete;
    enum class have_header
    {    have,    no_have  };

    string read_settings_to_whole_string(have_header h = have_header::no_have) const
    {
      if (_f.bad()) throw "file not exist";
        stringstream s;
        //skip settings header
        if ( h == have_header::have )
            _f.ignore(numeric_limits<streamsize>::max(),'\n');
        s << _f.rdbuf();
        return s.str();
    }
};


class SettingsParser
{
public:
    vector<settings_struct>
    get_settings_struct (const string& _portmap_ini_name, const string& _job_ini_name );

};
} //end Settings namespace
#endif // M_OPTIONS_PARSER_H
