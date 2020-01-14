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
    \brief emushark non-inline non-template implementation */

//#include "emushark.hh"
//#include "emushark.ih"

// Custom includes
#include <dlfcn.h>
#include <boost/signals2.hpp>
#include <iostream>
#include "EmulatedInterface.hh"
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/Console.hh>
#include <senf/Utils/hexdump.hh>

#include "WLAN/EmulatedWLANInterface.hh"

//#include "emushark.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    // We need to force linking with all emulated interface types so the corresponding packet
    // extensions are linked in
    void emulatedWLANInterface() { senf::emu::EmulatedWLANInterface iface; }
}

namespace {

    struct EmulatedWirelessControlListener
    {
        EmulatedWirelessControlListener()
            : group_ (senf::emu::emulatedFrequencyRegistry().consoleGroup()),
              enabled_ (false)
        {
            namespace fty = senf::console::factory;

            senf::console::root().add("emuctl", dir_);
            dir_.add("enable", fty::Command(&EmulatedWirelessControlListener::enable, this));
            dir_.add("address",fty::Variable(boost::ref(group_)));

            senf::console::DirectoryNode & ipc (dir_.add("ipc", fty::Directory()));

            ipc.add("start", fty::Command(&EmulatedWirelessControlListener::ipcStart, this));
            ipc.add("stop", fty::Command(&EmulatedWirelessControlListener::ipcStop, this));
            ipc.add("add", fty::Command(&EmulatedWirelessControlListener::ipcAdd, this));
            ipc.add("del", fty::Command(&EmulatedWirelessControlListener::ipcDel, this));
            ipc.add("join", fty::Command(&EmulatedWirelessControlListener::ipcJoin, this));
            ipc.add("leave", fty::Command(&EmulatedWirelessControlListener::ipcLeave, this));
            ipc.add("poll", fty::Command(&EmulatedWirelessControlListener::ipcPoll, this));
        }

        void start()
        {
            if (!enabled_)
                return;
            server_.reset(new senf::console::UDPServer(group_));
            server_->root(dir_["ipc"]).replies(false);
        }

        void ipcStart()
        {
            SENF_LOG(("Control channel: start"));
        }

        void ipcStop()
        {
            SENF_LOG(("Control channel: stop"));
        }

        void ipcAdd(unsigned nodeId, unsigned frequency, unsigned bandwidth,
                    senf::emu::UDPClientHandle::Address address)
        {
            SENF_LOG(("Control channel: add " << nodeId << " " << frequency << " "
                      << bandwidth << " " << address));
            channelInfo(nodeId, frequency, bandwidth, address);
        }

        void ipcDel(unsigned nodeId, unsigned frequency, unsigned bandwidth)
        {
            SENF_LOG(("Control channel: del " << nodeId << " " << frequency << " "
                      << bandwidth));
            channelInfo(nodeId, frequency, bandwidth, senf::emu::UDPClientHandle::Address());
        }

        void ipcJoin(unsigned nodeId, unsigned frequency, unsigned bandwidth)
        {
            SENF_LOG(("Control channel: join " << nodeId << " " << frequency << " "
                      << bandwidth));
        }

        void ipcLeave(unsigned nodeId, unsigned frequency, unsigned bandwidth)
        {
            SENF_LOG(("Control channel: leave " << nodeId << " " << frequency << " "
                      << bandwidth));
        }

        void ipcPoll(unsigned nodeId, unsigned frequency, unsigned bandwidth)
        {
            SENF_LOG(("Control channel: poll " << nodeId << " " << frequency << " "
                      << bandwidth));
        }

        void enable()
        {
            enabled_ = true;
        }

        boost::signals2::signal<void (unsigned, unsigned, unsigned,
                            senf::emu::UDPClientHandle::Address const &)> channelInfo;
        boost::scoped_ptr<senf::console::UDPServer> server_;
        senf::emu::UDPClientHandle::Address group_;
        senf::console::ScopedDirectory<> dir_;
        bool enabled_;
    };

    struct ChannelListener
    {
        ChannelListener(EmulatedWirelessControlListener & ctl)
            : frequency_ (0u), bandwidth_ (0u), ownerNodeId_ (0u), socket_ (senf::noinit),
              ctl_ (ctl), event_ ("ChannelListener", senf::membind(&ChannelListener::data, this))
        {
            namespace fty = senf::console::factory;

            senf::console::root().add("channel", dir_);
            dir_.add("frequency", fty::Command(&ChannelListener::frequency, this));
            dir_.add("bandwidth", fty::Command(&ChannelListener::bandwidth, this));
            dir_.add("address", fty::Command(&ChannelListener::address, this));
        }

        void start()
        {
            if (frequency_ && bandwidth_)
                ctl_.channelInfo.connect(senf::membind(&ChannelListener::channelInfo, this));
            socketSetup();
        }

        void socketSetup()
        {
            if (! address_)
                return;
            socket_ = senf::emu::UDPClientHandle();
            socket_.protocol().reuseaddr(true);
            socket_.bind(address_);
            socket_.protocol().mcAddMembership(address_.address());
            event_
                .handle(socket_)
                .events(senf::scheduler::FdEvent::EV_READ)
                .enable();
        }

        void frequency(unsigned f)
        {
            ctl_.enable();
            frequency_ = f;
        }

        void bandwidth(unsigned b)
        {
            ctl_.enable();
            bandwidth_ = b;
        }

        void address(senf::emu::UDPClientHandle::Address const & a)
        {
            address_ = a;
        }

        void data(int event)
        {
            if (event != senf::scheduler::FdEvent::EV_READ) {
                SENF_LOG((senf::log::FATAL)("Unknown socket error"));
                senf::scheduler::terminate();
            }
            senf::emu::PacketHeader p (senf::emu::PacketHeader::create(senf::noinit));
            socket_.read(p.data(), 0u);
            SENF_LOG_BLOCK(({
                        log << "Packet meta:\n"
                            << "  received at : "
                            << senf::ClockService::abstime(senf::ClockService::now()) << "\n"
                            << "  size        : " << p.data().size() << "\n";
                        senf::hexdump(p.data().begin(), p.data().end(), log);
                        p.dump(log);
                        senf::hexdump(p.last().data().begin(), p.last().data().end(), log);
                        log << ".\n";
                    }));
        }

        void channelInfo(unsigned nodeId, unsigned frequency, unsigned bandwidth,
                         senf::emu::UDPClientHandle::Address const & address)
        {
            if (frequency == frequency_ && bandwidth == bandwidth_ &&
                (! ownerNodeId_ || nodeId <= ownerNodeId_)) {
                ownerNodeId_ = nodeId;
                address_ = address;
                if (! address_)
                    ownerNodeId_ = 0;
                if (socket_)
                    socket_.close();
                socketSetup();
                if (address_)
                    SENF_LOG(("Channel " << frequency_ << "/" << bandwidth_
                              << " is on " << address_));
                else
                    SENF_LOG(("Channel " << frequency_ << "/" << bandwidth_
                              << " disabled. Waiting for new address."));
            }
        }

        unsigned frequency_;
        unsigned bandwidth_;
        unsigned ownerNodeId_;
        senf::emu::UDPClientHandle::Address address_;
        senf::emu::UDPClientHandle socket_;
        EmulatedWirelessControlListener & ctl_;
        senf::scheduler::FdEvent event_;
        senf::console::ScopedDirectory<> dir_;
    };

    void loadBundle(std::string const & path)
    {
        void *handle = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if (handle == NULL) {
            std::cerr << "could not load packet bundle " << path << ": "<< dlerror() << std::endl;
            exit(1);
        }
    }

}

int main(int argc, char const ** argv)
{
    namespace fty = senf::console::factory;

    senf::log::ConsoleTarget & consoleLog (senf::log::ConsoleTarget::instance());
    consoleLog.route<senf::log::VERBOSE>();
    consoleLog.showLevel(false);
    consoleLog.showArea(false);
    consoleLog.tag("");
    consoleLog.timeFormat("");

    if (argc <= 1) {
        std::cerr <<
            "Usage:\n"
            "    " << argv[0] << "\n" <<
            "            [--channel-frequency=<freq> --channel-bandwidth=<bw>]\n"
            "            [--channel-address=<a>]\n"
            "            [--emuctl-enable]\n"
            "            [--emuctl-address=<addr>\n"
            "\n" <<
            argv[0] << " allows listening to either a fixed multicast group (e.g. for emulated\n"
            "wired interfaces) using '--channel-address' or to a wireless channel identified by\n"
            "frequency and bandwidth using '--channel-frequency' and '--channel-bandwidth'\n"
            "\n"
            "When listening to a wireless channel, " << argv[0] << " will additionally listen to\n"
            "all control messages on the EmulatedFrequencyRegistry control channel at multicast\n"
            "address 239.202.104.1:4701.\n"
            "\n"
            "To listen to the control channel *only*, specify '--emuctl-enable'\n"
            "\n"
            "You may change the control group address using '--emuctl-address.\n";
        return 0;
    }

    EmulatedWirelessControlListener controlListener;
    ChannelListener channelListener (controlListener);

    senf::console::root().add("load-bundle", fty::Command(&loadBundle));

    // This disables the EmulatedFrequencyRegistry
    senf::emu::emulatedFrequencyRegistry().consoleGroup(senf::emu::UDPClientHandle::Address());

    senf::console::parseOptions(argc, argv);

    controlListener.start();
    channelListener.start();

    SENF_LOG(("Starting capturing ..."));

    senf::scheduler::process();

    return 1;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "emushark.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u emushark"
// End:
