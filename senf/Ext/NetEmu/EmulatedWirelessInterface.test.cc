// $Id:$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//       Stefan Bund <g0dil@berlios.de>

/** \file
    \brief EmulatedWirelessInterface.test unit tests */

#include "EmulatedWirelessInterface.hh"

// Custom includes
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/Logger.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    void timeout()
    {
        senf::scheduler::terminate();
    }

    senf::ClockService::clock_type delay(unsigned ms)
    {
        return senf::scheduler::now() + senf::ClockService::milliseconds(ms);
    }

    struct ConsoleGroupWatcher
    {
        ConsoleGroupWatcher()
            :  socket_ (senf::emu::emulatedFrequencyRegistry().consoleGroup()),
               event_ ("ConsoleGroupWatcher", senf::membind(&ConsoleGroupWatcher::read, this),
                       socket_, senf::scheduler::FdEvent::EV_READ),
               watchdog_ ("EmulatedFrequencyRegistry test timeout", &timeout)
            {
                socket_.protocol().mcAddMembership(
                    senf::emu::emulatedFrequencyRegistry().consoleGroup().address());
                socket_.protocol().mcLoop(true);
            }

        void read (int events)
            {
                BOOST_CHECK_EQUAL( events, senf::scheduler::FdEvent::EV_READ );
                std::string d;
                socket_.read(d,0u);
                data_ += d;
                data_ += "\n";
            }

        std::string const & data()
            {
                return data_;
            }

        void clear()
            {
                data_.clear();
            }

        void call(std::string const & cmd)
            {
                socket_.writeto(senf::emu::emulatedFrequencyRegistry().consoleGroup(), cmd);
                wait();
            }

        void wait()
            {
                watchdog_.timeout(delay(400u));
                senf::scheduler::process();
            }

        senf::emu::UDPClientHandle socket_;
        senf::scheduler::FdEvent event_;
        std::string data_;
        senf::scheduler::TimerEvent watchdog_;
    };

    struct AllocateChecker
    {
        typedef void result_type;

        AllocateChecker() : calls (0u) {}

        void operator()(senf::emu::UDPClientHandle::Address const & addr)
            {
                address = addr;
                ++ calls;
            }

        unsigned calls;
        senf::emu::UDPClientHandle::Address address;
    };

    struct TestEntry
    {
        unsigned frequency;
        unsigned bandwidth;
        char const * address;
        unsigned nUsers;
        unsigned ownerNodeId;

        bool operator!=(senf::emu::EmulatedFrequencyRegistry::Entry const & e)
            {
                return ! (frequency == e.frequency
                          && bandwidth == e.bandwidth
                          && senf::emu::UDPClientHandle::Address(address) == e.address
                          && nUsers == e.nUsers
                          && ownerNodeId == e.ownerNodeId);
            }
    };

    std::ostream & operator<<(std::ostream & os, TestEntry const & e)
    {
        os << "{TestEntry: " << e.frequency << " " << e.bandwidth << " " << e.address << " "
           << e.nUsers << " " << e.ownerNodeId << "}";
        return os;
    }

    struct ChangeWatcher
        : public boost::signals2::trackable
    {
        ChangeWatcher() : count (0u) {}

        void operator()(senf::emu::EmulatedFrequencyRegistry::Entry const &,
                        unsigned nodeId,
                        senf::emu::EmulatedFrequencyRegistry::ChangeType type)
            { ++ count; }
        void clear()
            { count = 0; }

        unsigned count;
    };
}

namespace senf {
namespace emu {

    std::ostream & operator<<(std::ostream & os, EmulatedFrequencyRegistry::Entry const & e)
    {
        os << "{Entry: " << e.frequency << " " << e.bandwidth << " " << e.address << " "
           << e.nUsers << " " << e.ownerNodeId << "}";
        return os;
    }

}}

namespace std {

    template <class T1, class T2>
    std::ostream & operator<<(std::ostream & os, std::pair<T1,T2> const & arg)
    {
        os << '(' << arg.first << ',' << arg.second << ')';
        return os;
    }

}

#define CHECK_ENTRIES( ... )                                            \
    {                                                                   \
        TestEntry entries[] = __VA_ARGS__;                              \
        senf::emu::EmulatedFrequencyRegistry::EntriesRange er (         \
            senf::emu::emulatedFrequencyRegistry().entries());          \
        BOOST_CHECK_EQUAL_COLLECTIONS(entries, entries+sizeof(entries)/sizeof(entries[0]), \
                                      er.begin(), er.end());            \
    }

#define CHECK_USERS( index, ... )                                       \
    {                                                                   \
        unsigned users[] = __VA_ARGS__;                                 \
        senf::emu::EmulatedFrequencyRegistry::UsersRange ur (           \
            senf::emu::emulatedFrequencyRegistry().users(               \
                boost::next(senf::emu::emulatedFrequencyRegistry().entries().begin(),index))); \
        BOOST_CHECK_EQUAL_COLLECTIONS( users, users+sizeof(users)/sizeof(users[0]), \
                                       ur.begin(), ur.end() );          \
    }

SENF_AUTO_TEST_CASE(emulatedFrequencyRegistry)
{
    senf::log::ConsoleTarget::instance().route<senf::log::VERBOSE>();

    ConsoleGroupWatcher watcher;
    ChangeWatcher changes;

    BOOST_CHECK_EQUAL( senf::emu::Interface::nodeId(), ::getpid() );
    SENF_CHECK_NO_THROW( senf::emu::Interface::nodeId(100u) );
    BOOST_CHECK_EQUAL( senf::emu::Interface::nodeId(), 100u );

    BOOST_CHECK_EQUAL ( senf::emu::emulatedFrequencyRegistry().consoleGroup(),
                        senf::emu::UDPClientHandle::Address ("239.202.104.1:4701") );

    BOOST_CHECK_EQUAL( senf::emu::emulatedFrequencyRegistry().addressRange(),
                       std::make_pair(senf::INet4Network("239.202.108.0/22"), 11264u) );
    SENF_CHECK_NO_THROW( senf::emu::emulatedFrequencyRegistry().addressRange(
                             senf::INet4Network("239.203.108.0/22"), 12264u) );
    BOOST_CHECK_EQUAL( senf::emu::emulatedFrequencyRegistry().addressRange(),
                       std::make_pair(senf::INet4Network("239.203.108.0/22"), 12264u) );

    //-/////////////////////////////////////////////////////////////////////////////////////////////

    senf::emu::emulatedFrequencyRegistry().channelPopulationChanged.connect(boost::ref(changes));
    senf::emu::emulatedFrequencyRegistry().start();

    watcher.wait();
    watcher.clear();

    BOOST_CHECK_EQUAL( changes.count, 0u );

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // Internal commands: allocate/release

    CHECK_ENTRIES({});
    SENF_CHECK_NO_THROW( senf::emu::emulatedFrequencyRegistry().nextAddress(1u) );

    AllocateChecker checker;

    SENF_CHECK_NO_THROW( senf::emu::emulatedFrequencyRegistry().allocate(
                             1u, 2412000, 20000, boost::ref(checker)) );
    watcher.wait();
    BOOST_CHECK_EQUAL( checker.calls, 1u );
    BOOST_CHECK_EQUAL( checker.address, senf::emu::UDPClientHandle::Address("239.203.108.1:12265") );
    BOOST_CHECK_EQUAL( watcher.data(),
                       "add 100 2412000 20000 239.203.108.1:12265\n"
                       "join 100 2412000 20000\n" );
    BOOST_CHECK_EQUAL( changes.count, 1u );
    watcher.clear();
    changes.clear();

    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.1:12265", 1, 100 }
        });
    CHECK_USERS( 0, { 1 } );

    SENF_CHECK_NO_THROW( senf::emu::emulatedFrequencyRegistry().allocate(
                             2u, 2412000, 20000, boost::ref(checker)) );
    watcher.wait();
    BOOST_CHECK_EQUAL( checker.calls, 2u );
    BOOST_CHECK_EQUAL( checker.address, senf::emu::UDPClientHandle::Address("239.203.108.1:12265") );
    BOOST_CHECK_EQUAL( watcher.data(), "join 100 2412000 20000\n" );
    watcher.clear();

    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.1:12265", 2, 100 }
        });
    CHECK_USERS( 0, { 1, 2 } );

    SENF_CHECK_NO_THROW( senf::emu::emulatedFrequencyRegistry().release(1u) );
    BOOST_CHECK_EQUAL(
        std::distance(senf::emu::emulatedFrequencyRegistry().entries().begin(),
                      senf::emu::emulatedFrequencyRegistry().entries().end()), 1u );
    watcher.wait();
    BOOST_CHECK_EQUAL( watcher.data(), "leave 100 2412000 20000\n" );
    watcher.clear();

    SENF_CHECK_NO_THROW( senf::emu::emulatedFrequencyRegistry().release(2u) );
    BOOST_CHECK_EQUAL(
        std::distance(senf::emu::emulatedFrequencyRegistry().entries().begin(),
                      senf::emu::emulatedFrequencyRegistry().entries().end()), 0u );
    watcher.wait();
    BOOST_CHECK_EQUAL( watcher.data(),
                       "leave 100 2412000 20000\n"
                       "del 100 2412000 20000\n" );
    watcher.clear();

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // external IPC stuff: add/join/leave/del/poll

    SENF_CHECK_NO_THROW( senf::emu::emulatedFrequencyRegistry().allocate(
                             1u, 2412000, 20000, boost::ref(checker)) );
    watcher.call("add 90 2420000 20000 239.203.108.100:12364");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.2:12266", 1, 100 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 }
        });
    CHECK_USERS( 0, { 1 } );
    CHECK_USERS( 1, {} );
    BOOST_CHECK_EQUAL( watcher.data(),
                       "add 100 2412000 20000 239.203.108.2:12266\n"
                       "join 100 2412000 20000\n"
                       "add 90 2420000 20000 239.203.108.100:12364\n");
    watcher.clear();

    // Test add frequency conflict: Same frequency, different address
    // a) lower nodeId, b) higher nodeId

    watcher.call("add 90 2412000 20000 239.203.108.101:12365");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.101:12365", 1, 90 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 }
        });

    watcher.call("add 110 2420000 20000 239.203.108.102:12366");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.101:12365", 1, 90 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 }
        });

    // Test add address conflict: Address already in use by other frequency
    // a) lower nodeId, b) higher nodeId

    watcher.call("add 80 2414000 20000 239.203.108.101:12365");
    CHECK_ENTRIES({
            { 2412000, 20000, "0.0.0.0:0", 1, 90 },
            { 2414000, 20000, "239.203.108.101:12365", 0, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 }
        });
    CHECK_USERS( 0, { 1 } );
    CHECK_USERS( 1, {} );
    CHECK_USERS( 2, {} );

    watcher.call("add 110 2422000 20000 239.203.108.100:12364");
    CHECK_ENTRIES({
            { 2412000, 20000, "0.0.0.0:0", 1, 90 },
            { 2414000, 20000, "239.203.108.101:12365", 0, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 }
        });

    BOOST_CHECK_EQUAL( watcher.data(),
                       "add 90 2412000 20000 239.203.108.101:12365\n"
                       "add 110 2420000 20000 239.203.108.102:12366\n"
                       "add 80 2414000 20000 239.203.108.101:12365\n"
                       "add 110 2422000 20000 239.203.108.100:12364\n" );
    watcher.clear();

    // Now test add address conflict where owner is this instance (ownerNodeId == nodeId_)
    changes.clear();
    SENF_CHECK_NO_THROW( senf::emu::emulatedFrequencyRegistry().allocate(
                             2u, 2424000, 20000, boost::ref(checker)) );
    CHECK_ENTRIES({
            { 2412000, 20000, "0.0.0.0:0", 1, 90 },
            { 2414000, 20000, "239.203.108.101:12365", 0, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 },
            { 2424000, 20000, "239.203.108.3:12267", 1, 100 }
        });
    CHECK_USERS( 3, { 2 } );

    watcher.call("add 70 2412000 20000 239.203.108.3:12267");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2414000, 20000, "239.203.108.101:12365", 0, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 },
            { 2424000, 20000, "239.203.108.4:12268", 1, 100 }
        });
    BOOST_CHECK_EQUAL( watcher.data(),
                       "add 100 2424000 20000 239.203.108.3:12267\n"
                       "join 100 2424000 20000\n"
                       "add 70 2412000 20000 239.203.108.3:12267\n"
                       "add 100 2424000 20000 239.203.108.4:12268\n" );
    watcher.clear();
    BOOST_CHECK_EQUAL( changes.count, 1u );
    changes.clear();
    CHECK_USERS( 0, { 1 } );
    CHECK_USERS( 1, {} );
    CHECK_USERS( 2, {} );
    CHECK_USERS( 3, { 2 } );

    // Test join and reversed join

    watcher.call("join 85 2424000 20000");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2414000, 20000, "239.203.108.101:12365", 0, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 },
            { 2424000, 20000, "239.203.108.4:12268", 2, 100 }
        });

    watcher.call("join 85 2426000 20000");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2414000, 20000, "239.203.108.101:12365", 0, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 },
            { 2424000, 20000, "239.203.108.4:12268", 2, 100 },
            { 2426000, 20000, "0.0.0.0:0", 1, 0 }
        });

    // Test leave
    // a) ordinary leave, b) leave of group with ownerNodeId == nodeId_
    // c) leave of orphaned entry (no owner/address), d) leave of unknown entry
    // e) extraneous leave

    // a)
    // Need to join some other group to test a)
    watcher.call("join 85 2414000 20000");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2414000, 20000, "239.203.108.101:12365", 1, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 },
            { 2424000, 20000, "239.203.108.4:12268", 2, 100 },
            { 2426000, 20000, "0.0.0.0:0", 1, 0 }
        });
    watcher.call("leave 85 2414000 20000");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2414000, 20000, "239.203.108.101:12365", 0, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 },
            { 2424000, 20000, "239.203.108.4:12268", 2, 100 },
            { 2426000, 20000, "0.0.0.0:0", 1, 0 }
        });
    BOOST_CHECK_EQUAL( watcher.data(),
                       "join 85 2424000 20000\n"
                       "join 85 2426000 20000\n"
                       "join 85 2414000 20000\n"
                       "leave 85 2414000 20000\n" );
    watcher.clear();

    // b)
    watcher.clear();
    SENF_CHECK_NO_THROW( senf::emu::emulatedFrequencyRegistry().release(2u) );
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2414000, 20000, "239.203.108.101:12365", 0, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 },
            { 2424000, 20000, "239.203.108.4:12268", 1, 100 },
            { 2426000, 20000, "0.0.0.0:0", 1, 0 }
        });
    CHECK_USERS( 3, {} );
    watcher.call("leave 85 2424000 20000");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2414000, 20000, "239.203.108.101:12365", 0, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 },
            { 2426000, 20000, "0.0.0.0:0", 1, 0 }
        });
    BOOST_CHECK_EQUAL( watcher.data(),
                       "leave 100 2424000 20000\n"
                       "leave 85 2424000 20000\n"
                       "del 100 2424000 20000\n" );
    watcher.clear();

    // c)
    watcher.call("leave 85 2426000 20000");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2414000, 20000, "239.203.108.101:12365", 0, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 }
        });
    BOOST_CHECK_EQUAL( watcher.data(),
                       "leave 85 2426000 20000\n" );
    watcher.clear();

    // d)
    watcher.call("leave 85 2428000 20000");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2414000, 20000, "239.203.108.101:12365", 0, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 }
        });

    // e)
    watcher.call("leave 85 2414000 20000");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2414000, 20000, "239.203.108.101:12365", 0, 80 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 }
        });
    BOOST_CHECK_EQUAL( watcher.data(),
                       "leave 85 2428000 20000\n"
                       "leave 85 2414000 20000\n" );
    watcher.clear();

    // Test del
    // a) ordinary del b) del of unknown entry c) del by non-owner

    // a)
    watcher.call("del 80 2414000 20000");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 }
        });

    // b)
    watcher.call("del 80 2416000 20000");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 }
        });

    // c)
    watcher.call("del 80 2420000 20000");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.3:12267", 1, 70 },
            { 2420000, 20000, "239.203.108.100:12364", 0, 90 }
        });
    BOOST_CHECK_EQUAL( watcher.data(),
                       "del 80 2414000 20000\n"
                       "del 80 2416000 20000\n"
                       "del 80 2420000 20000\n" );
    watcher.clear();

    // Test stop
    watcher.call("stop");
    CHECK_ENTRIES({
            { 2412000, 20000, "0.0.0.0:0", 0, 0 }
        });
    CHECK_USERS( 0, { 1 } );
    BOOST_CHECK_EQUAL( watcher.data(), "stop\n" );
    watcher.clear();

    // Test adding entries while not running
    SENF_CHECK_NO_THROW( senf::emu::emulatedFrequencyRegistry().allocate(
                             2, 2414000, 20000, checker ) );
    CHECK_ENTRIES({
            { 2412000, 20000, "0.0.0.0:0", 0, 0 },
            { 2414000, 20000, "0.0.0.0:0", 0, 0 }
        });
    CHECK_USERS( 0, { 1 } );
    CHECK_USERS( 1, { 2 } );
    BOOST_CHECK_EQUAL( watcher.data(), "" );

    // Add another interface to existing group
    SENF_CHECK_NO_THROW( senf::emu::emulatedFrequencyRegistry().allocate(
                             3, 2412000, 20000, checker ) );
    CHECK_ENTRIES({
            { 2412000, 20000, "0.0.0.0:0", 0, 0 },
            { 2414000, 20000, "0.0.0.0:0", 0, 0 }
        });
    CHECK_USERS( 0, { 1,3 } );
    CHECK_USERS( 1, { 2 } );
    BOOST_CHECK_EQUAL( watcher.data(), "" );

    // Test start
    changes.clear();
    watcher.call("start");
    CHECK_ENTRIES({
            { 2412000, 20000, "239.203.108.5:12269", 2, 100 },
            { 2414000, 20000, "239.203.108.6:12270", 1, 100 }
        });
    CHECK_USERS( 0, { 1,3 } );
    CHECK_USERS( 1, { 2 } );
    BOOST_CHECK_EQUAL( watcher.data(),
                       "start\n"
                       "add 100 2412000 20000 239.203.108.5:12269\n"
                       "join 100 2412000 20000\n"
                       "join 100 2412000 20000\n"
                       "add 100 2414000 20000 239.203.108.6:12270\n"
                       "join 100 2414000 20000\n" );
    BOOST_CHECK_EQUAL( changes.count, 3u );
    watcher.clear();
    changes.clear();
}

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
