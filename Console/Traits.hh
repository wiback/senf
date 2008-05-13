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
    \brief Traits public header */

#ifndef HH_Traits_
#define HH_Traits_ 1

// Custom includes
#include <iostream>
#include <boost/intrusive_ptr.hpp>
#include "../Utils/intrusive_refcount.hh"
#include "Parse.hh"
#include "Node.hh"

#include "Traits.ih"
//#include "Traits.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    /** \brief Customize return value formating

        ReturnValueTraits provides return value formatting. The default implementation provided here
        will forward the call directly to senf_console_format_value(). The default implementation of
        that function will write the \a value to \a os using standard iostream formatting.

        To customize this behavior for some type, either provide an implementation of
        senf_console_format_value() in the types namespace or provide a specialization of
        ReturnValueTraits.

        The output should \e not end in a newline since one is added automatically.
     */
    template <class Type>
    struct ReturnValueTraits
    {
        typedef Type type;

        static void format(Type const & value, std::ostream & os);
                                        ///< Write \a value to \a os
    };

    /** \brief Return value formatter

        \see ReturnValuetraits

        \related ReturnValueTraits
     */
    template <class Type>
    void senf_console_format_value(Type const & value, std::ostream & os);

    /** \brief Customize argument parsing
        
        ArgumentTraits provides argument parsing, Additionally, this class provides a way to get a
        string-description of a type and to convert a value back into it's string representation
        used to display default values.
        
        The default implementation provided here 
        \li will use senf_console_parse_argument() to parse a value. This functions default
            implementation uses \c boost::lexical_cast and thereby \c iostreams to convert an
            argument consisting of a single input token into the required type.
        \li will name types by returning the last component of the fully scoped name (e.g. \c
            "string" for \c std::string). 
        \li Will format values (for default value display) by forwarding the value to the
            ReturnValueTraits of that type.

        To customize just the argument parsing, just provide an implementation of
        senf_console_parse_argument(). Alternatively or to customize type naming or default value
        formatting, specialize ArgumentTraits  for the type.
     */
    template <class Type>
    struct ArgumentTraits
    {
        typedef Type type;

        static void parse(ParseCommandInfo::TokensRange const & tokens, Type & out);
                                        ///< Parse token range into value
                                        /**< This function needs to parse \a tokens and write the
                                             parsed value into \a out. This function needs to parse
                                             the \e complete list of tokens, additional tokens must
                                             be considered as syntax error.
                                             \throws SyntaxErrorException
                                             \param[in] tokens tokens to parse
                                             \param[out] out parsed value */

        static std::string description(); ///< String description of type
                                        /**< Returns the string description of \a Type. Used to
                                             generate online help. */
        static std::string str(Type const & value); ///< Stringify value
                                        /**< To show default values in the online help, this
                                             function converts a value back into a one-line string
                                             representation. The default implementation uses the
                                             ReturnValueTraits for this conversion. */
    };

    /** \brief Argument parser

        \see ArgumentTraits

        \related ArgumentTraits
     */
    template <class Type>
    void senf_console_parse_argument(ParseCommandInfo::TokensRange const & tokens, Type & out);

    /** \brief Parse token range

        This helper will invoke the correct ArgumentTraits::parse function to parse the input tokens
        into the passed in variable.

        \see ArgumentTraits
     */
    template <class Type>
    void parse(ParseCommandInfo::TokensRange const & tokens, Type & out);

    /** \brief Register enum type for argument parsing

        Enum types need to be registered explicitly to support parsing. 
        \code
        enum Foo { Foo1, Foo2 };
        SENF_CONSOLE_REGISTER_ENUM( Foo, (Foo1)(Foo2) );
        \endcode
        This macro will register an enum type and it's enumerators defined at namespace scope. See
        \ref SENF_CONSOLE_REGISTER_ENUM_MEMBER to register a member enum type.

        \ingroup console_commands
     */
#   define SENF_CONSOLE_REGISTER_ENUM(Type, Values) \
        SENF_CONSOLE_REGISTER_ENUM_(BOOST_PP_EMPTY(), Type, Values)

    /** \brief Register enum type for argument parsing

        Enum types need to be registered explicitly to support parsing. 
        \code
        class SomeClass
        {
            enum Foo { Foo1, Foo2 };
        };

        SENF_CONSOLE_REGISTER_ENUM_MEMBER( SomeClass, Foo, (Foo1)(Foo2) );
        \endcode This macro will register an enum type and it's enumerators defined in a class. See
        \ref SENF_CONSOLE_REGISTER_ENUM to register an enum type declared at namespace scope.

        \ingroup console_commands
     */
#   define SENF_CONSOLE_REGISTER_ENUM_MEMBER(Class, Type, Values) \
        SENF_CONSOLE_REGISTER_ENUM_(Class::, Type, Values)

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Traits.cci"
#include "Traits.ct"
#include "Traits.cti"
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
