// $Id$
//
// Copyright (C) 2008 
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
    \brief Server non-inline non-template implementation */

#include "Server.hh"
#include "Server.ih"

// Custom includes
#include <boost/algorithm/string/trim.hpp>
#include "../Utils/senfassert.hh"
#include "../Utils/membind.hh"
#include "../Utils/Logger/SenfLog.hh"

//#include "Server.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::console::start(senf::INet4SocketAddress const & address)
{
    senf::console::detail::Server::start(senf::TCPv4ServerSocketHandle(address));
    SENF_LOG((detail::Server::SENFLogArea)(log::NOTICE)( 
                 "Console server started at " << address ));
}

prefix_ void senf::console::start(senf::INet6SocketAddress const & address)
{
    senf::console::detail::Server::start(senf::TCPv6ServerSocketHandle(address));
    SENF_LOG((detail::Server::SENFLogArea)(log::NOTICE)( 
                 "Console server started at " << address ));
}

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::Server

boost::scoped_ptr<senf::console::detail::Server> senf::console::detail::Server::instance_;

prefix_ void senf::console::detail::Server::start(ServerHandle handle)
{
    SENF_ASSERT( ! instance_ );
    instance_.reset(new Server(handle));
}

prefix_ senf::console::detail::Server::Server(ServerHandle handle)
    : handle_ (handle)
{
    Scheduler::instance().add( handle_, senf::membind(&Server::newClient, this) );
}

prefix_ senf::console::detail::Server::~Server()
{
    Scheduler::instance().remove(handle_);
}

prefix_ void senf::console::detail::Server::newClient(Scheduler::EventId event)
{
    ServerHandle::ClientSocketHandle client (handle_.accept());
    boost::intrusive_ptr<Client> p (new Client(client));
    clients_.insert( p );
    SENF_LOG(( "Registered new client " << p.get() ));
}

prefix_ void senf::console::detail::Server::removeClient(Client & client)
{
    SENF_LOG(( "Disposing client " << & client ));
    // THIS DELETES THE CLIENT INSTANCE !!
    clients_.erase(boost::intrusive_ptr<Client>(&client));
}

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::Client

prefix_ senf::console::detail::Client::Client(ClientHandle handle)
    : handle_ (handle)
{
    ReadHelper<ClientHandle>::dispatch( handle_, 16384u, ReadUntil("\n"),
                                        senf::membind(&Client::clientData, this) );
}

prefix_ senf::console::detail::Client::~Client()
{}

prefix_ void senf::console::detail::Client::stopClient()
{
    // THIS COMMITS SUICIDE. THE INSTANCE IS GONE AFTER removeClient RETURNS
    Server::instance_->removeClient(*this);
}

prefix_ void senf::console::detail::Client::clientData(ReadHelper<ClientHandle>::ptr helper)
{
    if (helper->error() || handle_.eof()) {
        // THIS COMMITS SUICIDE. THE INSTANCE IS GONE AFTER stopClient RETURNS
        stopClient();
        return;
    }

    std::string data (tail_ + helper->data());
    tail_ = helper->tail();
    boost::trim(data); // Gets rid of superfluous  \r or \n characters

    SENF_LOG(( this << ": " << data ));
    ReadHelper<ClientHandle>::dispatch( handle_, 16384u, ReadUntil("\n"),
                                        senf::membind(&Client::clientData, this) );
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Server.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
