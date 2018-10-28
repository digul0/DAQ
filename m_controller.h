#ifndef M_CONTROLLER_H
#define M_CONTROLLER_H

#include "common_std_headers.h"


//forward declarations
class m_model;
class m_view;
namespace Settings
{
class settings_struct;
}

class m_controller
{
    /**
    * @brief
    * ������������ �������� ������ ����������: ���������� ���������� ������ com-����� � ������� ����������
    * ������, ������ ����������� �������, ��������� ������ � ������������ ������� �������� ������.
    * Provides the main application logic: control of the com-port polling algorithm using the model interface,
    * analysis of incoming responses, error handling and generation of an output data.
    */
public:
    explicit m_controller(const Settings::settings_struct& ss);
    m_controller(const m_controller&) = delete;
    m_controller& operator=(const m_controller&) = delete;
    ~m_controller();

    /** MVC interface functions*/
    void setView(m_view* _view);
    void setModel(m_model* _model);
    void setInterruptFlag(std::atomic<bool>* interrupt_flag);
public:
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
private:
    struct _MiniIOstate //enums mb?
    {
        int current_position;
        int current_mode;
        int current_temp;
    };
    /** interface functions*/
public:
    void acqure_temperature();
    void acqure_25();
    void acqure_55();
    bool test_temperature();
    ///experimental
    void experimental_measurements();
    void do_branch();
    void single_command_execute(const std::string& command);

    void parse_and_push_into_result(std::vector<std::string> res);
    std::vector<ResultsStorage> get_local_results_storage();

private:
    /** MVC members*/
    m_view* view;
    m_model* model;

private:
    std::string                 _block_place;
    int                         _temperature_mode;
    int                         _receptacle;
    _MiniIOstate                IOstate{};
    //delay before commands
    std::chrono::milliseconds   _delay;
    constexpr static size_t     _num_of_positions  {8} ; //Device channels constant
    std::vector<ResultsStorage> _local_results_storage;
    std::atomic<bool>*          _stop_thread_flag_pointer{nullptr};
};
#endif // M_CONTROLLER_H
