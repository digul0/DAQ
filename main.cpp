
#include "common_std_headers.h"
#include <iomanip>

#include "m_model.h"
#include "m_view.h"
#include "m_controller.h"
#include "m_options_parser.h"
#include "m_log.hpp"
/*
#include "m_model.h"
обеспечивает интерфейс для выбора и исполнения предварительно скомпилированного потока команд,
а так же получения ответа от com-порта.

Provides an interface for selecting and executing a precompiled command flow, as well as receiving a response from the com port.

#include "m_view.h"
Ћбеспечивает валидацию ответа и разбиению(trim/tagging) цельного ответа на составляющие согласно заданному шаблону

Provides validation of the response and splitting a complete response to the components according to a specified regular expressions.

#include "m_controller.h"
Ћбеспечивает основную логику приложения: управление алгоритмом опроса com-порта с помощью интерфейса
модели, анализ поступивших ответов, обработка ошибок и формирование массива выходных данных.

Provides the main application logic: control of the com-port polling algorithm using the model interface,
analysis of incoming responses, error handling and generation of an output data.

#include "m_options_parser.h"
Ћбеспечивает парсинг файлов текстовых настроек (вшитые Portmap.ini, Job.ini),
выборку (aka SQL "SELECT [Portmap.ini].* FROM [Job.ini], [Portmap.ini] WHERE [Portmap.ini].Position=[Job.ini].Position)
и передача результата пользователю.
Provides parsing of text settings files (Portmap.ini, Job.ini),
sampling (aka SQL "SELECT [Portmap.ini]. * FROM [Job.ini], [Portmap.ini] WHERE [Portmap.ini] .Position = [Job.ini] .Position") and transfer the result to the user.

#include "m_open_port.hpp"
Ћбеспечивает настройку, подключение и интерфейс синхронного чтения/записи в Com-порт.
Provides configuration, connection and synchronous read / write interface to the Com port.

#include "m_log.hpp"
Ћбеспечивает многопоточный вывод в консоль
Provides multi-threaded console output
*/
/**
CMake
Refactoring
*/


//TODO
// лог файл многократно переписывается -done
//мб call_once() для очистки лога -done
//передача настроек во все классы -done
//действия на abort()
//atexit((void*)foo) работает
//обработка ошибок
//механизм аварийного завершение потоков
using namespace std;

int main()
{
    vector<m_controller::ResultsStorage> global_results_storage;
    //vector<Settings::settings_struct>
    //Port Position Receptacle Temperature
// TODO (testuser#1#): make exeption or <optional>
    auto settings = Settings::SettingsParser().get_settings_struct("Portmap.ini", "Job.ini");
    auto now_time_point = std::chrono::system_clock::now();

    /** Main process
    */
    auto process = [&global_results_storage](auto ss)
    {
        //settings
        m_model m (ss);
        m_view v;
        m_controller c(ss);

        c.setModel(&m);
        c.setView(&v);

        try
            {

                c.acqure_temperature();
                if (c.test_temperature())
                    {
                        c.acqure_55();
                    }
                else
                    {
                        c.acqure_25();
                    }

            }
        catch (...)
            {
                //closing thread only
                //TODO adequate err handler
                return ;
            }

        static mutex global_storage_mutex;
        {
            unique_lock<mutex> ul(global_storage_mutex);
            auto local_results_storage = c.get_local_results_storage();
            std::move(local_results_storage.begin(),
                      local_results_storage.end(),
                      back_insert_iterator(global_results_storage));

        }

        /*
        //TODO handle exit
        m.choose_commands_pool(m_model::CommandsPoolName::switch_up);
        c.do_branch();
        */
        return ;
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
    /*
        for (size_t i = 0; i < global_results_storage.size(); i++)
            {
                out << setprecision(3) << fixed << showpoint
                    << global_results_storage[i].Position << " "
                    << global_results_storage[i].Serial << " "
                    << global_results_storage[i].PD_INT_average << " "
                    << setprecision(1)
                    << global_results_storage[i].I_SLD_REAL << " "
                    << global_results_storage[i].T_REAL << " "
                    << '\n';
            } //TODO show that
    */
    return 0;
}
