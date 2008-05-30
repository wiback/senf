// $Id$
//
// Copyright (C) 2008 
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
    \brief Readline non-inline non-template implementation */

#include "Readline.hh"
//#include "Readline.ih"

// Custom includes
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "../Utils/membind.hh"

//#include "Readline.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

//
// Readline integration is a bit awkward. There are several things to it:
//
//  - Readline uses global variables for all state. Therefore, we can use readline only for one
//    console.
//  - We need to make readline to read from the socket handle instead of some input stream. We can
//    do this by setting a custom rl_getc_function.
//  - We need to make readline to write to the NonblockingSocketOStream. This is possible in glibc
//    by using a 'cookie stream'.
//  - We need to correctly handle the terminal mode settings. Currently we unconditionally
//    initialize the remote telnet by sending a fixed telnet option string and ignoring any otpions
//    sent back to us.
//  - We need to make sure, readline never uses stderr -> we must disable beeping
//  - There are places, where readline calls read_key unconditionally even when NOT prompted by the
//    callback that another key is available. One such place is completion. (The 'show all
//    completions (y/n)?' question). For now, we disable completion support.
//

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::ReadlineClientReader

extern "C" {
    extern int readline_echoing_p;
    extern int _rl_bell_preference;

    void _rl_erase_entire_line();
}


namespace {

    int readline_getc_function(FILE *)
    {
        if (senf::console::detail::ReadlineClientReader::active())
            return senf::console::detail::ReadlineClientReader::instance().getc();
        else
            return -1;
    }

    void readline_callback(char * input)
    {
        if (senf::console::detail::ReadlineClientReader::active()) {
            if (input)
                return senf::console::detail::ReadlineClientReader::instance().callback(
                    std::string(input) );
            else // input == 0 -> EOF (or Ctrl-D)
                senf::console::detail::ReadlineClientReader::instance().eof();
        }
    }

    ssize_t readline_cookie_write_function(void * cookie, char const * buffer, size_t size)
    {
        if (senf::console::detail::ReadlineClientReader::active() && buffer)
            senf::console::detail::ReadlineClientReader::instance().write(
                std::string(buffer, size));
        return size;
    }

    void readline_prep_term(int meta)
    {
        readline_echoing_p = 1;
    }

    void readline_deprep_term()
    {}

    int restart_line(int count, int key)
    {
        rl_kill_full_line(count, key);
        rl_crlf();
        rl_forced_update_display();
        return 0;
    }

}

prefix_ senf::console::detail::ReadlineClientReader::ReadlineClientReader(Client & client)
    : ClientReader(client), ch_ (-1), skipChars_ (0), 
      schedBinding_ ( client.handle(), 
                      senf::membind(&ReadlineClientReader::charEvent, this),
                      Scheduler::EV_READ, 
                      false ),
      terminate_ (false)
{
    if (instance_ != 0)
        throw DuplicateReaderException();
    instance_ = this;

    cookie_io_functions_t cookie_io = { 0, &readline_cookie_write_function, 0, 0 };
    rl_outstream = fopencookie(0, "a", cookie_io);
    if (rl_outstream == 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    if (setvbuf(rl_outstream, 0, _IONBF, BUFSIZ) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    rl_instream = rl_outstream;
    rl_terminal_name = "vt100";
    strncpy(nameBuffer_, client.name().c_str(), 128);
    nameBuffer_[127] = 0;
    rl_readline_name = nameBuffer_;
    rl_prep_term_function = &readline_prep_term;
    rl_deprep_term_function = &readline_deprep_term;
    rl_getc_function = &readline_getc_function;
    rl_bind_key('\t', &rl_insert);
    rl_bind_key('\x03', &restart_line);
    using_history();
    
    // Don't ask me, where I found this ...
    static char options[] = { 0xFF, 0xFB, 0x01, // IAC WILL ECHO
                              0xFF, 0xFE, 0x22, // IAC DONT LINEMODE
                              0xFF, 0xFB, 0x03, // IAC WILL SGA
                              0x00 };
    handle().write(options, options+sizeof(options));
    handle().write(std::string("(readline support enabled)\r\n"));

    strncpy(promptBuffer_, promptString().c_str(), 1024);
    promptBuffer_[1023] = 0;
    rl_callback_handler_install(promptBuffer_, &readline_callback);

    _rl_bell_preference = 0; // Set this *after* the config file has been read

    schedBinding_.enable();
}

prefix_ senf::console::detail::ReadlineClientReader::~ReadlineClientReader()
{
    rl_callback_handler_remove();
    fclose(rl_outstream);
    rl_outstream = 0;
    rl_instream = 0;
    instance_ = 0;
}

prefix_ void senf::console::detail::ReadlineClientReader::callback(std::string line)
{
    boost::trim(line);
    if (!line.empty())
        add_history(line.c_str());
    handleInput(line);
    stream() << std::flush;
    strncpy(promptBuffer_, promptString().c_str(), 1024);
    promptBuffer_[1023] = 0;
    rl_set_prompt(promptBuffer_);
}

prefix_ void senf::console::detail::ReadlineClientReader::v_disablePrompt()
{
    _rl_erase_entire_line();
}

prefix_ void senf::console::detail::ReadlineClientReader::v_enablePrompt()
{
    rl_forced_update_display();
}

prefix_ void senf::console::detail::ReadlineClientReader::v_translate(std::string & data)
{
    boost::replace_all(data, "\n", "\n\r");
    boost::replace_all(data, "\xff", "\xff\xff");
}

prefix_ void senf::console::detail::ReadlineClientReader::charEvent(Scheduler::EventId event)
{
    char ch;
    if (event != Scheduler::EV_READ || handle().read(&ch, &ch+1) <= &ch) {
        stopClient();
        return;
    }
    ch_ = static_cast<unsigned char>(ch);

    if (skipChars_ > 0)
        --skipChars_;
    else if (ch_ == 0xff)
        skipChars_ = 2;
    else if (ch_ != 0)
        rl_callback_read_char();

    if (terminate_)
        stopClient();
}

senf::console::detail::ReadlineClientReader * 
    senf::console::detail::ReadlineClientReader::instance_ (0);

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::SafeReadlineClientReader

prefix_ void senf::console::detail::SafeReadlineClientReader::v_disablePrompt()
{
    reader_->disablePrompt();
}

prefix_ void senf::console::detail::SafeReadlineClientReader::v_enablePrompt()
{
    reader_->enablePrompt();
}

prefix_ void senf::console::detail::SafeReadlineClientReader::v_translate(std::string & data)
{
    reader_->translate(data);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Readline.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
