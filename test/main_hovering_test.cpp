//
// Created by Chidiebere Onyedinma on 2019-10-17.
//
//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/hovering.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
struct inp_inId : public cadmium::in_port<int>{};
struct inp_threat_details_sent : public cadmium::in_port<bool>{};

/***** Define output ports for coupled model *****/
struct outp_uav_alerted : public cadmium::out_port<int>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Bool : public iestream_input<bool,T> {
public:
    InputReader_Bool() = default;
    InputReader_Bool(const char* file_path) : iestream_input<bool ,T>(file_path) {}
};

template<typename T>
class InputReader_Int : public iestream_input<int,T> {
public:
    InputReader_Int () = default;
    InputReader_Int (const char* file_path) : iestream_input<int,T>(file_path) {}
};

int main(){


    /****** Input Reader atomic models instantiation *******************/
    const char * i_input_data_in_id = "../input_data/hovering_input_test_in_id.txt";
    shared_ptr<dynamic::modeling::model> input_reader_id = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader_id" , move(i_input_data_in_id));

    const char * i_input_data_threat_details_sent = "../input_data/hovering_input_test_threat_details_sent.txt";
    shared_ptr<dynamic::modeling::model> input_reader_details_sent = dynamic::translate::make_dynamic_atomic_model<InputReader_Bool , TIME, const char* >("input_reader_details_sent" , move(i_input_data_threat_details_sent));


    /****** Hovering atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> hover = dynamic::translate::make_dynamic_atomic_model<Hovering, TIME>("hover");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(outp_uav_alerted)};
    dynamic::modeling::Models submodels_TOP = {input_reader_id, input_reader_details_sent, hover};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
            dynamic::translate::make_EOC<Hovering_defs::uav_alerted,outp_uav_alerted>("hover"),
    };
    dynamic::modeling::ICs ics_TOP = {
            dynamic::translate::make_IC<iestream_input_defs<int>::out,Hovering_defs::inId>("input_reader_id","hover"),
            dynamic::translate::make_IC<iestream_input_defs<bool>::out,Hovering_defs::threat_details_sent>("input_reader_details_sent","hover")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
            "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/hovering_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/hovering_test_output_state.txt");
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
