// $Id$
//
// Copyright (C) 2009
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
    \brief Terminal public header */

#ifndef HH_SENF_Utils_Termlib_AbstractTerminal_
#define HH_SENF_Utils_Termlib_AbstractTerminal_ 1

// Custom includes
#include <string>

//#include "AbstractTerminal.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
