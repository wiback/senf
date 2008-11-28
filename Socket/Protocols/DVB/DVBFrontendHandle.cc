// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief DVBFrontendHandle non-inline non-template implementation */

#include "DVBFrontendHandle.hh"
//#include "DVBFrontendHandle.ih"

// Custom includes
#include <boost/format.hpp>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "../../../Utils/Exception.hh"
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>


//#include "DVBFrontendHandle.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::DVBFrontendHandle
using namespace std;

prefix_ void senf::DVBFrontendSocketProtocol::init_client(unsigned const short adapter,unsigned const short device)
{
    string devFrontend = str( boost::format("/dev/dvb/adapter%d/frontend%d") % adapter % device);
    int f = open(devFrontend.c_str(), O_RDONLY | O_NONBLOCK);
    if (f < 0)
        SENF_THROW_SYSTEM_EXCEPTION("")<< "Could not open frontend device of DVB adapter " << devFrontend << ".";
    fd(f);
    
    // later there will be a decision between waitTune and asyncTune
    cb2_X_Tune = boost::bind(&senf::DVBFrontendSocketProtocol::waitTune, this, _1);

    struct dvb_frontend_info info;
    ioctl(fd(), FE_GET_INFO, &info);

}

prefix_ unsigned senf::DVBFrontendSocketProtocol::available()
    const
{
    return 0;
}

prefix_ bool senf::DVBFrontendSocketProtocol::eof()
    const
{
    return false;
}

prefix_ struct dvb_frontend_event senf::DVBFrontendSocketProtocol::tune(const struct dvb_frontend_parameters & frontend){
    return cb2_X_Tune(frontend);
}
prefix_ struct dvb_frontend_event senf::DVBFrontendSocketProtocol::waitTune(const struct dvb_frontend_parameters & frontend) {
    // tuning
    if ( ::ioctl(fd(), FE_SET_FRONTEND, &frontend) ) 
        SENF_THROW_SYSTEM_EXCEPTION("") << "Could not write on frontend device. Socket should initialized with write permissions.";
    // getting event
    struct dvb_frontend_event ev;
    ::ioctl(fd(), FE_GET_EVENT, &ev);
    
    return ev;
}

prefix_ struct dvb_frontend_event senf::DVBFrontendSocketProtocol::asyncTune(const struct dvb_frontend_parameters & frontend){
    // tuning
    if ( ::ioctl(fd(), FE_SET_FRONTEND, &frontend) ) 
    SENF_THROW_SYSTEM_EXCEPTION("") << "Could not write on frontend device. Socket should initialized with write permissions.";
      
    // do something async here 
    struct dvb_frontend_event ev;
    return ev;
}

prefix_ void senf::DVBFrontendSocketProtocol::tuneDVB_T(unsigned int frequency, 
        fe_spectral_inversion_t inversion,
        fe_bandwidth_t bandwidth, 
        fe_code_rate_t code_rate_HP, /* high priority stream code rate */
        fe_code_rate_t code_rate_LP, /* low priority stream code rate */
        fe_modulation_t constellation, /* modulation type */
        fe_transmit_mode_t transmission_mode, 
        fe_guard_interval_t guard_interval,
        fe_hierarchy_t hierarchy_information
        )
{
    struct dvb_ofdm_parameters ofdm; /* DVB-T Parameters */
    struct dvb_frontend_parameters frontend;
    
    ofdm.bandwidth = bandwidth;
    ofdm.code_rate_HP = code_rate_HP;
    ofdm.code_rate_LP = code_rate_LP;
    ofdm.constellation = constellation; 
    ofdm.guard_interval = guard_interval;
    ofdm.hierarchy_information = hierarchy_information;
    
    frontend.frequency = frequency;
    frontend.inversion = inversion;
    frontend.u.ofdm = ofdm;
    
    tune(frontend);
    
}
prefix_ void senf::DVBFrontendSocketProtocol::tuneDVB_S(unsigned int frequency, 
        fe_spectral_inversion_t inversion, 
        unsigned int symbole_rate, /* symbol rate in Symbols per second */
        fe_code_rate_t fec_inner) /* forward error correction (see above) */
{
    struct dvb_qpsk_parameters qpsk; /* DVB-S Parameters*/
    struct dvb_frontend_parameters frontend;
    
    qpsk.symbol_rate = symbole_rate;
    qpsk.fec_inner = fec_inner;
    
    frontend.frequency = frequency;
    frontend.inversion = inversion;
    frontend.u.qpsk = qpsk;
    
    tune(frontend);
}

prefix_ void senf::DVBFrontendSocketProtocol::tuneDVB_C(unsigned int frequency, 
        fe_spectral_inversion_t inversion,
        unsigned int symbol_rate,
        fe_code_rate_t fec_inner,
        fe_modulation_t modulation)
{
    struct dvb_qam_parameters qam; /* DVB-C Parameters*/
    struct dvb_frontend_parameters frontend;
    
    qam.symbol_rate = symbol_rate;
    qam.fec_inner = fec_inner;
    qam.modulation = modulation;
    
    
    frontend.frequency = frequency;
    frontend.inversion = inversion;
    frontend.u.qam = qam;
       
    tune(frontend);
}
prefix_ struct dvb_frontend_info senf::DVBFrontendSocketProtocol::getInfo()
    const
{
    struct dvb_frontend_info info;
    if( ::ioctl(fd(), FE_GET_INFO, &info) ) {
        SENF_THROW_SYSTEM_EXCEPTION("") << "Could not read on fildescriptor.";
    }
    return info;
}

prefix_ struct dvb_frontend_parameters senf::DVBFrontendSocketProtocol::getFrontendParam() const {
    struct dvb_frontend_parameters frontend;
    
    if ( ::ioctl(fd(), FE_GET_FRONTEND, &frontend) ) 
           SENF_THROW_SYSTEM_EXCEPTION("") << "Could not read from frontend device, read-only access to the device is sufficient.";
    return frontend;
}

prefix_ void senf::DVBFrontendSocketProtocol::setFrequency(unsigned int frequency){

    struct dvb_frontend_parameters frontend = getFrontendParam();
    
    frontend.frequency = frequency;
    
    tune(frontend);
}

prefix_ void senf::DVBFrontendSocketProtocol::setInversion(fe_spectral_inversion_t inversion){
    
    struct dvb_frontend_parameters frontend = getFrontendParam();
        
    frontend.inversion = inversion;
        
    tune(frontend);
}

prefix_ void senf::DVBFrontendSocketProtocol::setCodeRate(fe_code_rate_t fec_inner){
    
    fe_type_t type = getInfo().type;
    struct dvb_frontend_parameters frontend = getFrontendParam();
    if( type == FE_QPSK ){ /* DVB-S */
        frontend.u.qpsk.fec_inner = fec_inner;
    }
    if( type == FE_QAM ){ /* DVB-C */
        frontend.u.qam.fec_inner = fec_inner;
    }
    if( type == FE_OFDM ){ /* DVB-T  ATTENTION sets high and low priority code rate!*/
        frontend.u.ofdm.code_rate_HP = fec_inner;
        frontend.u.ofdm.code_rate_LP = fec_inner;
    }
    tune(frontend);
}

prefix_ void senf::DVBFrontendSocketProtocol::setSymbolRate(unsigned int symbol_rate){
    
    fe_type_t type = getInfo().type;
    struct dvb_frontend_parameters frontend = getFrontendParam();
    
    if (! ( type == FE_QPSK || type == FE_QAM ) )
        SENF_THROW_SYSTEM_EXCEPTION("Symbole rate can only be set for DVB-S or DVB-C devices.");
    if( type == FE_QPSK ) /* DVB-S */
        frontend.u.qpsk.symbol_rate = symbol_rate;
    
    if( type == FE_QAM ) /* DVB-C */
        frontend.u.qam.symbol_rate = symbol_rate;
    
    tune(frontend);
}
prefix_ void senf::DVBFrontendSocketProtocol::setModulation(fe_modulation_t modulation){
    
    fe_type_t type = getInfo().type;
    struct dvb_frontend_parameters frontend = getFrontendParam();
    
    if (! ( type == FE_OFDM || type == FE_QAM ) )
        SENF_THROW_SYSTEM_EXCEPTION("Symbole rate can only be set for DVB-T or DVB-C devices.");
    if( type == FE_QAM ) /* DVB-C */
        frontend.u.qam.modulation = modulation;
    if( type == FE_OFDM ) /* DVB-T */
        frontend.u.ofdm.constellation = modulation;
    
    tune(frontend);
}

prefix_ void senf::DVBFrontendSocketProtocol::setBandwidth(fe_bandwidth_t bandwidth) {
    
    fe_type_t type = getInfo().type;
    struct dvb_frontend_parameters frontend = getFrontendParam();
    
    if (type != FE_OFDM)
        SENF_THROW_SYSTEM_EXCEPTION("") << "Bandwidth can only be set for DVB-T devices.";
    
    frontend.u.ofdm.bandwidth = bandwidth;
        
    tune(frontend);
}

prefix_ void senf::DVBFrontendSocketProtocol::setHighPriorityCodeRate(fe_code_rate_t code_rate_HP) {
    
    fe_type_t type = getInfo().type;
    struct dvb_frontend_parameters frontend = getFrontendParam();
    
    if (type != FE_OFDM)
        SENF_THROW_SYSTEM_EXCEPTION("") << "High priority bandwidth can only be set for DVB-T devices.";
    
    frontend.u.ofdm.code_rate_HP = code_rate_HP;
        
    tune(frontend);
}

prefix_ void senf::DVBFrontendSocketProtocol::setLowPriorityCodeRate(fe_code_rate_t code_rate_LP) {
    
    fe_type_t type = getInfo().type;
    struct dvb_frontend_parameters frontend = getFrontendParam();
    
    if (type != FE_OFDM)
        SENF_THROW_SYSTEM_EXCEPTION("") << "Low priority bandwidth can only be set for DVB-T devices.";
    
    frontend.u.ofdm.code_rate_LP = code_rate_LP;
        
    tune(frontend);
}

prefix_ void senf::DVBFrontendSocketProtocol::setGuardInterval(fe_guard_interval_t guard_interval) {
    
    fe_type_t type = getInfo().type;
    struct dvb_frontend_parameters frontend = getFrontendParam();
    
    if (type != FE_OFDM)
        SENF_THROW_SYSTEM_EXCEPTION("") << "Guard interval can only be set for DVB-T devices.";
    
    frontend.u.ofdm.guard_interval = guard_interval;
        
    tune(frontend);
}

prefix_ void senf::DVBFrontendSocketProtocol::setHierarchyInformation(fe_hierarchy_t hierarchy_information) {
    
    fe_type_t type = getInfo().type;
    struct dvb_frontend_parameters frontend = getFrontendParam();
    
    if (type != FE_OFDM)
        SENF_THROW_SYSTEM_EXCEPTION("") << "Hierachy information can only be set for DVB-T devices.";
    
    frontend.u.ofdm.hierarchy_information = hierarchy_information;
        
    tune(frontend);
}

prefix_ int16_t senf::DVBFrontendSocketProtocol::signalStrength()
    const
{
    int16_t strength;
    if (::ioctl(fd(), FE_READ_SIGNAL_STRENGTH, &strength) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not get signal strength of DVB adapter.");
    return strength;
}

prefix_ int16_t senf::DVBFrontendSocketProtocol::signalNoiseRatio()
    const
{
    int16_t snr;
    if (::ioctl(fd(), FE_READ_SNR, &snr) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not get signal-to-noise ratio of DVB adapter.");
    return snr;
}

prefix_ uint32_t senf::DVBFrontendSocketProtocol::bitErrorRate()
    const
{
    uint32_t ber;
    if (::ioctl(fd(), FE_READ_BER, &ber) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not get bit error rate of DVB adapter.");
    return ber;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "DVBFrontendHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
