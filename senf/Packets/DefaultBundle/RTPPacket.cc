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
    \brief RTPPacket non-inline non-template implementation */

#include "RTPPacket.hh"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::RTPPacket );

struct pt_item
{
    int pt;
    std::string name;
};

static pt_item ptList[] = {
    {0,"PCMU"},{1,"1016"},{2,"G721"},{3,"GSM"},
    {5,"DVI8"},{6,"DVI16"},{7,"LPC"},{8,"PCMA"},
    {9,"G722"},{10,"L16"},{11,"L16S"},{14,"MPA"},
    {25,"CELB"},{26,"JPEG"},{31,"H261"},{32,"MPV"},
    {33,"MP2T"},{34,"263"},{110,"MPEG AAC"},{111,"11L16"},
    {112,"11L16S"},{113,"22L16"},{114,"22L16S"},{115,"32L16"},
    {116,"32L16S"},{127,"HTML"},{-1,""}
};

static std::string ptName(int pt)
{
    int n = 0;
    while (ptList[n].pt != -1){
        if (ptList[n].pt == pt)
            return ptList[n].name;
        ++n;
    }
    
    return "UNKNOWN";
}

prefix_ void senf::RTPPacketType::dump(packet p, std::ostream & os)
{
    os << "Real Time Protocol:\n"
       << senf::fieldName("version")                 << p->version() << "\n"
       << senf::fieldName("padding")                 << p->padding() << "\n"
       << senf::fieldName("extension")               << p->extension() << "\n"
       << senf::fieldName("contributing source cnt") << p->csrcCount() << "\n"
       << senf::fieldName("marker")                  << p->marker() << "\n"
       << senf::fieldName("payload type")            << p->payloadType() << " "
           << ptName(p->payloadType() ) <<"\n"
       << senf::fieldName("sequence number")         << p->seqNumber() << "\n"
       << senf::fieldName("timestamp")               << p->timeStamp() << "\n"
       << senf::fieldName("sync source id")          << p->synSourceId() << "\n";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
