#ifndef M_OPEN_PORT_H
#define M_OPEN_PORT_H

#include "common_std_headers.h"

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#endif //_MSC_VER
#include "windows.h"




class m_open_port
{
    /**
    *@brief
    * Provides configuration, connection and synchronous
    * read/write interface to the Com port.
    */
public:
    m_open_port ()  =   delete;
    explicit m_open_port (unsigned int numPort);
    m_open_port(const m_open_port&) = delete;
    m_open_port& operator=(const m_open_port&) = delete;
    m_open_port(m_open_port&&);
    m_open_port& operator=(m_open_port&&);//=default;
    ~m_open_port();

    // Interface functions
    void write_line(const std::string& command);
    const std::string read_line ();
    const std::string get_portNum();

private:
    bool open ();
    void init();
    void close();
    void flushPort();
    bool is_valid();

    unsigned int _num_of_port;
    const std::string _port_init_string;
    HANDLE _com_port_handle;
    bool _valid; //rename to _valid, reverse logic
};
#endif // M_OPEN_PORT_H

