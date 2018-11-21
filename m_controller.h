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
        double I_SLD_SET;
        double T_SET_Ohm;
        double T_SET;
        double LIMIT;
        double I_SLD_REAL;
        double T_REAL_Ohm;
        double T_REAL;
        double PD_INT_average;
        double PD_INT_error;
        double PD_EXT_average;
        double PD_EXT_error;
    };
    /* MVC interface functions*/
    void setView(m_view* _view);
    void setModel(m_model* _model);
    void setInterruptFlag(std::atomic<bool>* interrupt_flag);

    /* interface functions*/
    void acqure_temperature();
    void acquire_25();
    void acquire_55();
    bool test_temperature();
    void do_branch_full();
    void single_command_execute(const std::string& command);

    void parse_and_push_into_result(const std::vector<std::string> splited_answer);
    std::vector<ResultsStorage> get_local_results_storage();
    ///interrupted sleep
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

    /* MVC members*/
    m_view* view;
    m_model* model;

    std::string                 _block_place;
    int                         _temperature_mode;
    int                         _receptacle;
    /// describe device's current state
    _MiniIOstate                _miniIOstate;
    /// delay before commands
    std::chrono::milliseconds   _delay;
    /// Device channels constant
    constexpr static size_t     _num_of_positions  {8} ;
    std::vector<ResultsStorage> _local_results_storage;
    std::atomic<bool>*          _stop_thread_flag_pointer{nullptr};
};
#endif // M_CONTROLLER_H
