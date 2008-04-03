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
#include "../Utils/intrusive_refcount.hh"
#include "../Socket/Protocols/INet/TCPSocketHandle.hh"
#include "../Socket/ServerSocketHandle.hh"
#include "../Scheduler/Scheduler.hh"
#include "../Scheduler/ReadHelper.hh"
#include "Parse.hh"
#include "Executor.hh"
#include "../Socket/Protocols/INet/INetAddressing.hh"
#include "../Utils/Logger.hh"

//#include "Server.mpp"
#include "Server.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    class Client;

    /** \brief Interactive console server

        This class provides an interactive console TCP server.

        \todo Add readline support
        \todo Add interactivity detection using timeout
        \idea To support blocking commands, we could give the Client 'suspend()' and 'resume()'
            members. suspend() would probably throw some kind of exception to transfer control back
            to the Client instance. on resume(), the command would be called again, maybe setting
            some flag or something. Example for use: Host name resolution: Here we can just built
            our own little host-name cache. When the name is not found, we ask the resolver to
            resolve it and call 'resume' when the name is found. Since it is in the cache now, the
            command will now complete.
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
                                        ///< Start server on given IPv4 address/port
        static Server & start(senf::INet6SocketAddress const & address);
                                        ///< Start server on given IPv6 address/port
        static Server & instance();

        void name(std::string const & name); ///< Set server name
                                        /**< This information is used in the prompt string. */

    protected:

    private:
        Server(ServerHandle handle);

        static void start(ServerHandle handle);
        static boost::scoped_ptr<Server> & instancePtr();

        void newClient(Scheduler::EventId event);
        void removeClient(Client & client);
        
        ServerHandle handle_;
        
        typedef std::set< boost::intrusive_ptr<Client> > Clients;
        Clients clients_;
        std::string name_;
        
        friend class Client;
    };
    
    /** \brief Server client instance

        Whenever a new client connects, a new instance of this class is created. This class shows a
        command prompt, receives the commands, parses them and then passes (using a CommandParser)
        and passes the commands to an Executor instance.

        \fixme Fix Client::clientData implementation
        \fixme Don't register a new ReadHelper every round
     */
    class Client
        : public senf::intrusive_refcount, 
          private boost::base_from_member< detail::NonblockingSocketOStream >,
          public senf::log::IOStreamTarget
    {
        typedef boost::base_from_member< detail::NonblockingSocketOStream > out_t;

        SENF_LOG_CLASS_AREA();
        SENF_LOG_DEFAULT_LEVEL( senf::log::NOTICE );
    public:
        typedef Server::ServerHandle::ClientSocketHandle ClientHandle;

        ~Client();

        void stopClient();              ///< Stop the client
                                        /**< This will close the client socket. */

    protected:
        
    private:
        Client(ClientHandle handle, std::string const & name);

        void clientData(ReadHelper<ClientHandle>::ptr helper);
        void showPrompt();

        virtual void v_write(boost::posix_time::ptime timestamp, std::string const & stream, 
                             std::string const & area, unsigned level, 
                             std::string const & message);
        
        ClientHandle handle_;
        std::string tail_;
        CommandParser parser_;
        Executor executor_;
        std::string name_;
        unsigned promptLen_;
        std::string lastCommand_;

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