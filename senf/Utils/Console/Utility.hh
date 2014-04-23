// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//   Stefan Bund <senf@g0dil.de>

/** \file
    \brief Utility public header */

#ifndef HH_SENF_Utils_Console_Utility_
#define HH_SENF_Utils_Console_Utility_ 1

// Custom includes
#include "Parse.hh"

//#include "Utility.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

/** \defgroup senf_console_utilities Special console utility types

    These types are provided by the %console library to support special argument or return value
    formatting rules.
 */

namespace senf {
namespace console {

    /** \brief Parse character value as single-char string

        This helper types is a wrapper around \a CharT which must be a character type. A value of
        this type will be represented as a single character string, not as numeric value (the
        default interpretation of char arguments in the console library).

        This wrapper is used via type conversion using \c boost::function to convert the
        argument/return type \a CharT  to CharAsString<CharT>:
        \code
        char foo(char v) { return v; }

        senf::console::root().add(
            "foo", fty::Command< CharAsString<char> (CharAsString<char>) >(&foo));
        \endcode

        \tparam CharT character type, one of \c char, \c signed \c char or \c unsigned \c char

        \ingroup senf_console_utilities
     */
    template <class CharT>
    struct CharAsString
    {
        CharAsString();
        CharAsString(CharT value_);
        operator CharT () const;
        CharT value;
    };

#ifndef DOXYGEN

    template <class CharT>
    struct ArgumentTraits< CharAsString<CharT> >
    {
        typedef CharAsString<CharT> type;
        static bool const singleToken = true;

        static void parse(ParseCommandInfo::TokensRange const & tokens, CharAsString<CharT> & out);
        static std::string description();
        static std::string str(CharAsString<CharT> value);
    };

    template <class CharT>
    struct ReturnValueTraits< CharAsString<CharT> >
    {
        typedef CharAsString<CharT> type;

        static void format(CharAsString<CharT> value, std::ostream & os);
    };

#endif

    /** \brief Value range

        A value range may be represented in the console either by a single value (setting both \a
        lower and \a upper to the same value) or as a lower and upper bound separated by a colon.

        \ingroup senf_console_utilities
     */
    template <class T>
    struct ValueRange
    {
        T lower;
        T upper;
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
        consoleDir.add("func", fty::Command<void (FlagCollection<MyFlags>)>(&func));
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

        \ingroup senf_console_utilities
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Utility.cci"
#include "Utility.ct"
#include "Utility.cti"
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
