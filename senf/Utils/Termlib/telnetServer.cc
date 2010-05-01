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
#include <senf/Utils/membind.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/Logger.hh>
#include <senf/Socket/Protocols/INet.hh>
#include "TelnetTerminal.hh"
#include "Editor.hh"

//#include "telnetServer.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    class MyTelnet
        : public senf::term::TelnetTerminal
    {
    public:
        explicit MyTelnet(Handle handle)
            : senf::term::BaseTelnetProtocol (handle),
              editor_ (*this, senf::membind(&MyTelnet::executeLine, this))
            {
                editor_.prompt("myTelnet-with-an-endlesssly-long-prompt$");
                editor_.defineKey(senf::term::KeyParser::Ctrl('D'),
                                  senf::membind(&MyTelnet::deleteCharOrExit, this));
            }

        void deleteCharOrExit(senf::term::LineEditor & editor)
            {
                if (editor.text().empty()) {
                    exit();
                }
                else
                    senf::term::bindings::deleteChar(editor);
            }

        void exit()
            {
                handle().facet<senf::TCPSocketProtocol>().shutdown(senf::TCPSocketProtocol::ShutRD);
            }

        virtual void v_setupFailed()
            {
                SENF_LOG(("Terminal setup failed"));
                exit();
            }

        virtual void v_eof()
            {
                SENF_LOG(("EOF"));
                delete this;
            }

        void executeLine(std::string const & text)
            {
                SENF_LOG(("Execute line: " << text));
                editor_.show();
            }

    private:
        senf::term::LineEditor editor_;
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
        try {
            new MyTelnet (client);
        }
        catch (std::exception & ex) {
            SENF_LOG(("Client open failed: " << ex.what()));
        }
        catch (...) {
            SENF_LOG(("Client open failed: unknown exception"));
        }
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
