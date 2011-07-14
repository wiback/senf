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
    \brief LineEditor public header */

#ifndef HH_SENF_Utils_Console_LineEditor_
#define HH_SENF_Utils_Console_LineEditor_ 1

// Custom includes
#include <boost/scoped_ptr.hpp>
#include <senf/Utils/Termlib/TelnetTerminal.hh>
#include <senf/Utils/Termlib/Editor.hh>
#include "Server.hh"

//#include "LineEditor.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace console {
namespace detail {

    /** \brief Internal: Client reader switching between LineEditorClientReader or DumbClientReader

        This ClientReader implementation will try to initialize a LineEditorClientReader. If the
        setup fails, a DumbClientReader is initialized instead. LineEditor support may fail to be
        initialized if

        \li The telnet client does not support necessary functionality
        \li or the terminal type of the client is missing some functionality.
     */
    class LineEditorSwitcher
        : public ClientReader
    {
    public:
        explicit LineEditorSwitcher(Client & client);

        void editorSetupFailed();

    private:
        virtual void v_disablePrompt();
        virtual void v_enablePrompt();
        virtual void v_write(std::string const & data);
        virtual unsigned v_width() const;

        boost::scoped_ptr<ClientReader> reader_;
    };

    /** \brief Internal: ClientReader using senf::term::LineEditor for interactive input

        This ClientReader implementation uses a senf::term::LineEditor for interactive input.
     */
    class LineEditorClientReader
        : public ClientReader, public term::TelnetTerminal
    {
    public:
        explicit LineEditorClientReader(Client & client, LineEditorSwitcher & switcher);

    private:
        // TelnetTerminal API implementation
        virtual void v_setupFailed();
        virtual void v_eof();

        // ClientReader API implementation
        virtual void v_disablePrompt();
        virtual void v_enablePrompt();
        virtual void v_write(std::string const & data);
        virtual unsigned v_width() const;

        // Editor callbacks
        void executeLine(std::string const & text);
        void deleteCharOrExit(term::LineEditor & editor);
        void completePath(term::LineEditor & editor, unsigned & b, unsigned & e,
                          std::string & prefix, std::vector<std::string> & completions);

        term::LineEditor editor_;
        LineEditorSwitcher * switcher_;
        std::string default_;
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "LineEditor.cci"
//#include "LineEditor.ct"
//#include "LineEditor.cti"
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
