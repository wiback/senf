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
    \brief TelnetTerminal non-inline non-template implementation */

#include "TelnetTerminal.hh"
//#include "TelnetTerminal.ih"

// Custom includes

//#include "TelnetTerminal.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
    const
{
    return telnethandler::NAWS::width();
}

prefix_ unsigned senf::term::TelnetTerminal::height()
    const
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
            else
                log << "    size=" << width() << "x" << height() << "\n";
            if (terminalType().empty())
                log << "    missing telnet client TERMINAL_TYPE support\n";
            else
                log << "    TERM=" << terminalType() << "\n";
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
