
#include "common_std_headers.h"
#include <iomanip>

#include "m_model.h"
#include "m_view.h"
#include "m_controller.h"
#include "m_options_parser.h"
#include "m_log.hpp"

//TODO
//�������� �� abort()
//atexit((void*)foo) ��������
//�������� ���������� ���������� �������
using namespace std;
void foo()
{
    this_thread::sleep_for(5s);
};
//Uiliams page. 416
int main()
{
    //atexit (foo);
    ///global_results_storage is common for all threads
    vector<m_controller::ResultsStorage> global_results_storage;
    auto now_time_point = std::chrono::system_clock::now();

    ///read settings
    try
        {
            auto settings = Settings::SettingsParser().get_settings_struct("Portmap.ini", "Job.ini");
        }
    catch(exception& ex)
        {
            m_log()<< ex.what() <<'\n';
            return -1;
        }
    auto settings = Settings::SettingsParser().get_settings_struct("Portmap.ini", "Job.ini");
    /** Main thread process
    */
    auto process = [&global_results_storage](auto ss)
    {
        try
            {
                m_model m (ss);
                m_view v;
                m_controller c(ss);

                c.setModel(&m);
                c.setView(&v);
//throw m_exception_inf("goodbye!");
                c.acqure_temperature();
                if (c.test_temperature())
                    {
                        c.acqure_55();
                    }
                else
                    {
                        c.acqure_25();
                    }
                //thread-safe writing from local_results_storage to global_results_storage
                static mutex global_storage_mutex;
                {
                    unique_lock<mutex> ul(global_storage_mutex);
                    auto local_results_storage = c.get_local_results_storage();
                    std::move(local_results_storage.begin(),
                              local_results_storage.end(),
                              back_insert_iterator(global_results_storage));
                }
            }
        catch (exception& ex)
            {
                //any exception lead to exit thread
                m_log()<< ex.what() << '\n';
                return;
            }
    };

    /** Threads launch
        one thread to each position in options
    */
    vector<thread> pool;
    for (auto& ss : settings)
        {
            pool.emplace_back(process, ss);
        }
    for (auto& tr : pool)
        {
            tr.join();
        }
    //Wait threads finished
    //sort by position
    sort(global_results_storage.begin(), global_results_storage.end(),
         [](m_controller::ResultsStorage& value1, m_controller::ResultsStorage& value2)
    {
        return value1.Position < value2.Position;
    }
        );
    //
    auto log_name = m_log::make_name_from_time(now_time_point);
    ofstream out (log_name);//, ofstream::trunc);
    /** Printing to log
    */
    ///log header
    out << "Position" << " "
        "Serial" << " "
        "I_SLD_SET" << " "
        "T_SET_Ohm" << " "
        "T_SET" << " "
        "LIMIT" << " "
        "I_SLD_REAL" << " "
        "T_REAL_Ohm " << " "
        "T_REAL"  << " "
        "PD_INT_average" << " "
        "PD_INT_error" << " "
        "PD_EXT_average" << " "
        "PD_EXT_error" << '\n';

    ///log of measurments
    for (size_t i = 0; i < global_results_storage.size(); i++)
        {
            out << setprecision(3) << fixed << showpoint
                << global_results_storage[i].Position << " "
                << global_results_storage[i].Serial << " "
                << global_results_storage[i].I_SLD_SET << " "
                << global_results_storage[i].T_SET_Ohm << " "
                << global_results_storage[i].T_SET << " "
                << global_results_storage[i].LIMIT << " "
                << global_results_storage[i].I_SLD_REAL << " "
                << global_results_storage[i].T_REAL_Ohm << " "
                << global_results_storage[i].T_REAL << " "
                << global_results_storage[i].PD_INT_average << " "
                << global_results_storage[i].PD_INT_error << " "
                << global_results_storage[i].PD_EXT_average << " "
                << global_results_storage[i].PD_EXT_error << " "
                << '\n';
        }
    return 0;
}
