/**@file Хедеры для набора классов для парсинга опций из обычных текстовых файлов.
 *@brief Файл с опциями из-за требований обратной совместимости
 * представляет собой список строк с '\n' на конце.
 * Два файла
 */
#ifndef M_OPTIONS_PARSER_H
#define M_OPTIONS_PARSER_H

#include "common_std_headers.h"

namespace Settings
{
/**
* @brief
* Обеспечивает парсинг файлов текстовых настроек (вшитые Portmap.ini, Job.ini),
* выборку (aka SQL "SELECT [Portmap.ini].* FROM [Job.ini], [Portmap.ini]
*                              WHERE [Portmap.ini].Position = [Job.ini].Position;")
* и передачу результата пользователю.
* Provides parsing of text settings files (Portmap.ini, Job.ini),
* sampling (aka SQL "SELECT [Portmap.ini].* FROM [Job.ini], [Portmap.ini]
*                              WHERE [Portmap.ini] .Position = [Job.ini].Position;")
* and transfering the result to the user.
*/
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
    const string _filename;
    mutable ifstream _f;
public:
    explicit SettingsReader( const string& ini_name ) : _filename(ini_name), _f(ini_name) {}
    SettingsReader( const SettingsReader& )  =  delete;
    enum class have_header
    {    have,    no_have  };
    string read_settings_to_whole_string(have_header h = have_header::no_have) const;
};


class SettingsParser
{
public:
    vector<settings_struct>
    get_settings_struct (const string& _portmap_ini_name, const string& _job_ini_name );

};
} //end Settings namespace
#endif // M_OPTIONS_PARSER_H
