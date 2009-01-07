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
            char const * cub1 (tifo_.getString(Terminfo::properties::CursorLeft));
            while (c < column_) {
                write(cub1);
                --column_;
            }
        }
    }
}

prefix_ void senf::term::BaseEditor::put(char ch)
{
    if (column_ >= width()-1)
        return;
    write(ch);
    ++ column_;
}

prefix_ void senf::term::BaseEditor::put(std::string const & text)
{
    if (text.size() > width()-column_-1) {
        write(text.substr(0,width()-column_-1));
        column_ = width() - 1;
    }
    else {
        write(text);
        column_ += text.size();
    }
}

prefix_ void senf::term::BaseEditor::clearLine()
{
    write("\r");
    write(tifo_.getString(Terminfo::properties::ClrEol));
    column_ = 0;
}

prefix_ void senf::term::BaseEditor::setBold()
{
    if (tifo_.hasProperty(Terminfo::properties::EnterBoldMode) &&
        tifo_.hasProperty(Terminfo::properties::ExitAttributeMode))
        write(tifo_.getString(Terminfo::properties::EnterBoldMode));
}

prefix_ void senf::term::BaseEditor::setNormal()
{
    if (tifo_.hasProperty(Terminfo::properties::EnterBoldMode) &&
        tifo_.hasProperty(Terminfo::properties::ExitAttributeMode))
        write(tifo_.getString(Terminfo::properties::ExitAttributeMode));
}

prefix_ unsigned senf::term::BaseEditor::currentColumn()
   const
{
    return column_;
}

prefix_ bool senf::term::BaseEditor::cb_init()
{
    try {
        tifo_.load(terminal_->terminalType());
        keyParser_.load(tifo_);
    }
    catch (Terminfo::InvalidTerminfoException & ex) {
        return false;
    }

    typedef Terminfo::properties p;
    if (! (tifo_.hasProperty(p::ClrEol) &&
           (tifo_.hasProperty(p::ParmRightCursor) || tifo_.hasProperty(p::CursorRight)) &&
           (tifo_.hasProperty(p::ParmLeftCursor) || tifo_.hasProperty(p::CursorLeft))))
        return false;

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

///////////////////////////////////////////////////////////////////////////

prefix_ senf::term::LineEditor::LineEditor(AbstractTerminal & terminal, AcceptCallback cb)
    : BaseEditor(terminal), enabled_ (true), prompt_ ("$"), promptWidth_ (1u), editWidth_ (0u), 
      text_ (""), point_ (0u), displayPos_ (0u), lastKey_ (0u), callback_ (cb)
{
    defineKey(KeyParser::Return,    &bindings::accept);
    defineKey(KeyParser::Right,     &bindings::forwardChar);
    defineKey(KeyParser::Left,      &bindings::backwardChar);
    defineKey(KeyParser::Backspace, &bindings::backwardDeleteChar);
    defineKey(KeyParser::Delete,    &bindings::deleteChar);
    defineKey(KeyParser::Home,      &bindings::beginningOfLine);
    defineKey(KeyParser::End,       &bindings::endOfLine);
    defineKey(KeyParser::Ctrl('K'), &bindings::deleteToEndOfLine);
    defineKey(KeyParser::Ctrl('A'), &bindings::beginningOfLine);
    defineKey(KeyParser::Ctrl('E'), &bindings::endOfLine);
    defineKey(KeyParser::Ctrl('D'), &bindings::deleteChar);
    defineKey(KeyParser::Ctrl('C'), &bindings::restartEdit);
}

prefix_ void senf::term::LineEditor::prompt(std::string const & text)
{
    prompt_ = text;
    promptWidth_ = prompt_.size();
    if (promptWidth_ > width() - 4 && width() > 4)
        promptWidth_ = width() - 4;
    editWidth_ = width() - promptWidth_ - 3;
    if (enabled_)
        redisplay();
}

prefix_ void senf::term::LineEditor::set(std::string const & text, unsigned pos)
{
    text_ = text;
    point_ = pos;
    if (point_ > text.size())
        point_ = text.size();
    displayPos_ = 0u;
    if (point_ > editWidth_)
        displayPos_ = point_ - editWidth_;
    redisplay();
}

prefix_ void senf::term::LineEditor::show()
{
    if (enabled_)
        return;
    enabled_ = true;
    redisplay();
}

prefix_ void senf::term::LineEditor::hide()
{
    if (! enabled_)
        return;
    clearLine();
    enabled_ = false;
}

prefix_ void senf::term::LineEditor::accept()
{
    if (enabled_)
        newline();
    hide();
    callback_(text_);
    clear();
}

prefix_ void senf::term::LineEditor::clear()
{
    set("");
}

prefix_ void senf::term::LineEditor::redisplay()
{
    redisplayNeeded_ = true;
}

prefix_ void senf::term::LineEditor::forceRedisplay()
{
    if (! enabled_)
        return;
    clearLine();
    setBold();
    if (prompt_.size() > promptWidth_)
        put(prompt_.substr(prompt_.size()-promptWidth_));
    else
        put(prompt_);
    put( displayPos_ > 0 ? '<' : ' ' );
    if (text_.size() > displayPos_ + editWidth_) {
        toColumn(editWidth_ + promptWidth_ + 1);
        put('>');
        toColumn(promptWidth_ + 1);
    }
    setNormal();
    put(text_.substr(displayPos_, editWidth_));
    toColumn(point_ - displayPos_ + promptWidth_ + 1);
    redisplayNeeded_ = false;
}

prefix_ void senf::term::LineEditor::gotoChar(unsigned n)
{
    point_ = n;
    if (point_ > text_.size())
        point_ = text_.size();
    if (point_ < displayPos_)
        displayPos_ = point_;
    if (point_ > displayPos_+editWidth_)
        displayPos_ = point_-editWidth_;
    redisplay();
}

prefix_ void senf::term::LineEditor::scrollTo(unsigned n)
{
    displayPos_ = n;
    if (displayPos_ > text_.size())
        displayPos_ = text_.size();
    if (point_ < displayPos_)
        point_ = displayPos_;
    if (point_ > displayPos_+editWidth_)
        point_ = displayPos_+editWidth_;
    redisplay();
}

prefix_ void senf::term::LineEditor::deleteChar(unsigned n)
{
    if (point_ >= text_.size())
        return;
    text_.erase(point_, n);
    redisplay();
}

prefix_ void senf::term::LineEditor::insert(char ch)
{
    text_.insert(point_, std::string(1, ch));
    gotoChar(point_+1);
    redisplay();
}

prefix_ void senf::term::LineEditor::insert(std::string const & text)
{
    text_.insert(point_, text);
    gotoChar(point_+text.size());
    redisplay();
}

prefix_ std::string const & senf::term::LineEditor::text()
{
    return text_;
}

prefix_ unsigned senf::term::LineEditor::point()
{
    return point_;
}

prefix_ unsigned senf::term::LineEditor::displayPos()
{
    return displayPos_;
}

prefix_ senf::term::LineEditor::keycode_t senf::term::LineEditor::lastKey()
{
    return lastKey_;
}

prefix_ void senf::term::LineEditor::defineKey(keycode_t key, KeyBinding binding)
{
    bindings_[key] = binding;
}

prefix_ void senf::term::LineEditor::unsetKey(keycode_t key)
{
    bindings_.erase(key);
}

prefix_ bool senf::term::LineEditor::cb_init()
{
    if (!BaseEditor::cb_init())
        return false;
    prompt(prompt_);
    forceRedisplay();
    return true;
}

prefix_ void senf::term::LineEditor::cb_windowSizeChanged()
{
    BaseEditor::cb_windowSizeChanged();
    prompt(prompt_);
    gotoChar(point_);
    forceRedisplay();
}

prefix_ void senf::term::LineEditor::v_keyReceived(keycode_t key)
{
    if (! enabled_)
        return;
    lastKey_ = key;
    KeyMap::iterator i (bindings_.find(key));
    if (i != bindings_.end())
        i->second(*this);
    else if (key >= ' ' && key < 256)
        insert(char(key));
    if (redisplayNeeded_)
        forceRedisplay();
}

///////////////////////////////////////////////////////////////////////////

prefix_ void senf::term::bindings::selfInsertCommand(LineEditor & editor)
{
    LineEditor::keycode_t key (editor.lastKey());
    if (key >= ' ' && key < 256)
        editor.insert(key);
}

prefix_ void senf::term::bindings::forwardChar(LineEditor & editor)
{
    editor.gotoChar(editor.point()+1);
}

prefix_ void senf::term::bindings::backwardChar(LineEditor & editor)
{
    unsigned p (editor.point());
    if (p>0)
        editor.gotoChar(p-1);
}

prefix_ void senf::term::bindings::accept(LineEditor & editor)
{
    editor.accept();
}

prefix_ void senf::term::bindings::backwardDeleteChar(LineEditor & editor)
{
    unsigned p (editor.point());
    if (p>0) {
        editor.gotoChar(p-1);
        editor.deleteChar();
    }
}

prefix_ void senf::term::bindings::deleteChar(LineEditor & editor)
{
    editor.deleteChar();
}

prefix_ void senf::term::bindings::beginningOfLine(LineEditor & editor)
{
    editor.gotoChar(0u);
}

prefix_ void senf::term::bindings::endOfLine(LineEditor & editor)
{
    editor.gotoChar(editor.text().size());
}

prefix_ void senf::term::bindings::deleteToEndOfLine(LineEditor & editor)
{
    editor.deleteChar(editor.text().size()-editor.point());
}

prefix_ void senf::term::bindings::restartEdit(LineEditor & editor)
{
    editor.newline();
    editor.clear();
    editor.redisplay();
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
