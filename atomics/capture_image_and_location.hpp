#ifndef _CAPTURE_IMAGE_AND_LOCATION_HPP__
#define _CAPTURE_IMAGE_AND_LOCATION_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
#include <iostream>

#include "../data_structures/threat_details.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct CaptureImageAndLocation_defs{
    struct threat_details : public out_port<Threat_t> {};
    struct details_sent : public out_port<bool> {};
    struct found : public in_port<Threat_t> {};
};

template<typename TIME> class CaptureImageAndLocation{
public:
    // ports definition
    using input_ports=tuple<typename CaptureImageAndLocation_defs::found>;
    using output_ports=tuple<typename CaptureImageAndLocation_defs::threat_details, typename CaptureImageAndLocation_defs::details_sent>;
    // state definition
    struct state_type{
        bool capturing;
        bool isThreatFound;
        Threat_t threat;
    };
    state_type state;
    // default constructor
    CaptureImageAndLocation() {
        state.capturing = false;
        state.isThreatFound = false;
        state.threat.image = "";
    }
    // internal transition
    void internal_transition() {
        state.capturing = false;
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        vector<Threat_t> bag_port_in;
        bag_port_in = get_messages<typename CaptureImageAndLocation_defs::found>(mbs);
        if(bag_port_in.size()>1) assert(false && "One message at a time");

        state.threat = bag_port_in[0];

        if(state.threat.lat > 0.0 && state.threat.lon > 0.0){
            state.isThreatFound = true;
            state.capturing = true;
            state.threat.image = "IMAGE_" + to_string(state.threat.id);

        } else{
            state.isThreatFound = false;
            state.capturing = false;
            state.threat.image = "NULL";
        }
    }
    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }
    // output function
    typename make_message_bags<output_ports>::type output() const {
        typename make_message_bags<output_ports>::type bags;
        vector<Threat_t> bag_port_out;
        bag_port_out.push_back(state.threat);
        get_messages<typename CaptureImageAndLocation_defs::threat_details>(bags) = bag_port_out;
        get_messages<typename CaptureImageAndLocation_defs::details_sent>(bags).push_back(state.isThreatFound);
        return bags;
    }
    // time_advance function
    TIME time_advance() const {
        TIME next_internal;
        if (state.capturing) {
            next_internal = TIME("00:00:05:000");
        }else {
            next_internal = numeric_limits<TIME>::infinity();
        }
        return next_internal;
    }

    friend ostringstream& operator<<(ostringstream& os, const typename CaptureImageAndLocation<TIME>::state_type& i) {
        string threatFound = i.isThreatFound ? "true" : "false";
        os << "threatId: " << i.threat.id << " & latitude: " << i.threat.lat << " & longitude: " << i.threat.lon << " & threat_captured: " << threatFound << " & threat_image: " << i.threat.image;
        cout << "threatId: " << i.threat.id << " & latitude: " << i.threat.lat << " & longitude: " << i.threat.lon << " & threat_captured: " << threatFound << " & threat_image: " << i.threat.image << endl;
        return os;
    }
};
#endif // _CAPTURE_IMAGE_AND_LOCATION_HPP__