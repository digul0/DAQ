#include "m_view.h"
#include "m_model.h"
m_view::m_view()
{
    //ctor
}

m_view::~m_view()
{
    //dtor
}

std::vector<std::string> m_view::split_answer(const std::string& answer)
{
    //validation of answer tests later in other class
    smatch sm;
    auto answer_key = std::string(answer.substr(0,2));
    if(     answer_key == "A0" )
        {
            regex_search(answer, sm, this->S0);
            return {answer_key, sm.str(1), sm.str(2), sm.str(3), sm.str(4)};
        }
    else if( answer_key == "A1" )
        {
            regex_search(answer, sm, this->S1);
            return {answer_key, sm.str(1)};
        }
    else if( answer_key == "A2" )
        {
            regex_search(answer, sm, this->S2);
            return {answer_key, sm.str(1)};
        }
    else if ( answer_key == "A3")
        {
            regex_search(answer, sm, this->S3);
            return {answer_key, sm.str(1), sm.str(2), sm.str(3), sm.str(4)};
        }
    else if ( answer_key == "A4")
        {
            regex_search(answer, sm, this->S4);
            return {answer_key, sm.str(1)};
        }
    else if ( answer_key == "AE")
        {
            return {answer_key, ""};
        }
    else
        {
            return {answer_key, ""};
        }
}

bool m_view::is_valid_answer(const std::string& answer)
{
    if (!answer.empty() && (answer.size()<=10) )
        {
            if (answer[0] == 'A')
                {
                    return all_of(answer.cbegin() + 1, answer.cend(), //first any,  aka ".[\d|E]\d*" //AE is error
                                  [](const char ch)
                    {
                        return (ch <= '9' && ch >= '0' ) || ( ch == 'E') ;
                    }
                                 );
                };
        }
    return false;
}
//  it restrict quantity of commands in model
//  maybe not good idea
bool m_view::is_regex_compatible_answer(const std::string& answer)
{
  auto command = model->get_current_command().substr(0,2);
  auto command_key = std::move(command.substr(0,2));
    if(     command_key == "S0" )
        {
            return regex_match(answer, this->S0);
        }
    else if( command_key == "S1" )
        {
            return regex_match(answer, this->S1);
        }
    else if( command_key == "S2" )
        {
            return regex_match(answer, this->S2);
        }
    else if ( command_key == "S3")
        {
            return regex_match(answer, this->S3);
        }
    else if ( command_key == "S4")
        {
            return regex_match(answer, this->S4);
        }
    else
        {
            return false;
        }
}
