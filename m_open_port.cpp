#include <algorithm>
#include "m_open_port.h"

#define OPEN_CLOSE_LOG_ON
#ifdef OPEN_CLOSE_LOG_ON
#include "m_log.h"
#endif // OPEN_CLOSE_LOG_ON

using namespace std::chrono_literals;
using std::string;
using std::mutex;
using std::atomic;
using std::logic_error;

///Construct m_open_port
m_open_port::m_open_port(unsigned int numPort):
    _num_of_port(numPort),
    _port_init_string("\\\\.\\COM"),
    hComPort(nullptr),
    _failure(true)
    //_m(nullptr)
{
    if ( open() )
        init();
}
m_open_port::~m_open_port()
{
    if(!_failure)
        close();
}
bool m_open_port::open()
{
    if( hComPort ==  nullptr )
        {
            const char *full_com_name = (_port_init_string + std::to_string(_num_of_port)).c_str();
            hComPort = CreateFile( full_com_name, GENERIC_READ|GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,/*FILE_FLAG_OVERLAPPED*/0, nullptr);
            _failure = (hComPort == INVALID_HANDLE_VALUE);
#ifdef OPEN_CLOSE_LOG_ON
            if (!_failure)
                m_log()<<"Port "<<_num_of_port<<" is opened!"<<'\n';
#endif // OPEN_CLOSE_LOG_ON
            if (_failure)
                {
                    //++m_log::_failure_to_open_port_Counter;
                    throw logic_error(string("IO error: Can't open port: COM") + std::to_string(_num_of_port)) ;
                }
        }
    return !_failure;
}

void m_open_port::init()
{
//structs DCB и COMMTIMEOUTS copied from port tuned by using TeraTerm.
    DCB dcb = {};
    dcb.DCBlength =             sizeof(DCB)/*28*/;
    dcb.BaudRate =             9600;
    dcb.fBinary =              1;
    dcb.fDtrControl =          1; //mb 0x0? Dtr line?
    dcb.fRtsControl =          1;
    dcb.ByteSize =             8;

    COMMTIMEOUTS timeouts = {};
    timeouts.ReadIntervalTimeout =        0xFFFFFFFF;
    timeouts.WriteTotalTimeoutConstant =    500; //500

    DCB dcb_current = {};
    COMMTIMEOUTS timeouts_current = {};
    GetCommState(hComPort, &dcb_current);
    GetCommTimeouts(hComPort, &timeouts_current);

    if ( memcmp(&dcb_current,&dcb, sizeof(DCB) )!=0 )
        SetCommState(hComPort, &dcb);
    if ( memcmp(&timeouts_current,&timeouts, sizeof(COMMTIMEOUTS) )!=0 )
        SetCommTimeouts(hComPort, &timeouts);

}

void m_open_port::close()
{

    if (is_valid())
        {
            CloseHandle(hComPort); ///так себе вообщения
            _failure = true;
            hComPort = nullptr;
#ifdef OPEN_CLOSE_LOG_ON
            m_log() << "Port COM" << _num_of_port << " closed!" << '\n';
#endif // OPEN_CLOSE_LOG_ON
        }
}
void m_open_port::write_raw(const std::string& command)
{
    /**
      Data exchange by protocol structure: [ASCII command][null-terminator][CL+LF].
    */
    //static const char eol[] = "\n";
    if (!this->is_valid() || command.empty())
        return;
    constexpr char CRLF[] = "\r\n";
    DWORD bytes_written = 0;

    flushPort();
    // split into 2 write operations
    //Send command string with null-terminator
    WriteFile(hComPort, command.data(), command.size() + 1, &bytes_written, nullptr);
    //                                                    ^^ + null-terminator
    //Send CRLF string without null-terminator
    WriteFile(hComPort, CRLF, sizeof(CRLF) - 1, &bytes_written, nullptr);
    //                                      ^^ - null-terminator
    return ;
}
string m_open_port::read_raw()
{
    using std::cbegin, std::cend, std::find_first_of;
    if (!is_valid())
        return {};
    DWORD bytes_read  =  0;
    char buf[30]      = {};
    DWORD sz_buf = sizeof(buf)/sizeof(*buf);
    constexpr char CRLF[] = "\r\n";
    ReadFile (hComPort, buf, sz_buf, &bytes_read, nullptr);
    auto findCRLF = find_first_of(cbegin(buf), cend(buf), cbegin(CRLF), cend(CRLF));
    size_t temp_br = bytes_read;

    for(size_t try_counter = 0; findCRLF == cend(buf); ++try_counter)
        {
            std::this_thread::sleep_for(100ms);

            ReadFile (hComPort, &buf[temp_br], sz_buf, &bytes_read, nullptr); //resume reading, to &buf[bytes_read]
            temp_br+= bytes_read;
            findCRLF = find_first_of(cbegin(buf), cend(buf), cbegin(CRLF), cend(CRLF));
            if (try_counter > 3)
                {
                    return "";
                }
        }
    string r ( cbegin(buf), findCRLF) ;

//#ifdef OPEN_CLOSE_LOG_ON
//    if (!bytes_read)
//        ++m_log::_failure_to_read_port_Counter;
//#endif // OPEN_CLOSE_LOG_ON

    return r;
}
bool m_open_port::is_valid() const
{
    return !_failure;
}
HANDLE m_open_port::getPortHwd() const
{
    return hComPort;
}

const std::string m_open_port::get_portNum() const
{
    return string("COM") + std::to_string(_num_of_port);
}
void m_open_port::flushPort(void)
{
    if (is_valid())
        {
            PurgeComm(hComPort, PURGE_TXCLEAR);// | PURGE_TXABORT);
        }
}
