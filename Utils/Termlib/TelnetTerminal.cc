// $Id$
//
// Copyright (C) 2009 
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
    \brief TelnetTerminal non-inline non-template implementation */

#include "TelnetTerminal.hh"
//#include "TelnetTerminal.ih"

// Custom includes

//#include "TelnetTerminal.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::term::TelnetTerminal::TelnetTerminal()
    : setupFailed_ (false)
{
    requestPeerOption(telnetopt::SUPPRESS_GO_AHEAD);
    requestLocalOption(telnetopt::SUPPRESS_GO_AHEAD);
    requestLocalOption(telnetopt::ECHO);
}

prefix_ void senf::term::TelnetTerminal::setCallbacks(AbstractTerminal::Callbacks & cb)
{
    callbacks_ = &cb;
}

prefix_ std::string senf::term::TelnetTerminal::terminalType()
{
    return telnethandler::TerminalType::terminalType();
}

prefix_ unsigned senf::term::TelnetTerminal::width()
{
    return telnethandler::NAWS::width();
}

prefix_ unsigned senf::term::TelnetTerminal::height()
{
    return telnethandler::NAWS::height();
}

prefix_ void senf::term::TelnetTerminal::write(char ch)
{
    BaseTelnetProtocol::write(ch);
}

prefix_ void senf::term::TelnetTerminal::v_setupComplete()
{
    bool init (true);

    if (setupFailed_)
        v_setupFailed();
    else if (! (width() > 0 
                && ! terminalType().empty()
                && localOption(telnetopt::SUPPRESS_GO_AHEAD)
                && peerOption(telnetopt::SUPPRESS_GO_AHEAD)
                && localOption(telnetopt::ECHO)
                && (init = callbacks_->cb_init()))) {

        SENF_LOG_BLOCK((senf::log::NOTICE)({
            log << "TelnetTerminal setup failed:\n";
            if (width() <= 0)
                log << "    missing telnet client NAWS support\n";
            if (terminalType().empty())
                log << "    missing telnet client TERMINAL_TYPE support\n";
            if (! localOption(telnetopt::SUPPRESS_GO_AHEAD) ||
                ! peerOption(telnetopt::SUPPRESS_GO_AHEAD))
                log << "    missing telnet clinet SGO support\n";
            if (! localOption(telnetopt::ECHO))
                log << "    missing telnet client ECHO support\n";
            if (! init)
                log << "    terminal initialization (cb_init) failed\n";
        }));
                  
        setupFailed_ = true;
        requestPeerOption(telnetopt::SUPPRESS_GO_AHEAD, false);
        requestLocalOption(telnetopt::SUPPRESS_GO_AHEAD, false);
        requestLocalOption(telnetopt::ECHO, false);
        requestPeerOption(telnetopt::TERMINAL_TYPE, false);
        requestPeerOption(telnetopt::NAWS, false);
        if (! requestsPending())
            v_setupFailed();
    }
    else
        SENF_LOG((senf::log::NOTICE)(
                     "Initialized TelnetTerminal: TERM=" << terminalType()
                     << ", size=" << width() << "x" << height()));
}

prefix_ void senf::term::TelnetTerminal::v_charReceived(char ch)
{
    callbacks_->cb_charReceived(ch);
}

prefix_ void senf::term::TelnetTerminal::v_windowSizeChanged()
{
    callbacks_->cb_windowSizeChanged();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "TelnetTerminal.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
