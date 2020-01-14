//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief EUI64Parser public header */

#ifndef HH_SENF_Packets_DefaultBundle_EUI64Parser_
#define HH_SENF_Packets_DefaultBundle_EUI64Parser_ 1

// Custom includes
#include <senf/Packets.hh>
#include <senf/Socket/Protocols/Raw/EUI64.hh>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {

    /** \brief Parse an EUI64 address

        The EUI64 address is returned by value as a 8-byte sequence

        \see EUI64
     */
    struct EUI64Parser
        : public ArrayValueParserBase<EUI64Parser, EUI64>
    {
        EUI64Parser(data_iterator i, state_type s) : Base(i,s) {}

        using Base::operator=;
    };

}
//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "EUI64Parser.cci"
//#include "EUI64Parser.ct"
//#include "EUI64Parser.cti"
#endif
