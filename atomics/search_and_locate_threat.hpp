
#ifndef _SEARCH_AND_LOCATE_THREAT_HPP__
#define _SEARCH_AND_LOCATE_THREAT_HPP__

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
struct SearchAndLocateThreat_defs{
    struct found : public out_port<Threat_t> {};
    struct uav_alerted : public in_port<int> {};
};

template<typename TIME> class SearchAndLocateThreat{
public:
    // ports definition
    using input_ports=tuple<typename SearchAndLocateThreat_defs::uav_alerted>;
    using output_ports=tuple<typename SearchAndLocateThreat_defs::found>;
    // state definition
    struct state_type{
        bool searching;
        Threat_t threat;
    };
    state_type state;
    // default constructor
    SearchAndLocateThreat() {
        state.searching = false;
        state.threat.id = 0;
        state.threat.lat = 0.0f;
        state.threat.lon = 0.0f;
    }
    // internal transition
    void internal_transition() {
        state.searching = false;
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        vector<int> bag_port_in;
        bag_port_in = get_messages<typename SearchAndLocateThreat_defs::uav_alerted>(mbs);
        if(bag_port_in.size()>1) assert(false && "One message at a time");
        //state.index ++;
        if ((double)rand() / (double) RAND_MAX  < 0.90){
            state.threat.id = bag_port_in[0];
            state.searching = true;
            state.threat.lat = 0.1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(90.0 - 0.1)));
            state.threat.lon = 0.1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(180.0 - 0.1)));
        }else{
            state.searching = false;
            state.threat.lat = 0.0f;
            state.threat.lon = 0.0f;
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
        get_messages<typename SearchAndLocateThreat_defs::found>(bags) = bag_port_out;
        return bags;
    }
    // time_advance function
    TIME time_advance() const {
        TIME next_internal;
        if (state.searching) {
            next_internal = TIME("00:00:05:000");
        }else {
            next_internal = numeric_limits<TIME>::infinity();
        }
        return next_internal;
    }

    friend ostringstream& operator<<(ostringstream& os, const typename SearchAndLocateThreat<TIME>::state_type& i) {
        os << "threatId: " << i.threat.id << " & latitude: " << i.threat.lat << " & longitude: " << i.threat.lon;
        cout << "threatId: " << i.threat.id << " & latitude: " << i.threat.lat << " & longitude: " << i.threat.lon << endl;
        return os;
    }
};
#endif // _SEARCH_AND_LOCATE_THREAT_HPP__