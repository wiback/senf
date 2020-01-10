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
    \brief InterfaceId public header */

#ifndef HH_SENF_Ext_NetEmu_InterfaceId_
#define HH_SENF_Ext_NetEmu_InterfaceId_ 1

// Custom includes
#include <set>
#include <senf/Utils/Console/ParsedCommand.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    struct MACAddress;

namespace emu {

    class InterfaceDeviceId
    {
    public:
        enum Bus_t { Unknown = 0, PCI = 1, PCIe, USB };

        InterfaceDeviceId();
        InterfaceDeviceId(Bus_t b, boost::uint16_t v, boost::uint16_t d);
        InterfaceDeviceId(boost::uint8_t b, boost::uint16_t v, boost::uint16_t d);

        static const InterfaceDeviceId None;

        static InterfaceDeviceId get(senf::MACAddress const & addr);

        bool operator<(InterfaceDeviceId const & other) const;
        bool operator==(InterfaceDeviceId const & other) const;

        Bus_t bus() const;
        boost::uint16_t vendor() const;
        boost::uint16_t device() const;

    private:
        Bus_t bus_;
        boost::uint16_t vendor_;
        boost::uint16_t device_;

        SENF_CONSOLE_PARSE_FRIEND( InterfaceDeviceId );
    };

    std::ostream & operator<<(std::ostream & os, InterfaceDeviceId const & id);

    SENF_CONSOLE_REGISTER_ENUM_MEMBER( InterfaceDeviceId, Bus_t, (Unknown)(PCI)(PCIe)(USB) );


    class InterfaceIdFactoryBase
    {
    public:
        virtual ~InterfaceIdFactoryBase() {};

        MACAddress getId();
        void releaseId(MACAddress const & id);

    private:
        virtual MACAddress v_getId() = 0;
        virtual void v_releaseId(MACAddress const & id) = 0;
    };


    class SimpleInterfaceIdFactory
        : public InterfaceIdFactoryBase
    {
    private:
        MACAddress v_getId();
        void v_releaseId(MACAddress const & id);

        boost::uint32_t calcFirstFreeId();

        typedef std::set<boost::uint16_t> IdSet;
        IdSet idSet_;
    };

}};

SENF_CONSOLE_PARSE_AND_FORMAT_AS_TUPLE( senf::emu::InterfaceDeviceId, (bus_)(vendor_)(device_) );


//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "InterfaceId.cci"
//#include "InterfaceId.ct"
//#include "InterfaceId.cti"
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
