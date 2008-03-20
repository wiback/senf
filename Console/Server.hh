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
    \brief Server public header */

#ifndef HH_Server_
#define HH_Server_ 1

// Custom includes
#include <set>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include "../Utils/intrusive_refcount.hh"
#include "../Socket/Protocols/INet/TCPSocketHandle.hh"
#include "../Socket/ServerSocketHandle.hh"
#include "../Scheduler/Scheduler.hh"
#include "../Scheduler/ReadHelper.hh"
#include "Parse.hh"
#include "Executor.hh"
#include "../Socket/Protocols/INet/INetAddressing.hh"

//#include "Server.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    class Client;

    /** \brief
      */
    class Server
        : boost::noncopyable
    {
        SENF_LOG_CLASS_AREA();
        SENF_LOG_DEFAULT_LEVEL( senf::log::NOTICE );
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef senf::ServerSocketHandle<
            senf::MakeSocketPolicy< senf::TCPv4SocketProtocol::Policy, 
                                    senf::UnspecifiedAddressingPolicy>::policy > ServerHandle;

        ~Server();

        static Server & start(senf::INet4SocketAddress const & address);
        static Server & start(senf::INet6SocketAddress const & address);

        void name(std::string const & name);

    protected:

    private:
        Server(ServerHandle handle);

        static void start(ServerHandle handle);

        void newClient(Scheduler::EventId event);
        void removeClient(Client & client);
        
        ServerHandle handle_;
        
        typedef std::set< boost::intrusive_ptr<Client> > Clients;
        Clients clients_;
        std::string name_;
        
        static boost::scoped_ptr<Server> instance_;
        
        friend class Client;
    };
    
    /** \brief
     */
    class Client
        : public senf::intrusive_refcount
    {
        SENF_LOG_CLASS_AREA();
        SENF_LOG_DEFAULT_LEVEL( senf::log::NOTICE );
    public:
        typedef Server::ServerHandle::ClientSocketHandle ClientHandle;

        ~Client();

        void stopClient();

    protected:
        
    private:
        Client(ClientHandle handle, std::string const & name);

        void clientData(ReadHelper<ClientHandle>::ptr helper);
        
        ClientHandle handle_;
        std::string tail_;
        SingleCommandParser parser_;
        Executor executor_;
        std::string name_;

        typedef boost::iostreams::stream<boost::iostreams::file_descriptor_sink> fdostream;
        fdostream out_;

        friend class Server;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Server.cci"
//#include "Server.ct"
//#include "Server.cti"
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
