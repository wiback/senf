// $Id$
//
// Copyright (C) 2008
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
//   Philipp Batroff <pug@berlios.de>

/** \file
    \brief RTPPacket non-inline non-template implementation */

#include "RTPPacket.hh"

// Custom includes
#include <boost/io/ios_state.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    std::string ptName(int pt)
    {
        struct pt_item
        {
            int pt;
            std::string name;
        };
        static pt_item ptList[27] = {
            {0,"PCMU"},{1,"1016"},{2,"G721"},{3,"GSM"},
            {5,"DVI8"},{6,"DVI16"},{7,"LPC"},{8,"PCMA"},
            {9,"G722"},{10,"L16"},{11,"L16S"},{14,"MPA"},
            {25,"CELB"},{26,"JPEG"},{31,"H261"},{32,"MPV"},
            {33,"MP2T"},{34,"263"},{110,"MPEG AAC"},{111,"11L16"},
            {112,"11L16S"},{113,"22L16"},{114,"22L16S"},{115,"32L16"},
            {116,"32L16S"},{127,"HTML"},{-1,""}
        };
        int n = 0;
        while (ptList[n].pt != -1)
        {
            if (ptList[n].pt == pt)
                return ptList[n].name;
            ++n;
        }

        return "UNKNOWN";
    }
}

prefix_ void senf::RTPPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Real Time Protocol:\n"
       << senf::fieldName("version")               << p->version() << "\n"
       << senf::fieldName("padding")               << p->padding() << "\n"
       << senf::fieldName("extension")             << p->extension() << "\n"
       << senf::fieldName("contributing source cnt")   << p->csrcCount() << "\n"
       << senf::fieldName("marker")                << p->marker() << "\n"
       << senf::fieldName("payload type")          << p->payloadType() << " "
           << ptName(p->payloadType() ) <<"\n"
       << senf::fieldName("sequence number")       << p->seqNumber() << "\n"
       << senf::fieldName("timestamp")             << p->timeStamp() << "\n"
       << senf::fieldName("sync source id")        << p->synSourceId() << "\n";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
