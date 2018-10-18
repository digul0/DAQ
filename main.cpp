
#include "common_std_headers.hpp"

#include "m_model.h"
#include "m_view.h"
#include "m_controller.h"
#include "m_options_parser.h"
//#include "m_open_port.hpp"

#include <iomanip>
/**
Precompiled headers
CMake
Refactoring
*/
using namespace std;

//TODO
// лог файл многократно переписывается
//мб call_once() для очистки лога
//передача настроек во все классы
//действия на abort()
//atexit((void*)foo) работает
//обработка ошибок
int main()
{
  vector<m_controller::_results_storage> global_results_storage;
  auto settings = Settings::SettingsParser().get_settings_struct("Portmap.ini", "Job.ini");
  //Port Position Receptacle Temperature

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

    // mb add common class to init all this
    c.acqure_temperature();
    if (c.test_temperature())
    {
      c.acqure_55();
    }
    else
    {
      c.acqure_25();
    }

    static mutex global_storage_mutex;

    {
      lock_guard<mutex> lg(global_storage_mutex);
      auto local_results_storage = c.get_local_results_storage();
      std::move(local_results_storage.begin(),
                local_results_storage.end(),
                back_insert_iterator(global_results_storage)); ///

    }

    /*
    //TODO handle exit
    m.choose_commands_pool(m_model::test::switch_up);
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

  //sort by position
  sort(global_results_storage.begin(), global_results_storage.end(), [](m_controller::_results_storage& value1, m_controller::_results_storage& value2)
                                                                        {
                                                                          return value1.Position < value2.Position;
                                                                        }
      );
  const string log_name = "123.log";
  ofstream out (log_name);//, ofstream::trunc);
  /** Printing
  */

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

return 0;
}
