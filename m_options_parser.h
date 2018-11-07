/**@file ������ ��� ������ ������� ��� �������� ����� �� ������� ��������� ������.
 *@brief ���� � ������� ��-�� ���������� �������� �������������
 * ������������ ����� ������ ����� � '\n' �� �����.
 * ��� �����
 */
#ifndef M_OPTIONS_PARSER_H
#define M_OPTIONS_PARSER_H

#include "common_std_headers.h"

namespace Settings
{
/**
* @brief
* ������������ ������� ������ ��������� �������� (������ Portmap.ini, Job.ini),
* ������� (aka SQL "SELECT [Portmap.ini].* FROM [Job.ini], [Portmap.ini]
*                              WHERE [Portmap.ini].Position = [Job.ini].Position;")
* � �������� ���������� ������������.
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
public:
    explicit SettingsReader( const string& ini_name ) : _filename(ini_name), _f(ini_name) {}
    SettingsReader( const SettingsReader& )  =  delete;
    enum class have_header
    {    have,    no_have  };
    string read_settings_to_whole_string(have_header h = have_header::no_have) const;
private:
    const string _filename;
    mutable ifstream _f;
};


class SettingsParser
{
public:
    vector<settings_struct>
    get_settings_struct (const string& _portmap_ini_name, const string& _job_ini_name );

};
} //end Settings namespace
#endif // M_OPTIONS_PARSER_H
