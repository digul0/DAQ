#include <iomanip>
#include <fstream>
#include <cmath>

#include "m_controller.h"
#include "m_model.h" //!!!
#include "m_view.h"
#include "m_log.h"
#include "m_options_parser.h"

using namespace std::chrono_literals;
using std::stoi, std::log, std::stod, std::vector, std::string, std::to_string;

m_controller::m_controller(const Settings::settings_struct& ss):
    _block_place(ss.position), _temperature_mode(ss.temperature_mode), _receptacle(ss.receptacle),
    _delay(500ms),
    _local_results_storage(std::move(vector<ResultsStorage>(_num_of_positions) ))
{
}
//  Obtain emergency exit for any close console events.
//  Do not add in m_controller::~m_controller()commands
//  with total duration > delay_before_exit value
//  in lock_ctrl_keys_exit(DWORD event_id)
m_controller::~m_controller()
{

  std::this_thread::sleep_for(_delay);
  single_command_execute(model->commands_list.switch_55);
  std::this_thread::sleep_for(_delay);
  single_command_execute(model->commands_list.go_1);
}// = default;

void m_controller::setView(m_view* _view)
{
    view = _view;
}
void m_controller::setModel(m_model* _model)
{
    model = _model;
}
void m_controller::setInterruptFlag(std::atomic<bool>* interrupt_flag)
{
  _stop_thread_flag_pointer = interrupt_flag;
}

void m_controller::do_branch()
{
    while(!model->end_commands())
        {
            //init try_counter for each command
            size_t try_counter = 0;
            string answer ;

            //Handler of invalid answer event
            do
                {
                    if (++try_counter > 3)
                        {
                            throw m_exception_inf("IO error: try counter > 3");
                        }
                    model->execute_current_command();
                    std::this_thread::sleep_for(_delay);
                    answer = model->read_answer();
                }
            while (!view->is_valid_answer(answer));
            /** ugly hook but it sufficient for simplify the code.
              *
              */
            if (_stop_thread_flag_pointer!=nullptr && _stop_thread_flag_pointer->load())  //global atomic variable from main thread
              throw m_exception_inf("Emergency interrupt");

            auto res = view->split_answer(answer);
            parse_and_push_into_result(res);
            m_log() << model->get_current_command()<< " << " << model->get_current_answer()<<'\n';
            model->go_next_command();
            std::this_thread::sleep_for(_delay);
        }
}
void m_controller::parse_and_push_into_result(vector<string> res)
{
    auto from_Ohm_to_T = [](double Ohm)
    {
      ///allowed thermoresistor resistance value range in Ohm
        if ((Ohm < 600) || (Ohm > 20000))
            return -273.15;
        return (-26.19*std::log(Ohm) + 265.7 ); //empiric formula
    };

    enum { SET = 1, REAL = 2};
    auto command_name = res[0];
    if (command_name == "A0")
        {
            auto serial = res[4];
            for (size_t i = 0; i < 8 ; ++i )
                {
                    _local_results_storage[i].Serial = serial;
                    _local_results_storage[i].Position = _block_place + "-" + to_string(i + 1);
                }

        }
    else if (command_name == "A3")
        {

            int param_num      = std::stoi(res[1]);
            int ch_number      = std::stoi(res[2]) - 1; // 0
            int mode           = std::stoi(res[3]);
            double measured_data  = std::stod(res[4]);
            if (mode == SET)
                {
                    if      (param_num == 1)
                        _local_results_storage[ch_number].I_SLD_SET          = measured_data / 10.0 ;
                    else if (param_num == 2)
                        {
                            _local_results_storage[ch_number].T_SET_Ohm          = measured_data ;
                            _local_results_storage[ch_number].T_SET              = from_Ohm_to_T(measured_data) ;
                        }
                    else if (param_num == 3)
                        _local_results_storage[ch_number].LIMIT              = measured_data / 10.0;
                    else if (param_num == 4) {}; //do nothing
                }
            else if (mode == REAL)
                {
                    if      (param_num == 1)
                        _local_results_storage[ch_number].I_SLD_REAL         = measured_data / 10.0 ;
                    else if (param_num == 2)
                        {
                            _local_results_storage[ch_number].T_REAL_Ohm         = measured_data;
                            _local_results_storage[ch_number].T_REAL             = from_Ohm_to_T(measured_data);
                        }
                    else if (param_num == 3)
                        _local_results_storage[ch_number].PD_INT_average     = measured_data / 1000.0;
                    else if (param_num == 4)
                        _local_results_storage[ch_number].PD_EXT_average     = measured_data / 1000.0;
                }
        }
    else //"A1, A2, A4, AE"
        {
            //IOstate changing need
            //go next command
            //don't need to compute and analyze anything
            //TODO AE handler!!!
        };

    return;
}

void m_controller::acqure_temperature()
{
    //delay = 500ms;
    model->choose_commands_pool(m_model::CommandsPoolName::acqure_temperature);
    do_branch();
}
bool m_controller::test_temperature()
{
    enum {TEMP_MODE_DEFAULT = 0, TEMP_MODE_NOSWITCH = 1 };
    if (_temperature_mode == TEMP_MODE_NOSWITCH )
        return false; // ->skip test by options requirements
    constexpr double deviance = 0.25 ; //%
    constexpr double ambient_temperature = 25.0;

    bool res = false;
    for (size_t i = 0; i < _num_of_positions; ++i)
        {
            if ((_local_results_storage[i].T_SET > ambient_temperature *( 1 + deviance )) ||
                    (_local_results_storage[i].T_SET < ambient_temperature *( 1 - deviance )))
                res = true;
        }
    return res;
}
void m_controller::acqure_25()
{
    model->choose_commands_pool(m_model::CommandsPoolName::acqure_25);
    do_branch();
}

//for interrupted sleep
template <typename Time_duration, typename Call>
void sleep_for_with_condition(Time_duration t, Call foo)
{
    auto now_time = std::chrono::steady_clock::now();
    while( !foo() && (std::chrono::steady_clock::now() - now_time < t) )
    {
      std::this_thread::sleep_for(500ms);
    }
}
void m_controller::acqure_55()
{
    model->choose_commands_pool(m_model::CommandsPoolName::switch_down);
    do_branch();
    m_log()<<"Switch and balanced temperature ... wait." << '\n';
    //Wait while temperature balanced.
    //std::this_thread::sleep_for(60s);.
    sleep_for_with_condition(60s, [self = this]()
                             {
                               return self->_stop_thread_flag_pointer->load();
                             }
                             );
    acqure_25();
    model->choose_commands_pool(m_model::CommandsPoolName::switch_up);
    do_branch();
}
void m_controller::experimental_measurements()
{


}
std::vector<m_controller::ResultsStorage>
m_controller::get_local_results_storage()
{
    return _local_results_storage;
}
void m_controller::single_command_execute(const string& command)
{
  model->execute_single_command(command);
}
