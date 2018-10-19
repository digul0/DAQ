#ifndef M_VIEW_H
#define M_VIEW_H

#include "common_std_headers.hpp"


//#include <boost/regex.hpp>

//forward declarations
class m_model;
class m_controller;

class m_view
{
public:
    m_view();
    ~m_view();
    m_view(const m_view&) = delete;
    m_view& operator=(const m_view&) = delete;

public:
    /** Parse answer functions */
    bool is_valid_answer(const std::string& answer);
    std::vector<std::string> split_answer(const std::string& answer);

private:
    /** regexes */
    /*inline static*/
    boost::regex S0{"A0"  "(\\d{1})"   "(\\d{1})"  "(\\d{1})"   "(\\d{5})"};
                  // ^^       ^^           ^^          ^^           ^^
                  // A0       don't_use    don't_use   don't_use    block_S/N
    /*inline static*/
    boost::regex S1{"A1"                           "(\\d{1})"};
                  // ^^                               ^^
                  // A1                               position
    /*inline static*/
    boost::regex S2{"A2"                           "(\\d{1})"};
                  // ^^                               ^^
                  // A2                               mode
    /*inline static*/
    boost::regex S3{"A3"  "(\\d{1})"   "(\\d{1})"  "(\\d{1})"  "(\\d{5})"};
                  // ^^       ^^           ^^          ^^          ^^
                  // A3       param_num    ch_number  mode        measured_data
    /*inline static*/
    boost::regex S4{"A4"                           "(\\d{1})"};
                  // ^^                               ^^
                  // A4                               temperature_mode


};
#endif // M_VIEW_H
