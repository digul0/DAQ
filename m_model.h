#ifndef M_MODEL_H
#define M_MODEL_H

#include "common_std_headers.h"


//forward declarations
namespace Settings
{
    struct settings_struct;
}
class m_open_port;


class m_model
{
    /**
    * @brief
    * Обеспечивает интерфейс для выбора и исполнения предварительно скомпилированного
    * потока команд, а так же получения ответа от com-порта.
    * Provides an interface for selecting and executing a precompiled command flow,
    * as well as receiving a response from the com port.
    */
private:
    using command_impl_type = std::string;
    using answer_imp_type   = command_impl_type;
    using commands_sequence = std::vector<command_impl_type>; // commands sequence
    using commands_pool = std::vector<commands_sequence>; // all commands sequence pool

public:
    enum class CommandsPoolName
    {
        acqure_temperature = 0,
        acqure_25 = 1,
        switch_up = 2,
        switch_down = 3,
        switch_to_default = 4
    };

public:
    explicit m_model(const Settings::settings_struct& ss );
    m_model(const m_model&) = delete;
    m_model& operator=(const m_model&) = delete;
    ~m_model();// = default;

    /** Struct contained list of aviable commands for IO*/
public:
    const struct _commands_list
    {
        const command_impl_type
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
    void choose_commands_pool(CommandsPoolName choose);
    void execute_single_command (const std::string& command);
    void execute_current_command ();
    answer_imp_type  read_answer() ;
    void go_next_command();
    bool end_commands ();
    const answer_imp_type get_current_command();
    const answer_imp_type get_current_answer();
private:
    void _commands_pool_init();
    void _check_end();
    /** MVC interface functions*/

    /** state members*/
private:
    std::unique_ptr<m_open_port>  _port;
    commands_pool                _commands_pool;
    commands_sequence*           _current_commands_sequence;
    command_impl_type            _current_command;
    commands_sequence::iterator  _current_command_seq_it;
    answer_imp_type              _answer;
    bool                         _end_of_branch{false};
};
#endif // M_MODEL_H
