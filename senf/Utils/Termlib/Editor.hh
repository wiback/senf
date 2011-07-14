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
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace term {

    /** \brief Provide editor support terminal functionality

        This base class utilizes an arbitrary AbstractTerminal and provides terminfo based
        terminal navigation and key parsing.

        All navigation is relative to the current line. The display area can then be extended
        downwards up to a maximum of height() lines. Resetting this extended display area will
        return to a one line area containing only the top line.

        All navigation is restricted to a width() x height() area. The last column may not be
        written to since auto-margin terminals will move the cursor to the next line when writing to
        that column.

        This base class calls v_keyReceived() which must be defined in a derived class whenever a
        key is fully parsed.
     */
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
        void put(std::string const & text); ///< Write \a text starting at current column
        void clearLine();               ///< Clear current line and move cursor to first column
        void setBold();                 ///< Set bold char display
        void setNormal();               ///< Set normal char display
        void maybeClrScr();             ///< Clear screen if possible

        void toLine(unsigned l);        ///< Move to relative display line \a l
        void reset();                   ///< Reset display area to single line

        unsigned currentColumn() const; ///< Return number of current column
        unsigned currentLine() const;   ///< Return number of current relative line
        unsigned width() const;         ///< Return current screen width
        unsigned height() const;        ///< Return current screen height

    protected:
        virtual bool cb_init();         ///< Called when terminal is initialized
        virtual void cb_windowSizeChanged();
                                        ///< Called whenever the terminal window size changes

#ifndef DOXYGEN
    private:
#endif
        virtual void v_keyReceived(keycode_t key) = 0; ///< Called whenever a key is received

    private:
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
        unsigned displayHeight_;
        unsigned line_;
    };

    /** \brief Single line interactive text editor

        LineEditor implements a single-line input widget on an arbitrary AbstractTerminal.

        \li It supports all the customary editing functions
        \li It is possible to arbitrarily assign functions to keys via a key map
        \li LineEditor has builtin TAB completion support
        \li The LineEditor has a built-in history
        \li The LineEditor supports an arbitrary auxiliary display area below the input line
        \li The LineEditor has hide() / show() support to allow editing to be temporarily
            interrupted.

        The LineEditor will query the user for an input line. When the user accepts a line,
        LineEditor will call a user callback function. After the callback has been called, the
        editor is disabled. To accept a new input line, call show().

        \section editor_keymap The editor key map

        Keys are defined in the keymap using defineKey(). The default bindings are:

        <table class="senf">
        <tr><td>\c Return</td>    <td>bindings::accept</td></tr>
        <tr><td>\c Right</td>     <td>bindings::forwardChar</td></tr>
        <tr><td>\c Left</td>      <td>bindings::backwardChar</td></tr>
        <tr><td>\c Up</td>        <td>bindings::prevHistory</td></tr>
        <tr><td>\c Down</td>      <td>bindings::nextHistory</td></tr>
        <tr><td>\c Backspace</td> <td>bindings::backwardDeleteChar</td></tr>
        <tr><td>\c Delete</td>    <td>bindings::deleteChar</td></tr>
        <tr><td>\c Home</td>      <td>bindings::beginningOfLine</td></tr>
        <tr><td>\c End</td>       <td>bindings::endOfLine</td></tr>
        <tr><td>\c Ctrl-K</td>    <td>bindings::deleteToEndOfLine</td></tr>
        <tr><td>\c Ctrl-A</td>    <td>bindings::beginningOfLine</td></tr>
        <tr><td>\c Ctrl-E</td>    <td>bindings::endOfLine</td></tr>
        <tr><td>\c Ctrl-D</td>    <td>bindings::deleteChar</td></tr>
        <tr><td>\c Ctrl-C</td>    <td>bindings::restartEdit</td></tr>
        <tr><td>\c Ctrl-L</td>    <td>bindings::clearScreen</td></tr>
        </table>

        See the senf::term::bindings namespace for a list of all default provided key binding
        functions.


        \section editor_complete Completion support

        Completion support is provided by senf::term::bindings::complete(). To use the completer,
        you need to implement a completion function and pass it as second argument to
        bindings::complete():

        \code
        void myCompleter(senf::term::LineEditor & editor, unsigned & b, unsigned & e,
                         std::string & prefix, std::vector<std::string> & completions)
        {
            // Get text to complete
            std::string text (editor.text().substr(b, e-b));

            // Return possible completions in 'completions' array
            completions.push_back( ... );
        }

        senf::term::LineEditor editor (...);
        editor.defineKey(senf::term::KeyParser::TAB,
                         boost::bind(&senf::term::bindings::complete, _1, &myCompleter));
        \endcode

        When \c myCompleter is a class member, use senf::membind() and pass this instead of \c
        &myCompleter to \c boost::bind() and thus to senf::term::bindings::complete.

        The completion protocol is as follows: When completion is desired, the completer function is
        called. \a b and \a e are set to 0 and <tt>editor.point()</tt> respectively. \a prefix and
        \a completions are empty.

        \li the completer may restrict the to-be-completed string to any subrange by changing \a b
            and \a e accordingly.
        \li If there is an initial substring which applies to \e all completions but should not be
            listed in the list of completions, assign this value to \a prefix.
        \li Add all possible completions to the \a completions vector not including the \a prefix.
        \li The completion result is taken from the size of the \a completions vector \e only: If
            this vector is empty, completion failed (even if \a prefix is set), a single entry in \a
            completions (even if it is the empty string) signals a unique completion.


        \section editor_auxarea The aux display area

        The auxiliary display area is accessed using auxDisplay() and clearAuxDisplay(). The aux
        display area is \e cleared \e before each new key is processed. Therefore it is only
        temporary. The aux display area however will survive hide() / show().


        \section editor_hideshow Temporarily disabling the editor

        Calling hide() will temporarily disable the editor. All editor display will be
        removed. Calling show() will redisplay the editor in it's current state including the aux
        display area.
     */
    class LineEditor
        : public BaseEditor
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (LineEditor&)> KeyBinding; ///< Type of a key binding function
        typedef boost::function<void (std::string const &)> AcceptCallback;
                                        ///< Callback function type

        static unsigned const MAX_HISTORY_SIZE = 1024u;

        //-////////////////////////////////////////////////////////////////////////

        LineEditor(AbstractTerminal & terminal, AcceptCallback cb);
                                        ///< Create a LineEditor
                                        /**< \param[in] terminal abstract terminal interface
                                             \param[in] cb callback to call for complete input
                                                 line */

        //-////////////////////////////////////////////////////////////////////////

        ///\name Overall edit control
        //\{

        void show();                    ///< Enable editor widget
        void hide();                    ///< Disable editor widget
        void accept();                  ///< Accept current user input and call the accept callback
        void clear();                   ///< Clear editor buffer
        void redisplay();               ///< Mark the editor buffer for redisplay
        void forceRedisplay();          ///< Redisplay the editor buffer \e now
        void prompt(std::string const & text); ///< Set prompt string

        //\}

        ///\name Cursor and display movement
        //\{

        void gotoChar(unsigned n);      ///< Move cursor to position \a n
        void scrollTo(unsigned n);      ///< Move position \n to beginning of display line

        //\}

        ///\name Text manipulation
        //\{

        void deleteChar(unsigned n=1);  ///< Delete \a n characters at point
        void insert(char ch);           ///< Insert \a ch at point
        void insert(std::string const & text); ///< Insert \a text at point
        void set(std::string const & text, unsigned pos = 0u);
                                        ///< Set edit buffer contents
                                        /**< The edit buffer contents will be replaced by \a
                                             text. The cursor will be placed at position \a pos
                                             within this text. */

        //\}

        ///\name History
        //\{

        void pushHistory(std::string const & text, bool accept = false);
                                        ///< Add string \a text to history
        void prevHistory();             ///< Switch to previous history entry
        void nextHistory();             ///< Switch to next history entry

        //\}

        ///\name Aux Display
        //\{

        void auxDisplay(unsigned line, std::string const & text);
                                        ///< Display \a text on aux display line \a line
        unsigned maxAuxDisplayHeight(); ///< Get maximum height of the aux display area
        void clearAuxDisplay();         ///< Clear the aux display area

        //\}

        ///\name Get information
        //\{

        std::string const & text();     ///< Get current editor buffer contents
        unsigned point();               ///< Get current cursor position
        unsigned displayPos();          ///< Get current display position
        keycode_t lastKey();            ///< Get last key code received

        //\}

        ///\name Key bindings
        //\{

        void defineKey(keycode_t key, KeyBinding binding);
                                        ///< Bind key \a key to \a binding
        void unsetKey(keycode_t key);   ///< Remove all bindings for \a key

        //\}

    private:
        virtual bool cb_init();
        virtual void cb_windowSizeChanged();
        virtual void v_keyReceived(keycode_t key);

        typedef std::map<keycode_t, KeyBinding> KeyMap;
        typedef std::vector<std::string> History;
        typedef std::vector<std::string> AuxDisplay;

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
        AuxDisplay auxDisplay_;
    };

/** \brief LineEditor key bindings
 */
namespace bindings {

    void selfInsertCommand   (LineEditor & editor); ///< Insert key as literal character
    void forwardChar         (LineEditor & editor); ///< Move one char forward
    void backwardChar        (LineEditor & editor); ///< Move one char backwards
    void accept              (LineEditor & editor); ///< Accept input line
    void acceptWithRepeat    (LineEditor & editor); ///< Accept, possibly repeat last history entry
    void backwardDeleteChar  (LineEditor & editor); ///< Delete char before cursor
    void deleteChar          (LineEditor & editor); ///< Delete char at cursor
    void beginningOfLine     (LineEditor & editor); ///< Move to beginning of line
    void endOfLine           (LineEditor & editor); ///< Move to end of line
    void deleteToEndOfLine   (LineEditor & editor); ///< Delete from cursor to end of line
    void restartEdit         (LineEditor & editor); ///< Clear edit buffer and restart edit
    void prevHistory         (LineEditor & editor); ///< Move to previous history entry
    void nextHistory         (LineEditor & editor); ///< Move to next history entry
    void clearScreen         (LineEditor & editor); ///< Clear screen and redisplay editor

    typedef boost::function<void (LineEditor &, unsigned & b, unsigned & e,
                                  std::string & prefix, std::vector<std::string> &)> Completer;
    void complete            (LineEditor & editor, Completer completer);
                                        ///< Complete text at cursor
                                        /**< This function calls \a completer to find the list of
                                             possible completions for the text between \a b and \a e
                                             (as passed to the completer). The completer must add
                                             all possible completions to the \a completions vector.

                                             \see \ref editor_complete */

}

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
