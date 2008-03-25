// $Id$
//
// Copyright (C) 2008
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
    \brief psi2ts.cc non-inline non-template implementation */

#include "psi2ts.hh"
//#include "psi2ts.ih"

// Custom includes

//#include "psi2ts.cc.mpp"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    template <class InputIterator, class Distance>
    void advance_max(InputIterator& i, Distance n, InputIterator& i_end)
    {
        Distance d = std::min( std::distance(i, i_end), n);
        std::advance( i, d);
    }
}


prefix_ Psi2TsModule::Psi2TsModule(unsigned pid)
{
    pid_ = pid;
    continuity_counter_ = 0;
    state_ = IDLE;
    route( input, output );
    input.onRequest( &Psi2TsModule::onRequest );
}

prefix_ void Psi2TsModule::onRequest()
{
    senf::PacketData & section = input.read().data();
    iterator sec_end = section.end();
    iterator begin = section.begin();
    iterator end = section.begin();
    advance_max( end, 184, sec_end);
    
    do {
        senf::TransportPacket tsPacket (senf::TransportPacket::create(188));
        tsPacket->continuity_counter() = next_continuity_counter();
        tsPacket->pid() = pid_;
        if (state_ == IDLE) {
            state_ = PROC;
            tsPacket->pusi() = true;
        }
        senf::PacketData & payloadData (tsPacket.next().data());
        std::fill(
                std::copy( begin, end, payloadData.begin() ),
                payloadData.end(),
                0xff
        );
        tsPacket.finalize();
        
        output.write( tsPacket);
        
        advance_max( begin, 184, sec_end);
        advance_max( end,   184, sec_end);
    } while (begin != end);
    state_ = IDLE;
}

prefix_ unsigned Psi2TsModule::next_continuity_counter()
{
    continuity_counter_ = (continuity_counter_ + 1) % 16;
    return continuity_counter_;
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "psi2ts.cc.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
