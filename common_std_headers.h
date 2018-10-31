#ifndef COMMON_STD_HEADERS_HPP_INCLUDED
#define COMMON_STD_HEADERS_HPP_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <map>
#include <optional>

#ifdef _MSC_VER
	#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif
//define from Cmake if boost is find
#ifdef HAS_BOOST
  #include <boost/regex.hpp>
#else
  #include <regex>
#endif // HAS_BOOST

#endif // COMMON_STD_HEADERS_HPP_INCLUDED
