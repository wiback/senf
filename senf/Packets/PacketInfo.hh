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
