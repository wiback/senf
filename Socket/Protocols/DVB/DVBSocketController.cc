// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Anton Gillert <atx@berlios.de>
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


#include "senf/Utils/Exception.hh"
#include "senf/Utils/Logger/Logger.hh"
#include "senf/Utils/membind.hh"

#include "DVBSocketController.hh"
#include <sstream>
using namespace std;
#define prefix_



senf::DVBSocketController::DVBSocketController(DVBFrontendHandle frontendHandle_, DVBDemuxSectionHandle sectionHandle_ , const Callback & cb_) :
    dir( this ),
    frontendHandle( frontendHandle_ ),
    sectionHandle( sectionHandle_ ),
    type( frontendHandle.protocol().getInfo().type ),
    parser( type ),
    cb( cb_ ),
    event( "senf::DVBSocketController::readEvent", senf::membind(&DVBSocketController::readEvent, this), frontendHandle, senf::scheduler::FdEvent::EV_PRIO, false )
{
    initConsole();
}

prefix_ senf::DVBSocketController::~DVBSocketController()
{
}
prefix_ void senf::DVBSocketController::tuneToCMD(const string & input, const string & mode){
    struct dvb_frontend_parameters frontend;
    
  
    // no valid configline, so it will be treaten like a channel name
    if (input.find(":")==string::npos){
        if (mode.c_str()[0]=='a')
            tuneTo(input);
        else
            tuneTo_sync(input);
        return;
    }
    // add psydo name "foo" to complete configline syntax
    frontend = parser.getFrontendParam("foo:"+input);
    
    if (mode.c_str()[0]=='a'){
        switch (type) {
            case FE_QPSK:
                tuneDVB_S(frontend.frequency, frontend.inversion, frontend.u.qpsk.symbol_rate, frontend.u.qpsk.fec_inner);
                break;
            case FE_QAM:
                tuneDVB_C(frontend.frequency, frontend.inversion, frontend.u.qam.symbol_rate, frontend.u.qam.fec_inner, frontend.u.qam.modulation);
                break;
            case FE_OFDM:
                tuneDVB_T(frontend.frequency, frontend.inversion, frontend.u.ofdm.bandwidth, frontend.u.ofdm.code_rate_HP, frontend.u.ofdm.code_rate_LP, frontend.u.ofdm.constellation, frontend.u.ofdm.transmission_mode, frontend.u.ofdm.guard_interval, frontend.u.ofdm.hierarchy_information);
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("Could not determine type of card.");
        }
    }
    else {
        switch (type) {
            case FE_QPSK: if (mode.c_str()[0]=='a')
                tuneDVB_S_sync(frontend.frequency, frontend.inversion, frontend.u.qpsk.symbol_rate, frontend.u.qpsk.fec_inner);
                break;
            case FE_QAM:
                tuneDVB_C_sync(frontend.frequency, frontend.inversion, frontend.u.qam.symbol_rate, frontend.u.qam.fec_inner, frontend.u.qam.modulation);
                break;
            case FE_OFDM:
                tuneDVB_T_sync(frontend.frequency, frontend.inversion, frontend.u.ofdm.bandwidth, frontend.u.ofdm.code_rate_HP, frontend.u.ofdm.code_rate_LP, frontend.u.ofdm.constellation, frontend.u.ofdm.transmission_mode, frontend.u.ofdm.guard_interval, frontend.u.ofdm.hierarchy_information);
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("Could not determine type of card.");
        }
    }
}

prefix_ void senf::DVBSocketController::tuneTo(const string & channel)
{
    struct dvb_frontend_parameters frontend;
   
    string configLine = parser.getConfigLine(channel);
    
    frontend = parser.getFrontendParam(configLine);
    switch (type) {
        case FE_QPSK:
            tuneDVB_S(frontend.frequency, frontend.inversion, frontend.u.qpsk.symbol_rate, frontend.u.qpsk.fec_inner);
            break;
        case FE_QAM:
            tuneDVB_C(frontend.frequency, frontend.inversion, frontend.u.qam.symbol_rate, frontend.u.qam.fec_inner, frontend.u.qam.modulation);
            break;
        case FE_OFDM:
            tuneDVB_T(frontend.frequency, frontend.inversion, frontend.u.ofdm.bandwidth, frontend.u.ofdm.code_rate_HP, frontend.u.ofdm.code_rate_LP, frontend.u.ofdm.constellation, frontend.u.ofdm.transmission_mode, frontend.u.ofdm.guard_interval, frontend.u.ofdm.hierarchy_information);
            break;
        default:
            SENF_THROW_SYSTEM_EXCEPTION("Could not determine type of card.");
    }    
}

prefix_ void senf::DVBSocketController::tuneDVB_T(unsigned int frequency, 
                fe_spectral_inversion_t inversion,
                fe_bandwidth_t bandwidth, 
                fe_code_rate_t code_rate_HP, /* high priority stream code rate */
                fe_code_rate_t code_rate_LP, /* low priority stream code rate */
                fe_modulation_t constellation, /* modulation type (see above) */
                fe_transmit_mode_t transmission_mode, 
                fe_guard_interval_t guard_interval,
                fe_hierarchy_t hierarchy_information
                )
{
    if(type != FE_OFDM)
        SENF_THROW_SYSTEM_EXCEPTION("Type of card is: ") << getTypeString() << " for this operation you need a DVB-T Card!";
    
    event.enable();
    
    frontendHandle.protocol().setNonBlock();
    frontendHandle.protocol().tuneDVB_T(frequency, 
                inversion, 
                bandwidth, 
                code_rate_HP, 
                code_rate_LP, 
                constellation, 
                transmission_mode, 
                guard_interval, 
                hierarchy_information);
}

prefix_ void senf::DVBSocketController::tuneDVB_S(unsigned int frequency, fe_spectral_inversion_t inversion, unsigned int symbole_rate, fe_code_rate_t code_rate){
    if(type != FE_QPSK)
        SENF_THROW_SYSTEM_EXCEPTION("Type of card is: ") << getTypeString() << " for this operation you need a DVB-S Card!";
    
    event.enable();
    
    frontendHandle.protocol().setNonBlock();
    frontendHandle.protocol().tuneDVB_S(frequency, inversion, symbole_rate, code_rate);
}

prefix_ void senf::DVBSocketController::tuneDVB_C(unsigned int frequency, 
                fe_spectral_inversion_t inversion,
                unsigned int symbol_rate,
                fe_code_rate_t fec_inner,
                fe_modulation_t modulation
                )
{   
    if(type != FE_QAM)
        SENF_THROW_SYSTEM_EXCEPTION("Type of card is: ") << getTypeString() << " for this operation you need a DVB-C Card!";
    
    event.enable();
    
    frontendHandle.protocol().setNonBlock();
    
    frontendHandle.protocol().tuneDVB_C(frequency, inversion, symbol_rate, fec_inner, modulation);
}

prefix_ dvb_frontend_event senf::DVBSocketController::tuneTo_sync(const string & channel)
{
    struct dvb_frontend_parameters frontend;
    dvb_frontend_event ev;
    string configLine = parser.getConfigLine(channel);
    
    frontend = parser.getFrontendParam(configLine);
    switch (type) {
        case FE_QPSK:
            ev = tuneDVB_S_sync(frontend.frequency, frontend.inversion, frontend.u.qpsk.symbol_rate, frontend.u.qpsk.fec_inner);
            break;
        case FE_QAM:
            ev = tuneDVB_C_sync(frontend.frequency, frontend.inversion, frontend.u.qam.symbol_rate, frontend.u.qam.fec_inner, frontend.u.qam.modulation);
            break;
        case FE_OFDM:
            ev = tuneDVB_T_sync(frontend.frequency, frontend.inversion, frontend.u.ofdm.bandwidth, frontend.u.ofdm.code_rate_HP, frontend.u.ofdm.code_rate_LP, frontend.u.ofdm.constellation, frontend.u.ofdm.transmission_mode, frontend.u.ofdm.guard_interval, frontend.u.ofdm.hierarchy_information);
            break;
        default:
            SENF_THROW_SYSTEM_EXCEPTION("Could not determine type of card.");
    }
    return ev;
}

prefix_ dvb_frontend_event senf::DVBSocketController::tuneDVB_T_sync(unsigned int frequency, 
                fe_spectral_inversion_t inversion,
                fe_bandwidth_t bandwidth, 
                fe_code_rate_t code_rate_HP, /* high priority stream code rate */
                fe_code_rate_t code_rate_LP, /* low priority stream code rate */
                fe_modulation_t constellation, /* modulation type (see above) */
                fe_transmit_mode_t transmission_mode, 
                fe_guard_interval_t guard_interval,
                fe_hierarchy_t hierarchy_information
                )
{
    if(type != FE_OFDM)
        SENF_THROW_SYSTEM_EXCEPTION("Type of card is: ") << getTypeString() << " for this operation you need a DVB-T Card!";
    
    event.disable();
    
    frontendHandle.protocol().setNonBlock(false);
    
    frontendHandle.protocol().tuneDVB_T(frequency, 
            inversion, 
            bandwidth, 
            code_rate_HP, 
            code_rate_LP, 
            constellation, 
            transmission_mode, 
            guard_interval, 
            hierarchy_information);
    
    if(!frontendHandle.waitOOBReadable(senf::ClockService::seconds(2)))
        SENF_THROW_SYSTEM_EXCEPTION("Could not tune to channel!");
    
    return frontendHandle.protocol().getEvent();
}

prefix_ dvb_frontend_event senf::DVBSocketController::tuneDVB_S_sync(unsigned int frequency, fe_spectral_inversion_t inversion, unsigned int symbole_rate, fe_code_rate_t code_rate){
    if(type != FE_QPSK)
        SENF_THROW_SYSTEM_EXCEPTION("Type of card is: ") << getTypeString() << " for this operation you need a DVB-S Card!";
    
    event.disable();
    
    frontendHandle.protocol().setNonBlock(false);
    
    frontendHandle.protocol().tuneDVB_S(frequency, inversion, symbole_rate, code_rate);
    
    if(!frontendHandle.waitOOBReadable(senf::ClockService::seconds(2)))
        SENF_THROW_SYSTEM_EXCEPTION("Could not tune to channel!");
    return frontendHandle.protocol().getEvent();
}

prefix_ dvb_frontend_event senf::DVBSocketController::tuneDVB_C_sync(unsigned int frequency, 
                fe_spectral_inversion_t inversion,
                unsigned int symbol_rate,
                fe_code_rate_t fec_inner,
                fe_modulation_t modulation
                )
{   
    if(type != FE_QAM)
        SENF_THROW_SYSTEM_EXCEPTION("Type of card is: ") << getTypeString() << " for this operation you need a DVB-C Card!";
    
    event.disable();
    
    frontendHandle.protocol().setNonBlock(false);
    
    frontendHandle.protocol().tuneDVB_C(frequency, inversion, symbol_rate, fec_inner, modulation);
    if(!frontendHandle.waitOOBReadable(senf::ClockService::seconds(2)))
        SENF_THROW_SYSTEM_EXCEPTION("Could not tune to channel!");

    return frontendHandle.protocol().getEvent();
}


prefix_ string senf::DVBSocketController::getTypeString(){
    switch (type) {
        case FE_QPSK:
            return "DVB-S";
        case FE_QAM:
            return "DVB-C";
        case FE_OFDM:
            return "DVB-T";
        default:
            SENF_THROW_SYSTEM_EXCEPTION("Could not determine type of card.");
    }
    
}
prefix_ unsigned int senf::DVBSocketController::bitErrorRate(){ 
    return frontendHandle.protocol().bitErrorRate();
}

prefix_ unsigned int senf::DVBSocketController::signalToNoiseRatio(){ 
    return frontendHandle.protocol().signalNoiseRatio();
}

prefix_ unsigned int senf::DVBSocketController::signalStrength(){ 
    return frontendHandle.protocol().signalStrength();
}

prefix_ string senf::DVBSocketController::getTuneInfo(const string & conf){
    const char* cConf = conf.c_str();
    stringstream info;
    
    fe_status_t status;
    frontendHandle.protocol().setNonBlock(false);
    uint16_t snr, signal;
    uint32_t ber, uncorrected_blocks;
    status = frontendHandle.protocol().status();
    snr = frontendHandle.protocol().signalNoiseRatio();
    signal = frontendHandle.protocol().signalStrength();
    ber = frontendHandle.protocol().bitErrorRate();
    uncorrected_blocks = frontendHandle.protocol().uncorrectedBlocks();
    
    info << hex;
    
    for(unsigned int i = 0; i < conf.size(); ++i){
        if(i>0)
            info << " | ";
        switch(cConf[i]){
            case 'S' :
                info << "signal " << signal;
                break;
            case 's' : 
                info << "snr " << snr;
                break;
            case 'b' :
                info << "ber " << ber;
                break;
            case 'u' :
                info << "unc " << uncorrected_blocks;
                break;
            case 'f' :
                info << "status: " << status2String(status);
                break;
            default:
                break;
        }
    }
    return info.str();
}
prefix_ string senf::DVBSocketController::status2String(fe_status_t status){
   string s("");
    if (status & FE_HAS_LOCK)
        return s += "HAS LOCK"; 
    if (status & FE_HAS_CARRIER)
        s += "HAS CARRIER";
    if (status & FE_HAS_VITERBI)
        s += "HAS VITERBI";
    if (status & FE_HAS_SYNC)
        s += "HAS SYNC";
    if (status & FE_HAS_SIGNAL)
        s += "HAS SIGNAL";
    if (status & FE_TIMEDOUT)
        s += "TIMED OUT";
    if (status & FE_REINIT)
        s += "REINIT";

    return s;
}

prefix_ void senf::DVBSocketController::setSectionFilter(unsigned short int pid, 
        u_int8_t filter,
        unsigned int flags, 
        u_int8_t mask, 
        u_int8_t mode,
        unsigned int timeout)
{
    sectionHandle.protocol().setSectionFilter(pid, timeout, flags, filter, mask, mode);
    
}

prefix_ void senf::DVBSocketController::setBufferSize(unsigned long size)
{
    sectionHandle.protocol().setBufferSize(size);
}

prefix_ void senf::DVBSocketController::startFiltering()
{
    sectionHandle.protocol().startFiltering();
}

prefix_ void senf::DVBSocketController::stopFiltering()
{
    sectionHandle.protocol().stopFiltering();
}

prefix_ fe_type_t senf::DVBSocketController::getType(){
    return type;
}

prefix_ void senf::DVBSocketController::readEvent(int event){
    if(cb)
        cb(frontendHandle.protocol().getEvent());
}

prefix_ void senf::DVBSocketController::initConsole(){
    // binding functions to console
    namespace kw = senf::console::kw;
    dir.doc("DVB Controller");
    
    dir.add("type", &DVBSocketController::getTypeString)
    .doc("Shows actual type of card DVB-{T, S, C}");
    
    dir.add("info", &DVBSocketController::getTuneInfo)
    .doc("Returns a string which shows actual tuning status.\n\
            \"S\" prints signal strength (in hex)\n\
            \"s\" prints singal to noise ration (in hex)\n\
            \"b\" prints bit error rate (in hex)\n\
            \"u\" prints uncorrected blocks (in hex)\n\
            \"f\" prints readable overal status e.g. \"Has Lock\"\n\n\
            These characters can be used to form the output. Be aware, some\n\
            features may not be supported be your current driver implementation\n\
            and could end in throwing an exception!")
    .arg("conf", "Ssbuf", kw::default_value = "Ssbuf");
    
    dir.add("tune", &DVBSocketController::tuneToCMD)
        .doc("tunes to channel listet in the configfile.")
        .arg("channel", "channel to tune")
        .arg("mode", "mode \"sync\" or \"async\"", kw::default_value = "async");
    
    dir.add("buffersize", &DVBSocketController::setBufferSize)
        .doc("Set the size of the circular buffer used for filtered data.")
        .arg("size", "in byte");
    
    dir.add("start", &DVBSocketController::startFiltering)
        .doc("Starts filtering");
        
    dir.add("stop", &DVBSocketController::setBufferSize)
        .doc("Stops filtering");
    
    dir.add("filter", &DVBSocketController::setSectionFilter)
        .arg("pid", "pid to filter")
        .arg("filter", "filter", kw::default_value = 62)
        .arg("flags", "or-able: DMX_CHECK_CRC(0x01), DMX_ONESHOT(0x02), DMX_IMMEDIATE_START(0x04), DMX_KERNEL_CLIENT(0x8000)", kw::default_value = DMX_IMMEDIATE_START | DMX_CHECK_CRC)
        .arg("mask", "mask", kw::default_value = 0xff)
        .arg("mode", "mode", kw::default_value = 0)
        .arg("timeout", "timeout", kw::default_value = 0)
        .doc("Sets parameters for section filter.");
       
    dir.add("stop", &DVBSocketController::setBufferSize)
            .doc("Stops filtering");
}
