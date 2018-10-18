/**@file Хедеры для набора классов для парсинга опций из обычных текстовых файлов.
 *@brief Файл с опциями из-за требований обратной совместимости
 * представляет собой список строк с '\n' на конце.
 * Два файла "
 */

#include "common_std_headers.hpp"

#pragma once
//#include "boost/regex.hpp"
//mb SQLite and "SELECT "Portmap.ini".position FROM "Job.ini" etc.
namespace Settings
{

using namespace std;
class SettingsReader
{
private:
    mutable ifstream f;
public:
    SettingsReader( const string& ini_name ) : f(ini_name) {}
    SettingsReader( const SettingsReader& )  =  delete;
    enum class have_header
    {    have,    no_have  };

    string read_settings_to_whole_string(have_header h = have_header::no_have) const
    {
        stringstream s;
        //skip settings header
        if ( h == have_header::have )
            f.ignore(numeric_limits<streamsize>::max(),'\n');
        s << f.rdbuf();
        return s.str();
    }
};
struct settings_struct
{
    settings_struct() = default;
    settings_struct(int _port,    string _position,    int _receptacle,    int _temperature_mode) :
        port(_port), position(_position), receptacle(_receptacle), temperature_mode(_temperature_mode)
    {}
    int port;
    string position;
    int receptacle;
    int temperature_mode;
};

class SettingsParser
{

public:
    vector<settings_struct>
    get_settings_struct (const string& _portmap_ini_name, const string& _job_ini_name );

};
} //end Settings namespace

