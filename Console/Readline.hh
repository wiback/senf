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
    \brief Readline public header */

#ifndef HH_Readline_
#define HH_Readline_ 1

// Custom includes
#include <boost/scoped_ptr.hpp>
#include "Server.hh"
#include "../Utils/Exception.hh"
#include "../Scheduler/Scheduler.hh"

//#include "Readline.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {
namespace detail {

#ifndef DOXYGEN

    /** \brief Internal: GNU readline based ClientReader implementation
        
        This implementation of the ClientReader interface uses GNU readline library to provide a
        rich editing environment for console sessions. Since an application can only use readline
        once, only one ReadlineReader can be allocated at any time.
     */
    class ReadlineClientReader
        : public ClientReader
    {
    public:
        ReadlineClientReader(Client & client);
        ~ReadlineClientReader();

        static bool active();
        static ReadlineClientReader & instance();

        struct DuplicateReaderException : public Exception
        { DuplicateReaderException() : Exception("duplicate readline instantiation") {} };

        int getc();
        void callback(std::string line);
        void write(std::string text);
        void terminate();
        void eof();
        
    private:
        virtual void v_disablePrompt();
        virtual void v_enablePrompt();
        virtual void v_translate(std::string & data);

        void charEvent(int event);

        static ReadlineClientReader * instance_;
        int ch_;
        unsigned skipChars_;
        char nameBuffer_[256];
        char promptBuffer_[1024];
        scheduler::FdEvent readevent_;
        bool terminate_;

        char * savedLineBuffer_;
        int savedPoint_;
        int savedEnd_;
        int savedMark_;
    };

    /** \brief Internal: Safe GNU readline based ClientReader implementation
        
        This implementation of the ClientReader interface forwards all functionality to either
        ReadlineClientReader or DumbClientReader. A RadlineClientReader is used, if none is active,
        otherwise a DumbClientReader is allocated. Using this ClientReader implementation, the first
        console client will have readline functionality enabled, all further will have it disabled.
     */
    class SafeReadlineClientReader
        : public ClientReader
    {
    public:
        SafeReadlineClientReader(Client & client);

    private:
        virtual void v_disablePrompt();
        virtual void v_enablePrompt();
        virtual void v_translate(std::string & data);

        boost::scoped_ptr<ClientReader> reader_;
    };

#endif

}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Readline.cci"
//#include "Readline.ct"
//#include "Readline.cti"
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
