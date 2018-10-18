#include "common_std_headers.hpp"
#include "m_options_parser.h"
//#include "m_view.h"
class m_model;
class m_view;


class m_controller
{

public:
    /** Default constructor */
    explicit m_controller(Settings::settings_struct ss);
    m_controller(const m_controller&) = delete;
    operator=(const m_controller&) = delete;
    /** Default destructor */
    ~m_controller();

    /** MVC interface functions*/
    void setView(m_view* _view)
    {
        view = _view;
    }
    void setModel(m_model* _model)
    {
        model = _model;
    }
public:
    struct _results_storage
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
    //void result_analyse(parse_result_type res);
    void parse_and_push_into_result(std::vector<std::string> res );


    void acqure_temperature();
    void acqure_25();
    void acqure_55();
    bool test_temperature();
    void experimental_measurements(); ///experimental
    void do_branch();

    void print_results();
    //std::unique_ptr<_results_storage[]> get_local_results_storage();
    std::vector<_results_storage> get_local_results_storage();

private:
    /** MVC members*/
    m_view* view;
    m_model* model;

private:
    std::string                 _block_place;
    int                         temperature_mode;
    int                         _receptacle;
    _MiniIOstate                IOstate{};
    //delay before commands
    std::chrono::milliseconds   _delay;
    constexpr static size_t     num_of_positions{ 8 }; //Device channels constant
    std::vector<_results_storage> local_results_storage;

};
