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
    \brief Server non-inline non-template implementation */

#include "Server.hh"
//#include "Server.ih"

// Custom includes
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/Logger/SenfLog.hh>
#include <senf/Utils/CpuStat.hh>
#include <senf/Version.hh>
#include "LineEditor.hh"
#include "ScopedDirectory.hh"
#include "Sysdir.hh"
#include "SysInfo.hh"
#include "ParsedCommand.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SENF_DEBUG
#   define BUILD_TYPE "development"
#else
#   define BUILD_TYPE "production"
#endif

namespace {
    senf::console::SysInfo::Proxy addSysInfo ("SENF",
            "SENF: The Simple and Extensible Network Framework\n"
            "  (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)\n"
            "  Contact: http://wiback.org\n"
            "  Version: " SENF_LIB_VERSION " Revision number: " SENF_REVISION "\n"
            "  Build-type: " BUILD_TYPE ", SenfLog compile time limit: " +
            senf::str(senf::log::LEVELNAMES[senf::SenfLog::compileLimit::value]), 0);

    senf::CpuStatConsole console;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::detail::SocketStreamSink

prefix_ std::streamsize senf::console::detail::SocketStreamSink::write(const char * s,
                                                                            std::streamsize n)
{
// since handle is now non blocking we done check for writeable
    try {
//        if (client_.handle().writeable()) {
            std::string data (s, n);
            client_.write(data);
//        }
    }
    catch (...) {}
    return n;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::Server

senf::console::ScopedDirectory<> senf::console::Server::sysConsoleDir_;

prefix_ senf::console::DirectoryNode & senf::console::Server::sysConsoleDir()
{
    if (not sysdir().hasChild("console"))
        sysdir().add("console", sysConsoleDir_);
    return sysConsoleDir_.node();
}

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

prefix_ senf::console::Server & senf::console::Server::start(ServerHandle handle)
{
    boost::intrusive_ptr<Server> p (new Server(handle));
    detail::ServerManager::add(boost::intrusive_ptr<Server>(p));
    return *p;
}

prefix_ senf::console::Server::Server(ServerHandle handle)
    : handle_ (handle),
      event_ ("senf::console::Server", senf::membind(&Server::newClient, this),
              handle_, scheduler::FdEvent::EV_READ),
      root_ (senf::console::root().thisptr()), mode_ (Automatic),
      name_ (::program_invocation_short_name)
{
    namespace fty = console::factory;
    sysConsoleDir().add("self", fty::Command(&Server::consoleSelf, this)
             .doc("Get the directory of the current network client. Example usage:\n"
                  "\n"
                  "Just get the directory\n"
                  "    $ /sys/console/self\n"
                  "    <Directory '/sys/console/client-xxx.xxx.xxx.xxx:xxx'>\n"
                  "\n"
                  "Get all properties of the currently connected client\n"
                  "    $ /sys/console/self { properties; }") );
}

prefix_ void senf::console::Server::newClient(int event)
{
    ServerHandle::ClientHandle client (handle_.accept());
    boost::intrusive_ptr<Client> p (new Client(*this, client));
    clients_.insert( p );
    SENF_LOG(( "Registered new client " << client.peer() ));
}

prefix_ void senf::console::Server::removeClient(Client & client)
{
    SENF_LOG_BLOCK(({
                log << "Disposing client ";
                try {
                    log << client.handle().peer();
                }
                catch (senf::SystemException & ex) {
                    log << "(dead socket)";
                }
            }));
    // THIS DELETES THE CLIENT INSTANCE !!
    clients_.erase(boost::intrusive_ptr<Client>(&client));
}

prefix_ senf::console::Server & senf::console::Server::welcomeMessage(std::string const & message)
{
    welcomeMsg_ = message;
    if (!message.empty() && message[message.size()-1] != '\n')
        welcomeMsg_ += '\n';
    return *this;
}

prefix_ boost::shared_ptr<senf::console::DirectoryNode> senf::console::Server::consoleSelf(std::ostream & os)
{
    return Client::get(os).dir_.node().thisptr();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::detail::DumbClientReader

prefix_ senf::console::detail::DumbClientReader::DumbClientReader(Client & client)
    : ClientReader(client), promptLen_ (0), promptActive_ (false)
{
    showPrompt();
    ReadHelper<ClientHandle>::dispatch(
            handle(), 16384u, ReadUntil("\n"), senf::membind(&DumbClientReader::clientData, this) );
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
    boost::trim(data); // Gets rid of superfluous \r or \n characters
    handleInput(data);

    showPrompt();
    ReadHelper<ClientHandle>::dispatch(
            handle(), 16384u, ReadUntil("\n"), senf::membind(&DumbClientReader::clientData, this) );
}

prefix_ void senf::console::detail::DumbClientReader::showPrompt()
{
    std::string prompt (promptString());
    prompt += " ";

    stream() << std::flush;
    promptLen_ = prompt.size();
    promptActive_ = true;
    v_write(prompt);
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

prefix_ void senf::console::detail::DumbClientReader::v_write(std::string const & data)
{
    try {
        handle().write(data);
    }
    catch (senf::ExceptionMixin & ex) {
        SENF_LOG(("unexpected failure writing to socket:" << ex.message()));
        try { handle().facet<senf::TCPSocketProtocol>().shutdown(senf::TCPSocketProtocol::ShutRD); }
        catch (...) {}
    }
    catch (std::exception & ex) {
        SENF_LOG(("unexpected failure writing to socket:" << ex.what()));
        try { handle().facet<senf::TCPSocketProtocol>().shutdown(senf::TCPSocketProtocol::ShutRD); }
        catch (...) {}
    }
    catch (...) {
        SENF_LOG(("unexpected failure writing to socket: unknown exception"));
        try { handle().facet<senf::TCPSocketProtocol>().shutdown(senf::TCPSocketProtocol::ShutRD); }
        catch (...) {}
    }
}

prefix_ unsigned senf::console::detail::DumbClientReader::v_width()
    const
{
    return 80;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::detail::NoninteractiveClientReader

prefix_
senf::console::detail::NoninteractiveClientReader::NoninteractiveClientReader(Client & client)
    : ClientReader (client), streamBufferMaxSize_( 1024*1024),
      readevent_ ("senf::console::detail::NoninteractiveClientReader",
                  senf::membind(&NoninteractiveClientReader::newData, this),
                  handle(), senf::scheduler::FdEvent::EV_READ),
      writeevent_ ("senf::console::detail::NoninteractiveClientReader",
                  membind(&NoninteractiveClientReader::writeHandler, this), handle(),
                  scheduler::FdEvent::EV_WRITE, false)
{}

prefix_ void senf::console::detail::NoninteractiveClientReader::v_disablePrompt()
{}

prefix_ void senf::console::detail::NoninteractiveClientReader::v_enablePrompt()
{}

prefix_ void senf::console::detail::NoninteractiveClientReader::streamBufferMaxSize(SendQueue::size_type size)
{
    streamBufferMaxSize_ = size;
}

prefix_ senf::console::detail::NoninteractiveClientReader::SendQueue::size_type
senf::console::detail::NoninteractiveClientReader::streamBufferMaxSize()
    const
{
    return streamBufferMaxSize_;
}

prefix_ void senf::console::detail::NoninteractiveClientReader::v_write(std::string const & data)
{
    if (sendQueue_.size() > streamBufferMaxSize_)
        return;
    sendQueue_.insert( sendQueue_.end(), data.begin(), data.end());
    writeHandler(scheduler::FdEvent::EV_WRITE);
    if (! sendQueue_.empty())
        writeevent_.enable();
}

prefix_ unsigned senf::console::detail::NoninteractiveClientReader::v_width()
    const
{
    return 80;
}

prefix_ void
senf::console::detail::NoninteractiveClientReader::newData(int event)
{
    if (event != senf::scheduler::FdEvent::EV_READ || handle().eof()) {
        if (! buffer_.empty())
            handleInput(buffer_);
        stopClient();
        return;
    }

    std::string::size_type n (buffer_.size());
    buffer_.resize(n + handle().available());
    buffer_.erase(handle().read(boost::make_iterator_range(buffer_.begin()+n, buffer_.end())),
                  buffer_.end());
    buffer_.erase(0, handleInput(buffer_, true));
    stream() << std::flush;
}

prefix_ void
senf::console::detail::NoninteractiveClientReader::writeHandler(int event)
{
    if (event != senf::scheduler::FdEvent::EV_WRITE) {
        writeevent_.disable();
        readevent_.disable();
        try { handle().facet<senf::TCPSocketProtocol>().shutdown(senf::TCPSocketProtocol::ShutRD); }
        catch (...) {}
        return;
    }
    try {
        sendQueue_.erase(sendQueue_.begin(),
                handle().write(boost::make_iterator_range(sendQueue_.begin(), sendQueue_.end())));
    }
    catch (senf::ExceptionMixin & ex) {
        SENF_LOG(("unexpected failure writing to socket:" << ex.message()));
        try { handle().facet<senf::TCPSocketProtocol>().shutdown(senf::TCPSocketProtocol::ShutRD); }
        catch (...) {}
    }
    catch (std::exception & ex) {
        SENF_LOG(("unexpected failure writing to socket:" << ex.what()));
        try { handle().facet<senf::TCPSocketProtocol>().shutdown(senf::TCPSocketProtocol::ShutRD); }
        catch (...) {}
    }
    catch (...) {
        SENF_LOG(("unexpected failure writing to socket: unknown exception"));
        try { handle().facet<senf::TCPSocketProtocol>().shutdown(senf::TCPSocketProtocol::ShutRD); }
        catch (...) {}
    }
    if (sendQueue_.empty())
        writeevent_.disable();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::Client

prefix_ senf::console::Client::Client(Server & server, ClientHandle handle)
    : out_t(boost::ref(*this)),
      senf::log::IOStreamTarget("client-" + senf::str(handle.peer()), out_t::member),
      server_ (server), handle_ (handle),
      readevent_ ("senf::console::Client::interactive_check",
                  boost::bind(&Client::setNoninteractive,this),
                  handle, scheduler::FdEvent::EV_READ, false),
      timer_ ("senf::console::Client::interactive_timer",
              boost::bind(&Client::setInteractive, this),
              scheduler::eventTime() + ClockService::milliseconds(INTERACTIVE_TIMEOUT),
              false),
      name_ (server.name()), reader_ (), mode_ (server.mode())
{
    handle_.facet<senf::TCPSocketProtocol>().nodelay(true);
    handle_.blocking(false);
    executor_.chroot(root());
    switch (mode_) {
    case Server::Interactive :
        setInteractive();
        break;
    case Server::Noninteractive :
        setNoninteractive();
        break;
    case Server::Automatic :
        readevent_.enable();
        timer_.enable();
        break;
    }

    namespace kw = console::kw;
    namespace fty = console::factory;
    Server::sysConsoleDir().add( "client-" + senf::str(handle.peer()), dir_);
    dir_.add("log", fty::Link(consoleDir()) );
    dir_.add("property", fty::Command<std::string (std::string const &)>(
            boost::bind(SENF_MEMFNP(std::string, Client, getProperty, (std::string const &, std::string const &) const), this, _1, ""))
            .doc("Get a property for this client")
            .arg("key", "property key") );
    dir_.add("property", fty::Command(&Client::setProperty, this) );
    dir_.add("properties", fty::Command(&Client::dumpProperties, this) );
    dir_.add("backtrace", fty::Command(&Client::backtrace, this)
            .doc("Display the backtrace of the last exception") );
}

prefix_ void senf::console::Client::setInteractive()
{
    readevent_.disable();
    timer_.disable();
    mode_ = Server::Interactive;
    properties_["mode"] = "Interactive";
    reader_.reset(new detail::LineEditorSwitcher(*this));
    executor_.autocd(true).autocomplete(true);
    if (! server_.welcomeMsg_.empty())
        write( server_.welcomeMsg_);
}

prefix_ void senf::console::Client::setNoninteractive()
{
    readevent_.disable();
    timer_.disable();
    mode_ = Server::Noninteractive;
    properties_["mode"] = "NonInteractive";
    detail::NoninteractiveClientReader * newReader (new detail::NoninteractiveClientReader(*this));
    reader_.reset( newReader);
    consoleDir().add( "streamBuffer", factory::Command( senf::membind(
            SENF_MEMFNP( detail::NoninteractiveClientReader::SendQueue::size_type, detail::NoninteractiveClientReader, streamBufferMaxSize, () const ),
            newReader )));
    consoleDir().add( "streamBuffer", factory::Command( senf::membind(
            SENF_MEMFNP( void, detail::NoninteractiveClientReader, streamBufferMaxSize, (detail::NoninteractiveClientReader::SendQueue::size_type) ),
            newReader )));
}

prefix_ std::string::size_type senf::console::Client::handleInput(std::string data,
                                                                  bool incremental)
{
    std::string::size_type n (data.size());

    try {
        if (incremental)
            n = parser_.parseIncremental(data, boost::bind<void>(
                    boost::ref(executor_), boost::ref(stream()), _1 ));
        else
            parser_.parse(data, boost::bind<void>(
                    boost::ref(executor_), boost::ref(stream()), _1 ));
    }
    catch (Executor::ExitException &) {
        // This generates an EOF condition on the Handle. This EOF condition is expected to be
        // handled gracefully by the ClientReader. We cannot call stop() here, since we are called
        // from the client reader callback and that will continue executing after stop() has been
        // called. stop() however will delete *this instance ... BANG ...
        try { handle_.facet<senf::TCPSocketProtocol>().shutdown(senf::TCPSocketProtocol::ShutRD); }
        catch (...) {}
    }
    catch (std::exception & ex) {
        std::string msg (ex.what());
        std::string::size_type i (msg.find("-- \n"));
        if (i != std::string::npos) {
            backtrace_ = msg.substr(0,i);
            msg = msg.substr(i+4);
        } else {
            backtrace_.clear();
        }

        stream() << msg << std::endl;
    }
    catch (...) {
        stream() << "unidentified error (unknown exception thrown)" << std::endl;
    }
    return n;
}

prefix_ void senf::console::Client::v_write(senf::log::time_type timestamp,
                                            std::string const & stream,
                                            std::string const & area, unsigned level,
                                            std::string const & message)
{
    reader_->disablePrompt();
    IOStreamTarget::v_write(timestamp, stream, area, level, message);
    out_t::member << std::flush;
    reader_->enablePrompt();
}

prefix_ unsigned senf::console::Client::getWidth(std::ostream & os, unsigned defaultWidth,
                                                 unsigned minWidth)
{
    unsigned rv (defaultWidth);
    try {
        rv = get(os).width();
    }
    catch (std::bad_cast &) {}
    return rv < minWidth ? defaultWidth : rv;
}

prefix_ void senf::console::Client::setProperty(std::string const & key, std::string const & value)
{
    properties_[key] = value;
}

prefix_ boost::optional<std::string> senf::console::Client::getProperty(std::string const & key)
    const
{
    PropertyMap::const_iterator entry (properties_.find(key));
    return entry != properties_.end() ? boost::make_optional(entry->second) : boost::none;
}

prefix_ std::string senf::console::Client::getProperty(std::string const & key, std::string const & defaultValue)
    const
{
    PropertyMap::const_iterator entry (properties_.find(key));
    return entry != properties_.end() ? entry->second : defaultValue;
}

prefix_ void senf::console::Client::dumpProperties(std::ostream & os)
    const
{
    for (PropertyMap::const_iterator entry (properties_.begin()); entry != properties_.end(); ++entry)
        os << entry->first << " : " << entry->second << std::endl;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::Client::SysBacktrace

prefix_ senf::console::Client::SysBacktrace::SysBacktrace()
{
    Server::sysConsoleDir().add("backtrace", factory::Command(&SysBacktrace::backtrace)
        .doc("Display the backtrace of the last error / exception in this console") );
}

prefix_ void senf::console::Client::SysBacktrace::backtrace(std::ostream & os)
{
    Client & client (Client::get(os));
    if (client.backtrace().empty())
        os << "(no backtrace)\n";
    else
        os << client.backtrace();
}

senf::console::Client::SysBacktrace senf::console::Client::SysBacktrace::instance_;

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
