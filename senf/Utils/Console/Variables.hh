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

#ifndef HH_SENF_Scheduler_Console_Variables_
#define HH_SENF_Scheduler_Console_Variables_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/ref.hpp>
#include "Node.hh"

#include "Variables.ih"
//#include "Variables.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace console {

    class ScopedDirectoryBase;
    template <class Variable> class VariableAttributor;

namespace factory {

#ifndef DOXYGEN

    template <class Variable>
    class ConstVariableFactory
        : public detail::NodeFactory
    {
    public:
        typedef typename detail::QueryVariable<Variable>::Traits::Overload QueryOverload;
        typedef typename QueryOverload::Formatter Formatter;
        typedef OverloadedCommandNode node_type;
        typedef OverloadedCommandNode & result_type;

        ConstVariableFactory doc(std::string const & doc);
        ConstVariableFactory shortdoc(std::string const & doc);
        ConstVariableFactory formatter(Formatter formatter);

        OverloadedCommandNode & create(DirectoryNode & dir, std::string const & name) const;

        explicit ConstVariableFactory(Variable const & var);

    private:
        typename QueryOverload::ptr queryOverload_;
        boost::optional<std::string> doc_;
        boost::optional<std::string> shortdoc_;
    };

#endif

    /** \brief Variable node factory

        Variable commands allow to register any arbitrary variable as a command node. The variable
        will be registered as two command overloads: One which takes a single argument of the
        variables type to set the variable and another one taking no arguments and just querying the
        current variable value.
        \code
            int var;
            ScopedDirectory<> dir;

        dir.add("var", fty::Variable(var));
        \endcode

        Variables should be registered only with a ScopedDirectory declared in the same scope
        (e.g. as a class member for member variables). This ensures, that the variable node is
        removed from the tree when the scope is destroyed.

        Since a variable command is added as a combination of two ordinary overloads, it is possible
        to register additional overloads with the same name before or after registering the
        variable.

        It is also possible, to register a variable read-only. To achieve this, just wrap it with \c
        boost::cref(). Such a variable only queried. Therefore, it does not have the parser() and
        typeName() attributes.
        \code
            dir.add("const_var", fty::Variable(boost::cref(var)));
        \endcode

        \note Even though the interface is documented as a class, in reality it is implemented using
            factory functions returning instances of an internal factory class.

        \see \ref console_variables
     */
#ifdef DOXYGEN
    class Variable
#else
    template <class Variable>
    class VariableFactory
        : public ConstVariableFactory<Variable>
#endif
    {
    public:
        typedef typename detail::SetVariable<Variable>::Traits::Overload SetOverload;
        typedef typename detail::ArgumentInfo<typename SetOverload::arg1_type>::Parser Parser;
        typedef typename detail::SetVariable<Variable>::OnChangeHandler OnChangeHandler;

        typedef typename ConstVariableFactory<Variable>::Formatter Formatter;
        typedef typename ConstVariableFactory<Variable>::QueryOverload QueryOverload;

        VariableFactory doc(std::string const & doc); ///< Set documentation of the variable
        VariableFactory shortdoc(std::string const & doc); ///< Set short documentation
        VariableFactory formatter(Formatter formatter); ///< Set formatter
                                        /**< The \a formatter must be a callable with a signature
                                             compatible with
                                             \code
                                                 void formatter(Variable const & value, std::ostream & os);
                                             \endcode
                                                 The \a formatter takes the return value of the call \a
                                                 value and writes it properly formated to \a os. */
        VariableFactory parser(Parser parser); ///< Set argument parser
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
        VariableFactory typeName(std::string const & name); ///< Set name of the variable type
        VariableFactory onChange(OnChangeHandler handler); ///< Set change callback
                                        /**< The \a handler callback is called, whenever the value
                                             of the variable is changed. The new value has already
                                             been set, when the callback is called, the old value is
                                             passed to the callback. The callback must have a
                                             signature compatible to
                                             \code
                                                 void handler(Variable const & oldValue);
                                             \endcode */

        explicit VariableFactory(Variable & var); ///< Create Variable node

    protected:

    private:
        OverloadedCommandNode & create(DirectoryNode & dir, std::string const & name) const;

        typename SetOverload::ptr setOverload_;
        Variable & var_;

        friend class senf::console::DirectoryNode;
    };

#ifndef DOXYGEN

    template <class Var>
    VariableFactory<Var> Variable(Var & var);

    template <class Var>
    VariableFactory<Var> Variable(boost::reference_wrapper<Var> var);

    template <class Var>
    ConstVariableFactory<Var> Variable(Var const & var);

    template <class Var>
    ConstVariableFactory<Var> Variable(boost::reference_wrapper<Var const> var);

#endif

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
