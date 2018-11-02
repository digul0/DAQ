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
  *  Обеспечивает валидацию ответа и разбиению цельного ответа на составляющие
  * согласно заданному регулярному шаблону
  * Provides validation of the response and splitting a complete response
  * to the components according to a specified regular expressions.
  */
public:
    m_view();
    ~m_view();
    m_view(const m_view&) = delete;
    m_view& operator=(const m_view&) = delete;
public:
    void setModel(m_model* model);

public:
    /** Parse answer functions */
    std::vector<std::string> split_answer(const std::string& answer);
    bool is_valid_answer(const std::string& answer);
private:
    bool is_expected_answer(const std::string& answer);
    bool is_regex_compatible_answer(const std::string& answer);
private:
    /** regexes */
    /*inline static*/
    regex S0{"A0"  "(\\d{1})"   "(\\d{1})"  "(\\d{1})"   "(\\d{5})"};
                  // ^^       ^^           ^^          ^^           ^^
                  // A0       don't_use    don't_use   don't_use    block_S/N
    /*inline static*/
    regex S1{"A1"                           "(\\d{1})"};
                  // ^^                               ^^
                  // A1                               position
    /*inline static*/
    regex S2{"A2"                           "(\\d{1})"};
                  // ^^                               ^^
                  // A2                               mode
    /*inline static*/
    regex S3{"A3"  "(\\d{1})"   "(\\d{1})"  "(\\d{1})"  "(\\d{5})"};
                  // ^^       ^^           ^^          ^^          ^^
                  // A3       param_num    ch_number  mode        measured_data
    /*inline static*/
    regex S4{"A4"                           "(\\d{1})"};
                  // ^^                               ^^
                  // A4                               temperature_mode
    m_model* _model;

};
#endif // M_VIEW_H
