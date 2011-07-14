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
    \brief TelnetTerminal public header */

#ifndef HH_SENF_Utils_Termlib_TelnetTerminal_
#define HH_SENF_Utils_Termlib_TelnetTerminal_ 1

// Custom includes
#include "Telnet.hh"
#include "AbstractTerminal.hh"
#include <senf/Utils/Logger.hh>

//#include "TelnetTerminal.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
        //\{
        virtual void setCallbacks(AbstractTerminal::Callbacks & cb);
        virtual std::string terminalType();
        virtual unsigned width() const;
        virtual unsigned height() const;
        virtual void write(char ch);
        //\}

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
