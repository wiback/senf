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
    \brief FastConnector public header */

#ifndef HH_SENF_PPI_FastConnector_
#define HH_SENF_PPI_FastConnector_ 1

// Custom includes
#include <boost/noncopyable.hpp>
#include <senf/Packets/Packets.hh>
#include <senf/Utils/FastDelegate/FastDelegate.h>
#include "Connectors.hh"

//#include "FastConnector.mpp"
///////////////////////////////hh.p////////////////////////////////////////


namespace senf {
namespace ppi {
namespace connector {

    template <class PacketType=Packet> class FastPassiveInput;
    template <class PacketType=Packet> class FastActiveOutput;

}}}

namespace senf {
namespace ppi {

    template <class PacketType>
    void connect(connector::FastActiveOutput<PacketType> & source,
                 connector::FastPassiveInput<PacketType> & target);

}}

namespace senf {
namespace ppi {
namespace connector {

    /** \defgroup fast_connectors Fast Connectors

        Fast connectors are simplified form of the generic PPI connectors:

        \li they do not support automatic queueing
        \li they do not support throttling
        \li no in-module routing (have to be initialized using \c noroute)
        \li they do not support automatic packet type casting
        \li callbacks are restricted to module member functions
        \li only passive input and active output are supported

        With these restrictions, fast connectors provide an almost zero overhead call path for
        simple packet forwarding needs.

        To allow more flexible fast connector connections, the PassiveConnectorAdapter template is
        provided. This class converts between an arbitrary passive input and an arbitrary active
        output. If needed, it will perform automatic packet type casting.
     */

    ///\addtogroup fast_connectors
    //\{

    /** \brief Fast passive input connector
      */
    template <class PacketType>
    class FastPassiveInput : public Connector
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        FastPassiveInput();
        virtual ~FastPassiveInput();

        ///@}

        FastActiveOutput<PacketType> & peer() const;

        template <class Handler>
        void onRequest(Handler handler);

        PacketType const & operator()() const;

    protected:

    private:
        virtual void v_init();

        fastdelegate::FastDelegate<void ()> cb_;
        PacketType const * packet_;

        friend class FastActiveOutput<PacketType>;
    };

    /** \brief Fast active output connector
      */
    template <class PacketType>
    class FastActiveOutput : public Connector
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        FastActiveOutput();
        virtual ~FastActiveOutput();

        ///@}

        FastPassiveInput<PacketType> & peer() const;

        template <class Handler>
        void onRequest(Handler handler);

        void operator()(PacketType const & packet) const;

    protected:

    private:
        virtual void v_init();

        friend void senf::ppi::connect<>(connector::FastActiveOutput<PacketType> &,
                                         connector::FastPassiveInput<PacketType> &);
    };

    ///\}

}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "FastConnector.cci"
//#include "FastConnector.ct"
#include "FastConnector.cti"
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
