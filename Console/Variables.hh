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

    template <class Variable>
    class VariableAttributor
        : public ConstVariableAttributor<Variable>
    {
    public:
        typedef typename detail::SetVariable<Variable>::Traits::Overload SetOverload;
        typedef typename detail::ArgumentInfo<typename SetOverload::arg1_type>::Parser Parser;
        typedef OverloadedCommandNode node_type;
        typedef VariableAttributor return_type;

        typedef typename ConstVariableAttributor<Variable>::Formatter Formatter;
        typedef typename ConstVariableAttributor<Variable>::QueryOverload QueryOverload;

        VariableAttributor parser(Parser parser);
        VariableAttributor typeName(std::string const & name);
 
        VariableAttributor doc(std::string const & doc);
        VariableAttributor formatter(Formatter formatter);
       
    protected:

    private:
        VariableAttributor(QueryOverload & queryOverload, SetOverload & setOverload);

        SetOverload & setOverload_;

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
