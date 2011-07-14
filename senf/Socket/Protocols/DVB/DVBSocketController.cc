// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Anton Gillert <atx@berlios.de>
/** \file
    \brief DVBSocketController non-inline non-template implementation */

#include "DVBSocketController.hh"

// Custom includes
#include <sstream>
#include <senf/Utils/Exception.hh>
#include <senf/Utils/Logger/Logger.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/Console/ParsedCommand.hh>
#include <boost/shared_ptr.hpp>
#include "DVBProtocolWrapper.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int senf::DVBSocketController::controllerNr(0);

senf::DVBSocketController::DVBSocketController(DVBFrontendHandle frontendHandle_,
                                               const Callback & cb_)
    : dir( this ),
      frontendHandle( frontendHandle_ ),
      type( frontendHandle.protocol().getInfo().type ),
      parser( type ),
      cb( cb_ ),
      sectionNr(1),
      pesNr(1),
      event( "senf::DVBSocketController::readEvent",
             senf::membind(&DVBSocketController::readEvent, this), frontendHandle,
             senf::scheduler::FdEvent::EV_PRIO, false )
{
    initConsole();
}

prefix_ senf::DVBSocketController::~DVBSocketController()
{}

prefix_ senf::DVBDemuxSectionHandle
senf::DVBSocketController::createDVBDemuxSectionHandle(int adapternumber, int demuxnumber,
                                                       bool addToConsole)
{
    DVBDemuxSectionHandle sectionHandle(adapternumber, demuxnumber);
    if (addToConsole)
        this->addToConsole(sectionHandle);
    return sectionHandle;

}

prefix_ senf::DVBDemuxPESHandle
senf::DVBSocketController::createDVBDemuxPESHandle(int adapternumber, int demuxnumber,
                                                   bool addToConsole)
{
    DVBDemuxPESHandle pesHandle(adapternumber, demuxnumber);
    if (addToConsole)
        this->addToConsole(pesHandle);
    return pesHandle;

}

prefix_ void senf::DVBSocketController::addToConsole(senf::DVBDemuxSectionHandle sh)
{
    boost::shared_ptr<DVBSectionProtocolWrapper> wrap(new DVBSectionProtocolWrapper(sh));
    sh.protocol().addWrapper(wrap);
    dir.node().add("section" + senf::str(sectionNr), wrap->dir);
    sectionNr++;
}

prefix_ void senf::DVBSocketController::addToConsole(senf::DVBDemuxPESHandle sh)
{
    boost::shared_ptr<DVBPESProtocolWrapper> wrap(new DVBPESProtocolWrapper(sh));
    sh.protocol().addWrapper(wrap);
    dir.node().add("pes"+ senf::str(pesNr), wrap->dir);
    pesNr++;
}

prefix_ void senf::DVBSocketController::tuneToCMD(std::string const & input, std::string const & mode)
{
    struct dvb_frontend_parameters frontend;

    // no valid configline, so it will be treaten like a channel name
    if (input.find(":") == std::string::npos)
    {
        if (mode.c_str()[0]=='a')
            tuneTo(input);
        else
            tuneTo_sync(input);
        return;
    }
    // add psydo name "foo" to complete configline syntax
    frontend = parser.getFrontendParam("foo:"+input);

    if (mode.c_str()[0]=='a') {
        switch (type) {
            case FE_QPSK:
                tuneDVB_S(frontend.frequency, frontend.inversion, frontend.u.qpsk.symbol_rate,
                          frontend.u.qpsk.fec_inner);
                break;
            case FE_QAM:
                tuneDVB_C(frontend.frequency, frontend.inversion, frontend.u.qam.symbol_rate,
                          frontend.u.qam.fec_inner, frontend.u.qam.modulation);
                break;
            case FE_OFDM:
                tuneDVB_T(frontend.frequency, frontend.inversion, frontend.u.ofdm.bandwidth,
                          frontend.u.ofdm.code_rate_HP, frontend.u.ofdm.code_rate_LP,
                          frontend.u.ofdm.constellation, frontend.u.ofdm.transmission_mode,
                          frontend.u.ofdm.guard_interval, frontend.u.ofdm.hierarchy_information);
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("Could not determine type of card.");
        }
    }
    else {
        switch (type) {
            case FE_QPSK:
                tuneDVB_S_sync(frontend.frequency, frontend.inversion, frontend.u.qpsk.symbol_rate,
                               frontend.u.qpsk.fec_inner);
                break;
            case FE_QAM:
                tuneDVB_C_sync(frontend.frequency, frontend.inversion, frontend.u.qam.symbol_rate,
                               frontend.u.qam.fec_inner, frontend.u.qam.modulation);
                break;
            case FE_OFDM:
                tuneDVB_T_sync(frontend.frequency, frontend.inversion, frontend.u.ofdm.bandwidth,
                               frontend.u.ofdm.code_rate_HP, frontend.u.ofdm.code_rate_LP,
                               frontend.u.ofdm.constellation, frontend.u.ofdm.transmission_mode,
                               frontend.u.ofdm.guard_interval,
                               frontend.u.ofdm.hierarchy_information);
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("Could not determine type of card.");
        }
    }
}

prefix_ void senf::DVBSocketController::tuneTo(std::string const & channel)
{
    struct dvb_frontend_parameters frontend;

    std::string configLine = parser.getConfigLine(channel);

    frontend = parser.getFrontendParam(configLine);
    switch (type) {
        case FE_QPSK:
            tuneDVB_S(frontend.frequency, frontend.inversion, frontend.u.qpsk.symbol_rate,
                      frontend.u.qpsk.fec_inner);
            break;
        case FE_QAM:
            tuneDVB_C(frontend.frequency, frontend.inversion, frontend.u.qam.symbol_rate,
                      frontend.u.qam.fec_inner, frontend.u.qam.modulation);
            break;
        case FE_OFDM:
            tuneDVB_T(frontend.frequency, frontend.inversion, frontend.u.ofdm.bandwidth,
                      frontend.u.ofdm.code_rate_HP, frontend.u.ofdm.code_rate_LP,
                      frontend.u.ofdm.constellation, frontend.u.ofdm.transmission_mode,
                      frontend.u.ofdm.guard_interval, frontend.u.ofdm.hierarchy_information);
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
    if (type != FE_OFDM)
        SENF_THROW_SYSTEM_EXCEPTION("Type of card is: ")
            << getTypeString() << " for this operation you need a DVB-T Card!";

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

prefix_ void senf::DVBSocketController::tuneDVB_S(unsigned int frequency,
                                                  fe_spectral_inversion_t inversion,
                                                  unsigned int symbole_rate,
                                                  fe_code_rate_t code_rate)
{
    if (type != FE_QPSK)
        SENF_THROW_SYSTEM_EXCEPTION("Type of card is: ")
            << getTypeString() << " for this operation you need a DVB-S Card!";

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
    if (type != FE_QAM)
        SENF_THROW_SYSTEM_EXCEPTION("Type of card is: ")
            << getTypeString() << " for this operation you need a DVB-C Card!";

    event.enable();

    frontendHandle.protocol().setNonBlock();

    frontendHandle.protocol().tuneDVB_C(frequency, inversion, symbol_rate, fec_inner, modulation);
}

prefix_ dvb_frontend_event senf::DVBSocketController::tuneTo_sync(std::string const & channel)
{
    struct dvb_frontend_parameters frontend;
    dvb_frontend_event ev;
    std::string configLine = parser.getConfigLine(channel);

    frontend = parser.getFrontendParam(configLine);
    switch (type) {
        case FE_QPSK:
            ev = tuneDVB_S_sync(frontend.frequency, frontend.inversion,
                                frontend.u.qpsk.symbol_rate, frontend.u.qpsk.fec_inner);
            break;
        case FE_QAM:
            ev = tuneDVB_C_sync(frontend.frequency, frontend.inversion, frontend.u.qam.symbol_rate,
                                frontend.u.qam.fec_inner, frontend.u.qam.modulation);
            break;
        case FE_OFDM:
            ev = tuneDVB_T_sync(frontend.frequency, frontend.inversion, frontend.u.ofdm.bandwidth,
                                frontend.u.ofdm.code_rate_HP, frontend.u.ofdm.code_rate_LP,
                                frontend.u.ofdm.constellation, frontend.u.ofdm.transmission_mode,
                                frontend.u.ofdm.guard_interval,
                                frontend.u.ofdm.hierarchy_information);
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
    if (type != FE_OFDM)
        SENF_THROW_SYSTEM_EXCEPTION("Type of card is: ")
            << getTypeString() << " for this operation you need a DVB-T Card!";

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

    if (!frontendHandle.waitOOBReadable(senf::ClockService::seconds(2)))
        SENF_THROW_SYSTEM_EXCEPTION("Could not tune to channel!");

    return frontendHandle.protocol().getEvent();
}

prefix_ dvb_frontend_event
senf::DVBSocketController::tuneDVB_S_sync(unsigned int frequency,
                                          fe_spectral_inversion_t inversion,
                                          unsigned int symbole_rate, fe_code_rate_t code_rate)
{
    if (type != FE_QPSK)
        SENF_THROW_SYSTEM_EXCEPTION("Type of card is: ")
            << getTypeString() << " for this operation you need a DVB-S Card!";

    event.disable();

    frontendHandle.protocol().setNonBlock(false);

    frontendHandle.protocol().tuneDVB_S(frequency, inversion, symbole_rate, code_rate);

    if (!frontendHandle.waitOOBReadable(senf::ClockService::seconds(2)))
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
    if (type != FE_QAM)
        SENF_THROW_SYSTEM_EXCEPTION("Type of card is: ")
            << getTypeString() << " for this operation you need a DVB-C Card!";

    event.disable();

    frontendHandle.protocol().setNonBlock(false);

    frontendHandle.protocol().tuneDVB_C(frequency, inversion, symbol_rate, fec_inner, modulation);
    if (!frontendHandle.waitOOBReadable(senf::ClockService::seconds(2)))
        SENF_THROW_SYSTEM_EXCEPTION("Could not tune to channel!");

    return frontendHandle.protocol().getEvent();
}


prefix_ std::string senf::DVBSocketController::getTypeString()
{
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

prefix_ unsigned int senf::DVBSocketController::bitErrorRate()
{
    return frontendHandle.protocol().bitErrorRate();
}

prefix_ unsigned int senf::DVBSocketController::signalToNoiseRatio()
{
    return frontendHandle.protocol().signalNoiseRatio();
}

prefix_ unsigned int senf::DVBSocketController::signalStrength()
{
    return frontendHandle.protocol().signalStrength();
}

prefix_ std::string senf::DVBSocketController::getTuneInfo(std::string const & conf)
{
    const char* cConf = conf.c_str();
    std::stringstream info;

    fe_status_t status;
    frontendHandle.protocol().setNonBlock(false);
    uint16_t snr, signal;
    uint32_t ber, uncorrected_blocks;
    status = frontendHandle.protocol().status();
    snr = frontendHandle.protocol().signalNoiseRatio();
    signal = frontendHandle.protocol().signalStrength();
    ber = frontendHandle.protocol().bitErrorRate();
    uncorrected_blocks = frontendHandle.protocol().uncorrectedBlocks();

    info << std::hex;

    for (unsigned int i = 0; i < conf.size(); ++i) {
        if (i>0)
            info << " | ";
        switch (cConf[i]) {
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

prefix_ std::string senf::DVBSocketController::status2String(fe_status_t status)
{
    std::string s("");
    if (status & FE_HAS_LOCK)
        return s += "HAS LOCK";
    if (status & FE_HAS_CARRIER)
        s += "HAS CARRIER ";
    if (status & FE_HAS_VITERBI)
        s += "HAS VITERBI ";
    if (status & FE_HAS_SYNC)
        s += "HAS SYNC ";
    if (status & FE_HAS_SIGNAL)
        s += "HAS SIGNAL ";
    if (status & FE_TIMEDOUT)
        s += "TIMED OUT ";
    if (status & FE_REINIT)
        s += "REINIT ";

    return s;
}

prefix_ fe_type_t senf::DVBSocketController::getType()
{
    return type;
}

prefix_ void senf::DVBSocketController::readEvent(int event)
{
    if (cb)
        cb(frontendHandle.protocol().getEvent());
}

prefix_ void senf::DVBSocketController::initConsole()
{
    namespace fty = console::factory;
    namespace kw = console::kw;

    dir.doc("DVB Controller " + controllerNr);
    ++controllerNr;

    dir.add("type", fty::Command(&DVBSocketController::getTypeString, this)
            .doc("Shows actual type of card DVB-{T, S, C}") );

    dir.add("info", fty::Command(&DVBSocketController::getTuneInfo, this)
            .doc("Returns a string which shows actual tuning status.\n"
                 "'S' prints signal strength (in hex)\n"
                 "'s' prints singal to noise ration (in hex)\n"
                 "'b' prints bit error rate (in hex)\n"
                 "'u' prints uncorrected blocks (in hex)\n"
                 "'f' prints readable overal status e.g. 'Has Lock'\n\n"
                 "These characters can be used to form the output. Be aware, some\n"
                 "features may not be supported be your current driver implementation\n"
                 "and could end in throwing an exception!")
            .arg("conf", "Ssbuf", kw::default_value = "Ssbuf") );

    dir.add("tune", fty::Command(&DVBSocketController::tuneToCMD, this)
            .doc("tunes to channel listet in the configfile.")
            .arg("channel", "channel to tune")
            .arg("mode", "mode 'sync' or 'async'", kw::default_value = "async") );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
