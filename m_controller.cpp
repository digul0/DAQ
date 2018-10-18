#include <iomanip>
#include <fstream>

#include "m_controller.h"
#include "m_model.h" //!!!
#include "m_view.h"
#include "m_log.hpp"
#include <cassert>
using namespace std::chrono_literals;
using std::stoi, std::log, std::stod;

m_controller::m_controller(Settings::settings_struct ss):
    _block_place(ss.position), temperature_mode(ss.temperature_mode), _receptacle(ss.receptacle),
    _delay(500ms),
    local_results_storage(std::move(std::vector<_results_storage>(num_of_positions) ))
{

}
m_controller::~m_controller()
{

}
using namespace std;

void m_controller::do_branch()
{
    while(!model->end_commands())
        {
          size_t try_counter {1};
          string answer ;
          do
            {
              if (try_counter++ > 3) { m_log()<< model->get_current_command()<<" 3-times fold"<<'\n'; } //add exit code

              model->execute_command();
              this_thread::sleep_for(_delay);
              answer = model->read_answer();
            } while (!view->is_valid_answer(answer));

            //TODO valid test if not execute_command()

            auto res = view->split_answer(answer);
            parse_and_push_into_result(res);
            m_log() << model->get_current_command()<< " << " << model->get_current_answer()<<'\n';
            model->go_next_command();
            this_thread::sleep_for(_delay);
        }
}
void m_controller::parse_and_push_into_result(std::vector<std::string> res )
{
    //check size res
    auto from_Ohm_to_T = [](double Ohm)
    {
        if (Ohm < 0.01)
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
                    local_results_storage[i].Serial = serial;
                    local_results_storage[i].Position = _block_place + "-" + to_string(i + 1);
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
                        local_results_storage[ch_number].I_SLD_SET          = measured_data / 10.0 ;
                    else if (param_num == 2)
                        {
                            local_results_storage[ch_number].T_SET_Ohm          = measured_data ;
                            local_results_storage[ch_number].T_SET              = from_Ohm_to_T(measured_data) ;
                        }
                    else if (param_num == 3)
                        local_results_storage[ch_number].LIMIT              = measured_data / 10.0;
                    else if (param_num == 4) {}; //do nothing
                }
            else if (mode == REAL)
                {
                    if      (param_num == 1)
                        local_results_storage[ch_number].I_SLD_REAL         = measured_data / 10.0 ;
                    else if (param_num == 2)
                        {
                            local_results_storage[ch_number].T_REAL_Ohm         = measured_data;
                            local_results_storage[ch_number].T_REAL             = from_Ohm_to_T(measured_data);
                        }
                    else if (param_num == 3)
                        local_results_storage[ch_number].PD_INT_average     = measured_data / 1000.0;
                    else if (param_num == 4)
                        local_results_storage[ch_number].PD_EXT_average     = measured_data / 1000.0;
                }
            //go next command
        }
    else //"A1, A2, A4"
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
    model->choose_commands_pool(m_model::test::acqure_temperature); //ужс
    do_branch();
}
bool m_controller::test_temperature()
{
    enum {TEMP_MODE_DEFAULT = 0, TEMP_MODE_NOSWITCH = 1 };
    if (temperature_mode == TEMP_MODE_NOSWITCH )
        return false; // ->skip test by options requirements
    constexpr double deviance = 0.25 ; //%
    constexpr double ambient_temperature = 25.0;

    bool res = false;
    for (size_t i = 0; i < num_of_positions; i++)
        {
            if ((local_results_storage[i].T_SET > ambient_temperature *( 1 + deviance )) ||
                    (local_results_storage[i].T_SET < ambient_temperature *( 1 - deviance )))
                res = true;
        }
    return res;
}
void m_controller::print_results()
{
    const string log_name = "123.log";
    static once_flag flag {}; //mb not thread safe
    //log clear
    call_once( flag, [&log_name]()
    {
        ofstream (log_name, ofstream::trunc);
    } );
    ofstream logfile (log_name, ofstream::app);
    auto&& out = m_log(logfile); ///???
    for (size_t i = 0; i < num_of_positions; i++)
        {
            std::move(out) << setprecision(3) << fixed << showpoint
                           << local_results_storage[i].Position << " "
                           << local_results_storage[i].Serial << " "
                           << local_results_storage[i].PD_INT_average << " "
                           << setprecision(1)
                           << local_results_storage[i].I_SLD_REAL << " "
                           << local_results_storage[i].T_REAL << " "
                           << '\n';
        }
    //int i = m_log::_failure_to_read_port_Counter;
    //std::move(out) << i <<'\n';

}
void m_controller::acqure_25()
{
    //delay = 500ms;
    model->choose_commands_pool(m_model::test::acqure_25);
    do_branch();
}
//остается переключенным на +25 если оборвать опрос - bug!
void m_controller::acqure_55()
{
    //delay = 500ms;
    model->choose_commands_pool(m_model::test::switch_down);
    do_branch();
    //Wait while temperature balanced.
    this_thread::sleep_for(60s);
    acqure_25();
    model->choose_commands_pool(m_model::test::switch_up);
    do_branch();
}
void m_controller::experimental_measurements()
{


}
std::vector<m_controller::_results_storage>
m_controller::get_local_results_storage()
{
    return local_results_storage;
}
