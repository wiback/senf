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
    \brief UDPServer non-inline non-template implementation */

#include "UDPServer.hh"
//#include "UDPServer.ih"

// Custom includes
#include <boost/algorithm/string/trim.hpp>
#include <senf/Utils/membind.hh>

//#include "UDPServer.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
    SENF_ASSERT( handle_.local().family() == senf::INet4SocketAddress::addressFamily,
                 "Internal failure: INet6 address on INet4 socket ??" );
    target_ = address;
    return *this;
}

prefix_ senf::console::UDPServer &
senf::console::UDPServer::replies(senf::INet6SocketAddress const & address)
{
    SENF_ASSERT( handle_.local().family() == senf::INet6SocketAddress::addressFamily,
                 "Internal failure: INet4 address on INet6 socket ??" );
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
