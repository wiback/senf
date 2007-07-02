// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#ifndef HH_EthernetPacket_
#define HH_EthernetPacket_ 1

// Custom includes
#include "Packets/Packet.hh"
#include "Packets/ParseInt.hh"
#include "Packets/ParseArray.hh"
#include "Packets/PacketRegistry.hh"

//#include "EthernetPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {


    template <class Iterator=nil, class IPacket=nil>
    struct Parse_Ethernet : public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_Ethernet<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_Ethernet() {}
        Parse_Ethernet(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        static unsigned bytes() { return 14; }

        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_Array  < 6, Parse_UInt8<>, Iterator > Parse_MAC;
        typedef Parse_UInt16 < Iterator  >                  Parse_Type;

        Parse_MAC  destination() const { return Parse_MAC  (this->i() ); }
        Parse_MAC  source()      const { return Parse_MAC  (this->i() + Parse_MAC::size() ); }
        Parse_Type type()        const { return Parse_Type (this->i() + 2*Parse_MAC::size() ); }
    };

    struct EtherTypes {
        // See http://www.iana.org/assignments/ethernet-numbers
        typedef boost::uint16_t key_t;
    };

    class EthernetPacket
        : public Packet,
          public Parse_Ethernet<Packet::iterator, EthernetPacket>,
          public PacketRegistryMixin<EtherTypes,EthernetPacket>
    {
        using PacketRegistryMixin<EtherTypes,EthernetPacket>::registerInterpreter;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<EthernetPacket>::ptr ptr;

        ///////////////////////////////////////////////////////////////////////////

    private:
        template <class Arg>
        EthernetPacket(Arg const & arg);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize();
        virtual void v_dump(std::ostream & os) const;

        friend class Packet;
    };

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_EthVLan : public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_Ethernet<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_EthVLan() {}
        Parse_EthVLan(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        static unsigned bytes() { return 4; }

        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_UIntField < 0,  3, Iterator > Parse_Priority;
        typedef Parse_Flag          < 3, Iterator > Parse_CFI;
        typedef Parse_UIntField < 4, 16, Iterator > Parse_VLanId;
        typedef Parse_UInt16           < Iterator > Parse_Type;

        Parse_Priority priority() const { return Parse_Priority(this->i()); }
        Parse_CFI      cfi()      const { return Parse_CFI(this->i()); }
        Parse_VLanId   vlanId()   const { return Parse_VLanId(this->i()); }
        Parse_Type     type()     const { return Parse_Type(this->i()+2); }
    };

    class EthVLanPacket
        : public Packet,
          public Parse_EthVLan<Packet::iterator, EthVLanPacket>,
          public PacketRegistryMixin<EtherTypes, EthVLanPacket>
    {
        using PacketRegistryMixin<EtherTypes, EthVLanPacket>::registerInterpreter;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<EthVLanPacket>::ptr ptr;

        ///////////////////////////////////////////////////////////////////////////

    private:
        template <class Arg>
        EthVLanPacket(Arg const & arg);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize();
        virtual void v_dump(std::ostream & os) const;

        friend class Packet;
    };

}


///////////////////////////////hh.e////////////////////////////////////////
//#include "EthernetPacket.cci"
//#include "EthernetPacket.ct"
#include "EthernetPacket.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
