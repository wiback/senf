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
//#include "Server.ih"

// Custom includes
#include <unistd.h>
#include <iostream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/bind.hpp>
#include "../Utils/senfassert.hh"
#include "../Utils/membind.hh"
#include "../Utils/Logger/SenfLog.hh"

//#include "Server.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::console::Server &
senf::console::Server::start(senf::INet4SocketAddress const & address)
{
    senf::TCPv4ServerSocketHandle handle (address);
    senf::console::Server::start(handle);
    SENF_LOG((Server::SENFLogArea)(log::NOTICE)( 
                 "Console server started at " << address ));
    return *instance_;
}

prefix_ senf::console::Server &
senf::console::Server::start(senf::INet6SocketAddress const & address)
{
    senf::TCPv6ServerSocketHandle handle (address);
    senf::console::Server::start(handle);
    SENF_LOG((Server::SENFLogArea)(log::NOTICE)( 
                 "Console server started at " << address ));
    return *instance_;
}

///////////////////////////////////////////////////////////////////////////
// senf::console::Server

boost::scoped_ptr<senf::console::Server> senf::console::Server::instance_;

prefix_ void senf::console::Server::start(ServerHandle handle)
{
    SENF_ASSERT( ! instance_ );
    instance_.reset(new Server(handle));
}

prefix_ senf::console::Server::Server(ServerHandle handle)
    : handle_ (handle)
{
    Scheduler::instance().add( handle_, senf::membind(&Server::newClient, this) );
}

prefix_ senf::console::Server::~Server()
{
    Scheduler::instance().remove(handle_);
}

prefix_ void senf::console::Server::newClient(Scheduler::EventId event)
{
    ServerHandle::ClientSocketHandle client (handle_.accept());
    boost::intrusive_ptr<Client> p (new Client(client, name_));
    clients_.insert( p );
    SENF_LOG(( "Registered new client " << p.get() ));
}

prefix_ void senf::console::Server::removeClient(Client & client)
{
    SENF_LOG(( "Disposing client " << & client ));
    // THIS DELETES THE CLIENT INSTANCE !!
    clients_.erase(boost::intrusive_ptr<Client>(&client));
}

///////////////////////////////////////////////////////////////////////////
// senf::console::Client

prefix_ senf::console::Client::Client(ClientHandle handle, std::string const & name)
    : handle_ (handle), name_ (name), out_(::dup(handle.fd()))
{
    showPrompt();
    ReadHelper<ClientHandle>::dispatch( handle_, 16384u, ReadUntil("\n"),
                                        senf::membind(&Client::clientData, this) );
}

prefix_ senf::console::Client::~Client()
{}

prefix_ void senf::console::Client::stopClient()
{
    // THIS COMMITS SUICIDE. THE INSTANCE IS GONE AFTER removeClient RETURNS
    Server::instance_->removeClient(*this);
}

prefix_ void senf::console::Client::clientData(ReadHelper<ClientHandle>::ptr helper)
{
    if (helper->error() || handle_.eof()) {
        // THIS COMMITS SUICIDE. THE INSTANCE IS GONE AFTER stopClient RETURNS
        stopClient();
        return;
    }

    ///\fixme Fix Client::clientData implementation
    /// Remove the 'dup' needed here so we don't close the same fd twice (see Client constructor)
    /// Make output non-blocking
    /// Don't register a new ReadHelper every round

    std::string data (tail_ + helper->data());
    tail_ = helper->tail();
    boost::trim(data); // Gets rid of superfluous  \r or \n characters

    try {
        if (! parser_.parse(data, boost::bind<void>(boost::ref(executor_), _1, boost::ref(out_))))
            out_ << "syntax error" << std::endl;
    }
    catch (Executor::ExitException &) {
        // THIS COMMITS SUICIDE. THE INSTANCE IS GONE AFTER stopClient RETURNS
        stopClient();
        return;
    }        

    showPrompt();
    ReadHelper<ClientHandle>::dispatch( handle_, 16384u, ReadUntil("\n"),
                                        senf::membind(&Client::clientData, this) );
}

prefix_ void senf::console::Client::showPrompt()
{
    out_ << name_ << ":" << executor_.cwd().path() << "# " << std::flush;
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
