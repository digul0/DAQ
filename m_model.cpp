#include "m_model.h"

m_model::m_model(Settings::settings_struct ss)
               :p(ss.port),//mock settingse
               end_of_branch(false)
{
  commands_pool_init();
}
/**
  Commands sequenses
  Add new sequenses here
 */
void m_model::commands_pool_init()
{
   commands_sequence test_t = {
    commands_list.ask_name,
    commands_list.switch_set,
        commands_list.go_1,
    commands_list.get_T,
        commands_list.go_2,
    //commands_list.switch_real,
    commands_list.get_T,
        commands_list.go_3,
    //commands_list.switch_real,
    commands_list.get_T,
        commands_list.go_4,
    //commands_list.switch_real,
    commands_list.get_T,
        commands_list.go_5,
    //commands_list.switch_real,
    commands_list.get_T,
        commands_list.go_6,
    //commands_list.switch_real,
    commands_list.get_T,
        commands_list.go_7,
    //commands_list.switch_real,
    commands_list.get_T,
        commands_list.go_8,
    //commands_list.switch_real,
    commands_list.get_T,
        commands_list.go_1
    };
   commands_sequence acqure_25 = {
    commands_list.switch_set,
    commands_list.go_1,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_2,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_3,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_4,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_5,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_6,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_7,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_8,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,

    commands_list.switch_real,
    commands_list.go_1,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_2,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_3,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_4,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_5,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_6,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_7,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_8,
      commands_list.get_T,
      commands_list.get_Isld,
      commands_list.get_pdi,
    commands_list.go_1
    };
    commands_sequence switch_up = {
      commands_list.switch_55
    };
    commands_sequence switch_down = {
      commands_list.switch_25
    };
   _commands_pool.push_back(std::move(test_t));
   _commands_pool.push_back(std::move(acqure_25));
   _commands_pool.push_back(std::move(switch_up));
   _commands_pool.push_back(std::move(switch_down));


}
void  m_model::execute_command ()
{
  if (!end_of_branch)
  {
    current_command = *current_command_seq_it;
    p.write_raw(current_command);
  }
}

void m_model::go_next_command()
{
  ++current_command_seq_it;
  check_end();
}
void m_model::check_end()
{
  if(current_command_seq_it != current_commands_sequence->end()) {
    end_of_branch = false;
  } else {
    end_of_branch = true;
  }
}

m_model::answer_imp_type
m_model::read_answer()
{
  answer = p.read_raw();//Wait_wraper(p.read_raw())();
  return answer;
}
void m_model::choose_commands_pool(test choose)
{
  current_commands_sequence   =   &_commands_pool[static_cast<unsigned int> (choose)];
  current_command_seq_it =   current_commands_sequence->begin();
  check_end();
}
  bool  m_model::end_commands ()
  {
    return end_of_branch;
  }
  m_model::answer_imp_type
  m_model::get_current_command()
    {
      return current_command;
    }
  m_model::answer_imp_type
  m_model::get_current_answer()
  {
    return answer;
  }
