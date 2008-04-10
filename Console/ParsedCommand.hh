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
    \brief ParsedCommand public header */

#ifndef HH_ParsedCommand_
#define HH_ParsedCommand_ 1

// Custom includes
#include <vector>
#include <boost/type_traits/function_traits.hpp>
#include <boost/type_traits/is_member_pointer.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/if.hpp>
#include <boost/utility.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/parameter/parameters.hpp>
#include "../config.hh"
#include "OverloadedCommand.hh"
#include "ParseParameter.hh"
#include "../Utils/type_traits.hh"

#include "ParsedCommand.ih"
#include "ParsedCommand.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    /** \brief CommandOverload implementation with automatic argument parsing

        ParsedCommandOverloadBase implements a CommandOverload implementation supporting automatic
        parsing of arguments. This is \e not a node, it's a CommandOverload which is then added to
        an OverloadedCommandNode instance.

        Automatic argument parsing and return value processing consists of several components:
        \li \ref overload_add Adding overload instances to the tree
        \li (Advanced) \ref overload_parse
        \li (Advanced) \ref overload_format

        \section overload_add Adding argument parsing callbacks to the tree

        Adding appropriate callbacks to the tree is very simple: just path a function pointer to
        DirectoryNode::add() or a member function pointer to ScopedDirectory::add().
        \code
        std::string taskStatus(int id);

        senf::console::root().add("taskStatus", &taskStatus);
        \endcode

        There are quite a number of additional parameters available to be set. These parameters are
        documented in ParsedAttributeAttributor. Parameters are set by adding them as additional
        calls after adding the node:

        \code
        senf::console::root().add("taskStatus", &taskStatus)
            .doc("Query the current task status")
            .arg( name = "id",
                  description = "numeric id of task to check, -1 for the current task."
                  default_value = -1 );
        \endcode

        You may also add an additional \c std::ostream & Argument as first argument to the
        callback. If this argument is present, the stream connected to the console which issued the
        command will be passed there. This allows writing arbitrary messages to the console.

        Additionally, overloading is supported by registering multiple commands under the same
        name. So, elaborating on above example:
        \code
        std::string taskStatus(int id);
        std::string taskStatus(std::string const & name);

        senf::console::root()
            .add("taskStatus", static_cast<std::string (*)(int)>(&taskStatus))
            .doc("Query the current task status")
            .overloadDoc("Query status by id")
            .arg( name = "id",
                  description = "numeric id of task to check, -1 for the current task."
                  default_value = -1 );
        senf::console::root()
            .add("taskStatus", static_cast<std::string (*)(std::string const &)>(&taskStatus))
            .overloadDoc("Query status by name")
            .arg( name = "name",
                  description = "name of task to check" );
        \endcode

        We can see here, that taking the address of an overloaded function requires a cast. If you
        can give unique names to each of the C++ overloads (not the overloads in the console), you
        should do so to make the unwieldy casts unnecessary.

        \section overload_parse Custom parameter parsers
        
        By default, parameters are parsed using \c boost::lexical_cast and therefore using \c
        iostreams. This means, that any type which can be read from a stream can automatically be
        used as argument type.

        However, argument parsing can be configured by specializing
        senf::console::detail::ParameterTraits. See that class for more information.

        \section overload_format Custom return-value formatters

        By default, return values are streamed to an ostream. This automatically allows any
        streamable type to be used as return value. To add new types or customize the formating, the
        senf::console::detail::ReturnValueTraits template needs to be specialized for that type. See
        that class for more information.

        \ingroup console_commands
     */
    class ParsedCommandOverloadBase
        : public CommandOverload
    {
    public:
        typedef boost::intrusive_ptr<ParsedCommandOverloadBase> ptr;

        detail::ParameterInfoBase & arg(unsigned n) const;
        template <class Type> detail::ParameterInfo<Type> & arg(unsigned n) const;

        void doc(std::string const & d);

    protected:
        ParsedCommandOverloadBase();

        template <class Type>
        void addParameter();

    private:
        virtual unsigned v_numArguments() const;
        virtual void v_argumentDoc(unsigned index, ArgumentDoc & doc) const;
        virtual std::string v_doc() const;

        typedef std::vector<detail::ParameterInfoBase::ptr> Parameters;
        Parameters parameters_;
        std::string doc_;
    };

    template <class FunctionTraits, unsigned arity=FunctionTraits::arity>
    class ParsedCommandOverload {};

#   define BOOST_PP_ITERATION_PARAMS_1 (4, (0, SENF_CONSOLE_MAX_COMMAND_ARITY,                     \
                                            SENF_ABSOLUTE_INCLUDE_PATH(Console/ParsedCommand.mpp), \
                                            1))
#   include BOOST_PP_ITERATE()

    class ParsedCommandAttributorBase
    {
    public:
        OverloadedCommandNode & node() const;
        operator OverloadedCommandNode & () const;

    protected:
        ParsedCommandAttributorBase(ParsedCommandOverloadBase & overload, unsigned index);

        void argName(std::string const & name) const;
        void argDoc(std::string const & doc) const;
        template <class Type> void defaultValue(Type const & value) const;

        ParsedCommandOverloadBase & overload() const;
        void overloadDoc(std::string const & doc) const;

        void nodeDoc(std::string const & doc) const;
        
    private:
        ParsedCommandOverloadBase & overload_;
        unsigned index_;
    };

    template <class Overload>
    class ParsedCommandAttributor
        : public ParsedCommandAttributorBase
    {
    public:
        Overload & overload() const;

    protected:
        ParsedCommandAttributor(Overload & overload, unsigned index);

    private:
    };
    
    namespace kw {
        BOOST_PARAMETER_KEYWORD(type, name);
        BOOST_PARAMETER_KEYWORD(type, description);
        BOOST_PARAMETER_KEYWORD(type, default_value);
    }

    template <class Overload, class Self>
    class ParsedAttributeAttributorBase
        : public ParsedCommandAttributor<Overload>
    {
    public:
        Self doc(std::string const & doc) const;
        Self overloadDoc(std::string const & doc) const;

    protected:
        ParsedAttributeAttributorBase(Overload & overload, unsigned index);

    private:
    };

    template < class Overload, 
               unsigned index=0, 
               bool flag=(index < unsigned(Overload::traits::arity)) >
    class ParsedAttributeAttributor
        : public ParsedAttributeAttributorBase< Overload, 
                                                ParsedAttributeAttributor<Overload, index, flag> >
    {
    public:
        typedef typename senf::function_traits_arg_type< 
            typename Overload::traits, int(index) >::type arg_type;
        typedef typename senf::remove_cvref< arg_type >::type value_type;
        typedef ParsedAttributeAttributor<Overload, index+1> next_type;

        typedef OverloadedCommandNode node_type;
        typedef ParsedAttributeAttributor return_type;

        typedef boost::parameter::parameters<
            kw::type::name,
            kw::type::description,
            kw::type::default_value> arg_params;

        next_type arg() const;

#       define BOOST_PP_ITERATION_PARAMS_1 \
            (4, (1, 3, SENF_ABSOLUTE_INCLUDE_PATH(Console/ParsedCommand.mpp), 5))
#       include BOOST_PP_ITERATE()

    private:
        explicit ParsedAttributeAttributor(Overload & overload);

        template <class ArgumentPack>
        next_type argInfo(ArgumentPack const & args) const;

        template <class Kw, class ArgumentPack>
        void argInfo(Kw const &, ArgumentPack const &, boost::mpl::true_) 
            const;
        template <class ArgumentPack>
        void argInfo(boost::parameter::keyword<kw::type::name> const &, 
                     ArgumentPack const & args, boost::mpl::false_) 
            const;
        template <class ArgumentPack>
        void argInfo(boost::parameter::keyword<kw::type::description> const &, 
                     ArgumentPack const & args, boost::mpl::false_) 
            const;
        template <class ArgumentPack>
        void argInfo(boost::parameter::keyword<kw::type::default_value> const &, 
                     ArgumentPack const & args, boost::mpl::false_) 
            const;

        next_type next() const;

        void defaultValue(value_type const & value) const;

        template <class O, unsigned i, bool f>
        friend class ParsedAttributeAttributor;
        
        template <class Function>
        friend ParsedAttributeAttributor<
            ParsedCommandOverload<typename detail::ParsedCommandTraits<Function>::traits> >
        senf_console_add_node(DirectoryNode & node, std::string const & name, Function fn, int);
        
        template <class Owner, class Function>
        friend ParsedAttributeAttributor<
            ParsedCommandOverload<typename detail::ParsedCommandTraits<Function>::traits> >
        senf_console_add_node(DirectoryNode & node, Owner & owner, std::string const & name,
                              Function fn, int,
                              typename boost::enable_if_c<
                                  detail::ParsedCommandTraits<Function>::is_member>::type * = 0);
    };

    template <class Overload, unsigned index>
    class ParsedAttributeAttributor<Overload, index, false>
        : public ParsedAttributeAttributorBase< Overload, 
                                                ParsedAttributeAttributor<Overload, index, false> >
    {
    public:
        typedef OverloadedCommandNode node_type;
        typedef ParsedAttributeAttributor return_type;

    private:
        explicit ParsedAttributeAttributor(Overload & overload);

        template <class O, unsigned i, bool f>
        friend class ParsedAttributeAttributor;
        
        template <class Function>
        friend ParsedAttributeAttributor<
            ParsedCommandOverload<typename detail::ParsedCommandTraits<Function>::traits> >
        senf_console_add_node(DirectoryNode & node, std::string const & name, Function fn, int);
        
        template <class Owner, class Function>
        friend ParsedAttributeAttributor<
            ParsedCommandOverload<typename detail::ParsedCommandTraits<Function>::traits> >
        senf_console_add_node(DirectoryNode & node, Owner & owner, std::string const & name,
                              Function fn, int,
                              typename boost::enable_if_c<
                                  detail::ParsedCommandTraits<Function>::is_member>::type * = 0);
    };

#ifndef DOXYGEN

    template <class Function>
    ParsedAttributeAttributor<
        ParsedCommandOverload<typename detail::ParsedCommandTraits<Function>::traits> >
    senf_console_add_node(DirectoryNode & node, std::string const & name, Function fn, int);

    template <class Owner, class Function>
    ParsedAttributeAttributor<
        ParsedCommandOverload<typename detail::ParsedCommandTraits<Function>::traits> >
    senf_console_add_node(DirectoryNode & node, Owner & owner, std::string const & name,
                          Function fn, int,
                          typename boost::enable_if_c<
                              detail::ParsedCommandTraits<Function>::is_member>::type * = 0);

#endif

}}

#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

BOOST_TYPEOF_REGISTER_TEMPLATE(senf::console::ParsedCommandOverload, (class,unsigned))
BOOST_TYPEOF_REGISTER_TEMPLATE(senf::console::ParsedAttributeAttributor, (class, unsigned, bool))
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::function_traits, 1)

///////////////////////////////hh.e////////////////////////////////////////
#include "ParsedCommand.cci"
#include "ParsedCommand.ct"
#include "ParsedCommand.cti"
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
