// $Id:$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//       Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief PacketDropper non-inline non-template implementation */

#include "PacketDropper.hh"
//#include "PacketDropper.ih"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
prefix_ senf::emu::PacketDropper::PacketDropper(double rate)
{
    route( input, output);
    randomEngine_.seed( senf::ClockService::in_nanoseconds(ClockService::now()));
    lossRate( rate);
}

prefix_ void senf::emu::PacketDropper::lossRate(double lossRate)
{
    lossRate_ = lossRate;
    if (lossRate == 0) {
        input.onRequest( &PacketDropper::onRequest_forwardAll);
        return;
    }
    if (lossRate >= 1) {
        input.onRequest( &PacketDropper::onRequest_discardAll);
        return;
    }
    input.onRequest( &PacketDropper::onRequest_random);
    randomDistribution_.reset( new RandomDistribution(lossRate));
    randomGenerator_.reset( new RandomGenerator(randomEngine_, *randomDistribution_));
}

prefix_ double senf::emu::PacketDropper::lossRate()
    const
{
    return lossRate_;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
