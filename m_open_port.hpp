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
public:
  m_open_port ()  =   delete;
  explicit m_open_port (unsigned int numPort);
  m_open_port(const m_open_port&) = delete;
  m_open_port& operator=(const m_open_port&) = delete;
  m_open_port(m_open_port&&) = default;
  ~m_open_port();
  //open-close without any delays, checks, naive
  ///Из-за специфики работы со строками не завершающимися нулем

public:
  void write_raw(const std::string& command);
  std::string read_raw ();//?
  const std::string get_portNum() const ;
  const HANDLE getPortHwd() const; //typedef void HANDLE; (???!!)
  void flushPort();

private:
  bool open ();
  void init();
  void close();
  const bool is_valid() const; // virtual com port checking only, not device!

private:
  unsigned int _num_of_port;
  const std::string _port_init_string;
  HANDLE hComPort;
  bool _failure; //rename to port_is_open
};

// TODO
// Syncronize IO operations for minimum 50 ms between any threads
#endif // M_OPEN_PORT_H

