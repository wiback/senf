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
    \brief Editor non-inline non-template implementation */

#include "Editor.hh"
//#include "Editor.ih"

// Custom includes
#include <senf/Utils/membind.hh>
#include <senf/Scheduler/Scheduler.hh>

//#include "Editor.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::term::BaseEditor::BaseEditor(AbstractTerminal & terminal)
    : terminal_ (&terminal),
      keyTimeout_ (senf::ClockService::milliseconds(DEFAULT_KEY_TIMEOUT_MS)),
      timer_ ("senf::term::BaseEditor::keySequenceTimeout", 
              senf::membind(&BaseEditor::keySequenceTimeout, this)),
      column_ (0u)
{
    terminal_->setCallbacks(*this);
}

prefix_ void senf::term::BaseEditor::newline()
{
    write("\r\n");
    write(tifo_.getString(Terminfo::properties::ClrEol));
    column_ = 0;
}

prefix_ void senf::term::BaseEditor::toColumn(unsigned c)
{
    if (c >= width())
        c = width();
    if (c > column_) {
        if (tifo_.hasProperty(Terminfo::properties::ParmRightCursor)) {
            write(tifo_.formatString(Terminfo::properties::ParmRightCursor, c - column_));
            column_ = c;
        }
        else {
            char const * cuf1 (tifo_.getString(Terminfo::properties::CursorRight));
            while (c > column_) {
                write(cuf1);
                ++column_;
            }
        }
    }
    else if (c < column_) {
        if (tifo_.hasProperty(Terminfo::properties::ParmLeftCursor)) {
            write(tifo_.formatString(Terminfo::properties::ParmLeftCursor, column_ - c));
            column_ = c;
        }
        else {
            char const * cub1 (tifo_.getString(Terminfo::properties::CursorRight));
            while (c < column_) {
                write(cub1);
                --column_;
            }
        }
    }
}

prefix_ void senf::term::BaseEditor::insertChar(char ch)
{
    if (column_+1 >= width())
        return;
    if (tifo_.hasProperty(Terminfo::properties::InsertCharacter))
        write(tifo_.getString(Terminfo::properties::InsertCharacter));
    else
        write(tifo_.formatString(Terminfo::properties::ParmIch, 1));
    write(ch);
    ++ column_;
}

prefix_ void senf::term::BaseEditor::overwriteChar(char ch)
{
    write(ch);
    ++ column_;
}

prefix_ void senf::term::BaseEditor::deleteChar()
{
    if (tifo_.hasProperty(Terminfo::properties::DeleteCharacter))
        write(tifo_.getString(Terminfo::properties::DeleteCharacter));
    else
        write(tifo_.formatString(Terminfo::properties::ParmDch, 1));
}

prefix_ unsigned senf::term::BaseEditor::currentColumn()
    const
{
    return column_;
}

prefix_ void senf::term::BaseEditor::cb_init()
{
    tifo_.load(terminal_->terminalType());
    keyParser_.load(tifo_);
    if (tifo_.hasProperty(Terminfo::properties::KeypadXmit))
        write(tifo_.getString(Terminfo::properties::KeypadXmit));
}

prefix_ void senf::term::BaseEditor::cb_charReceived(char c)
{
    inputBuffer_ += c;
    timer_.timeout(senf::scheduler::eventTime() + keyTimeout_);
    processKeys();
}

prefix_ void senf::term::BaseEditor::cb_windowSizeChanged()
{
    if (column_ >= width())
        column_ = width()-1;
}

prefix_ void senf::term::BaseEditor::keySequenceTimeout()
{
    while (!inputBuffer_.empty()) {
        processKeys();
        v_keyReceived(keycode_t(inputBuffer_[0]));
        inputBuffer_.erase(0, 1);
    }
}

prefix_ void senf::term::BaseEditor::processKeys()
{
    do {
        std::pair<senf::term::KeyParser::keycode_t, std::string::size_type> result
            (keyParser_.lookup(inputBuffer_));
        if (result.first == senf::term::KeyParser::Incomplete)
            return;
        v_keyReceived(result.first);
        inputBuffer_.erase(0, result.second);
    } while (! inputBuffer_.empty());
    timer_.disable();
}

prefix_ unsigned senf::term::BaseEditor::width()
{
    return terminal_->width();
}

prefix_ void senf::term::BaseEditor::write(char ch)
{
    terminal_->write(ch);
}

prefix_ void senf::term::BaseEditor::write(std::string const & s)
{
    for (std::string::const_iterator i (s.begin()); i != s.end(); ++i)
        write(*i);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Editor.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
