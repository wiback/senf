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
