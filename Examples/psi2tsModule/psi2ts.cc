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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief psi2ts.cc non-inline non-template implementation */

#include "psi2ts.hh"
//#include "psi2ts.ih"

// Custom includes

//#include "psi2ts.cc.mpp"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    template <class InputIterator, class Distance>
    void advance_max(InputIterator& i, Distance n, InputIterator& i_end)
    {
        Distance d = std::min( std::distance(i, i_end), n);
        std::advance( i, d);
    }
}


prefix_ Psi2TsModule::Psi2TsModule(unsigned pid, senf::ClockService::clock_type timeout)
{
    pid_ = pid;
    continuity_counter_ = 0;
    state_ = IDLE;
    route( input, output );
    input.onRequest( &Psi2TsModule::onRequest );
    timeout_ = timeout;
}
#include <senf/Utils/hexdump.hh>
prefix_ void Psi2TsModule::onRequest()
{
    senf::PacketData & section = input.read().data();
    iterator sec_end = section.end();
    iterator begin = section.begin();
    iterator end = section.begin();
    advance_max( end, 183, sec_end);

    do {
        senf::TransportPacket tsPacket (senf::TransportPacket::create(188));
        tsPacket->continuity_counter() = next_continuity_counter();
        tsPacket->pid() = pid_;
        if (state_ == IDLE) {
            state_ = PROC;
            tsPacket->setPUSI(true);
            tsPacket->pointer_field() = 0;
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


//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
