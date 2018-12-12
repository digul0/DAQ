#include "m_model.h"
#include "m_options_parser.h"
#include "m_open_port.h"
#include "m_log.h"
using namespace std::chrono_literals;
m_model::m_model(const settings::settings_struct& ss):
    _port_num(ss.port), _port_impl(nullptr)
{
    _commands_pool_init();
}
m_model::m_model(m_model&&)=default;
m_model& m_model::operator=(m_model&&) =default;
m_model::~m_model() = default;
bool m_model::open_connection()
{
  _port_impl = std::make_unique<m_open_port>(_port_num); //can throw exceptions
  return true;
}
/**
  Commands sequenses
  Add new sequenses here
 */
void m_model::_commands_pool_init()
{
    commands_sequence acqure_temperature =
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
    commands_sequence acquire_25 =
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
    _commands_pool.emplace(m_model::CommandsPoolId::acqure_temperature,
                           std::move(acqure_temperature));
    _commands_pool.emplace(m_model::CommandsPoolId::acquire_25,
                           std::move(acquire_25));
    _commands_pool.emplace(m_model::CommandsPoolId::switch_up,
                           std::move(switch_up));
    _commands_pool.emplace(m_model::CommandsPoolId::switch_down,
                           std::move(switch_down));
    _commands_pool.emplace(m_model::CommandsPoolId::switch_to_default,
                           std::move(switch_to_default));


}
void  m_model::execute_current_command ()
{
    if (!_end_of_branch)
        execute_single_command(*_current_command_seq_it);

}
void  m_model::execute_single_command (const std::string& command)
{
    _current_command = command;
    if (_port_impl)
    _port_impl->write_line(_current_command);
    else  throw std::logic_error("Port not opened!");
}
void m_model::go_next_command()
{

    if (!_end_of_branch)
        ++_current_command_seq_it;
    _check_end();

}
void m_model::_check_end()
{
    _end_of_branch = (_current_command_seq_it == _current_commands_sequence->end());
}

const std::string
m_model::read_answer()
{
    return _answer = _port_impl->read_line();
}
//Choosing branch and recheck model::_end_of_branch value
void m_model::choose_commands_pool(CommandsPoolId poolid)
{
    auto find_it =_commands_pool.find(poolid);
    if ( find_it==_commands_pool.end()) throw std::logic_error ("Set of commands_sequence incorrect!");
    _current_commands_sequence = &find_it->second;//may return end();
    _current_command_seq_it    = _current_commands_sequence->begin();
    _check_end();
}
bool  m_model::end_commands ()
{
    return _end_of_branch;
}
const std::string
m_model::get_current_command()
{
    return _current_command;
}
const std::string
m_model::get_current_answer()
{
    return _answer;
}
