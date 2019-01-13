// TODO (digul0#1#): Add project description

#include "common_std_headers.h"
#include <signal.h>

#include "m_model.h"
#include "m_view.h"
#include "m_controller.h"
#include "m_options_parser.h"
#include "m_log.h"
#include "windows.h"

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

using namespace std;

//signal flag threads to prepare exit
static atomic<bool> stop_thread_flag{false};
void exit_handler(int event_id);
int run_acquisition_proc();

//Application entry
int main()
{
    run_acquisition_proc();
    return 0;
}

int run_acquisition_proc()
{
    //set callback function for emergency exit()
    signal(SIGINT, exit_handler);
    signal(SIGBREAK, exit_handler);
    // global_results_storage is common for all threads
    vector<m_controller::ResultsStorage> global_results_storage;
    // fixate app run timepoint
    auto now_time_point = chrono::system_clock::now();
    vector<settings::settings_struct> settings {};
    // try to read settings
    try
        {
            settings =
                settings::SettingsParser().get_settings_struct("Portmap.ini",
                        "Job.ini");
        }
    catch(exception& ex)
        {
            m_log() << ex.what() <<'\n';
            return -1;
        }
    //set answers log file name
    auto log_answers_name = m_log::make_name_from_time("Answers", now_time_point, ".txt");
    auto full_log_answers_name = string(".\\Answers\\") + log_answers_name;
    ofstream log_answers (full_log_answers_name);
    m_log::setLogfile(&log_answers);

    // Main thread process
    auto process = [&global_results_storage](auto settings_struct)
    {
        // do not brake init order: model, view, controller
        m_model model (settings_struct);
        m_view view;
        m_controller controller(settings_struct);
        view.setModel(&model);
        controller.setInterruptFlag(&::stop_thread_flag);
        controller.setModel(&model);
        controller.setView(&view);
        try
            {
                model.try_to_open_connection(3, 5s); //3 times, delay 5s
                controller.acqure_temperature();
                if (controller.test_temperature())
                    {
                        controller.acquire_55();
                    }
                else
                    {
                        controller.acquire_25();
                    }

            }
        catch (exception& ex)
            {
                // any exception lead to exit thread
                m_log()<< ex.what() << '\n';
                m_log(m_log::other::to_setted_log_file)<< ex.what() <<'\n';
                return;
            }
        // thread-safe writing from local_results_storage
        // to global_results_storage.
        {
            static mutex global_storage_mutex;
            unique_lock<mutex> ul(global_storage_mutex);
            auto local_results_storage = controller.get_local_results_storage();
            std::move(local_results_storage.begin(),
                      local_results_storage.end(),
                      back_insert_iterator(global_results_storage));
        }
        return;
    };

    // Threads launch
    // one thread to each position in options
    vector<thread> pool;
    for (const auto& ss : settings)
        {
            pool.emplace_back(process, ss);
        }

    // Wait until threads finished their work
    // and when sorting by position order.
    for (auto& tr : pool)
        {
            tr.join();
        }
    sort(global_results_storage.begin(), global_results_storage.end(),
         [](const m_controller::ResultsStorage& value1,
            const m_controller::ResultsStorage& value2)
    {
        return value1.Position < value2.Position;
    }
        );

    //set results log file name
    auto log_results_name = m_log::make_name_from_time("Results", now_time_point, ".txt");
    auto full_log_results_name = string(".\\Results\\") + log_results_name;
    ofstream out (full_log_results_name);

    // Printing to log
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

    // log of measurements.
    for (size_t i = 0; i < global_results_storage.size(); i++)
        {
            out << fixed
                << global_results_storage[i].Position << " "
                << global_results_storage[i].Serial << " "
                << setprecision(1) << showpoint << global_results_storage[i].I_SLD_SET      << " "
                << setprecision(0) << noshowpoint << global_results_storage[i].T_SET_Ohm    << " "
                << setprecision(3) << showpoint << global_results_storage[i].T_SET          << " "
                << setprecision(1) << showpoint << global_results_storage[i].LIMIT          << " "
                << setprecision(1) << showpoint << global_results_storage[i].I_SLD_REAL     << " "
                << setprecision(0) << noshowpoint << global_results_storage[i].T_REAL_Ohm   << " "
                << setprecision(3) << showpoint << global_results_storage[i].T_REAL         << " "
                << setprecision(3) << showpoint << global_results_storage[i].PD_INT_average << " "
                << setprecision(3) << showpoint << global_results_storage[i].PD_INT_error   << " "
                << setprecision(3) << showpoint << global_results_storage[i].PD_EXT_average << " "
                << setprecision(3) << showpoint << global_results_storage[i].PD_EXT_error   << '\n';
        }
    return 0;
}

//callback function for emergency exit() (close console, Ctrl+z, Ctrl+break and etc.)
void exit_handler(int)
{
    using namespace std::chrono_literals;
    auto delay_before_exit = 5s;
    stop_thread_flag.store(true);
    //compiler cut this wait foo
    this_thread::sleep_for(delay_before_exit);
    return ;
}




#ifdef BUILD_DLL
//Dll entry
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
    return TRUE; // successful
}

extern "C"
{
    int DLL_EXPORT run_acquisition();
    int DLL_EXPORT stop_acquisition();
}
//Dll API
int DLL_EXPORT run_acquisition()
{
    thread t (run_acquisition_proc);
    t.detach();
    return 0;
}

//Dll API
int DLL_EXPORT stop_acquisition()
{
    exit_handler(0);
    return 0;
}
#endif
