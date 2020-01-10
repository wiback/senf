//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
