// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <pug@berlios.de>
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
    \brief RTPPacket non-inline non-template implementation */

#include "RTPPacket.hh"
//#include "UDPPacket.ih"

// Custom includes
#include <boost/io/ios_state.hpp>

#define prefix_

namespace 
{
    std::string ptName(int pt){
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
        while ( ptList[n].pt != -1)
        {
            if( ptList[n].pt == pt)
                return ptList[n].name;
            ++n;
        }
        
        return "UNKNOWN";
    }
}

prefix_ void senf::RTPPacketType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);
    os << "Real Time Protocol:\n"
       << "  Version                            : " << p->version() << "\n"
       << "  Padding                            : " << p->padding() << "\n"
       << "  Extension                          : " << p->extension() << "\n"
       << "  Contributing Source Count(CC)      : " << p->csrcCount() << "\n"
       << "  Marker                             : " << p->marker() << "\n"
       << "  Payload Type                       : " << p->payloadType() << " "<< ptName(p->payloadType() ) <<"\n"
       << "  Sequence number                    : " << p->seqNumber() << "\n"
       << "  Timestamp                          : " << p->timeStamp() << "\n"
       << "  Synchronisation Source Identifier  : " << p->synSourceId() << "\n";

}

#undef prefix_
