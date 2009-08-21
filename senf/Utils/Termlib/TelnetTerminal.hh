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
#include "Telnet.hh"
#include "AbstractTerminal.hh"
#include <senf/Utils/Logger.hh>

//#include "TelnetTerminal.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace term {

    /** \brief AbstractTerminal interface implementation based on telnet

        This class provides a telnet server implementation implementing the AbstractTerminal
        interface. 

        TelnetTerminal provides one additional callback which needs to be implemented in a derived
        class: v_setupFailed(). This member will be called, when not all required telnet options are
        supported by the telnet client. In this case, the communication will be switched back into
        line-oriented mode and v_setupFailed() is called.

        \ingroup telnet_group
     */
    class TelnetTerminal
        : public telnethandler::TerminalType,
          public telnethandler::NAWS,
          public AbstractTerminal
    {
        SENF_LOG_CLASS_AREA();
    public:
        TelnetTerminal();

        ///\name AbstractTerminal interface implementation
        ///\{
        virtual void setCallbacks(AbstractTerminal::Callbacks & cb);
        virtual std::string terminalType();
        virtual unsigned width() const;
        virtual unsigned height() const;
        virtual void write(char ch);
        ///\}

    protected:

#   ifndef DOXYGEN
    private:
#   endif
        virtual void v_setupFailed() = 0; ///< Called when the telnet setup has failed

    private:
        virtual void v_setupComplete();
        virtual void v_charReceived(char ch);
        virtual void v_windowSizeChanged();

        AbstractTerminal::Callbacks * callbacks_;
        bool setupFailed_;
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
