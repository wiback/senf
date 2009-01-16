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
#include "../../Utils/range.hh"

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
    editor_.defineKey(senf::term::KeyParser::Tab,
                      boost::bind(&term::bindings::complete,
                                  _1, 
                                  senf::membind(&LineEditorClientReader::completePath, this)));
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

prefix_ void senf::console::detail::LineEditorClientReader::
completePath(term::LineEditor & editor, unsigned b, unsigned e,
             std::vector<std::string> & completions)
{
    std::string base (editor.text().substr(b,e));
    CommandParser parser;
    ParseCommandInfo cmd;
    try {
        parser.parsePath(base, cmd);
    }
    catch (CommandParser::ParserErrorException & ex) {
        return;
    }
 
    ParseCommandInfo::TokensRange path (cmd.commandPath());
    if (path.empty()) {
        DirectoryNode::ChildrenRange cs (client().cwd().children());
        for (DirectoryNode::ChildrenRange::iterator i (cs.begin()); i != cs.end(); ++i)
            completions.push_back(i->first + (i->second->followLink().isDirectory() ? "/" : " "));
        return;
    }
    
    ParseCommandInfo::TokensRange::const_iterator i (path.begin());
    ParseCommandInfo::TokensRange::const_iterator const i_end (boost::prior(path.end()));
    DirectoryNode * dir (& client().cwd());
    std::string basePath;
    for (; i != i_end; ++i)
        if (*i == NoneToken()) {
            if (i == path.begin()) {
                dir = & client().root();
                basePath = "/";
            }
        }
        else if (*i == WordToken("..")) {
            DirectoryNode * parent (dir->parent().get());
            if (parent) dir = parent;
            basePath += "../";
        }
        else if (*i == WordToken(".")) 
            basePath += "./";
        else {
            if (dir->hasChild(i->value())) {
                try {
                    dir = & dir->getDirectory(i->value());
                }
                catch (std::bad_cast &) {
                    return;
                }
                basePath += i->value() + "/";
            } 
            else {
                DirectoryNode::ChildrenRange cs (dir->completions(i->value()));
                if (has_one_elt(cs)) {
                    GenericNode & node (cs.begin()->second->followLink());
                    if (!node.isDirectory())
                        return;
                    dir = static_cast<DirectoryNode*>(&node);
                    basePath += cs.begin()->first + "/";
                }
                else
                    return;
            }
        }

    DirectoryNode::ChildrenRange cs (dir->completions(i->value()));
    for (DirectoryNode::ChildrenRange::iterator j (cs.begin()); j != cs.end(); ++j)
        completions.push_back(basePath + j->first 
                              + (j->second->followLink().isDirectory() ? "/" : " "));
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
