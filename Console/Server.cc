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
#include <iostream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/bind.hpp>
#include "../Utils/senfassert.hh"
#include "../Utils/membind.hh"
#include "../Utils/Logger/SenfLog.hh"
#include "Readline.hh"

//#include "Server.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::NonBlockingSocketSink

prefix_ std::streamsize senf::console::detail::NonblockingSocketSink::write(const char * s,
                                                                            std::streamsize n)
{
    try {
        if (client_.handle().writeable()) {
            std::string data (s, n);
            client_.translate(data);
            client_.handle().write( data );
        }
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
// senf::console::detail::DumbClientReader

prefix_ senf::console::detail::DumbClientReader::DumbClientReader(Client & client)
    : ClientReader(client), promptLen_ (0), promptActive_ (false)
{
    showPrompt();
    ReadHelper<ClientHandle>::dispatch( handle(), 16384u, ReadUntil("\n"),
                                        senf::membind(&DumbClientReader::clientData, this) );
}

prefix_ void
senf::console::detail::DumbClientReader::clientData(senf::ReadHelper<ClientHandle>::ptr helper)
{
    if (helper->error() || handle().eof()) {
        // THIS COMMITS SUICIDE. THE INSTANCE IS GONE AFTER stopClient RETURNS
        stopClient();
        return;
    }
    
    promptLen_ = 0;
    promptActive_ = false;

    std::string data (tail_ + helper->data());
    tail_ = helper->tail();
    boost::trim(data);                  // Gets rid of superfluous  \r or \n characters
    handleInput(data);

    showPrompt();
    ReadHelper<ClientHandle>::dispatch( handle(), 16384u, ReadUntil("\n"),
                                        senf::membind(&DumbClientReader::clientData, this) );

}

prefix_ void senf::console::detail::DumbClientReader::showPrompt()
{
    std::string prompt (promptString());

    stream() << std::flush;
    handle().write(prompt);
    promptLen_ = prompt.size();
    promptActive_ = true;
}

prefix_ void senf::console::detail::DumbClientReader::v_disablePrompt()
{
    if (promptActive_ && promptLen_ > 0) {
        stream() << '\r' << std::string(' ', promptLen_) << '\r';
        promptLen_ = 0;
    }
}

prefix_ void senf::console::detail::DumbClientReader::v_enablePrompt()
{
    if (promptActive_ && ! promptLen_)
        showPrompt();
}

prefix_ void senf::console::detail::DumbClientReader::v_translate(std::string & data)
{}

///////////////////////////////////////////////////////////////////////////
// senf::console::Client

prefix_ senf::console::Client::Client(Server & server, ClientHandle handle,
                                      std::string const & name)
    : out_t(boost::ref(*this)), senf::log::IOStreamTarget(out_t::member), server_ (server),
      handle_ (handle), name_ (name), reader_ (new detail::SafeReadlineClientReader (*this))
{
    executor_.autocd(true).autocomplete(true);
    handle_.facet<senf::TCPSocketProtocol>().nodelay();
    // route< senf::SenfLog, senf::log::NOTICE >();
}

prefix_ void senf::console::Client::translate(std::string & data)
{
    reader_->translate(data);
}

prefix_ void senf::console::Client::handleInput(std::string data)
{
    if (data.empty())
        data = lastCommand_;
    else
        lastCommand_ = data;

    try {
        if (! parser_.parse(data, boost::bind<void>( boost::ref(executor_),
                                                     boost::ref(stream()),
                                                     _1 )) )
            stream() << "syntax error" << std::endl;
    }
    catch (Executor::ExitException &) {
        // This generates an EOF condition on the Handle. This EOF condition is expected
        // to be handled gracefully by the ClientReader. We cannot call stop() here, since we
        // are called from the client reader callback and that will continue executing even if we
        // call stop here ...
        handle_.facet<senf::TCPSocketProtocol>().shutdown(senf::TCPSocketProtocol::ShutRD);
        return;
    }
    catch (std::exception & ex) {
        stream() << ex.what() << std::endl;
    }
    catch (...) {
        stream() << "unidentified error (unknown exception thrown)" << std::endl;
    }
}

prefix_ void senf::console::Client::v_write(boost::posix_time::ptime timestamp,
                                            std::string const & stream,
                                            std::string const & area, unsigned level,
                                            std::string const & message)
{
    reader_->disablePrompt();
    IOStreamTarget::v_write(timestamp, stream, area, level, message);
    out_t::member << std::flush;
    reader_->enablePrompt();
}

prefix_ std::ostream & senf::console::operator<<(std::ostream & os, Client const & client)
{
    typedef ClientSocketHandle< MakeSocketPolicy<
        INet4AddressingPolicy,ConnectedCommunicationPolicy>::policy > V4Socket;
    typedef ClientSocketHandle< MakeSocketPolicy<
        INet6AddressingPolicy,ConnectedCommunicationPolicy>::policy > V6Socket;

    if (check_socket_cast<V4Socket>(client.handle()))
        os << dynamic_socket_cast<V4Socket>(client.handle()).peer();
    else if (check_socket_cast<V6Socket>(client.handle()))
        os << dynamic_socket_cast<V6Socket>(client.handle()).peer();
    else
        os << static_cast<void const *>(&client);

    return os;
}

prefix_ std::ostream & senf::console::operator<<(std::ostream & os, Client * client)
{
    return os << *client;
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
