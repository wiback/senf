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
#include <senf/Utils/membind.hh>

//#include "TelnetTerminal.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::term::TelnetTerminal::TelnetTerminal()
    : keyTimeout_ (senf::ClockService::milliseconds(DEFAULT_KEY_TIMEOUT_MS)),
      timer_ ("senf::term::TelnetTerminal::keySequenceTimeout", 
              senf::membind(&TelnetTerminal::keySequenceTimeout, this))
{
    requestPeerOption(telnetopt::SUPPRESS_GO_AHEAD);
    requestLocalOption(telnetopt::SUPPRESS_GO_AHEAD);
    requestLocalOption(telnetopt::ECHO);
}

prefix_ void senf::term::TelnetTerminal::v_setupComplete()
{
    tifo_.load(terminalType());
    keyParser_.load(tifo_);
}

prefix_ void senf::term::TelnetTerminal::v_charReceived(char c)
{
    inputBuffer_ += c;
    timer_.timeout(senf::scheduler::eventTime() + keyTimeout_);
    processKeys();
}

prefix_ void senf::term::TelnetTerminal::keySequenceTimeout()
{
    while (!inputBuffer_.empty()) {
        processKeys();
        v_keyReceived(keycode_t(inputBuffer_[0]));
        inputBuffer_.erase(0, 1);
    }
}

prefix_ void senf::term::TelnetTerminal::processKeys()
{
    do {
        std::pair<senf::term::KeyParser::keycode_t, std::string::size_type> result
            (keyParser_.lookup(inputBuffer_));
        if (result.first == senf::term::KeyParser::Incomplete)
            return;
        v_keyReceived(result.first);
        inputBuffer_.erase(0, result.second);
    } while (! inputBuffer_.empty());
    timer_.disable();
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
