// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Anton Gillert <atx@berlios.de>
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
#include <senf/Utils/Exception.hh>
#include <sys/ioctl.h>


//#include "DVBFrontendHandle.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::DVBFrontendHandle
using namespace std;

prefix_ void senf::DVBFrontendSocketProtocol::init_client(unsigned short adapter, unsigned short device, int flags)
    const
{
    string devFrontend = str( boost::format("/dev/dvb/adapter%d/frontend%d") % adapter % device);
    int f = open(devFrontend.c_str(), flags);
    if (f < 0)
        SENF_THROW_SYSTEM_EXCEPTION("")<< "Could not open frontend device of DVB adapter " << devFrontend << ".";
    fd(f);
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

prefix_ void senf::DVBFrontendSocketProtocol::tune(const struct dvb_frontend_parameters & frontend)
    const
{
    // tuning
    if (::ioctl(fd(), FE_SET_FRONTEND, &frontend) )
        SENF_THROW_SYSTEM_EXCEPTION("") << "ioctl FE_SET_FRONTEND failed. Socket should initialized with r/w permissions.";;
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
    const
{
    struct dvb_ofdm_parameters ofdm; /* DVB-T Parameters */
    struct dvb_frontend_parameters frontend;

    ::memset(&frontend, 0, sizeof(struct dvb_frontend_parameters));
    ::memset(&ofdm, 0, sizeof(struct dvb_ofdm_parameters));

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
        fe_code_rate_t fec_inner /* forward error correction (see above) */
    )
    const
{
    struct dvb_qpsk_parameters qpsk; /* DVB-S Parameters*/
    struct dvb_frontend_parameters frontend;

    ::memset(&frontend, 0, sizeof(struct dvb_frontend_parameters));
    ::memset(&qpsk, 0, sizeof(struct dvb_qpsk_parameters));

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
        fe_modulation_t modulation
        )
const
{
    struct dvb_qam_parameters qam; /* DVB-C Parameters*/
    struct dvb_frontend_parameters frontend;

    ::memset(&frontend, 0, sizeof(struct dvb_frontend_parameters));
    ::memset(&qam, 0, sizeof(struct dvb_qam_parameters));

    qam.symbol_rate = symbol_rate;
    qam.fec_inner = fec_inner;
    qam.modulation = modulation;


    frontend.frequency = frequency;
    frontend.inversion = inversion;
    frontend.u.qam = qam;

    tune(frontend);
}
prefix_ void senf::DVBFrontendSocketProtocol::setNonBlock(bool on)
    const
{
   if (on)
       ::fcntl(fd(), F_SETFL, ::fcntl(fd(), F_GETFL) | O_NONBLOCK);
   else
       ::fcntl(fd(), F_SETFL, ::fcntl(fd(), F_GETFL) & ~O_NONBLOCK);

}
prefix_ dvb_frontend_info senf::DVBFrontendSocketProtocol::getInfo()
    const
{
    struct dvb_frontend_info info;
    ::memset(&info, 0, sizeof(struct dvb_frontend_info));

    if (::ioctl(fd(), FE_GET_INFO, &info)) {
        SENF_THROW_SYSTEM_EXCEPTION("") << "Could not read on fildescriptor.";
    }
    return info;
}

prefix_ dvb_frontend_parameters senf::DVBFrontendSocketProtocol::getFrontendParam() const {
    struct dvb_frontend_parameters frontend_;

    ::memset(&frontend_, 0, sizeof(struct dvb_frontend_parameters));

    if (::ioctl(fd(), FE_GET_FRONTEND, &frontend_)) {
        switch(errno) {
            case EBADF:
                SENF_THROW_SYSTEM_EXCEPTION("fd is not a valid open file descriptor.");
                break;
            case EFAULT:
                SENF_THROW_SYSTEM_EXCEPTION( "frontend_ points to invalid address." );
                break;
            case EINVAL:
                SENF_THROW_SYSTEM_EXCEPTION( "Maximum supported symbol rate reached. This call may not be implemented by kernel driver!" );
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("Errno: ") << errno;
        }
    }
    return frontend_;
}

prefix_ dvb_frontend_event senf::DVBFrontendSocketProtocol::getEvent() const{
    struct dvb_frontend_event ev ;

    ::memset(&ev, 0, sizeof(struct dvb_frontend_event));

    if (::ioctl(fd(), FE_GET_EVENT, &ev)) {
        switch(errno) {
            case EBADF:
                SENF_THROW_SYSTEM_EXCEPTION("Could not read from frontend device, read-only access to the device is sufficient.");
                break;
            case EWOULDBLOCK:
                SENF_THROW_SYSTEM_EXCEPTION( "No event pending and device is in nonblocking mode." );
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("Errno: ") << errno;
        }
    }
    return ev;
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

prefix_ uint32_t senf::DVBFrontendSocketProtocol::uncorrectedBlocks()
    const
{
    uint32_t uncorrected_blocks;
    if (::ioctl(fd(), FE_READ_UNCORRECTED_BLOCKS, &uncorrected_blocks) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not get bit error rate of DVB adapter.");
    return uncorrected_blocks;
}


prefix_ fe_status_t senf::DVBFrontendSocketProtocol::status()
    const
{
    fe_status_t status;
    if (::ioctl(fd(), FE_READ_STATUS, &status) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not get bit error rate of DVB adapter.");
    return status;
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
