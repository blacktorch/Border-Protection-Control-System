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
#include "../atomics/hovering.hpp"
#include "../atomics/search_and_locate_threat.hpp"
#include "../atomics/capture_image_and_location.hpp"

//C++ headers
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>


using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
struct inId : public in_port<int>{};
struct intrusion_detected : public in_port<int>{};
//struct inp_1 : public in_port<Message_t>{};
//struct inp_2 : public in_port<Message_t>{};
/***** Define output ports for coupled model *****/
struct outp_threat_details : public out_port<Threat_t>{};
struct outp_threat_details_received : public out_port<Threat_t>{};
//struct outp_1 : public out_port<Message_t>{};
//struct outp_2 : public out_port<Message_t>{};
//struct outp_pack : public out_port<int>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Int : public iestream_input<int,T> {
public:
    InputReader_Int() = default;
    InputReader_Int(const char* file_path) : iestream_input<int,T>(file_path) {}
};

int main(int argc, char ** argv) {

    if (argc < 2) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the input file " << endl;
        return 1;
    }
    /****** Input Reader atomic model instantiation *******************/
    string input = argv[1];
    const char * i_input = input.c_str();
    shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader" , move(i_input));

    /****** Hovering atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> hovering = dynamic::translate::make_dynamic_atomic_model<Hovering, TIME>("hovering");

    /****** Search_And_Locate atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> search_and_locate = dynamic::translate::make_dynamic_atomic_model<SearchAndLocateThreat, TIME>("search_and_locate");

    /****** Capture_Image_Location atomic models instantiation *******************/
    shared_ptr<dynamic::modeling::model> capture_image_location = dynamic::translate::make_dynamic_atomic_model<CaptureImageAndLocation, TIME>("capture_image_location");

    /*******UAV COUPLED MODEL********/
    dynamic::modeling::Ports iports_UAV = {typeid(inId)};
    dynamic::modeling::Ports oports_UAV = {typeid(outp_threat_details)};
    dynamic::modeling::Models submodels_UAV = {hovering, search_and_locate, capture_image_location};
    dynamic::modeling::EICs eics_UAV = {
            dynamic::translate::make_EIC<inId, Hovering_defs::inId>("hovering"),
    };
    dynamic::modeling::EOCs eocs_UAV = {
            dynamic::translate::make_EOC<CaptureImageAndLocation_defs::threat_details,outp_threat_details>("capture_image_location"),
    };
    dynamic::modeling::ICs ics_UAV = {
            dynamic::translate::make_IC<Hovering_defs::uav_alerted, SearchAndLocateThreat_defs::uav_alerted>("hovering","search_and_locate"),
            dynamic::translate::make_IC<SearchAndLocateThreat_defs::found, CaptureImageAndLocation_defs::found>("search_and_locate","capture_image_location"),
            dynamic::translate::make_IC<CaptureImageAndLocation_defs::details_sent, Hovering_defs::threat_details_sent>("capture_image_location","hovering"),
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> UAV;
    UAV = make_shared<dynamic::modeling::coupled<TIME>>(
            "UAV", submodels_UAV, iports_UAV, oports_UAV, eics_UAV, eocs_UAV, ics_UAV
    );

    /*******ABP SIMULATOR COUPLED MODEL********/
    dynamic::modeling::Ports iports_BPCS = {typeid(intrusion_detected)};
    dynamic::modeling::Ports oports_BPCS = {typeid(outp_threat_details_received)};
    dynamic::modeling::Models submodels_BPCS = {UAV};
    dynamic::modeling::EICs eics_BPCS = {
            cadmium::dynamic::translate::make_EIC<intrusion_detected, inId>("UAV")
    };
    dynamic::modeling::EOCs eocs_BPCS = {
            dynamic::translate::make_EOC<outp_threat_details,outp_threat_details_received>("UAV"),
    };
    dynamic::modeling::ICs ics_BPCS = {
            //dynamic::translate::make_IC<CaptureImageAndLocation_defs::threat_details, outp_threat_details>("capture_image_location","UAV"),
            //dynamic::translate::make_IC<inId, Hovering_defs::inId>("UAV","hovering"),
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> BPCS;
    BPCS = make_shared<dynamic::modeling::coupled<TIME>>(
            "BPCS", submodels_BPCS, iports_BPCS, oports_BPCS, eics_BPCS, eocs_BPCS, ics_BPCS
    );


    /*******TOP COUPLED MODEL********/
    dynamic::modeling::Ports iports_TOP = {typeid(intrusion_detected)};
    dynamic::modeling::Ports oports_TOP = {typeid(outp_threat_details_received)};
    dynamic::modeling::Models submodels_TOP = {input_reader, BPCS};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
            dynamic::translate::make_EOC<outp_threat_details_received,outp_threat_details_received>("BPCS")
    };
    dynamic::modeling::ICs ics_TOP = {
            dynamic::translate::make_IC<iestream_input_defs<int>::out, intrusion_detected>("input_reader","BPCS")
    };
    shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
            "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/BPCS_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/BPCS_output_state.txt");
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
    r.run_until_passivate();
    return 0;
}