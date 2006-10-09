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

#ifndef HH_GenericPacket_
#define HH_GenericPacket_ 1

// Custom includes
#include "Packet.hh"

///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {

    /** \brief General packet comprised of header, trailer and payload

        This class implements a generic packet with three sections: a
        header, a trailer and a payload section. The header and
        trailer are not interpreted in any way. The payload can be
        manually chained to any packet interpreter.
      */
    template <unsigned HEADER, unsigned TRAILER=0>
    class GenericPacket : public Packet
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef typename Packet::ptr_t<GenericPacket>::ptr ptr;
        typedef iterator byte_iterator;

        ///////////////////////////////////////////////////////////////////////////

        void init();

        iterator begin_header() const;
        iterator end_header() const;
        static size_type header_len();

        iterator begin_trailer() const;
        iterator end_trailer() const;
        static size_type trailer_len();

        static bool check(iterator const & b, iterator const & e) 
            { return unsigned(e - b) >= HEADER + TRAILER; }
        
    protected:

    private:  
        template <class Arg>
        GenericPacket(Arg const & arg);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize();
        virtual void v_dump(std::ostream & os) const;

        friend class Packet;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "GenericPacket.cci"
#include "GenericPacket.ct"
#include "GenericPacket.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
