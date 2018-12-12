#ifndef M_MODEL_H
#define M_MODEL_H

#include "common_std_headers.h"


//forward declarations
namespace settings
{
    struct settings_struct;
}
class m_open_port;

/** Struct contained list of aviable commands for IO*/
struct Commands_list
{
    const std::string
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
};

class m_model
{
    /**
    * @brief
    * Provides an interface for selecting and executing a precompiled command flow,
    * as well as receiving a response from the com port.
    */
    using commands_sequence = std::vector<std::string>; // commands sequence
    using commands_pool = std::vector<commands_sequence>; // all commands sequence pool

public:
    enum class CommandsPoolId
    {
        acqure_temperature = 0,
        acquire_25 = 1,
        switch_up = 2,
        switch_down = 3,
        switch_to_default = 4
    };

    explicit m_model(const settings::settings_struct& ss );
    m_model(const m_model&) = delete;
    m_model& operator=(const m_model&) = delete;
    m_model(m_model&&);
    m_model& operator=(m_model&&);
    ~m_model();// = default;

    /** Struct contained list of aviable commands for IO*/
    const static inline Commands_list commands_list;

    /** state changing interface functions*/

    void choose_commands_pool(CommandsPoolId poolid);
    void execute_current_command ();
    void execute_single_command (const std::string& command);
    void go_next_command();
    bool end_commands ();
    bool open_connection();
    const std::string read_answer() ;
    const std::string get_current_command();
    const std::string get_current_answer();
private:
    void _commands_pool_init();
    void _check_end();
    int                           _port_num;
    std::unique_ptr<m_open_port>  _port_impl;
    /** state members*/
    commands_pool                _commands_pool;
    commands_sequence*           _current_commands_sequence;
    std::string                  _current_command;
    commands_sequence::iterator  _current_command_seq_it;
    std::string                  _answer;
    bool                         _end_of_branch{false};
};
#endif // M_MODEL_H
