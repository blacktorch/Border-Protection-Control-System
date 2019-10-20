//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Messages structures
#include "../data_structures/threat_details.hpp"

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/search_and_locate_threat.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/

/***** Define output ports for coupled model *****/
struct top_out: public out_port<Threat_t>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Int : public iestream_input<int,T> {
public:
    InputReader_Int () = default;
    InputReader_Int (const char* file_path) : iestream_input<int,T>(file_path) {}
};

int main(){

    /****** Input Reader atomic model instantiation *******************/
    const char * i_input_data = "../input_data/search_and_locate_input_test.txt";
    shared_ptr<dynamic::modeling::model> input_reader;
    input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char*>("input_reader", move(i_input_data));

    /****** Search_And_Locate atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> search_and_locate;
    search_and_locate = dynamic::translate::make_dynamic_atomic_model<SearchAndLocateThreat, TIME>("search_and_locate");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};
    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(top_out)};
    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_reader, search_and_locate};
    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
            dynamic::translate::make_EOC<SearchAndLocateThreat_defs::found,top_out>("search_and_locate")
    };
    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
            dynamic::translate::make_IC<iestream_input_defs<int>::out,SearchAndLocateThreat_defs::uav_alerted>("input_reader","search_and_locate")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
            "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/search_and_locate_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/search_and_locate_test_output_state.txt");
    struct oss_sink_state{
        static ostream& sink(){
            return out_state;
        }
    };

    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    /************** Runner call ************************/
    dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(NDTime("04:00:00:000"));
    return 0;
}


