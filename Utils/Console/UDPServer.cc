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
    \brief UDPServer non-inline non-template implementation */

#include "UDPServer.hh"
//#include "UDPServer.ih"

// Custom includes
#include <boost/algorithm/string/trim.hpp>
#include "../membind.hh"

//#include "UDPServer.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::console::UDPServer::UDPServer(senf::INet4SocketAddress const & address)
    : replies_ (true), emptyReplies_ (true), target_ (), 
      handle_ (senf::UDPv4ClientSocketHandle(address)), 
      readevent_ ("senf::console::UDPServer::readevent", 
                  senf::membind(&UDPServer::handleInput, this), 
                  handle_, 
                  senf::scheduler::FdEvent::EV_READ),
      parser_ (), executor_ ()
{
    if (address.address().multicast())
        handle_.facet<senf::INet4MulticastSocketProtocol>().mcAddMembership(address.address());
    SENF_LOG(("UDP Console server started at " << address ));
}

prefix_ senf::console::UDPServer::UDPServer(senf::INet6SocketAddress const & address)
    : replies_ (true), target_ (), handle_ (senf::UDPv6ClientSocketHandle(address)), 
      readevent_ ("senf::console::UDPServer::readevent", 
                  senf::membind(&UDPServer::handleInput, this), 
                  handle_, 
                  senf::scheduler::FdEvent::EV_READ),
      parser_ (), executor_ ()
{
    if (address.address().multicast())
        handle_.facet<senf::INet6MulticastSocketProtocol>().mcAddMembership(address.address());
    SENF_LOG(("UDP Console server started at " << address ));
}

prefix_ senf::console::UDPServer & senf::console::UDPServer::replies(bool enable)
{
    replies_ = enable;
    return *this;
}

prefix_ senf::console::UDPServer &
senf::console::UDPServer::replies(senf::INet4SocketAddress const & address)
{
    SENF_ASSERT( handle_.local().family() == senf::INet4SocketAddress::addressFamily );
    target_ = address; 
    return *this;
}

prefix_ senf::console::UDPServer &
senf::console::UDPServer::replies(senf::INet6SocketAddress const & address)
{
    SENF_ASSERT( handle_.local().family() == senf::INet6SocketAddress::addressFamily );
    target_ = address;
    return *this;
}

prefix_ senf::console::UDPServer & senf::console::UDPServer::emptyReplies(bool enable)
{
    emptyReplies_ = enable;
    return *this;
}

prefix_ senf::console::DirectoryNode & senf::console::UDPServer::root()
    const
{
    return executor_.chroot();
}

prefix_ senf::console::UDPServer & senf::console::UDPServer::root(DirectoryNode & root)
{
    executor_.chroot(root);
    return *this;
}

prefix_ void senf::console::UDPServer::handleInput(int events)
{
    if (events != senf::scheduler::FdEvent::EV_READ) {
        SENF_LOG((senf::log::IMPORTANT)("Input handle read error. Closing socket."));
        readevent_.disable();
        handle_.close();
        return;
    }

    std::string data;
    senf::GenericBSDSocketAddress address;
    handle_.readfrom(data, address, 0u);
    boost::trim(data);
    
    executor_.cwd(executor_.chroot());
    std::stringstream stream;
    try {
        parser_.parse(data, boost::bind<void>( boost::ref(executor_), boost::ref(stream), _1));
    }
    catch (Executor::ExitException &) {
        // Ignored
    }
    catch (ExceptionMixin & ex) {
        stream << ex.message() << '\n';
        SENF_LOG((senf::log::IMPORTANT)("Error: " << ex.message()));
        SENF_LOG((senf::log::NOTICE)(ex.backtrace()));
    }
    catch (std::exception & ex) {
        stream << ex.what() << '\n';
        SENF_LOG((senf::log::IMPORTANT)("Error: " << ex.what()));
    }
    if (replies_ && (emptyReplies_ || ! stream.str().empty())) {
        if (target_)
            address = target_;
        if (stream.str().empty())
            stream << '\0';
        handle_.writeto(address, stream.str());
    }
    
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "UDPServer.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
