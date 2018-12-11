#include <iomanip>
#include <cmath>

#include "m_controller.h"
#include "m_model.h" //!!!
#include "m_view.h"
#include "m_log.h"
#include "m_options_parser.h"

using namespace std::chrono_literals;
using std::stoi, std::log, std::stod, std::vector, std::string, std::to_string;
using std::logic_error;

m_controller::m_controller(const settings::settings_struct& ss):
    _block_place(ss.position),
    _temperature_mode(ss.temperature_mode),
    _receptacle(ss.receptacle),
    _miniIOstate({0,0,false}),
    _delay(500ms),
    _local_results_storage(vector<ResultsStorage>(_num_of_positions))
{
}
//  Destructor provide emergency exit for any close console events.
//  Do not add in m_controller::~m_controller() commands
//  with total duration > delay_before_exit value
//  in lock_ctrl_keys_exit(DWORD event_id).
m_controller::~m_controller()
{
    if(_miniIOstate.current_channel_number!= 0)
        {
            std::this_thread::sleep_for(_delay);
            single_command_execute(model->commands_list.go_1);
        }
    if(_miniIOstate.temp_was_switched)
        {
            std::this_thread::sleep_for(_delay);
            single_command_execute(model->commands_list.switch_55);
        }
}

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

void m_controller::do_branch_full()
{
    while(!model->end_commands())
        {
            // init try_counter for each command.
            size_t try_counter = 0;
            string answer ;

            // Handler of invalid answer event.
            do
                {
                    if (++try_counter > 3)
                        {
                            throw logic_error("IO error: invalid answer recieved three times!");
                        }
                    model->execute_current_command(); //try ex
                    std::this_thread::sleep_for(_delay);
                    answer = model->read_answer();
                }
            while (!view->is_valid_answer(answer));

            /** thread interrupt point here
              */
            if (_stop_thread_flag_pointer!=nullptr &&
                    _stop_thread_flag_pointer->load())  // global atomic variable from main thread.
                throw logic_error("Emergency interrupt");

            auto splited_answer = view->split_answer(answer);
            //
            _miniIOstate.state_changer(splited_answer);
//            m_log() << _miniIOstate.current_channel_number
//                  << _miniIOstate.current_mode
//                  << _miniIOstate.temp_was_switched
//                  << '\n';
            parse_and_push_into_result(splited_answer);
            m_log() << _block_place
                    << " : "  << model->get_current_command()
                    << " << " << model->get_current_answer()
                    << '\n';
            m_log(m_log::other::to_setted_log_file)
                    << _block_place
                    << " : "  << model->get_current_command()
                    << " << " << model->get_current_answer()
                    << '\n';
            model->go_next_command();
            std::this_thread::sleep_for(_delay);
        }
}
void m_controller::parse_and_push_into_result(const vector<string> splited_answer)
{
    auto from_Ohm_to_T = [](double Ohm)
    {
        //possible range thermoresistor resistance values, in Ohm
        if ((Ohm < 600) || (Ohm > 20000))
            return -273.15;
        return (-26.19*std::log(Ohm) + 265.7 ); // empiric formula.
    };

    enum { SET = 1, REAL = 2};
    auto command_name = splited_answer[0];
    if (command_name == "A0")
        {
            auto serial = splited_answer[4];
            for (size_t i = 0; i < _num_of_positions ; ++i )
                {
                    _local_results_storage[i].Serial = serial;
                    _local_results_storage[i].Position = _block_place + "-" + to_string(i + 1);
                }

        }
    else if (command_name == "A3")
        {

            int param_num      = std::stoi(splited_answer[1]);
            int ch_number      = std::stoi(splited_answer[2]) - 1; // 0
            int mode           = std::stoi(splited_answer[3]);
            double measured_data  = std::stod(splited_answer[4]);
            //check consistency between recieved commands and internal state of the device
            if (_miniIOstate.current_channel_number!= ch_number ||
                    _miniIOstate.current_mode!= mode
               )
                throw logic_error(string("Don't push any buttons on the device ")
                                  + _block_place + "\nwhile taking measurements!" );
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
    else // "A1, A2, A4, AE"
        {
            // don't need to compute and analyze anything.
        };

    return;
}

void m_controller::acqure_temperature()
{
    model->choose_commands_pool(m_model::CommandsPoolName::acqure_temperature);
    do_branch_full();
}



// if any _local_results_storage[i].T_SET value is
// outside spicified range or set skip the test
// by options requirements it return false.

// don't run before acqure_temperature().
bool m_controller::test_temperature()
{
    enum {TEMP_MODE_DEFAULT = 0, TEMP_MODE_NOSWITCH = 1 };
    if (_temperature_mode == TEMP_MODE_NOSWITCH )
        return false; // ->skip test by options requirements
    constexpr double deviance = 0.25 ; //%
    constexpr double ambient_temperature = 25.0;

    bool non_standart_temperature = false;
    for (size_t i = 0; i < _num_of_positions; ++i)
        {
            if ((_local_results_storage[i].T_SET > ambient_temperature *( 1 + deviance )) ||
                    (_local_results_storage[i].T_SET < ambient_temperature *( 1 - deviance )))
                non_standart_temperature = true;
        }
    return non_standart_temperature;
}
void m_controller::acquire_25()
{
    model->choose_commands_pool(m_model::CommandsPoolName::acquire_25);
    do_branch_full();
}


template <typename Time_duration, typename Call>
void sleep_for_with_condition(Time_duration t, Call foo)
{
    auto now_time = std::chrono::steady_clock::now();
    while( !foo() && (std::chrono::steady_clock::now() - now_time < t) )
        {
            std::this_thread::sleep_for(100ms);
        }
}
void m_controller::acquire_55()
{
    model->choose_commands_pool(m_model::CommandsPoolName::switch_down);
    do_branch_full();
    m_log()<<"Switch and balanced temperature ... wait." << '\n';
    // Wait while temperature balanced.
    // std::this_thread::sleep_for(60s);.
    sleep_for_with_condition(60s, [self = this]()
    {
        return self->_stop_thread_flag_pointer->load();
    }
                            );
    acquire_25();
    model->choose_commands_pool(m_model::CommandsPoolName::switch_up);
    do_branch_full();
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
void m_controller::_MiniIOstate::state_changer(const vector<string> splited_answer)
{
    // look at model regex schema
    const auto& command_name = splited_answer[0];
    if (command_name == "A1")
        {
            current_channel_number = stoi(splited_answer[1]) - 1; //0
        }
    else if (command_name == "A2")
        {
            current_mode = stoi(splited_answer[1]);
        }
    else if (command_name == "A4")
        {
            temp_was_switched = true;
        }
}
