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
    \brief MonitorModule public header */

#ifndef HH_SENF_PPI_MonitorModule_
#define HH_SENF_PPI_MonitorModule_ 1

// Custom includes
#include "Module.hh"
#include "Connectors.hh"

//#include "MonitorModule.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Base class providing simple monitor %module support

        A monitor %module is a \ref senf::ppi::module::Module "module" which needs information
        about traversing packets but does not really act on the packets. Because of this, it is
        \e optional to connect the output: If the output is not connected, the packets will be
        silently dropped.

        This allows to add monitor modules either into an existing chain or add them using an
        ActiveDuplicator.

        To write a monitor %module, derive from senf::ppi::module::MonitorModule instead of
        senf::ppi::module and implement v_handlePacket():

        \code
        class CountPackets
            : public senf::ppi::module::MonitorModule<>
        {
            SENF_PPI_MODULE(CountPackets);
        public:
            CountPackets() : counter_ (0u) {}

        private:
            virtual void v_handlePacket(Packet const & p)
            { ++ counter_; }

            unsigned counter_;
        };
        \endcode

        You may of course add events (or even further connectors besides \c input and \c output
        provided by MonitorModule) to the %module.

        \tparam PacketType type of packet expected on input and sent on output. This is also the
            type of the v_handlePacket() argument.
     */
    template <class PacketType=Packet>
    class MonitorModule : public Module
    {
    public:
        senf::ppi::connector::PassiveInput<PacketType> input;
        senf::ppi::connector::ActiveOutput<PacketType> output;

    protected:
        MonitorModule();

        virtual void v_handlePacket(PacketType const & p) = 0; ///< Called for each packet

    private:
        void request();
        void throttle();
        void unthrottle();
    };


}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MonitorModule.cci"
#include "MonitorModule.ct"
#include "MonitorModule.cti"
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
