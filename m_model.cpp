#include "m_model.h"
#include "m_options_parser.h"
#include "m_open_port.h"
#include "m_log.h"
using namespace std::chrono_literals;

m_model::m_model(const settings::settings_struct& ss):
    port_num_(ss.port), port_impl_(nullptr)
{
    commands_pool_init();
}
m_model::m_model(m_model&&)=default;
m_model& m_model::operator=(m_model&&) =default;
m_model::~m_model() = default;
bool m_model::open_connection()
{
    port_impl_ = std::make_unique<m_open_port>(port_num_); //can throw exception during construct object
    return true;
}

void m_model::try_to_open_connection(size_t n_tries, std::chrono::seconds delay_before_tries)
{
    for(size_t tries_counter = 0; tries_counter < n_tries ;)
        {
            try
                {
                    if (open_connection())
                    {
                      connection_opened = true;
                      return;
                    }
                }
            catch(std::exception_ptr& pex)
                {
                    std::this_thread::sleep_for(delay_before_tries);
                    ++tries_counter;
                    m_log() << "Try to open number: " << tries_counter <<'\n';
                    m_log(m_log::other::to_setted_log_file)
                            << "Try to open number: " << tries_counter <<'\n';
                    if (tries_counter == n_tries ) std::rethrow_exception(pex);
                }
        }
}

bool m_model::is_connection_opened()
{
    return port_impl_->is_valid();
}

//  Commands sequenses
//  Add new sequenses here
void m_model::commands_pool_init()
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
    commands_pool_.emplace(m_model::CommandsPoolId::acqure_temperature,
                           std::move(acqure_temperature));
    commands_pool_.emplace(m_model::CommandsPoolId::acquire_25,
                           std::move(acquire_25));
    commands_pool_.emplace(m_model::CommandsPoolId::switch_up,
                           std::move(switch_up));
    commands_pool_.emplace(m_model::CommandsPoolId::switch_down,
                           std::move(switch_down));
    commands_pool_.emplace(m_model::CommandsPoolId::switch_to_default,
                           std::move(switch_to_default));


}

void  m_model::execute_current_command ()
{
    if (!end_of_branch_)
        execute_single_command(*current_command_seq_it_);

}

void  m_model::execute_single_command (const std::string& command) noexcept (false)
{
    current_command_ = command;
    if (port_impl_)
        port_impl_->write_line(current_command_);
    else
        throw std::logic_error("Port not opened!");
}

void m_model::go_next_command()
{

    if (!end_of_branch_)
        ++current_command_seq_it_;
    check_end();

}

void m_model::check_end()
{
    end_of_branch_ = (current_command_seq_it_ == current_commands_sequence_->end());
}

const std::string
m_model::read_answer()
{
      if (port_impl_)
          answer_ = port_impl_->read_line();
      else
          throw std::logic_error("Port not opened!");
    return answer_;
}

//Choosing branch and recheck model::end_of_branch_ value
void m_model::choose_commands_pool(CommandsPoolId poolid)
{
    auto find_it =commands_pool_.find(poolid);
    if ( find_it==commands_pool_.end())
        throw std::logic_error ("Set of commands_sequence incorrect!");
    current_commands_sequence_ = &find_it->second;
    current_command_seq_it_    = current_commands_sequence_->begin();
    check_end();
}

bool  m_model::end_commands ()
{
    return end_of_branch_;
}

const std::string
m_model::get_current_command()
{
    return current_command_;
}

const std::string
m_model::get_current_answer()
{
    return answer_;
}
