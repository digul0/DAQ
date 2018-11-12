#ifndef M_CONTROLLER_H
#define M_CONTROLLER_H

#include "common_std_headers.h"


// forward declarations
class m_model;
class m_view;
namespace Settings
{
struct settings_struct;
}

class m_controller
{
    /**
    * @brief
    * Provides the main application logic: control of the com-port polling algorithm using the model interface,
    * analysis of incoming responses, error handling and generation of an output data.
    */
public:
    explicit m_controller(const Settings::settings_struct& ss);
    m_controller(const m_controller&) = delete;
    m_controller& operator=(const m_controller&) = delete;
    ~m_controller();

    struct ResultsStorage
    {
        std::string Position;
        std::string Serial;
        double I_SLD_SET      = 0;
        double T_SET_Ohm      = 0;
        double T_SET          = 0;
        double LIMIT          = 0;
        double I_SLD_REAL     = 0;
        double T_REAL_Ohm     = 0;
        double T_REAL         = 0;
        double PD_INT_average = 0;
        double PD_INT_error   = 0;
        double PD_EXT_average = 0;
        double PD_EXT_error   = 0;
    };
    /** MVC interface functions*/
    void setView(m_view* _view);
    void setModel(m_model* _model);
    void setInterruptFlag(std::atomic<bool>* interrupt_flag);

    /** interface functions*/
    void acqure_temperature();
    void acquire_25();
    void acquire_55();
    bool test_temperature();
    void do_branch_full();
    void single_command_execute(const std::string& command);

    void parse_and_push_into_result(const std::vector<std::string> splited_answer);
    std::vector<ResultsStorage> get_local_results_storage();
    template <typename Time_duration, typename Call>
    friend  void sleep_for_with_condition(Time_duration t, Call foo);
private:
    struct _MiniIOstate
    {
        int current_channel_number;
        int current_mode;
        bool temp_was_switched;

        void state_changer(const std::vector<std::string> splited_answer);
    };
    friend class m_controller::_MiniIOstate;

    /** MVC members*/
    m_view* view;
    m_model* model;

    std::string                 _block_place;
    int                         _temperature_mode;
    int                         _receptacle;
    _MiniIOstate                _miniIOstate;// {0,0,false};
    // delay before commands
    std::chrono::milliseconds   _delay;
    constexpr static size_t     _num_of_positions  {8} ; //Device channels constant
    std::vector<ResultsStorage> _local_results_storage;
    std::atomic<bool>*          _stop_thread_flag_pointer{nullptr};
};
#endif // M_CONTROLLER_H
