// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <g0dil@berlios.de>
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
    \brief IpV6Packet public header */

#ifndef HH_IpV6Packet_
#define HH_IpV6Packet_ 1

// Custom includes
#include "Packets/Packet.hh"
#include "Packets/ParseInt.hh"
#include "Packets/ParseArray.hh"
#include "Packets/PacketRegistry.hh"
#include "IpV4Packet.hh"

//#include "IpV6Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    // See RFC2460
    template <class Iterator=nil, class IpV6Packet=nil>
    struct Parse_IpV6 : public ParserBase<Iterator,IpV6Packet>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_IpV6<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_IpV6() {}
        Parse_IpV6(Iterator const & i) : ParserBase<Iterator,IpV6Packet>(i) {}

        static unsigned bytes() { return 40; }

        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_UIntField <  0,  4, Iterator > Parse_Version;
        typedef Parse_UIntField <  4, 12, Iterator > Parse_Class;
        typedef Parse_UIntField < 12, 32, Iterator > Parse_FlowLabel;
        typedef Parse_UInt8     <         Iterator > Parse_8bit;
        typedef Parse_UInt16    <         Iterator > Parse_16bit;

        typedef Parse_Array < 16, Parse_8bit, Iterator > Parse_Addr;

        Parse_Version    version()       const { return Parse_Version   (this->i()      ); }
        Parse_Class      trafficClass()  const { return Parse_Class     (this->i()      ); }
        Parse_FlowLabel  flowLabel()     const { return Parse_FlowLabel (this->i()      ); }
        Parse_16bit      length()        const { return Parse_16bit     (this->i() +  4 ); }
        Parse_8bit       nextHeader()    const { return Parse_8bit      (this->i() +  6 ); }
        Parse_8bit       hopLimit()      const { return Parse_8bit      (this->i() +  7 ); }
        Parse_Addr       source()        const { return Parse_Addr      (this->i() +  8 ); }
        Parse_Addr       destination()   const { return Parse_Addr      (this->i() + 24 ); }
    };

    class IpV6Packet
        : public Packet,
          public Parse_IpV6<Packet::iterator, IpV6Packet>,
          public PacketRegistryMixin<IpTypes, IpV6Packet>
    {
        using PacketRegistryMixin<IpTypes,IpV6Packet>::registerInterpreter;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<IpV6Packet>::ptr ptr;

        ///////////////////////////////////////////////////////////////////////////

    private:
        template <class Arg>
        IpV6Packet(Arg const & arg);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize();
        virtual void v_dump(std::ostream & os) const;

        friend class Packet;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "IpV6Packet.cci"
//#include "IpV6Packet.ct"
#include "IpV6Packet.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
