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
    num_of_port_(numPort),
    port_init_string_("\\\\.\\COM"),
    com_port_handle_(nullptr),
    valid_(false)
{
    if ( open() )
        init();
}

m_open_port::m_open_port(m_open_port&& rhs):
    num_of_port_(rhs.num_of_port_),
    port_init_string_ (rhs.port_init_string_),
    com_port_handle_ (rhs.com_port_handle_),
    valid_ (rhs.valid_)
{
    rhs.valid_ = false;
}
m_open_port& m_open_port::operator=(m_open_port&& rhs)
{
  if (this!=&rhs) m_open_port(std::move(rhs));
  return *this;
}
m_open_port::~m_open_port()
{
    if(valid_)
        close();
}
bool m_open_port::open()
{
    if( com_port_handle_ ==  nullptr )
        {
            const char *full_com_name = (port_init_string_
                                         + std::to_string(num_of_port_)).c_str();
            com_port_handle_ = CreateFile( full_com_name, GENERIC_READ|GENERIC_WRITE,
                                   0, nullptr, OPEN_EXISTING, 0, nullptr);
            valid_ = (com_port_handle_ != INVALID_HANDLE_VALUE);
#ifdef OPEN_CLOSE_LOG_ON
            if (valid_)
            {
              m_log(m_log::other::to_setted_log_file)
                     <<"Port "<<num_of_port_<<" is opened!"<<'\n';
              m_log()<<"Port "<<num_of_port_<<" is opened!"<<'\n';
            }

#endif // OPEN_CLOSE_LOG_ON
            if (!valid_)
                {
                    throw logic_error(string("IO error: Can't open port: COM")
                                      + std::to_string(num_of_port_)) ;
                }
        }
    return valid_;
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
    GetCommState(com_port_handle_, &dcb_current);
    GetCommTimeouts(com_port_handle_, &timeouts_current);

    if ( memcmp(&dcb_current,&dcb, sizeof(DCB) )!=0 )
        SetCommState(com_port_handle_, &dcb);
    if ( memcmp(&timeouts_current,&timeouts, sizeof(COMMTIMEOUTS) )!=0 )
        SetCommTimeouts(com_port_handle_, &timeouts);

}

void m_open_port::close()
{
    if (is_valid())
        {
            CloseHandle(com_port_handle_);
            valid_ = false;
            com_port_handle_ = nullptr;
#ifdef OPEN_CLOSE_LOG_ON
            m_log(m_log::other::to_setted_log_file)
                    << "Port COM" << num_of_port_ << " closed!" << '\n';
            m_log() << "Port COM" << num_of_port_ << " closed!" << '\n';
#endif // OPEN_CLOSE_LOG_ON
        }
}
void m_open_port::write_line(const string& command)
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
    const string full_command = command + string(CRLF);
    size_t full_command_size = command.size() + sizeof(CRLF);
    WriteFile(com_port_handle_, full_command.data(), full_command_size , &bytes_written, nullptr);
    return ;
}
const string m_open_port::read_line()
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
                ReadFile (com_port_handle_, &buf[total_bytes_read], sz_buf, &bytes_read, nullptr);
                total_bytes_read+= bytes_read;
                if (total_bytes_read > sz_buf)
                    throw logic_error(string("IO error: reseived too long bytes sequence: ") + string(buf, sizeof(buf)) );
                findCRLF = std::search(cbegin(buf), cend(buf), cbegin(CRLF), cend(CRLF));
            }
        while ( findCRLF==cend(buf) );
    const string r (cbegin(buf), findCRLF);
    return r;
}

bool m_open_port::is_valid()
{
    return valid_;
}

const string m_open_port::get_portNum()
{
    return string("COM") + std::to_string(num_of_port_);
}

void m_open_port::flushPort(void)
{
    if (is_valid())
        {
            PurgeComm(com_port_handle_, PURGE_RXCLEAR | PURGE_RXABORT);
            PurgeComm(com_port_handle_, PURGE_TXCLEAR | PURGE_TXABORT);
        }
}
