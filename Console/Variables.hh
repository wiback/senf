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
    \brief Variables public header */

#ifndef HH_Variables_
#define HH_Variables_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/ref.hpp>
#include "ParsedCommand.hh"

#include "Variables.ih"
//#include "Variables.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    class ScopedDirectoryBase;
    template <class Variable> class VariableAttributor;

    template <class Variable>
    VariableAttributor<Variable> senf_console_add_node(
        DirectoryNode & node, std::string const & name, Variable & var, int,
        typename boost::disable_if< boost::is_convertible<Variable*, ScopedDirectoryBase*> >::type * = 0);

    template <class Variable>
    typename detail::VariableNodeCreator<Variable>::result_type
    senf_console_add_node(DirectoryNode & node, std::string const & name, 
                          boost::reference_wrapper<Variable> var, int);

    /** \brief Variable command attributes (const)
        
        \see VariableAttributor
     */
    template <class Variable>
    class ConstVariableAttributor
    {
    public:
        typedef typename detail::QueryVariable<Variable>::Traits::Overload QueryOverload;
        typedef typename QueryOverload::Formatter Formatter;
        typedef OverloadedCommandNode node_type;
        typedef ConstVariableAttributor return_type;

        ConstVariableAttributor doc(std::string const & doc);
        ConstVariableAttributor formatter(Formatter formatter);

    protected:
        explicit ConstVariableAttributor(QueryOverload & queryOverload);

    private:
        QueryOverload & queryOverload_;

        friend class detail::VariableNodeCreator<Variable const>;
    };
 
    /** \brief Variable command attributes

        Variable commands allow to register any arbitrary variable as a command node. The variable
        will be registered as two command overloads: One which takes a single argument of the
        variables type to set the variable and another one taking no arguments and just querying the
        current variable value.
        \code
        int var;
        ScopedDirectory<> dir;

        dir.add("var", var);
        \endcode

        Variables should be registered only with a ScopedDirectory declared in the same scope
        (e.g. as a class member for member variables). This ensures, that the variable node is
        removed from the tree when the scope is destroyed.

        Since a variable command is added as a combination of two ordinary overloads, it is possible
        to register additional overloads with the same name before or after registering the
        variable. 

        It is also possible, to register a variable read-only. To achieve this, just wrap it with \c
        boost::cref(). Such a variable cannot be changed only queried. Therefore, it does not have
        the parser() and typeName() attributes.
        \code
        dir.add("const_var", boost::cref(var))
        \endcode

        \ingroup console_commands
     */
   template <class Variable>
    class VariableAttributor
        : public ConstVariableAttributor<Variable>
    {
    public:
        typedef typename detail::SetVariable<Variable>::Traits::Overload SetOverload;
        typedef typename detail::ArgumentInfo<typename SetOverload::arg1_type>::Parser Parser;
        typedef typename detail::SetVariable<Variable>::OnChangeHandler OnChangeHandler;
        typedef OverloadedCommandNode node_type;
        typedef VariableAttributor return_type;

        typedef typename ConstVariableAttributor<Variable>::Formatter Formatter;
        typedef typename ConstVariableAttributor<Variable>::QueryOverload QueryOverload;

        VariableAttributor doc(std::string const & doc); ///< Set documentation of the variable
        VariableAttributor formatter(Formatter formatter); ///< Set formatter
                                        /**< The \a formatter must be a callable with a signature
                                             compatible with
                                             \code
                                             void formatter(Variable const & value, std::ostream & os);
                                             \endcode
                                             The \a formatter takes the return value of the call \a
                                             value and writes it properly formated to \a os. */
       
        VariableAttributor parser(Parser parser); ///< Set argument parser
                                        /**< The parser is an arbitrary callable object with
                                             the signature
                                             \code
                                                 void parser(senf::console::ParseCommandInfo::TokensRange const & tokens, value_type & out);
                                             \endcode

                                             where \c value_type is the type of the overload
                                             parameter. The parser must read and parse the complete
                                             \a tokens range and return the parsed value in \a
                                             out. If the parser fails, it must raise a
                                             senf::console::SyntaxErrorException. */
        VariableAttributor typeName(std::string const & name); ///< Set name of the variable type
        VariableAttributor onChange(OnChangeHandler handler); ///< Set change callback
                                        /**< The \a handler callback is called, whenever the value
                                             of the variable is changed. The new value has already
                                             been set, when the callback is called, the old value is
                                             passed to the callback. The callback must have a
                                             signature compatible to
                                             \code
                                             void handler(Variable const & oldValue);
                                             \endcode */
 
    protected:

    private:
        VariableAttributor(QueryOverload & queryOverload, SetOverload & setOverload, 
                           Variable & var);

        SetOverload & setOverload_;
        Variable & var_;

        friend class detail::VariableNodeCreator<Variable>;
    };
}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Variables.cci"
//#include "Variables.ct"
#include "Variables.cti"
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
