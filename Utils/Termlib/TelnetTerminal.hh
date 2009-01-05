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
    \brief TelnetTerminal public header */

#ifndef HH_SENF_Utils_Termlib_TelnetTerminal_
#define HH_SENF_Utils_Termlib_TelnetTerminal_ 1

// Custom includes
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Scheduler/ClockService.hh>
#include "Telnet.hh"
#include "Terminfo.hh"

//#include "TelnetTerminal.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace term {

    class TelnetTerminal
        : public telnethandler::TerminalType,
          public telnethandler::NAWS
    {
    public:
        typedef KeyParser::keycode_t keycode_t;

        static unsigned const DEFAULT_KEY_TIMEOUT_MS = 500u;

        TelnetTerminal();

    protected:
        virtual void v_setupComplete();

    private:
        virtual void v_keyReceived(keycode_t key) = 0;

        virtual void v_charReceived(char c);
        void keySequenceTimeout();
        void processKeys();

        senf::term::Terminfo tifo_;
        senf::term::KeyParser keyParser_;
        std::string inputBuffer_;
        senf::ClockService::clock_type keyTimeout_;
        senf::scheduler::TimerEvent timer_;
    };
}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "TelnetTerminal.cci"
//#include "TelnetTerminal.ct"
//#include "TelnetTerminal.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
