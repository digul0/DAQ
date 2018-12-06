#include <algorithm>
#include "m_open_port.h"

#define OPEN_CLOSE_LOG_ON
#ifdef OPEN_CLOSE_LOG_ON
#include "m_log.h"
#endif // OPEN_CLOSE_LOG_ON

using namespace std::chrono_literals;
using std::string;
using std::mutex;
using std::logic_error;

///Construct m_open_port
m_open_port::m_open_port(unsigned int numPort):
    _num_of_port(numPort),
    _port_init_string("\\\\.\\COM"),
    _com_port_handle(nullptr),
    _valid(false)
{
    if ( open() )
        init();
}
m_open_port::m_open_port(m_open_port&& rhs):
    _num_of_port(rhs._num_of_port),
    _port_init_string (rhs._port_init_string),
    _com_port_handle (rhs._com_port_handle),
    _valid (rhs._valid)
{
    rhs._valid = false; //rename to _valid, reverse logic
}
m_open_port& m_open_port::operator=(m_open_port&& rhs)
{
  if (this!=&rhs) m_open_port(std::move(rhs));
  return *this;
}
m_open_port::~m_open_port()
{
    if(_valid)
        close();
}
bool m_open_port::open()
{
    if( _com_port_handle ==  nullptr )
        {
            const char *full_com_name = (_port_init_string
                                         + std::to_string(_num_of_port)).c_str();
            _com_port_handle = CreateFile( full_com_name, GENERIC_READ|GENERIC_WRITE,
                                   0, nullptr, OPEN_EXISTING, 0, nullptr);
            _valid = (_com_port_handle != INVALID_HANDLE_VALUE);
#ifdef OPEN_CLOSE_LOG_ON
            if (_valid)
            {
              m_log(m_log::other::to_setted_log_file)
                     <<"Port "<<_num_of_port<<" is opened!"<<'\n';
              m_log()<<"Port "<<_num_of_port<<" is opened!"<<'\n';
            }

#endif // OPEN_CLOSE_LOG_ON
            if (!_valid)
                {
                    throw logic_error(string("IO error: Can't open port: COM")
                                      + std::to_string(_num_of_port)) ;
                }
        }
    return _valid;
}

void m_open_port::init()
{
//structs DCB and COMMTIMEOUTS copied from port tuned by using TeraTerm.
    DCB dcb = {};
    dcb.DCBlength =             sizeof(DCB)/*28*/;
    dcb.BaudRate =             9600;
    dcb.fBinary =              1;
    dcb.fDtrControl =          1; //mb 0x0? Dtr line?
    dcb.fRtsControl =          1;
    dcb.ByteSize =             8;

    COMMTIMEOUTS timeouts = {};
    timeouts.ReadIntervalTimeout =        0xFFFFFFFF;
    timeouts.WriteTotalTimeoutConstant =    500;

    DCB dcb_current = {};
    COMMTIMEOUTS timeouts_current = {};
    GetCommState(_com_port_handle, &dcb_current);
    GetCommTimeouts(_com_port_handle, &timeouts_current);

    if ( memcmp(&dcb_current,&dcb, sizeof(DCB) )!=0 )
        SetCommState(_com_port_handle, &dcb);
    if ( memcmp(&timeouts_current,&timeouts, sizeof(COMMTIMEOUTS) )!=0 )
        SetCommTimeouts(_com_port_handle, &timeouts);

}

void m_open_port::close()
{
    if (is_valid())
        {
            CloseHandle(_com_port_handle);
            _valid = false;
            _com_port_handle = nullptr;
#ifdef OPEN_CLOSE_LOG_ON
            m_log(m_log::other::to_setted_log_file)
                    << "Port COM" << _num_of_port << " closed!" << '\n';
            m_log() << "Port COM" << _num_of_port << " closed!" << '\n';
#endif // OPEN_CLOSE_LOG_ON
        }
}
void m_open_port::write_raw(const std::string& command)
{
    /**
      Data transmit by protocol structure is: [ASCII command][CL+LF].
    */
    //static const char eol[] = "\n";
    if (!is_valid() || command.empty())
        return;
    constexpr char CRLF[2] = {'\r','\n'};
    DWORD bytes_written = 0;
    flushPort();
    //Send command string without null-terminator
    const std::string full_command = command + std::string(CRLF);
    size_t full_command_size = command.size() + sizeof(CRLF);
    WriteFile(_com_port_handle, full_command.data(), full_command_size , &bytes_written, nullptr);
    return ;
}
const string m_open_port::read_raw()
{
    /**
      Data receive by protocol structure is: [ASCII answer][CL+LF].
    */
    using std::cbegin, std::cend, std::search;
    if (!is_valid())
        return {};

    char buf[256]      = {};
    DWORD sz_buf = sizeof(buf)/sizeof(*buf);
    constexpr char CRLF[2] = {'\r','\n'};
    // Handler of invalid answer event.
    size_t try_counter = 0;
    DWORD total_bytes_read = 0,
          bytes_read = 0;
    auto findCRLF = cend(buf);
        do
            {
                if (++try_counter > 3) return "";//handle this obviously invalid answer later
                //add delay before new try
                if ( try_counter > 1 )
                {
                std::this_thread::sleep_for(100ms);
                m_log()<<"I read the shit!"<<'\n';
                }

                ReadFile (_com_port_handle, &buf[total_bytes_read], sz_buf, &bytes_read, nullptr);
                total_bytes_read+= bytes_read;
                if (total_bytes_read > sz_buf)
                  throw logic_error(string("IO error: reseived too long bytes sequence: ") + string(buf, sizeof(buf)) );
                findCRLF = std::search(cbegin(buf), cend(buf), cbegin(CRLF), cend(CRLF));
            }
        while ( findCRLF==cend(buf) );
    const string r (cbegin(buf), findCRLF);
    return r;
}

bool m_open_port::is_valid() const
{
    return _valid;
}

const std::string m_open_port::get_portNum() const
{
    return string("COM") + std::to_string(_num_of_port);
}

void m_open_port::flushPort(void)
{
    if (is_valid())
        {
            PurgeComm(_com_port_handle, PURGE_RXCLEAR | PURGE_RXABORT);
            PurgeComm(_com_port_handle, PURGE_TXCLEAR | PURGE_TXABORT);
        }
}
