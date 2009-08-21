// $Id: DVBSocketController.hh 1119 2009-02-16 13:36:59Z tho $
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


#ifndef HH_SENF_Socket_Protocols_DVB_DVBProtocolWrapper_
#define HH_SENF_Socket_Protocols_DVB_DVBProtocolWrapper_ 1
#include <senf/Utils/Console/Console.hh>
#include <senf/Utils/Console/Console.hh>
#include "DVBDemuxHandles.hh"
// Custom includes

SENF_CONSOLE_REGISTER_ENUM( dmx_input_t, (DMX_IN_FRONTEND)(DMX_IN_DVR) )
;
SENF_CONSOLE_REGISTER_ENUM( dmx_output_t, (DMX_OUT_DECODER)(DMX_OUT_TAP)(DMX_OUT_TS_TAP) )
;
SENF_CONSOLE_REGISTER_ENUM( dmx_pes_type_t, (DMX_PES_AUDIO0)(DMX_PES_VIDEO0)(DMX_PES_TELETEXT0)(DMX_PES_SUBTITLE0)(DMX_PES_PCR0)
        (DMX_PES_AUDIO1)(DMX_PES_VIDEO1)(DMX_PES_TELETEXT1)(DMX_PES_SUBTITLE1)(DMX_PES_PCR1)
        (DMX_PES_AUDIO2)(DMX_PES_VIDEO2)(DMX_PES_TELETEXT2)(DMX_PES_SUBTITLE2)(DMX_PES_PCR2)
        (DMX_PES_AUDIO3)(DMX_PES_VIDEO3)(DMX_PES_TELETEXT3)(DMX_PES_SUBTITLE3)(DMX_PES_PCR3)
        (DMX_PES_OTHER))
;

namespace senf {

    /** \brief Helperclass for configuration and controlling DVB protokoll handles.


     */

    class DVBSectionProtocolWrapper : public senf::DVBProtocolWrapper {

public:
        enum Flags {CHECK_CRC = DMX_CHECK_CRC,
            ONESHOT = DMX_ONESHOT,
            IMMEDIATE_START = DMX_IMMEDIATE_START,
            KERNEL_CLIENT = DMX_KERNEL_CLIENT};
        senf::console::ScopedDirectory<DVBSectionProtocolWrapper> dir;
private:
        const senf::DVBDemuxSectionSocketProtocol& protocol;
public:
        DVBSectionProtocolWrapper(senf::DVBDemuxSectionHandle sh) :
            dir(this), protocol(sh.protocol()) {

            namespace kw = senf::console::kw;
            dir.add("buffersize", &DVBSectionProtocolWrapper::setBufferSize)
            .doc("Set the size of the circular buffer used for filtered data.")
            .arg("size", "in byte");

            dir.add("start", &DVBSectionProtocolWrapper::startFiltering)
            .doc("Starts filtering");

            dir.add("stop", &DVBSectionProtocolWrapper::setBufferSize)
            .doc("Stops filtering");

            dir.node().add("filter", boost::function<void (unsigned short int, unsigned, senf::console::FlagCollection<Flags>, unsigned, unsigned, unsigned)>(senf::membind(&DVBSectionProtocolWrapper::setSectionFilter, this)))
            .arg("pid", "pid to filter")
            .arg("filter", "filter", kw::default_value = 62, kw::default_doc = "0x3e")
            .arg("flags", "or-able: CHECK_CRC, ONESHOT, IMMEDIATE_START, KERNEL_CLIENT", kw::default_value = DMX_IMMEDIATE_START | DMX_CHECK_CRC, kw::default_doc = "(IMMEDIATE_START CHECK_CRC)")
            .arg("mask", "mask", kw::default_value = 0xff, kw::default_doc = "0xff")
            .arg("mode", "mode", kw::default_value = 0, kw::default_doc = "0x00")
            .arg("timeout", "timeout", kw::default_value = 0, kw::default_doc = "0x00")
            .doc("Sets parameters for section filter.");

            dir.add("stop", &DVBSectionProtocolWrapper::setBufferSize)
            .doc("Stops filtering");
        }
        ~DVBSectionProtocolWrapper() {
        }
        void setSectionFilter(unsigned short int pid, u_int8_t filter,
                unsigned int flags, u_int8_t mask, u_int8_t mode,
                unsigned int timeout) {
            protocol.setSectionFilter(pid, timeout, flags, filter, mask, mode);

        }

        ///< Set the size of the circular buffer used for filtered data.
        /**< The default size is two maximum sized sections, i.e. if
         this function is not called a buffer size of 2 * 4096
         bytes will be used.
         \param[in] size Size of circular buffer. */
        void setBufferSize(unsigned long size) {
            protocol.setBufferSize(size);
        }

        void startFiltering() ///< Starts filtering
        {
            protocol.startFiltering();
        }

        void stopFiltering() ///< Stops filtering
        {
            protocol.stopFiltering();
        }

    };
    SENF_CONSOLE_REGISTER_ENUM_MEMBER(DVBSectionProtocolWrapper, Flags,
            (CHECK_CRC)(ONESHOT)(IMMEDIATE_START)(KERNEL_CLIENT))
    ;

    class DVBPESProtocolWrapper : public senf::DVBProtocolWrapper {

public:
        enum Flags {CHECK_CRC = DMX_CHECK_CRC,
            ONESHOT = DMX_ONESHOT,
            IMMEDIATE_START = DMX_IMMEDIATE_START,
            KERNEL_CLIENT = DMX_KERNEL_CLIENT};
        senf::console::ScopedDirectory<DVBPESProtocolWrapper> dir;
private:
        const senf::DVBDemuxPESSocketProtocol& protocol;
public:
        DVBPESProtocolWrapper(senf::DVBDemuxPESHandle sh) :
            dir(this), protocol(sh.protocol()) {
            namespace kw = senf::console::kw;

            dir.node().add("filter", boost::function<void ( unsigned short int, dmx_input_t, dmx_output_t, dmx_pes_type_t, senf::console::FlagCollection<Flags>)>(senf::membind(&DVBPESProtocolWrapper::setPESFilter, this)))
            .arg("pid", "pid to filter")
            .arg("input", "input-filter: DMX_IN_FRONTEND DMX_IN_DVR ")
            .arg("output", "output-filter: DMX_OUT_DECODER DMX_OUT_TAP DMX_OUT_TS_TAP ")
            .arg("pesType", "PES type: DMX_PES_AUDIO[0-3] DMX_PES_VIDEO[0-3] DMX_PES_TELETEXT[0-3], DMX_PES_SUBTITLE[0-3], DMX_PES_PCR[0-3], DMX_PES_OTHER")
            .arg("flags", "or-able: CHECK_CRC, ONESHOT, IMMEDIATE_START, KERNEL_CLIENT", kw::default_value = DMX_IMMEDIATE_START | DMX_CHECK_CRC, kw::default_doc = "(IMMEDIATE_START CHECK_CRC)")
            .doc("Sets parameters for PES filter.");
            dir.add("start", &DVBPESProtocolWrapper::startFiltering)
            .doc("Starts filtering");

            dir.add("stop", &DVBPESProtocolWrapper::stopFiltering)
            .doc("Stops filtering");
        }
        ~DVBPESProtocolWrapper() {
        }
        ;

        void setPESFilter(unsigned short int pid, dmx_input_t input,
                dmx_output_t output, dmx_pes_type_t pesType, unsigned int flags) {
            protocol.setPESFilter(pid, input, output, pesType, flags);
        }
        void startFiltering() ///< Starts filtering
        {
            protocol.startFiltering();
        }

        void stopFiltering() ///< Stops filtering
        {
            protocol.stopFiltering();
        }
    };
    SENF_CONSOLE_REGISTER_ENUM_MEMBER(DVBPESProtocolWrapper, Flags,
            (CHECK_CRC)(ONESHOT)(IMMEDIATE_START)(KERNEL_CLIENT))
    ;
}

#endif

