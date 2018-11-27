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
    m_open_port(m_open_port&&) = default;
    ~m_open_port();

    // Interface functions
    void write_raw(const std::string& command);
    std::string read_raw ();
    const std::string get_portNum() const ;
    HANDLE getPortHwd() const;
    void flushPort();

private:
    bool open ();
    void init();
    void close();
    bool is_valid() const; // virtual com port checking only, not device!

    unsigned int _num_of_port;
    const std::string _port_init_string;
    HANDLE _com_port_handle;
    bool _failure; //rename to port_is_open
};
#endif // M_OPEN_PORT_H

