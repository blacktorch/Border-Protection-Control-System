//
// Created by Chidiebere Onyedinma on 2019-10-18.
//


#ifndef __HOVERING_HPP__
#define __HOVERING_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <iostream>

using namespace cadmium;
using namespace std;

//Port definitions
struct Hovering_defs {
    struct inId : public in_port<int> {
    };
    struct threat_details_sent : public in_port<bool> {
    };
    struct uav_alerted : public out_port<int> {
    };
};

template<typename TIME>
class Hovering {
public:

    TIME alertingTime;
    bool isThreatProcessed;
    // state definition
    struct state_type {
        int threatId;
        int totalThreatProcessed;
        bool alerting;
        bool model_active;
        TIME next_internal;
    };

    state_type state;

    using input_ports=std::tuple<typename Hovering_defs::inId, typename Hovering_defs::threat_details_sent>;
    using output_ports=std::tuple<typename Hovering_defs::uav_alerted>;

    Hovering() noexcept {
        alertingTime = TIME("00:00:10");
        state.next_internal = std::numeric_limits<TIME>::infinity();
        state.model_active = false;
        state.threatId = 0;
        state.totalThreatProcessed = 0;
        isThreatProcessed = false;
        state.alerting = false;
    }

    void internal_transition() {
        if(state.model_active == true){
            if(isThreatProcessed == true){
                isThreatProcessed = false;
            } else {
                state.model_active = false;
                state.next_internal = std::numeric_limits<TIME>::infinity();
            }
        } else {
            if(isThreatProcessed == true){
                state.totalThreatProcessed++;
                isThreatProcessed = false;
            }
        }
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        if ((get_messages<typename Hovering_defs::inId>(mbs).size() +
             get_messages<typename Hovering_defs::threat_details_sent>(mbs).size()) > 1)
            assert(false && "one message per time uniti");
        for (const auto &x : get_messages<typename Hovering_defs::inId>(mbs)) {

            if (state.model_active == false) {
                state.threatId = x;
                if (state.threatId > 0) {
                    state.alerting = true;
                    state.model_active = true;
                    state.next_internal = alertingTime;
                } else {
                    state.alerting = false;
                    if (state.next_internal != std::numeric_limits<TIME>::infinity()) {
                        state.next_internal = state.next_internal - e;
                    }
                }
            }
        }
        for (const auto &x : get_messages<typename Hovering_defs::threat_details_sent>(mbs)) {

            if (state.model_active == true) {
                if (x == true) {
                    state.totalThreatProcessed++;
                    isThreatProcessed = true;

                } else {
                    isThreatProcessed = false;
                    if (state.next_internal != std::numeric_limits<TIME>::infinity()) {
                        state.next_internal = state.next_internal - e;
                    }
                }
            } else {
                state.alerting = false;
                state.totalThreatProcessed++;
                isThreatProcessed = true;
            }
        }
    }

    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), std::move(mbs));
    }

    // output function
    typename make_message_bags<output_ports>::type output() const {
        typename make_message_bags<output_ports>::type bags;
        if (state.alerting){
            get_messages<typename Hovering_defs::uav_alerted>(bags).push_back(state.threatId);
        }
        return bags;
    }

    // time_advance function
    TIME time_advance() const {
        return state.next_internal;
    }

    friend std::ostringstream& operator<<(std::ostringstream& os, const typename Hovering<TIME>::state_type& i) {
        os << "threatId: " << i.threatId << " & totalThreatProcessed: " << i.totalThreatProcessed;
        cout << "threatId: " << i.threatId << " & totalThreatProcessed: " << i.totalThreatProcessed << endl;
        return os;
    }
};
#endif // __HOVERING_HPP__