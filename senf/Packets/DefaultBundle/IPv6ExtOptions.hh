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
    \brief IPv6Extension-Options public header */

#ifndef HH_SENF_Packets_DefaultBundle_IPv6ExtOptions_
#define HH_SENF_Packets_DefaultBundle_IPv6ExtOptions_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//#include "IPv6ExtOptions.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {

    /** \brief IPv6 Extension Option parser

        Definition of parser fields in RFC2460 and assignments of <b>type</b> field at <a href="http://www.iana.org/assignments/ipv6-parameters">IANA</a>

        Used in \ref IPv6HopByHopOptionsPacketParser and  \ref IPv6DestinationOptionsPacketParser.

        \see <a href="http://www.iana.org/assignments/ipv6-parameters">IANA IPv6 Parameters</a>\n
             \ref IPv6GenericOptionParser

        \ingroup protocolbundle_default
     */

    class IPv6OptionParser : public PacketParserBase
    {
    public:
#       include SENF_PARSER()

        SENF_PARSER_FIELD   ( type,       UInt8Parser );
        SENF_PARSER_GOTO    ( type                    );
        SENF_PARSER_BITFIELD( altAction,  2, unsigned );
        SENF_PARSER_BITFIELD( changeFlag, 1, unsigned );
        SENF_PARSER_BITFIELD( optionType, 5, unsigned );
        SENF_PARSER_FIELD   ( length,     UInt8Parser );

        SENF_PARSER_FINALIZE( IPv6OptionParser );

        typedef GenericTLVParserRegistry<IPv6OptionParser> Registry;
    };

    typedef GenericTLVParserBase<IPv6OptionParser> IPv6GenericOptionParser;

}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "IPv6ExtOptions.cci"
//#include "IPv6ExtOptions.ct"
//#include "IPv6ExtOptions.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
