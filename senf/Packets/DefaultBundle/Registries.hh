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
    \brief DefaultBundle packet registries public header */

#ifndef HH_SENF_Packets_DefaultBundle_Registries_
#define HH_SENF_Packets_DefaultBundle_Registries_ 1

// Custom includes

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief EtherType registry

        This registry registers packet types with their EtherType number.

        \see <a href="http://www.iana.org/assignments/ethernet-numbers">Ethernet numbers</a> \n
             <a href="http://standards.ieee.org/develop/regauth/ethertype/eth.txt">IEEE Public EtherType list</a> \n
            \ref PacketRegistry
     */
    struct EtherTypes {
        typedef boost::uint16_t key_t;
    };

    struct EtherOUIExtTypes {
        typedef boost::uint64_t key_t;

        static key_t type(boost::uint32_t oui, boost::uint16_t extType) {
            return (boost::uint64_t(oui & 0x00ffffff) << 16) | extType;
        }
        static boost::uint32_t oui(key_t key) {
            return boost::uint32_t( key >> 16);
        }
        static boost::uint16_t ext_type(key_t key) {
            return boost::uint16_t( key);
        }
    };

    
    /** \brief IP protocol number registry

        This registers packets with their IP protocol number.

        \see <a href="http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xml">Protocol numbers</a> \n
            PacketRegistry
     */
    struct IPTypes {
        typedef boost::uint8_t key_t;
    };

    // ICMP type registry
    struct ICMPTypes {
        typedef boost::uint8_t key_t;
    };
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "EthernetPacket.cci"
//#include "EthernetPacket.ct"
//#include "EthernetPacket.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
