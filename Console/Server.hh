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
#include "../Scheduler/Binding.hh"
#include "../Scheduler/Timer.hh"
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

        \idea To support blocking commands, we could give the Client 'suspend()' and 'resume()'
            members. suspend() would probably throw some kind of exception to transfer control back
            to the Client instance. on resume(), the command would be called again, maybe setting
            some flag or something. Example for use: Host name resolution: Here we can just built
            our own little host-name cache. When the name is not found, we ask the resolver to
            resolve it and call 'resume' when the name is found. Since it is in the cache now, the
            command will now complete.

        \ingroup console_access
      */
    class Server
        : public senf::intrusive_refcount
    {
        SENF_LOG_CLASS_AREA();
        SENF_LOG_DEFAULT_LEVEL( senf::log::NOTICE );
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        typedef detail::ServerHandle ServerHandle;

        enum Mode { Automatic, Interactive, Noninteractive };

        ///////////////////////////////////////////////////////////////////////////

        ~Server();

        static Server & start(senf::INet4SocketAddress const & address);
                                        ///< Start server on given IPv4 address/port
        static Server & start(senf::INet6SocketAddress const & address);
                                        ///< Start server on given IPv6 address/port

        std::string const & name() const; ///< Get server name
                                        /**< This information is used in the prompt string. */

        Server & name(std::string const & name); ///< Set server name
                                        /**< This information is used in the prompt string. */

        DirectoryNode & root() const;   ///< Get root node

        Server & root(DirectoryNode & root); ///< Set root node
                                        /**< \a node will be the root node for all clients launched
                                             from this server. */

        Mode mode() const;              ///< Get mode
                                        /**< \see \ref mode(Mode) */
        
        Server & mode(Mode mode);       ///< Set mode
                                        /**< There are two Server types: 
                                             \li An interactive server displays a command prompt and
                                                 optionally supports command-line editing.
                                             \li A non-interactive server does not display any
                                                 prompt and does not allow any interactive
                                                 editing. This type of server is used for (remote)
                                                 scripting.

                                             The \a mode parameter selects between these modes. In
                                             \c Automatic (the default), a client connection is
                                             considered to be interactive if there is no data
                                             traffic in the first 500ms after the connection is
                                             opened. */

        void stop();                    ///< Stop the server
                                        /**< All clients will be closed 
                                             \warning The Server instance itself will be deleted */

    protected:

    private:
        Server(ServerHandle handle);

        static Server & start(ServerHandle handle);

        void newClient(int event);
        void removeClient(Client & client);
        
        ServerHandle handle_;
        DirectoryNode::ptr root_;
        Mode mode_;
        
        typedef std::set< boost::intrusive_ptr<Client> > Clients;
        Clients clients_;
        std::string name_;
        
        friend class Client;
    };
    
    /** \brief Server client instance

        Whenever a new client connects, a new instance of this class is created. This class shows a
        command prompt, receives the commands, parses them and then passes (using a CommandParser)
        and passes the commands to an Executor instance.

        \ingroup console_access
     */
    class Client
        : public senf::intrusive_refcount, 
          private boost::base_from_member< detail::NonblockingSocketOStream >,
          public senf::log::IOStreamTarget
    {
        typedef boost::base_from_member< detail::NonblockingSocketOStream > out_t;

        SENF_LOG_CLASS_AREA();
        SENF_LOG_DEFAULT_LEVEL( senf::log::NOTICE );

        static const unsigned INTERACTIVE_TIMEOUT = 500; // milliseconds;

    public:
        typedef Server::ServerHandle::ClientSocketHandle ClientHandle;

        ~Client();

        void stop();                    ///< Stop the client
                                        /**< This will close the client socket. */

        std::string const & name() const;
        ClientHandle handle() const;
        std::ostream & stream();
        std::string promptString() const;
        DirectoryNode & root() const;
        Server::Mode mode() const;

        static Client & get(std::ostream & os);

    protected:
        
    private:
        Client(Server & server, ClientHandle handle);

        void setInteractive();
        void setNoninteractive();
        
        void translate(std::string & data);
        unsigned handleInput(std::string input, bool incremental = false);
        virtual void v_write(senf::log::time_type timestamp, std::string const & stream, 
                             std::string const & area, unsigned level, 
                             std::string const & message);
        
        Server & server_;
        ClientHandle handle_;
        SchedulerBinding binding_;
        SchedulerTimer timer_;
        CommandParser parser_;
        Executor executor_;
        std::string name_;
        std::string lastCommand_;
        boost::scoped_ptr<detail::ClientReader> reader_;
        Server::Mode mode_;

        friend class Server;
        friend class detail::ClientReader;
        friend class detail::NonblockingSocketSink;
    };
        
    /** \brief Output Console Client instance as it's string representation
        \related Client
     */
    std::ostream & operator<<(std::ostream & os, Client const & client);

    /** \brief Output Console Client instance as it's string representation
        \related Client
     */
    std::ostream & operator<<(std::ostream & os, Client * client);

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
