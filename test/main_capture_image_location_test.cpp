//
// Created by Chidiebere Onyedinma on 2019-10-18.
//

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
#include "../atomics/capture_image_and_location.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/

/***** Define output ports for coupled model *****/
struct top_threat_details: public out_port<Threat_t>{};
struct top_details_sent: public out_port<bool>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Threat_t : public iestream_input<Threat_t,T> {
public:
    InputReader_Threat_t () = default;
    InputReader_Threat_t (const char* file_path) : iestream_input<Threat_t,T>(file_path) {}
};

int main(){

    /****** Input Reader atomic model instantiation *******************/
    const char * i_input_data = "../input_data/capture_image_location_input_test.txt";
    shared_ptr<dynamic::modeling::model> input_reader;
    input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_Threat_t, TIME, const char*>("input_reader", move(i_input_data));

    /****** Capture_Image_Location atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> capture_image_location;
    capture_image_location = dynamic::translate::make_dynamic_atomic_model<CaptureImageAndLocation, TIME>("capture_image_location");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};
    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(top_threat_details), typeid(top_details_sent)};
    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_reader, capture_image_location};
    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
            dynamic::translate::make_EOC<CaptureImageAndLocation_defs::threat_details,top_threat_details>("capture_image_location"),
            dynamic::translate::make_EOC<CaptureImageAndLocation_defs::details_sent,top_details_sent>("capture_image_location")
    };
    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
            dynamic::translate::make_IC<iestream_input_defs<Threat_t>::out,CaptureImageAndLocation_defs::found>("input_reader","capture_image_location")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
            "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/capture_image_location_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/capture_image_location_test_output_state.txt");
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

