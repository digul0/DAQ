#include "m_model.h"
#include "m_options_parser.h"
#include "m_open_port.h"

using namespace std::chrono_literals;
m_model::m_model(const Settings::settings_struct& ss)
    :_port(std::make_unique<m_open_port>(ss.port))

{
    commands_pool_init();
}
m_model::~m_model() = default;
/**
  Commands sequenses
  Add new sequenses here
 */
void m_model::commands_pool_init()
{
    commands_sequence test_t =
    {
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
    commands_sequence acqure_25 =
    {
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
    commands_sequence switch_up =
    {
        commands_list.switch_55
    };
    commands_sequence switch_down =
    {
        commands_list.switch_25
    };
    commands_sequence switch_to_default =
    {
        commands_list.go_1,
        commands_list.switch_real,
        commands_list.switch_55
    };
    //Strict adding order
    _commands_pool.push_back(std::move(test_t));
    _commands_pool.push_back(std::move(acqure_25));
    _commands_pool.push_back(std::move(switch_up));
    _commands_pool.push_back(std::move(switch_down));
    _commands_pool.push_back(std::move(switch_to_default));


}
void  m_model::execute_current_command ()
{
    if (!_end_of_branch)
        execute_single_command(*_current_command_seq_it);

}
void  m_model::execute_single_command (const std::string& command)
{
    _current_command = command;
    _port->write_raw(_current_command);
}
void m_model::go_next_command()
{

    if (!_end_of_branch)
        ++_current_command_seq_it;
    check_end();

}
void m_model::check_end()
{
    _end_of_branch = (_current_command_seq_it == _current_commands_sequence->end());
}

m_model::answer_imp_type
m_model::read_answer()
{
    return _answer = _port->read_raw();
}
//Choosing branch and recheck model::_end_of_branch value
void m_model::choose_commands_pool(CommandsPoolName choose)
{
    _current_commands_sequence   =   &_commands_pool[static_cast<unsigned int> (choose)];
    _current_command_seq_it =   _current_commands_sequence->begin();
    check_end();
}
bool  m_model::end_commands ()
{
    return _end_of_branch;
}
const m_model::answer_imp_type
m_model::get_current_command()
{
    return _current_command;
}
const m_model::answer_imp_type
m_model::get_current_answer()
{
    return _answer;
}
