#include "lwIOLinkMaster.hpp"

namespace lwIOLink{

bool Master::begin(){
    for(uint_fast8_t port =0;port<m_total_ports;port++) {
        auto const res = m_port_list[port]->begin();
        if (res == false) {
            return false;
        }
    }
    return true;
}    
}