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
    \brief Editor public header */

#ifndef HH_SENF_Utils_Termlib_Editor_
#define HH_SENF_Utils_Termlib_Editor_ 1

// Custom includes
#include <map>
#include <vector>
#include <string>
#include <senf/Scheduler/ClockService.hh>
#include <senf/Scheduler/TimerEvent.hh>
#include "AbstractTerminal.hh"
#include "Terminfo.hh"

//#include "Editor.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace term {

    class BaseEditor
        : public AbstractTerminal::Callbacks
    {
    public:
        typedef KeyParser::keycode_t keycode_t;

        static unsigned const DEFAULT_KEY_TIMEOUT_MS = 500u;

        explicit BaseEditor(AbstractTerminal & terminal);

        void newline();                 ///< Move to beginning of a new, empty line
        void toColumn(unsigned c);      ///< Move cursor to column \p c
        void put(char ch);              ///< Write \p ch at current column
        void put(std::string const & text);
        void clearLine();               ///< Clear current line and move cursor to first column
        void setBold();                 ///< Set bold char display
        void setNormal();               ///< Set normal char display
        void maybeClrScr();             ///< Clear screen if possible

        unsigned currentColumn() const; ///< Return number of current column
        unsigned width();

    protected:
        virtual bool cb_init();
        virtual void cb_windowSizeChanged();

    private:
        virtual void v_keyReceived(keycode_t key) = 0;

        virtual void cb_charReceived(char c);

        void keySequenceTimeout();
        void processKeys();

        void write(char ch);
        void write(std::string const & s);
        
        AbstractTerminal * terminal_;
        Terminfo tifo_;
        KeyParser keyParser_;
        std::string inputBuffer_;
        ClockService::clock_type keyTimeout_;
        scheduler::TimerEvent timer_;
        unsigned column_;
    };

    class LineEditor
        : public BaseEditor
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (LineEditor&)> KeyBinding;
        typedef boost::function<void (std::string const &)> AcceptCallback;

        static unsigned const MAX_HISTORY_SIZE = 1024u;

        ///////////////////////////////////////////////////////////////////////////

        LineEditor(AbstractTerminal & terminal, AcceptCallback cb);
        
        ///////////////////////////////////////////////////////////////////////////

        void prompt(std::string const & text);
        void set(std::string const & text, unsigned pos = 0u);

        // Overall edit control
        void show();
        void hide();
        void accept();
        void clear();
        void redisplay();
        void forceRedisplay();
        
        // Cursor and display movement
        void gotoChar(unsigned n);
        void scrollTo(unsigned n);

        // Text manipulation
        void deleteChar(unsigned n=1);
        void insert(char ch);
        void insert(std::string const & text);

        // History
        void pushHistory(std::string const & text);
        void prevHistory();
        void nextHistory();

        // Get information
        std::string const & text();
        unsigned point();
        unsigned displayPos();
        keycode_t lastKey();

        // Key bindings
        void defineKey(keycode_t key, KeyBinding binding);
        void unsetKey(keycode_t key);
        
    private:
        virtual bool cb_init();
        virtual void cb_windowSizeChanged();
        virtual void v_keyReceived(keycode_t key);

        typedef std::map<keycode_t, KeyBinding> KeyMap;
        typedef std::vector<std::string> History;

        bool enabled_;
        bool redisplayNeeded_;
        std::string prompt_;
        unsigned promptWidth_;
        unsigned editWidth_;
        std::string text_;
        unsigned point_;
        unsigned displayPos_;
        keycode_t lastKey_;
        AcceptCallback callback_;
        KeyMap bindings_;
        History history_;
        unsigned historyPoint_;
    };

namespace bindings {

    void selfInsertCommand   (LineEditor & editor);
    void forwardChar         (LineEditor & editor);
    void backwardChar        (LineEditor & editor);
    void accept              (LineEditor & editor);
    void backwardDeleteChar  (LineEditor & editor);
    void deleteChar          (LineEditor & editor);
    void beginningOfLine     (LineEditor & editor);
    void endOfLine           (LineEditor & editor);
    void deleteToEndOfLine   (LineEditor & editor);
    void restartEdit         (LineEditor & editor);
    void prevHistory         (LineEditor & editor);
    void nextHistory         (LineEditor & editor);
    void clearScreen         (LineEditor & editor);

}

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Editor.cci"
//#include "Editor.ct"
//#include "Editor.cti"
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
