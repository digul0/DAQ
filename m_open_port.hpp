#pragma once
#include "windows.h"

#include <iostream>
#include <mutex>
#include <atomic>
#include <thread>

class m_open_port
{
public:
  m_open_port ()  =   delete;
  explicit m_open_port (unsigned int numPort);
  m_open_port(const m_open_port&) = delete;
  operator=(const m_open_port&) = delete;
  ~m_open_port();
  //open-close without any delays, checks, naive
  ///Из-за специфики работы со строками не завершающимися нулем
  void write_raw(const std::string& command);
  std::string read_raw ();//?
  const std::string get_portNum() const ;
  //void setMutex (std::mutex& m);
  const HANDLE getPortHwd() const; //typedef void HANDLE; (???!!)
  void flushPort(void);
private:
  bool open ();
  void init(); //mb add "S0" emit-resieve?
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


