//
// Created by Chidiebere Onyedinma on 2019-10-18.
//

#ifndef BOOST_SIMULATION_MESSAGE_HPP
#define BOOST_SIMULATION_MESSAGE_HPP

#include <assert.h>
#include <iostream>
#include <string>

using namespace std;

/*******************************************/
/**************** Threat_t ****************/
/*******************************************/

struct Threat_t{
    Threat_t(){}
    Threat_t(int i_id, float i_lat, float i_lon, string i_image)
            :id(i_id), lat(i_lat), lon(i_lon), image(i_image){}

    int   id;
    float lat;
    float lon;
    string image;
};

istream& operator>> (istream& is, Threat_t& msg);

ostream& operator<<(ostream& os, const Threat_t& msg);


#endif // BOOST_SIMULATION_MESSAGE_HPP