//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief WLANController public header */

#ifndef HH_SENF_Ext_NetEmu_Ethernet_EthernetController_
#define HH_SENF_Ext_NetEmu_Ethernet_EthernetController_ 1

// Custom includes
#include <senf/Socket/NetdeviceController.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    /** \brief Control ethernet interfaces

     */
    class EthernetController
        : public NetdeviceController
    {
    public:
        enum Duplex_mode { HALF_DUPLEX, FULL_DUPLEX, UNKNOWN_DUPLEX } ;

        explicit EthernetController(std::string const & interface_name);
        explicit EthernetController(int interface_index);

        unsigned speed() const;
        Duplex_mode duplex() const;
        std::string duplexAsString() const;
        static std::string duplexAsString(Duplex_mode mode);
        bool hasLink() const;
        bool nWayReset();
        bool ringSize(std::uint32_t rx, std::uint32_t tx);
        std::pair<std::uint32_t,std::uint32_t> ringSize();
        bool setEEE(bool on);
        bool getEEE();
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "EthernetController.cci"
//#include "EthernetController.ct"
//#include "EthernetController.cti"
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
