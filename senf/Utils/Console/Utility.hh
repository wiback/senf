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
    \brief Utility public header */

#ifndef HH_SENF_Utils_Console_Utility_
#define HH_SENF_Utils_Console_Utility_ 1

// Custom includes
#include "Parse.hh"
#include "Traits.hh"

//#include "Utility.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    /** \brief Value range
        
        A value range may be represented in the console either by a single value (setting both \a
        low and \a high to the same value) or as a lower and upper bound seperated by a colon
     */
    template <class T>
    struct ValueRange
    {
        T low;
        T high;
    };

#ifndef DOXYGEN

    template <class T>
    struct ArgumentTraits< ValueRange<T> >
    {
        typedef ValueRange<T> type;
        static bool const singleToken = true;

        static void parse(ParseCommandInfo::TokensRange const & tokens, type & out);
        static std::string description();
        static std::string str(type const & value);
    };

    template <class T>
    struct ReturnValueTraits< ValueRange<T> >
    {
        typedef ValueRange<T> type;

        static void format(type const & value, std::ostream & os);
    };

#endif

    /** \brief Bit-mask flag argument type

        senf::console::FlagCollection supplies a special argument type for use in registering
        console commands. This argument type is used to represent a bit-mask of single flags. 

        \code
        // Function taking a flags argument
        void func(unsigned flags);

        // Enum containing all the possible flag values
        enum MyFlags { Foo = 1,
                     Bar = 2,
                     Baz = 4,
                     Doo = 8 };
        SENF_CONSOLE_REGISTER_ENUM(MyFlags, (Foo)(Bar)(Baz)(Boo));
        
        // Register the function with a FlagCollection argument type
        consoleDir.add("func", boost::function<void (FlagCollection<MyFlags>)>(&func));
        \endcode

        To use the FlagCollection class
        \li you need a function which takes a bit-mask of flags as argument
        \li you define and register an enum with all possible flag values
        \li you register the function with a FlagCollection argument type using \c boost::function
            for the conversion. This is also possible for return values.

        The nice thing is, that \c boot::function supports compatible argument types and does
        automatic type conversion. Since a FlagCollection is convertible to and from unsigned long,
        this conversion will work. 

        After registering this function, you can call it with a collection of flags as argument

        <pre>
        console:/$ help func
        Usage:
            func arg11:MyFlags
        console:/$ func Foo
        console:/$ func (Foo Boo)
        </pre>
     */
    template <class Enum>
    struct FlagCollection
    {
        operator unsigned long() const { return value; }
        FlagCollection() : value (0) {}
        FlagCollection(unsigned long value_) : value (value_) {}
        FlagCollection(Enum value_) : value (value_) {}
        unsigned long value;
    };

#ifndef DOXYGEN

    template <class Enum>
    struct ArgumentTraits< FlagCollection<Enum> >
    {
        typedef FlagCollection<Enum> type;
        static bool const singleToken = false;
        static void parse(ParseCommandInfo::TokensRange const & tokens, type & out);
        static std::string description();
        static std::string str(type const & value);
    };

    template <class Enum>
    struct ReturnValueTraits< FlagCollection<Enum> >
    {
        typedef FlagCollection<Enum> type;
        static void format(type const & value, std::ostream & os);
    };

#endif

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Utility.cci"
#include "Utility.ct"
//#include "Utility.cti"
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
