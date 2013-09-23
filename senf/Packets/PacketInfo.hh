// $Id$
//
// Copyright (C) 2013
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief PacketInfo public header */

#ifndef HH_SENF_Packets_PacketInfo_
#define HH_SENF_Packets_PacketInfo_ 1

// Custom includes
#include <senf/Utils/safe_bool.hh>
#include <boost/intrusive_ptr.hpp>
#include "Packet.hh"

//#include "PacketInfo.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    class PacketInfo : public senf::safe_bool<PacketInfo>
    {
    public:
        PacketInfo();
        explicit PacketInfo(Packet const & packet);

        bool boolean_test() const;

        Packet first() const;

        template <class Annotation>
        Annotation & annotation();
        void clearAnnotations();
        void dumpAnnotations(std::ostream & os);

        bool is_shared() const;
        bool usingExternalMemory() const;
        void releaseExternalMemory() const;

        void memDebug(std::ostream & os) const;

    private:
        boost::intrusive_ptr<detail::PacketImpl> impl_;

        friend class detail::PacketImpl;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#include "PacketInfo.cci"
//#include "PacketInfo.ct"
//#include "PacketInfo.cti"
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
