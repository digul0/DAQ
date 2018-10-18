#include "common_std_headers.hpp"

#include "m_open_port.hpp"
#include "m_options_parser.h"

using namespace std::chrono_literals;
class m_model
{
  public:
    /** Default constructor */
    explicit m_model(Settings::settings_struct ss );
    m_model(const m_model&) = delete;
    operator=(const m_model&) = delete;
    /** Default destructor */
    ~m_model() = default;

  /** typedef block */
  private:
    using command_impl_type = std::string;
    using answer_imp_type   = command_impl_type;
    using commands_sequence = std::vector<command_impl_type>; // commands sequence
    using commands_pool = std::vector<commands_sequence>; // all commands sequence pool


  public:
    enum class test {
    acqure_temperature = 0,
    acqure_25 = 1,
    switch_up = 2,
    switch_down = 3 };


  /** aviable commands*/
  private:
    struct _commands_list
    {
      command_impl_type //pimpl mb?
      ask_name    = "S0",
      ask_posit   = "S10",
      go_1        = "S11",
      go_2        = "S12",
      go_3        = "S13",
      go_4        = "S14",
      go_5        = "S15",
      go_6        = "S16",
      go_7        = "S17",
      go_8        = "S18",
      ask_mode    = "S20",
      switch_set  = "S21",
      switch_real = "S22",
      get_Isld    = "S31",
      get_T       = "S32",
      get_pdi     = "S33",
      switch_25   = "S41",
      switch_55   = "S40";
      /** */
    }
    commands_list;

  /** state changing interface functions*/

  public:
    void choose_commands_pool(test choose);
    void execute_command ();
    answer_imp_type  read_answer() ;
    void go_next_command();
    bool end_commands ();
    answer_imp_type get_current_command();
    answer_imp_type get_current_answer();

  /** MVC interface functions*/

  /** state members*/
  private:
    m_open_port                  p; //aka IO
    commands_pool                _commands_pool;
    commands_sequence*           current_commands_sequence;
    //commands_sequence c_s;
    command_impl_type            current_command;
    commands_sequence::iterator  current_command_seq_it;
    answer_imp_type              answer;
    bool                         end_of_branch;

  private:
    void commands_pool_init();
    void check_end();
};

