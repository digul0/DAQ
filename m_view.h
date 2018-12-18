#ifndef M_VIEW_H
#define M_VIEW_H

#include "common_std_headers.h"

#ifdef HAS_BOOST
using boost::regex, boost::sregex_iterator, boost::regex, boost::smatch;
#else
using std::regex, std::sregex_iterator, std::regex, std::smatch;
#endif // HAS_BOOST
class m_model;

class m_view
{
  /**
  * @brief
  * Provides validation of the response and splitting a complete response
  * to the components according to a specified regular expressions.
  */
public:
    m_view();
    ~m_view();
    m_view(const m_view&) = delete;
    m_view& operator=(const m_view&) = delete;

    /** MVC interface functions*/
    void setModel(m_model* model);

    /** Parse answer functions */
    std::vector<std::string> split_answer(const std::string& answer);
    bool is_valid_answer(const std::string& answer);
private:
    bool is_expected_answer(const std::string& answer);
    bool is_regex_compatible_answer(const std::string& answer);
    /** MVC members*/
    m_model* model_;

    /** regexes */
    inline const static
    regex S0{"A0"  "(\\d{1})"   "(\\d{1})"  "(\\d{1})"   "(\\d{5})"};
    //        ^^       ^^           ^^          ^^           ^^
    //        A0       don't_use    don't_use   don't_use    block_S/N
    inline const static
    regex S1{"A1"                           "(\\d{1})"};
    //        ^^                               ^^
    //        A1                               position
    inline const static
    regex S2{"A2"                           "(\\d{1})"};
    //        ^^                               ^^
    //        A2                               mode
    inline const static
    regex S3{"A3"  "(\\d{1})"   "(\\d{1})"  "(\\d{1})"  "(\\d{5})"};
    //        ^^       ^^           ^^          ^^          ^^
    //        A3       param_num    ch_number  mode        measured_data
    inline const static
    regex S4{"A4"                           "(\\d{1})"};
    //        ^^                               ^^
    //        A4                               temperature_mode

};
#endif // M_VIEW_H
