// $Id$
//
// Copyright (C) 2009
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
    \brief UDPServer public header */

#ifndef HH_SENF_Utils_Console_UDPServer_
#define HH_SENF_Utils_Console_UDPServer_ 1

// Custom includes
#include <boost/noncopyable.hpp>
#include <senf/Socket/Protocols/INet/UDPSocketHandle.hh>
#include <senf/Utils/Logger/SenfLog.hh>
#include <senf/Scheduler/FdEvent.hh>
#include "Parse.hh"
#include "Executor.hh"

//#include "UDPServer.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace console {

    /** \brief UDP Console server

        This class provides UDP access to the console to allow remote scripting. The UDP console
        does support multicast operation.

        Every UDP packet will be executed in a clean context: No directory groups are open/closed,
        and the current directory is always the root directory.

        By default, the server will send command replies via UDP to the sender of the corresponding
        incoming command. Replies may however either be completely disabled or be sent to a fixed
        address (which may be a multicast address).

        \ingroup console_access
      */
    class UDPServer
        : public boost::noncopyable
    {
        SENF_LOG_CLASS_AREA();
        SENF_LOG_DEFAULT_LEVEL(senf::log::NOTICE);
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef senf::ClientSocketHandle<
            senf::MakeSocketPolicy<senf::UDPv4SocketProtocol::Policy,
                                   senf::BSDAddressingPolicy>::policy > Handle;

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        explicit UDPServer(senf::INet4SocketAddress const & address);
                                        ///< Open UDP server on \a address
        explicit UDPServer(senf::INet6SocketAddress const & address);
                                        ///< Open UDP server on \a address

        //\}
        //-////////////////////////////////////////////////////////////////////////

        UDPServer & replies(bool enable); ///< Enable or disable reply packets
                                        /** \e Default: enabled */
        UDPServer & replies(senf::INet4SocketAddress const & address);
                                        ///< Send replies to \a address
                                        /**< \e Default: no address set (send replies to sender) */
        UDPServer & replies(senf::INet6SocketAddress const & address);
                                        ///< Send replies to \a address
                                        /**< \e Default: no address set (send replies to sender) */

        UDPServer & emptyReplies(bool enable); ///< Enable or disable empty reply packets
                                        /**< \e Default: enabled */

        DirectoryNode & root() const;   ///< Get root node

        UDPServer & root(DirectoryNode & root); ///< Set root node
                                        /**< \a node will be the root node for all clients launched
                                             from this server. */

    protected:

    private:
        void handleInput(int events);

        bool replies_;
        bool emptyReplies_;
        senf::GenericBSDSocketAddress target_;

        Handle handle_;
        senf::scheduler::FdEvent readevent_;
        CommandParser parser_;
        Executor executor_;
    };


}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "UDPServer.cci"
//#include "UDPServer.ct"
//#include "UDPServer.cti"
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
