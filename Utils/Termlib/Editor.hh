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
    \brief Editor public header */

#ifndef HH_SENF_Utils_Termlib_Editor_
#define HH_SENF_Utils_Termlib_Editor_ 1

// Custom includes
#include <senf/Scheduler/ClockService.hh>
#include <senf/Scheduler/TimerEvent.hh>
#include "AbstractTerminal.hh"
#include "Terminfo.hh"

//#include "Editor.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace term {

    class BaseEditor
        : public AbstractTerminal::Callbacks
    {
    public:
        typedef KeyParser::keycode_t keycode_t;

        static unsigned const DEFAULT_KEY_TIMEOUT_MS = 500u;

        explicit BaseEditor(AbstractTerminal & terminal);

        void newline();                 ///< Move to beginning of a new, empty line
        void toColumn(unsigned c);      ///< Move cursor to column \p c
        void insertChar(char ch);       ///< Insert \p ch at current column, shifting text right
        void overwriteChar(char ch);    ///< Write \p ch at current column
        void deleteChar();              ///< Delete a character a current column

        unsigned currentColumn() const; ///< Return number of current column

    private:
        virtual void v_keyReceived(keycode_t key) = 0;

        virtual void cb_init();
        virtual void cb_charReceived(char c);
        virtual void cb_windowSizeChanged();

        void keySequenceTimeout();
        void processKeys();

        unsigned width();
        void write(char ch);
        void write(std::string const & s);
        
        AbstractTerminal * terminal_;
        Terminfo tifo_;
        KeyParser keyParser_;
        std::string inputBuffer_;
        ClockService::clock_type keyTimeout_;
        scheduler::TimerEvent timer_;
        unsigned column_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Editor.cci"
//#include "Editor.ct"
//#include "Editor.cti"
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
