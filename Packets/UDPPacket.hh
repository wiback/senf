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

#ifndef HH_UDPPacket_
#define HH_UDPPacket_ 1

// Custom includes
#include "Packet.hh"
#include "ParseInt.hh"
#include "ParseArray.hh"
#include "PacketRegistry.hh"

//#include "UDPPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    // See RFC768
    template <class Iterator=nil, class IPacket=nil>
    struct Parse_UDP : public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_UDP<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_UDP() {}
        Parse_UDP(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        static unsigned bytes() { return 8; }

        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_UInt16 < Iterator > Parse_16bit;

        Parse_16bit source()          const { return Parse_16bit      (this->i()     ); }
        Parse_16bit destination()     const { return Parse_16bit      (this->i() + 2 ); }
        Parse_16bit length()          const { return Parse_16bit      (this->i() + 4 ); }
        Parse_16bit crc()             const { return Parse_16bit      (this->i() + 6 ); }

    };

    class UDPPacket
        : public Packet,
          public Parse_UDP<Packet::iterator, UDPPacket>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<UDPPacket>::ptr ptr;

        ///////////////////////////////////////////////////////////////////////////

    private:
        template <class Arg>
        UDPPacket(Arg const & arg);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize();
        virtual void v_dump(std::ostream & os) const;

        friend class Packet;
    };
}


///////////////////////////////hh.e////////////////////////////////////////
//#include UDPPacket.cci"
//#include "UDPPacket.ct"
#include "UDPPacket.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
