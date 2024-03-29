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
    \brief MultiMCLoop non-inline non-template implementation */

//#include "MultiMCLoop.hh"
//#include "MultiMCLoop.ih"

// Custom includes
#include <senf/Scheduler/ClockService.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/membind.hh>
#include <senf/Socket/Protocols/INet.hh>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

//#include "MultiMCLoop.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

typedef senf::UDPv4ClientSocketHandle UDPSocket;

class MCReader
{
    std::string name;
    UDPSocket socket;
    senf::scheduler::FdEvent event;

    void handler(int events);

public:
    MCReader(unsigned n, std::string const & name, UDPSocket::Address const & group);
};

prefix_ MCReader::MCReader(unsigned n, std::string const & name_,
                           UDPSocket::Address const & group)
    : name (name_), socket (),
      event (name, senf::membind(&MCReader::handler, this), socket,
             senf::scheduler::FdEvent::EV_READ)
{
    socket.protocol().reuseaddr(true);
    socket.bind(group);
    socket.protocol().bindInterface("dummy0");
    socket.protocol().mcAddMembership(group.address(), "dummy0");
}

prefix_ void MCReader::handler(int events)
{
    std::cout << "I " << name << ": " << socket.read() << "\n";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////

class MCWriter
{
    std::string name;
    UDPSocket::Address group;
    UDPSocket socket;
    senf::ClockService::clock_type interval;
    senf::scheduler::TimerEvent event;
    unsigned count;

    void handler();

public:
    MCWriter(std::string const & name, UDPSocket::Address const & group,
             senf::ClockService::clock_type interval);
};

prefix_ MCWriter::MCWriter(std::string const & name_, UDPSocket::Address const & group_,
                           senf::ClockService::clock_type interval_)
    : name (name_), group (group_), socket (), interval (interval_),
      event (name, senf::membind(&MCWriter::handler, this), senf::ClockService::now() + interval),
      count (0)
{}

prefix_ void MCWriter::handler()
{
    std::stringstream ss;
    ss << name << "-" << ++count;
    std::cout << "O " << name << ": " << ss.str() << "\n";
    socket.writeto(group, ss.str());
    event.timeout(senf::scheduler::eventTime() + interval);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////

class IfSetup
{
    std::string iface;

public:
    IfSetup(std::string const & iface_);
    ~IfSetup();

    void sys(std::string const & cmd);

    struct SystemException : public senf::Exception
    { SystemException() : senf::Exception("IfSetup::SystemException") {} };

};

prefix_ IfSetup::IfSetup(std::string const & iface_)
    : iface (iface_)
{
    sys((boost::format("ifconfig %s up") % iface).str());
    sys((boost::format("ifconfig %s 192.168.192.1") % iface).str());
    sys((boost::format("ip route add 224.0.0.0/4 dev %s") % iface).str());
}

prefix_ IfSetup::~IfSetup()
{
    try {
        sys((boost::format("ifconfig %s down") % iface).str());
    }
    catch (SystemException & ex) {
        std::cerr << ex.what() << "\n";
    }
}

prefix_ void IfSetup::sys(std::string const & cmd)
{
    int rv (system(cmd.c_str()));
    if (rv != 0)
        throw SystemException() << ": code " << rv << ": " << cmd;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////

void sigintHandler(siginfo_t const &)
{
    senf::scheduler::terminate();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char * argv[])
{
    try {
        boost::scoped_ptr<IfSetup> setup (
            (argc != 2 || std::string(argv[1]) != "-n") ? new IfSetup("dummy0") : 0);

        senf::scheduler::SignalEvent sigint (SIGINT, &sigintHandler);

        UDPSocket::Address g1 ("225.1:43434");
        UDPSocket::Address g2 ("225.2:43434");

        MCReader r1g1 (1u, "r1g1", g1);
        MCReader r2g1 (2u, "r2g1", g1);
        MCReader r1g2 (3u, "r1g2", g2);
        MCReader r2g2 (4u, "r2g2", g2);

        MCWriter w1g1 ("w1g1", g1, senf::ClockService::milliseconds(600));
        MCWriter w2g1 ("w2g1", g1, senf::ClockService::milliseconds(800));
        MCWriter w1g2 ("w1g2", g2, senf::ClockService::milliseconds(700));
        MCWriter w2g2 ("w2g2", g2, senf::ClockService::milliseconds(900));

        senf::scheduler::process();
    }
    catch (std::exception const & ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
};

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "MultiMCLoop.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u multimcloop"
// End:
