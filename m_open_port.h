#ifndef M_OPEN_PORT_H
#define M_OPEN_PORT_H

#include "common_std_headers.h"

#ifdef MSC_VER_
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#endif //MSC_VER_
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

    unsigned int num_of_port_;
    const std::string port_init_string_;
    HANDLE com_port_handle_;
    bool valid_; //rename to valid_, reverse logic
};
#endif // M_OPEN_PORT_H

