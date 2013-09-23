// $Id$
//
// Copyright (C) 2013
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
//   Stefan Bund <g0dil@berlios.de>

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
