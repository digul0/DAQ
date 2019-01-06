#ifndef COMMON_STD_HEADERS_HPP_INCLUDED
#define COMMON_STD_HEADERS_HPP_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <atomic>
#include <sstream>
#include <thread>
#include <mutex>
#include <fstream>


// define from Cmake if boost is find.
#ifdef HAS_BOOST
  #include <boost/regex.hpp>
#else
  #include <regex>
#endif // HAS_BOOST

#endif // COMMON_STD_HEADERS_HPP_INCLUDED
