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

#ifndef HH_DataPacket_
#define HH_DataPacket_ 1

// Custom includes
#include "Packet.hh"

///////////////////////////////hh.p////////////////////////////////////////

namespace senf {


    /** \brief Non-interpreted Packet

        A DataPacket is an uninterpreted blob of data. It is terminal
        in the sense, that no further packet header may follow after
        this packet. A DataPacket implements the abstract Packet
        interface and nothing more.
     */
    class DataPacket : public Packet
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<DataPacket>::ptr ptr;
        typedef iterator byte_iterator;

        ///////////////////////////////////////////////////////////////////////////

        static void init() {}
        static bool check(iterator const & b, iterator const & e) { return true; }

    private:
        template <class Arg>
        DataPacket(Arg const & arg);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize();
        virtual void v_dump(std::ostream & os) const;

        friend class Packet;
    };


}

///////////////////////////////hh.e////////////////////////////////////////
//#include "DataPacket.cci"
//#include "DataPacket.ct"
#include "DataPacket.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
