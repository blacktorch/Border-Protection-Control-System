//
// Created by Chidiebere Onyedinma on 2019-10-18.
//

#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "threat_details.hpp"

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator<<(ostream& os, const Threat_t& msg) {
    os << msg.id << " " << msg.lat << " " << msg.lon << " " << msg.image;
    return os;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator>> (istream& is, Threat_t& msg) {
    is >> msg.id;
    is >> msg.lat;
    is >> msg.lon;
    is >> msg.image;
    return is;
}
