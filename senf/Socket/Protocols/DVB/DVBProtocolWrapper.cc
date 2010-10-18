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

/** \file
    \brief DVBSocketController non-inline non-template implementation */

#include "DVBProtocolWrapper.hh"

// Custom includes
#include <senf/Utils/Console/ParsedCommand.hh>
#include <senf/Utils/Console/Utility.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::DVBSectionProtocolWrapper::DVBSectionProtocolWrapper(senf::DVBDemuxSectionHandle sh)
    : protocol(sh.protocol()), dir(this)
{
    namespace kw = senf::console::kw;
    namespace fty = senf::console::factory;

    dir.add("buffersize", fty::Command(&DVBSectionProtocolWrapper::setBufferSize, this)
            .doc("Set the size of the circular buffer used for filtered data.")
            .arg("size", "in byte") );

    dir.add("start", fty::Command(&DVBSectionProtocolWrapper::startFiltering, this)
            .doc("Starts filtering") );

    dir.add("stop", fty::Command(&DVBSectionProtocolWrapper::setBufferSize, this)
            .doc("Stops filtering") );

    dir.add("filter",
            fty::Command<void (unsigned short int,
                                    unsigned,
                                    senf::console::FlagCollection<Flags>,
                                    unsigned,
                                    unsigned,
                                    unsigned)
            >(&DVBSectionProtocolWrapper::setSectionFilter, this)
            .arg("pid", "pid to filter")
            .arg("filter", "filter",
                 kw::default_value = 62,
                 kw::default_doc = "0x3e")
            .arg("flags", "or-able: CHECK_CRC, ONESHOT, IMMEDIATE_START, KERNEL_CLIENT",
                 kw::default_value = DMX_IMMEDIATE_START | DMX_CHECK_CRC,
                 kw::default_doc = "(IMMEDIATE_START CHECK_CRC)")
            .arg("mask", "mask",
                 kw::default_value = 0xff,
                 kw::default_doc = "0xff")
            .arg("mode", "mode",
                 kw::default_value = 0,
                 kw::default_doc = "0x00")
            .arg("timeout", "timeout",
                 kw::default_value = 0,
                 kw::default_doc = "0x00")
            .doc("Sets parameters for section filter.") );

    dir.add("stop", fty::Command(&DVBSectionProtocolWrapper::setBufferSize, this)
            .doc("Stops filtering") );
}


prefix_ senf::DVBPESProtocolWrapper::DVBPESProtocolWrapper(senf::DVBDemuxPESHandle sh)
    : protocol(sh.protocol()), dir(this)
{
    namespace kw = senf::console::kw;
    namespace fty = senf::console::factory;

    dir.add("filter",
            fty::Command<void (unsigned short int,
                                    dmx_input_t,
                                    dmx_output_t,
                                    dmx_pes_type_t,
                                    senf::console::FlagCollection<Flags>)
            >(&DVBPESProtocolWrapper::setPESFilter, this)
            .arg("pid", "pid to filter")
            .arg("input", "input-filter: DMX_IN_FRONTEND DMX_IN_DVR ")
            .arg("output", "output-filter: DMX_OUT_DECODER DMX_OUT_TAP DMX_OUT_TS_TAP ")
            .arg("pesType", "PES type: DMX_PES_AUDIO[0-3] DMX_PES_VIDEO[0-3] "
                            "DMX_PES_TELETEXT[0-3], DMX_PES_SUBTITLE[0-3], DMX_PES_PCR[0-3], "
                            "DMX_PES_OTHER")
            .arg("flags", "or-able: CHECK_CRC, ONESHOT, IMMEDIATE_START, KERNEL_CLIENT",
                 kw::default_value = DMX_IMMEDIATE_START | DMX_CHECK_CRC,
                 kw::default_doc = "(IMMEDIATE_START CHECK_CRC)")
            .doc("Sets parameters for PES filter.") );

    dir.add("start", fty::Command(&DVBPESProtocolWrapper::startFiltering, this)
            .doc("Starts filtering") );

    dir.add("stop", fty::Command(&DVBPESProtocolWrapper::stopFiltering, this)
            .doc("Stops filtering") );
}

///////////////////////////////cc.e////////////////////////////////////////
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
