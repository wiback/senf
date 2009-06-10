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
    \brief UDPServer public header */

#ifndef HH_SENF_Utils_Console_UDPServer_
#define HH_SENF_Utils_Console_UDPServer_ 1

// Custom includes
#include <boost/utility.hpp>
#include "../../Socket/Protocols/INet/UDPSocketHandle.hh"
#include "../Logger/SenfLog.hh"
#include "../../Scheduler/Scheduler.hh"
#include "Parse.hh"
#include "Executor.hh"

//#include "UDPServer.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef senf::ClientSocketHandle<
            senf::MakeSocketPolicy<senf::UDPv4SocketProtocol::Policy,
                                   senf::BSDAddressingPolicy>::policy > Handle;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit UDPServer(senf::INet4SocketAddress const & address);
                                        ///< Open UDP server on \a address
        explicit UDPServer(senf::INet6SocketAddress const & address);
                                        ///< Open UDP server on \a address
        
        ///@}
        ///////////////////////////////////////////////////////////////////////////

        UDPServer & replies(bool enable); ///< Enable or disable reply packets
        UDPServer & replies(senf::INet4SocketAddress const & address);
                                        ///< Send replies to \a address
        UDPServer & replies(senf::INet6SocketAddress const & address);
                                        ///< Send replies to \a address

        UDPServer & emptyReplies(bool enable); ///< Enable or disable empty reply packets

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

///////////////////////////////hh.e////////////////////////////////////////
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
