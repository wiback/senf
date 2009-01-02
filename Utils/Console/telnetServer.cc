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
    \brief telnetServer non-inline non-template implementation */

//#include "telnetServer.hh"
//#include "telnetServer.ih"

// Custom includes
#include <boost/bind.hpp>
#include "Telnet.hh"
#include "../../Scheduler/Scheduler.hh"
#include "../Logger.hh"
#include "../../Socket/Protocols/INet.hh"

//#include "telnetServer.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    class MyTelnet 
        : public virtual senf::console::detail::BaseTelnetProtocol,
          public senf::console::detail::telnethandler::TerminalType,
          public senf::console::detail::telnethandler::NAWS
    {
    public:
        explicit MyTelnet(Handle handle) : senf::console::detail::BaseTelnetProtocol(handle) 
            {
                requestPeerOption(senf::console::detail::telnetopt::SUPPRESS_GO_AHEAD);
                requestLocalOption(senf::console::detail::telnetopt::SUPPRESS_GO_AHEAD);
                requestLocalOption(senf::console::detail::telnetopt::ECHO);
            }
        
    private:
        virtual void v_charReceived(char c)
            {
                SENF_LOG(("Char: " << c));
            }

        virtual void v_eof()
            {
                SENF_LOG(("EOF"));
                delete this;
            }

        virtual void v_setupComplete()
            {
                SENF_LOG(("Terminal type is '" << terminalType() << "', window size is "
                          << width() << "x" << height()));
            }

        virtual void v_windowSizeChanged()
            {
                SENF_LOG(("New window size: " << width() << "x" << height()));
            }
    };

    typedef senf::TCPv4ServerSocketHandle ServerHandle;
    typedef ServerHandle::ClientHandle ClientHandle;

    void connect(ServerHandle handle, int events)
    {
        if (events != senf::scheduler::FdEvent::EV_READ) {
            senf::scheduler::terminate();
            return;
        }
        ClientHandle client (handle.accept());
        SENF_LOG(("new client ..."));
        new MyTelnet (client);
    }

}

int main(int argc, char const ** argv)
{
    senf::log::ConsoleTarget::instance().timeFormat("");

    ServerHandle server (ServerHandle::Address("127.0.0.1:22344"));
    senf::scheduler::FdEvent serverEvent ("telnetServer", boost::bind(&connect, server, _1),
                                          server, senf::scheduler::FdEvent::EV_READ);
    SENF_LOG(("Server started at " << server.local()));

    senf::scheduler::process();

    return 0;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "telnetServer.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u telnetServer"
// End:
