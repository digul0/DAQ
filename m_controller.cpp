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
    //copy of settings::settings_struct
    settings_     (std::make_unique<settings::settings_struct>(ss)),
    //default device state
    miniIOstate_  ({0,0,false}),
    //default delay
    delay_        (500ms),
    //storage thread local measurements
    local_results_storage_     (vector<ResultsStorage>(num_of_positions_)),
    //pointer to atomic variable(for this thread interruption)
    stop_thread_flag_pointer_  {nullptr}
{
}
/** Destructor provide emergency exit for any close console events
 *  and handle exit procedure handlers
 *  @warning Do not add in m_controller::~m_controller() commands
 *  with total duration > delay_before_exit value
 *  in lock_ctrl_keys_exit(DWORD event_id).
 */
m_controller::~m_controller()
{
    if(miniIOstate_.current_channel_number!= 0)
        {
            std::this_thread::sleep_for(delay_);
            single_command_execute(model_->commands_list.go_1);
        }
    if(miniIOstate_.temp_was_switched)
        {
            std::this_thread::sleep_for(delay_);
            single_command_execute(model_->commands_list.switch_55);
        }
}



void m_controller::setView(m_view* view)
{
    view_ = view;
}



void m_controller::setModel(m_model* model)
{
    model_ = model;
}



void m_controller::setInterruptFlag(std::atomic<bool>* interrupt_flag)
{
    stop_thread_flag_pointer_ = interrupt_flag;
}


/**
Implements a polling algorithm using the model interface to manage it.
Validate and handle invalid incoming response.

*/
void m_controller::do_branch_full()
{
    while(!model_->end_commands())
        {
            // initialization try_counter for each command.
            size_t try_counter = 0;
            string answer ;

            // Handler of invalid answer event.
            // Validation proceed here.
            do
                {
                    if (++try_counter > 3)
                        {
                            throw logic_error("IO error: invalid answer received three times!");
                        }
                    model_->execute_current_command();
                    std::this_thread::sleep_for(delay_);
                    answer = model_->read_answer();
                }
            while (!view_->is_valid_answer(answer));

            /** thread interrupt point here
              */
            if (stop_thread_flag_pointer_!=nullptr &&
                    stop_thread_flag_pointer_->load())  // global atomic variable from main thread.
                throw logic_error("Emergency interrupt");

            auto splited_answer = view_->split_answer(answer);
            //
            miniIOstate_.change_state(splited_answer);
            parse_and_push_into_result(splited_answer);
            m_log()
                    << std::left << std::setw(6) << std::string("COM") + std::to_string(settings_->port)
                    << " (" << settings_->position <<")"
                    << " : "  << model_->get_current_command()
                    << " << " << model_->get_current_answer()
                    << '\n';
            m_log(m_log::other::to_setted_log_file)
                    << std::left << std::setw(6) << std::string("COM") + std::to_string(settings_->port)
                    << " (" << settings_->position <<")"
                    << " : "  << model_->get_current_command()
                    << " << " << model_->get_current_answer()
                    << '\n';
            model_->go_next_command();
            std::this_thread::sleep_for(delay_);
        }
}


/// Tokenize response and fill local_results_storage_ with these values.
/// Also check consistency between received commands and
/// internal state of the device by comparison with m_controller::miniIOstate_.
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
            for (size_t i = 0; i < num_of_positions_ ; ++i )
                {
                    local_results_storage_[i].Serial = serial;
                    local_results_storage_[i].Position = settings_->position + "-" + to_string(i + 1);
                }

        }
    else if (command_name == "A3")
        {

            int param_num      = std::stoi(splited_answer[1]);
            int ch_number      = std::stoi(splited_answer[2]) - 1; // converted to index
            int mode           = std::stoi(splited_answer[3]);
            double measured_data  = std::stod(splited_answer[4]);
            //check consistency between received commands and internal state of the device
            if (miniIOstate_.current_channel_number!= ch_number ||
                    miniIOstate_.current_mode!= mode
               )
                throw logic_error(string("Don't push any buttons on the device ")
                                  + settings_->position + "\nwhile taking measurements!" );
            if (mode == SET)
                {
                    if      (param_num == 1)
                        local_results_storage_[ch_number].I_SLD_SET          = measured_data / 10.0 ;
                    else if (param_num == 2)
                        {
                            local_results_storage_[ch_number].T_SET_Ohm      = measured_data ;
                            local_results_storage_[ch_number].T_SET          = from_Ohm_to_T(measured_data) ;
                        }
                    else if (param_num == 3)
                        local_results_storage_[ch_number].LIMIT              = measured_data / 10.0;
                    else if (param_num == 4) {}; //do nothing
                }
            else if (mode == REAL)
                {
                    if      (param_num == 1)
                        local_results_storage_[ch_number].I_SLD_REAL         = measured_data / 10.0 ;
                    else if (param_num == 2)
                        {
                            local_results_storage_[ch_number].T_REAL_Ohm     = measured_data;
                            local_results_storage_[ch_number].T_REAL         = from_Ohm_to_T(measured_data);
                        }
                    else if (param_num == 3)
                        local_results_storage_[ch_number].PD_INT_average     = measured_data / 1000.0;
                    else if (param_num == 4)
                        local_results_storage_[ch_number].PD_EXT_average     = measured_data / 1000.0;
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
    model_->choose_commands_pool(m_model::CommandsPoolId::acqure_temperature);
    do_branch_full();
}

// if any local_results_storage_[i].T_SET value is
// outside specified range or set skip the test
// by options requirements it return false.

/// @warning don't run before acquire_temperature().
bool m_controller::test_temperature()
{
    enum {TEMP_MODE_DEFAULT = 0, TEMP_MODE_NOSWITCH = 1 };
    if (settings_->temperature_mode == TEMP_MODE_NOSWITCH )
        return false; // ->skip test by options requirements
    constexpr double deviance            = 0.25; //%
    constexpr double ambient_temperature = 25.0;

    bool non_standart_temperature = false;
    for (size_t i = 0; i < num_of_positions_; ++i)
        {
            if ((local_results_storage_[i].T_SET > ambient_temperature *( 1 + deviance )) ||
                    (local_results_storage_[i].T_SET < ambient_temperature *( 1 - deviance )))
                non_standart_temperature = true;
        }
    return non_standart_temperature;
}



void m_controller::acquire_25()
{
    model_->choose_commands_pool(m_model::CommandsPoolId::acquire_25);
    do_branch_full();
}


///interrupted sleep
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
    model_->choose_commands_pool(m_model::CommandsPoolId::switch_down);
    do_branch_full();

    m_log()<<"Switch and balanced temperature ... wait." << '\n';

    // Wait while temperature balanced.
    // std::this_thread::sleep_for(60s);.
    sleep_for_with_condition(60s, [self = this]()
    {
        return self->stop_thread_flag_pointer_->load();
    }
                            );
    acquire_25();
    model_->choose_commands_pool(m_model::CommandsPoolId::switch_up);
    do_branch_full();
}


/// Used to return the copy of measurements data storage to main thread
std::vector<m_controller::ResultsStorage>
m_controller::get_local_results_storage()
{
    return local_results_storage_;
}


/**  @brief Execute separate command
 *   @warning this function do not change miniIOstate_!
 *   don't use it nowhere but destructor!
 */
void m_controller::single_command_execute(const string& command)
{
    model_->execute_single_command(command);
}



void m_controller::MiniIOstate::change_state(const vector<string> splited_answer)
{
    // look at model_ regex schema
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
