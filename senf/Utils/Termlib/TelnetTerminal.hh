//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief TelnetTerminal public header */

#ifndef HH_SENF_Utils_Termlib_TelnetTerminal_
#define HH_SENF_Utils_Termlib_TelnetTerminal_ 1

// Custom includes
#include "Telnet.hh"
#include "AbstractTerminal.hh"
#include <senf/Utils/Logger/Log.hh>

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
