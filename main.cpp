
#include "common_std_headers.h"
#include <iomanip>
#include <signal.h>
#include <iostream>

#include "m_model.h"
#include "m_view.h"
#include "m_controller.h"
#include "m_options_parser.h"
#include "m_log.h"

#include "windows.h"
// TODO (digul0#1#): Add project description
#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT
#endif
using namespace std;

extern "C"
int DLL_EXPORT main_process();
int main()
{
  main_process();
  return 0;
}

//signal flag threads to prepare exit
static atomic<bool> stop_thread_flag{false};
//callback function for emergency exit() (close console, ctrl+z, ctrl+break and etc.)
void exit_handler(int event_id);

int DLL_EXPORT main_process()
{
    // global_results_storage is common for all threads
    vector<m_controller::ResultsStorage> global_results_storage;
    // fixate run timepoint
    auto now_time_point = std::chrono::system_clock::now();
    vector<Settings::settings_struct> settings {};
    // try to read settings
    try
        {
            settings =
            Settings::SettingsParser().get_settings_struct("Portmap.ini",
                                                                       "Job.ini");
        }
    catch(exception& ex)
        {
            m_log()<< ex.what() <<'\n';
            return -1;
        }
    // set callback for emergency exit()
    signal(SIGINT, exit_handler);
    signal(SIGBREAK, exit_handler);
    auto log_answers_name = m_log::make_name_from_time("Answers", now_time_point, ".txt");
    ofstream log_answers (log_answers_name);
    m_log::setLogfile(&log_answers);

    /* Main thread process
    */
    auto process = [&global_results_storage](auto settings_struct)
    {
        try
            {
              // do not brake init order: model, view, controller
                m_model m (settings_struct);
                m_view v;
                m_controller c(settings_struct);

                v.setModel(&m);
                c.setInterruptFlag(&stop_thread_flag);
                c.setModel(&m);
                c.setView(&v);

                c.acqure_temperature();
                if (c.test_temperature())
                    {
                        c.acquire_55();
                    }
                else
                    {
                        c.acquire_25();
                    }
                // thread-safe writing from local_results_storage
                // to global_results_storage.
                {
                    static mutex global_storage_mutex;
                    unique_lock<mutex> ul(global_storage_mutex);
                    auto local_results_storage = c.get_local_results_storage();
                    std::move(local_results_storage.begin(),
                              local_results_storage.end(),
                              back_insert_iterator(global_results_storage));
                }
            }
        catch (exception& ex)
            {
                // any exception lead to exit thread
                m_log()<< ex.what() << '\n';
                m_log(m_log::other::to_setted_log_file)<< ex.what() <<'\n';
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
    // Wait untile threads finished
    // and when sorting by position order.
    sort(global_results_storage.begin(), global_results_storage.end(),
         [](m_controller::ResultsStorage& value1, m_controller::ResultsStorage& value2)
    {
        return value1.Position < value2.Position;
    }
        );
    // return log filename as "Results%Y_%m_%d-%H%M%S.txt".
    auto log_results_name = m_log::make_name_from_time("Results", now_time_point, ".txt");
    ofstream out (log_results_name);
    /** Printing to log
    */
    // log table header.
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

    // log of measurments.
    for (size_t i = 0; i < global_results_storage.size(); i++)
        {
     out << fixed
         << global_results_storage[i].Position << " "
         << global_results_storage[i].Serial << " "
         << setprecision(1) << showpoint << global_results_storage[i].I_SLD_SET << " "
         << setprecision(0) << noshowpoint << global_results_storage[i].T_SET_Ohm << " "
         << setprecision(3) << showpoint << global_results_storage[i].T_SET << " "
         << setprecision(1) << showpoint << global_results_storage[i].LIMIT << " "
         << setprecision(1) << showpoint << global_results_storage[i].I_SLD_REAL << " "
         << setprecision(0) << noshowpoint << global_results_storage[i].T_REAL_Ohm << " "
         << setprecision(3) << showpoint << global_results_storage[i].T_REAL << " "
         << setprecision(3) << showpoint << global_results_storage[i].PD_INT_average << " "
         << setprecision(3) << showpoint << global_results_storage[i].PD_INT_error << " "
         << setprecision(3) << showpoint << global_results_storage[i].PD_EXT_average << " "
         << setprecision(3) << showpoint << global_results_storage[i].PD_EXT_error << " "
         << '\n';
        }
    return 0;
}
void exit_handler(int event_id)
{
  using namespace std::chrono_literals;
  auto delay_before_exit = 5s;
  stop_thread_flag.store(true);
  //compiler cut this wait foo
  std::this_thread::sleep_for(delay_before_exit);
  return ;
}
#ifdef BUILD_DLL
extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
#endif
