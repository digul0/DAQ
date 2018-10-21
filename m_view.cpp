#include "m_view.h"

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
    //if(!is_valid_answer(answer)) { std::cerr <<'\n'<< answer <<'\n';}
    smatch sm;
    auto command_name = std::string(answer.substr(0,2));
    if(     command_name == "A0" )
        {
            regex_search(answer, sm, this->S0);
            return {command_name, sm.str(1), sm.str(2), sm.str(3), sm.str(4)};
        }
    else if( command_name == "A1" )
        {
            regex_search(answer, sm, this->S1);
            return {command_name, sm.str(1)};
        }
    else if( command_name == "A2" )
        {
            regex_search(answer, sm, this->S2);
            return {command_name, sm.str(1)};
        }
    else if ( command_name == "A3")
        {
            regex_search(answer, sm, this->S3);
            return {command_name, sm.str(1), sm.str(2), sm.str(3), sm.str(4)};
        }
    else if ( command_name == "A4")
        {
            regex_search(answer, sm, this->S4);
            return {command_name, sm.str(1)};
        }
    else if ( command_name == "AE")
        {
            return {command_name, ""};
        }
    else
        {
            return {command_name, ""};
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
