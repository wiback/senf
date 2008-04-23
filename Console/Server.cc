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

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::NonBlockingSocketSink

prefix_ std::streamsize senf::console::detail::NonblockingSocketSink::write(const char * s,
                                                                            std::streamsize n)
{
    try {
        if (handle_.writeable()) 
            handle_.write(s, s+n);
    }
    catch (SystemException & ex) {
        ;
    }
    return n;
}

///////////////////////////////////////////////////////////////////////////
// senf::console::Server

prefix_ senf::console::Server &
senf::console::Server::start(senf::INet4SocketAddress const & address)
{
    senf::TCPv4ServerSocketHandle handle (address);
    Server & server (senf::console::Server::start(handle));
    SENF_LOG((Server::SENFLogArea)(log::NOTICE)( 
                 "Console server started at " << address ));
    return server;
}

prefix_ senf::console::Server &
senf::console::Server::start(senf::INet6SocketAddress const & address)
{
    senf::TCPv6ServerSocketHandle handle (address);
    Server & server (senf::console::Server::start(handle));
    SENF_LOG((Server::SENFLogArea)(log::NOTICE)( 
                 "Console server started at " << address ));
    return server;
}

prefix_ boost::scoped_ptr<senf::console::Server> & senf::console::Server::instancePtr()
{
    // We cannot make 'instance' a global or class-static variable, since it will then be destructed
    // at an unknown time which may fail if the scheduler or the file-handle pool allocators have
    // already been destructed.
    static boost::scoped_ptr<senf::console::Server> instance;
    return instance;
}

prefix_ senf::console::Server & senf::console::Server::start(ServerHandle handle)
{
    // Uah .... ensure the scheduler is created before the instance pointer so it get's destructed
    // AFTER it.
    (void) senf::Scheduler::instance();
    SENF_ASSERT( ! instancePtr() );
    instancePtr().reset(new Server(handle));
    return * instancePtr();
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
    boost::intrusive_ptr<Client> p (new Client(*this, client, name_));
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

prefix_ senf::console::Client::Client(Server & server, ClientHandle handle,
                                      std::string const & name)
    : out_t(handle), senf::log::IOStreamTarget(out_t::member), server_ (server),
      handle_ (handle), name_ (name), promptLen_(0) 
{
    showPrompt();
    ReadHelper<ClientHandle>::dispatch( handle_, 16384u, ReadUntil("\n"),
                                        senf::membind(&Client::clientData, this) );
    route< senf::SenfLog, senf::log::NOTICE >();
}

prefix_ senf::console::Client::~Client()
{}

prefix_ void senf::console::Client::stopClient()
{
    // THIS COMMITS SUICIDE. THE INSTANCE IS GONE AFTER removeClient RETURNS
    server_.removeClient(*this);
}

prefix_ void senf::console::Client::clientData(ReadHelper<ClientHandle>::ptr helper)
{
    promptLen_ = 0;
    if (helper->error() || handle_.eof()) {
        // THIS COMMITS SUICIDE. THE INSTANCE IS GONE AFTER stopClient RETURNS
        stopClient();
        return;
    }

    std::string data (tail_ + helper->data());
    tail_ = helper->tail();
    boost::trim(data); // Gets rid of superfluous  \r or \n characters

    if (data.empty())
        data = lastCommand_;
    else
        lastCommand_ = data;

    try {
        if (! parser_.parse(data, boost::bind<void>( boost::ref(executor_),
                                                     boost::ref(out_t::member),
                                                     _1 )) )
            out_t::member << "syntax error" << std::endl;
    }
    catch (Executor::ExitException &) {
        // THIS COMMITS SUICIDE. THE INSTANCE IS GONE AFTER stopClient RETURNS
        stopClient();
        return;
    }
    catch (std::exception & ex) {
        out_t::member << ex.what() << std::endl;
    }
    catch (...) {
        out_t::member << "unidentified error (unknown exception thrown)" << std::endl;
    }

    showPrompt();
    ReadHelper<ClientHandle>::dispatch( handle_, 16384u, ReadUntil("\n"),
                                        senf::membind(&Client::clientData, this) );
}

prefix_ void senf::console::Client::showPrompt()
{
    std::string path (executor_.cwd().path());
    out_t::member << name_ << ":" << path << "# " << std::flush;
    promptLen_ = name_.size() + 1 + path.size() + 1;
}

prefix_ void senf::console::Client::v_write(boost::posix_time::ptime timestamp,
                                            std::string const & stream,
                                            std::string const & area, unsigned level,
                                            std::string const & message)
{
    if (promptLen_)
        out_t::member << '\r' << std::string(' ', promptLen_) << '\r';
    IOStreamTarget::v_write(timestamp, stream, area, level, message);
    if (promptLen_)
        showPrompt();
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
