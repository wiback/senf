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
    \brief LineEditor non-inline non-template implementation */

#include "LineEditor.hh"
//#include "LineEditor.ih"

// Custom includes
#include "../Logger/SenfLog.hh"

//#include "LineEditor.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::LineEditorSwitcher

prefix_ senf::console::detail::LineEditorSwitcher::LineEditorSwitcher(Client & client)
    : ClientReader(client),
      reader_ (new LineEditorClientReader(client, *this))
{}

prefix_ void senf::console::detail::LineEditorSwitcher::editorSetupFailed()
{
    // We need to delete the old reader *before* creating the new one such that all old scheduler
    // events are removed before installing the new ones.
    reader_.reset(0);
    reader_.reset(new DumbClientReader(client()));
}

prefix_ void senf::console::detail::LineEditorSwitcher::v_disablePrompt()
{
    reader_->disablePrompt();
}

prefix_ void senf::console::detail::LineEditorSwitcher::v_enablePrompt()
{
    reader_->enablePrompt();
}

prefix_ void senf::console::detail::LineEditorSwitcher::v_write(std::string const & data)
{
    reader_->write(data);
}

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::LineEditorClientReader

prefix_ senf::console::detail::LineEditorClientReader::
LineEditorClientReader(Client & client, LineEditorSwitcher & switcher)
    : term::BaseTelnetProtocol(client.handle()), ClientReader(client),
      editor_ (*this, senf::membind(&LineEditorClientReader::executeLine, this)),
      switcher_ (&switcher)
{
    editor_.prompt(promptString());
    editor_.defineKey(senf::term::KeyParser::Ctrl('D'),
                      senf::membind(&LineEditorClientReader::deleteCharOrExit, this));
}

prefix_ void senf::console::detail::LineEditorClientReader::v_setupFailed()
{
    // Commits suicide
    switcher_->editorSetupFailed();
}

prefix_ void senf::console::detail::LineEditorClientReader::v_eof()
{
    stopClient();
}

prefix_ void senf::console::detail::LineEditorClientReader::v_disablePrompt()
{
    editor_.hide();
}

prefix_ void senf::console::detail::LineEditorClientReader::v_enablePrompt()
{
    editor_.show();
}

prefix_ void senf::console::detail::LineEditorClientReader::v_write(std::string const & data)
{
    BaseTelnetProtocol::write(data);
}

prefix_ void
senf::console::detail::LineEditorClientReader::executeLine(std::string const & text)
{
    handleInput(text);
    stream() << std::flush;
    editor_.prompt(promptString());
    editor_.show();
}

prefix_ void
senf::console::detail::LineEditorClientReader::deleteCharOrExit(term::LineEditor & editor)
{
    if (editor.text().empty())
        ClientReader::handle().facet<TCPSocketProtocol>().shutdown(TCPSocketProtocol::ShutRD);
    else
        term::bindings::deleteChar(editor);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "LineEditor.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
