// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
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
//   Stefan Bund <g0dil@berlios.de>

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
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    class MyTelnet
        : public senf::term::TelnetTerminal
    {
        SENF_LOG_CLASS_AREA();
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
