// $Id$
//
// Copyright (C) 2012
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
