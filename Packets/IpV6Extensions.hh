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
    \brief IpV6Extensions public header */

#ifndef HH_IpV6Extensions_
#define HH_IpV6Extensions_ 1

// Custom includes
#include "IpV6Packet.hh"

//#include "IpV6Extensions.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    // See RFC2460
    template <class Iterator=nil, class IPacket=nil>
    struct Parse_IpV6Extension_Fragment
	: public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_IpV6Extension_Fragment<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_IpV6Extension_Fragment() {}
        Parse_IpV6Extension_Fragment(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        static unsigned bytes() { return 8; }
        
        ///////////////////////////////////////////////////////////////////////////

	typedef Parse_UInt8     <         Iterator > Parse_8bit;
	typedef Parse_UIntField <  0, 13, Iterator > Parse_Offset;
	typedef Parse_UIntField < 13, 15, Iterator > Parse_Reserved;
	typedef Parse_Flag      < 15,     Iterator > Parse_More;
	typedef Parse_UInt32    <         Iterator > Parse_32bit;

	Parse_8bit      nextHeader()      const { return Parse_8bit      (this->i()      ); }
	Parse_8bit      reserved1()       const { return Parse_8bit      (this->i() +  1 ); }
	Parse_Offset    fragmentOffset()  const { return Parse_Offset    (this->i() +  2 ); }
	Parse_Reserved  reserved2()       const { return Parse_Reserved  (this->i() +  2 ); }
	Parse_More      moreFragments()   const { return Parse_More      (this->i() +  2 ); }
	Parse_32bit     id()              const { return Parse_32bit     (this->i() +  4 ); }
    };

    class IpV6Extension_Fragment
	: public Packet,
	  public Parse_IpV6Extension_Fragment<Packet::iterator, IpV6Extension_Fragment>,
	  public PacketRegistryMixin<IpTypes, IpV6Extension_Fragment>
    {
	using PacketRegistryMixin<IpTypes,IpV6Extension_Fragment>::registerInterpreter;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<IpV6Extension_Fragment>::ptr ptr;

        ///////////////////////////////////////////////////////////////////////////

    private:
        template <class Arg>
        IpV6Extension_Fragment(Arg const & arg);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize();
        virtual void v_dump(std::ostream & os) const;

        friend class Packet;
    };
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "IpV6Extensions.cci"
#include "IpV6Extensions.ct"
//#include "IpV6Extensions.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// ispell-local-dictionary: "american"
// End:
