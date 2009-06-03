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
    \brief LineEditor public header */

#ifndef HH_SENF_Utils_Console_LineEditor_
#define HH_SENF_Utils_Console_LineEditor_ 1

// Custom includes
#include <boost/scoped_ptr.hpp>
#include "../Termlib/TelnetTerminal.hh"
#include "../Termlib/Editor.hh"
#include "Server.hh"

//#include "LineEditor.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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
        void completePath(term::LineEditor & editor, unsigned b, unsigned e, 
                          std::vector<std::string> & completions);

        term::LineEditor editor_;
        LineEditorSwitcher * switcher_;
    };

}}}

///////////////////////////////hh.e////////////////////////////////////////
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
