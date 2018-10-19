#ifndef COMMON_STD_HEADERS_HPP_INCLUDED
#define COMMON_STD_HEADERS_HPP_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <map>
#include <optional>

#ifdef _MSC_VER
	#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif
#include <boost/regex.hpp> //try precomp headers
#endif // COMMON_STD_HEADERS_HPP_INCLUDED
