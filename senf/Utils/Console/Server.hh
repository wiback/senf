//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief Server public header */

#ifndef HH_SENF_Utils_Console_Server_
#define HH_SENF_Utils_Console_Server_ 1

// Custom includes
#include <set>
#include <boost/scoped_ptr.hpp>
#include <senf/Scheduler/FdEvent.hh>
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Socket/Protocols/INet/INetAddressing.hh>
#include <senf/Utils/Logger.hh>
#include <senf/Utils/intrusive_refcount.hh>
#include "ScopedDirectory.hh"
#include "Executor.hh"

#include "Server.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef detail::ServerHandle ServerHandle;

        enum Mode { Automatic, Interactive, Noninteractive };

        //-////////////////////////////////////////////////////////////////////////

        static Server & start(senf::INet4SocketAddress const & address);
                                        ///< Start server on given IPv4 address/port
        static Server & start(senf::INet6SocketAddress const & address);
                                        ///< Start server on given IPv6 address/port

        std::string const & name() const; ///< Get server name
                                        /**< This information is used in the prompt string. */
        Server & name(std::string const & name); ///< Set server name
                                        /**< This information is used in the prompt string. */

        Server & welcomeMessage(std::string const & message); ///< Set server welcome message
                                        /**< This message is printed on login. */

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
        boost::shared_ptr<DirectoryNode> consoleSelf(std::ostream & os);
        static DirectoryNode & sysConsoleDir();

        ServerHandle handle_;
        scheduler::FdEvent event_;
        DirectoryNode::ptr root_;
        static ScopedDirectory<> sysConsoleDir_;
        Mode mode_;

        typedef std::set< boost::intrusive_ptr<Client> > Clients;
        Clients clients_;
        std::string name_;
        std::string welcomeMsg_;

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
          private boost::base_from_member< detail::SocketStreamOStream >,
          public senf::log::IOStreamTarget
    {
        typedef boost::base_from_member< detail::SocketStreamOStream > out_t;

        SENF_LOG_CLASS_AREA();
        SENF_LOG_DEFAULT_LEVEL( senf::log::NOTICE );

        static const unsigned INTERACTIVE_TIMEOUT = 500; // milliseconds;

    public:
        typedef Server::ServerHandle::ClientHandle ClientHandle;

        ~Client();

        void stop();                    ///< Stop the client
                                        /**< This will close the client socket. */

        std::string const & name() const; ///< Get name of the client instance
                                        /**< This name is used in the prompt string and is set by
                                             the server. */
        ClientHandle handle() const;    ///< Get the client's network socket handle
        std::ostream & stream();        ///< Get client's output stream
                                        /**< Data sent to this stream is sent out over the network
                                             via the client's socket handle. Write operation is
                                             non-blocking and data may be dropped. Data is written
                                             using Client::write(). */
        std::string promptString() const; ///< Get the prompt string
        DirectoryNode & root() const;   ///< Get configured root node
        DirectoryNode & cwd() const;    ///< Get current directory
                                        /**< This is the directory, the console currently is changed
                                             into by the user of the console. */
        Server::Mode mode() const;      ///< Get operation mode
                                        /**< \see Server::mode() */
        void write(std::string const & data) const;
                                        ///< Write data to network socket
                                        /**< The data is automatically filtered depending on the
                                             type of connection (e.g. on a telnet connection,
                                             specific bytes are quoted). */
        std::string const & backtrace() const; ///< Get backtrace of last console error, if any
        unsigned width() const;         ///< Get console width
                                        /**< If possible, this will be the width of the connected
                                             terminal, otherwise a default value (normally 80) is
                                             returned. */

        static Client & get(std::ostream & os);
                                        ///< Access client instance
                                        /**< Allows to access the client instance from console
                                             command implementations. The first argument to a
                                             console command is a stream object. \e If this stream
                                             object belongs to a network console client, this call
                                             will return the associated Client instance reference.
                                             \throws std::bad_cast if \a os is not associated with a
                                                 Client instance. */
        static unsigned getWidth(std::ostream & os, unsigned defaultWidth = 0,
                                 unsigned minWidth = 0);
                                        ///< Get width of client console if possible
                                        /**< If possible, the width of the client console attached
                                             to the stream \a os is returned. If this is not
                                             possible, the \a defaultValue will be used.

                                             If the width obtained this way is smaller than \a
                                             minWidth, \a defaultValue will be returned instead. */

        void setProperty(std::string const & key, std::string const & value);
        std::string getProperty(std::string const & key, std::string const & defaultValue) const;
        boost::optional<std::string> getProperty(std::string const & key) const;

    private:
        Client(Server & server, ClientHandle handle);

        void setInteractive();
        void setNoninteractive();

        size_t handleInput(std::string input, bool incremental = false);
        virtual void v_write(senf::log::time_type timestamp, std::string const & stream,
                             std::string const & area, unsigned level,
                             std::string const & message);

        void dumpProperties(std::ostream & os) const;

        ScopedDirectory<> dir_;
        Server & server_;
        ClientHandle handle_;
        scheduler::FdEvent readevent_;
        scheduler::TimerEvent timer_;
        CommandParser parser_;
        Executor executor_;
        std::string name_;
        boost::scoped_ptr<detail::ClientReader> reader_;
        Server::Mode mode_;
        std::string backtrace_;
        typedef std::map<std::string, std::string> PropertyMap;
        PropertyMap properties_;

        friend class Server;
        friend class detail::ClientReader;
        friend class detail::SocketStreamSink;

        class SysBacktrace
        {
            SysBacktrace();
            static void  backtrace(std::ostream & os);
            static SysBacktrace instance_;
        };

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
