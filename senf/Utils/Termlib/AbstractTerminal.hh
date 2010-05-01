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
    \brief Terminal public header */

#ifndef HH_SENF_Utils_Termlib_AbstractTerminal_
#define HH_SENF_Utils_Termlib_AbstractTerminal_ 1

// Custom includes
#include <string>

//#include "AbstractTerminal.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace term {

    /** \brief Abstract terminal interface

        This abstract interface base class provides an abstract interface to a terminal. There are
        two parts to this interface:

        \li The interface which allows the terminal user to get information about the terminal
        \li The interface which allows the terminal to send messages to the terminal user

        The first part is implemented by providing abstract virtual members in AbstractTerminal. To
        allow the terminal to send messages to the terminal user, the terminal user implements the
        AbstractTerminal::Callbacks interface. The terminal user must register himself with the
        AbstractTerminal by calling setCallbacks(). Afterwards, the AbstractTerminal implementation
        will send calls to the terminal user via the AbstractTerminal::Callbacks API.
     */
    struct AbstractTerminal
    {
        /** \brief AbstractTerminal callbacks

            \see AbastractTerminal
         */
        struct Callbacks {
            virtual ~Callbacks() {}
            virtual bool cb_init() = 0; ///< Called after terminal initialization is complete
                                        /**< This member may return \c false. In this case, the
                                             terminal setup is considered to have failed. */
            virtual void cb_charReceived(char ch) = 0; ///< Called whenever a char is received
            virtual void cb_windowSizeChanged() = 0; ///< Called when the terminal window is changed
        };

        virtual ~AbstractTerminal() {}

        virtual void setCallbacks(Callbacks & cb) = 0; ///< Register terminal callbacks

        virtual std::string terminalType() = 0; ///< Get the terminal type
        virtual unsigned width() const = 0;   ///< Get current terminal window width
        virtual unsigned height() const = 0;  ///< Get current terminal window height

        virtual void write(char ch) = 0; ///< Write character to terminal
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "AbstractTerminal.cci"
//#include "AbstractTerminal.ct"
//#include "AbstractTerminal.cti"
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
