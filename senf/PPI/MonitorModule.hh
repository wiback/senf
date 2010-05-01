// $Id$
//
// Copyright (C) 2009
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
    \brief MonitorModule public header */

#ifndef HH_SENF_PPI_MonitorModule_
#define HH_SENF_PPI_MonitorModule_ 1

// Custom includes
#include "Module.hh"
#include "Connectors.hh"

//#include "MonitorModule.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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

///////////////////////////////hh.e////////////////////////////////////////
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
