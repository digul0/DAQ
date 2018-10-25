#ifndef M_EXEPTION_INF_H
#define M_EXEPTION_INF_H
#include <string>
#include <exception>

class m_exception_inf: public std::exception
{
  public:
     explicit m_exception_inf(const std::string& message): _message(message){};

  virtual const char*
  what() const noexcept override
  {
    return _message.c_str();
  }
  private:
    const std::string _message;
};

#endif // M_EXEPTION_INF_H
