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

#define BOOST_PARAMETER_MAX_ARITY 6

#include <vector>
#include <boost/type_traits/function_traits.hpp>
#include <boost/type_traits/is_member_pointer.hpp>
#include <boost/mpl/if.hpp>
#include <boost/utility.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/parameter/parameters.hpp>
#include "../config.hh"
#include "OverloadedCommand.hh"
#include "Traits.hh"
#include "../Utils/type_traits.hh"

#include "ParsedCommand.ih"
#include "ParsedCommand.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    namespace detail { class ArgumentInfoBase; }

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
        documented in ParsedArgumentAttributor. Parameters are set by adding them as additional
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
        senf::console::ArgumentTraits. See that class for more information.

        \section overload_format Custom return-value formatters

        By default, return values are streamed to an ostream. This automatically allows any
        streamable type to be used as return value. To add new types or customize the formating, the
        senf::console::ReturnValueTraits template needs to be specialized for that type. See
        that class for more information.

        \ingroup console_commands
     */
    class ParsedCommandOverloadBase
        : public CommandOverload
    {
    public:
        typedef boost::intrusive_ptr<ParsedCommandOverloadBase> ptr;

        detail::ArgumentInfoBase & arg(unsigned n) const;
        template <class Type> detail::ArgumentInfo<Type> & arg(unsigned n) const;

        void doc(std::string const & d);

    protected:
        ParsedCommandOverloadBase();

        template <class Type>
        void addParameter();

    private:
        virtual unsigned v_numArguments() const;
        virtual void v_argumentDoc(unsigned index, ArgumentDoc & doc) const;
        virtual std::string v_doc() const;

        typedef std::vector<detail::ArgumentInfoBase::ptr> Parameters;
        Parameters parameters_;
        std::string doc_;
    };

    /** \brief Parsed command overload

        ParsedCommandOverload provides the command overload added to an OverloadedCommandNode for an
        automatically parsed command.

        This class is normally instantiated automatically when adding a function or member-function
        pointer as callback to the tree. Manually instantiation this type of overload is \e not
        simple, since the function signature has to be manipulated correctly to support the optional
        \c std::ostream first argument.

        \implementation This class is specialized for each supported number of command arguments.

        \todo Implement automatic binding of member functions for parser and formatter
     */
    template <class FunctionTraits, class ReturnType, unsigned arity>
    class ParsedCommandOverload : public ParsedCommandOverloadBase
    {
    public:
        typedef boost::intrusive_ptr<ParsedCommandOverload> ptr;

#ifdef DOXYGEN
        static ptr create(Function fn);
#endif
    };

#ifndef DOXYGEN

#   define BOOST_PP_ITERATION_PARAMS_1 (4, (0, SENF_CONSOLE_MAX_COMMAND_ARITY,                     \
                                            SENF_ABSOLUTE_INCLUDE_PATH(Console/ParsedCommand.mpp), \
                                            1))
#   include BOOST_PP_ITERATE()

#endif

    /** \brief Generic ParsedCommandOverladBase attributes

        Attributes for parsed commands are not set directly on the node. They are set via a special
        attributor temporary returned when adding a parsed command to the tree.
        
        This class is the base class for those attributors. It provides members which do not depend
        in any way on the exact type of command added.

        \see \ref console_autoparse
     */
    class ParsedCommandAttributorBase
    {
    public:
        OverloadedCommandNode & node() const; ///< Return the node object
        operator OverloadedCommandNode & () const; ///< Automatically convert to node object

    protected:
        ParsedCommandAttributorBase(ParsedCommandOverloadBase & overload, unsigned index);

        void argName(std::string const & name) const;
        void argDoc(std::string const & doc) const;
        template <class Type> void defaultValue(Type const & value) const;
        void typeName(std::string const & doc) const;
        void defaultDoc(std::string const & doc) const;

        ParsedCommandOverloadBase & overload() const;
        void overloadDoc(std::string const & doc) const;
        void nodeDoc(std::string const & doc) const;
        template <class Type, class Fn> void parser(Fn fn) const;
        
    private:
        ParsedCommandOverloadBase & overload_;
        unsigned index_;
    };

    /** \brief Non argument dependent ParsedCommandBase attributes 
        
        Attributes for parsed commands are not set directly on the node. They are set via a special
        attributor temporary returned when adding a parsed command to the tree.

        This class adds all those members, which do depend on the type of command added (and thereby
        on that commands signature) but do not depend on the type of any single argument.

        \see \ref console_autoparse
     */
    template <class Overload>
    class ParsedCommandAttributor
        : public ParsedCommandAttributorBase
    {
    public:
        Overload & overload() const;    ///< Get the command overload

    protected:
        ParsedCommandAttributor(Overload & overload, unsigned index);

    private:
    };

    /** \brief Keyword argument tags

        The tags defined in this namespace are used as keyword arguments via the <a
        href="http://www.boost.org/doc/libs/1_33_1/libs/parameter/doc/html/index.html">Boost.Parameter</a>
        library.

        For the keyword tags, the standard C++ scoping rules apply
        \code
        // Either qualify them with their complete namespace
        dir.add(...)
            .arg( senf::console::kw::name = "name" );
        
        // Or use a namespace alias
        namespace kw = senf::console::kw;
        dir.add(...)
            .arg( kw::name = "name" );

        // Or import the keywords into the current namespace (beware of name collisions)
        using namespace senf::console::kw;
        dir.add(...)
            .arg( name = "name" );
        \endcode

        The second alternative is preferred, the <tt>using namespace</tt> directive may be used as
        long as the keyword names do not clash with another visible symbol.

        \section kw_attributes Argument attribute values

        The keywords are used to set argument attributes.  The keywords \ref default_value and \ref
        parser influence, how an argument is parsed/interpreted whereas \ref name, \ref description,
        \ref type_name and \ref default_doc are used to change the arguments documentation:
        \code
        void command(int);

        dir.add("command", &command)
            .arg( kw::name          = "name",
                  kw::description   = "description",
                  kw::default_value = 1,
                  kw::type_name     = "type_name",
                  kw::default_doc   = "default_doc" );
        \endcode
        Will create the following documentation:
        \htmlonly
        <pre>
        Usage:
            command [name:type_name]
        
        With:
            name      description
                default: default_doc
        </pre>
        \endhtmlonly

        \see \ref senf::console::ParsedArgumentAttributor::arg()
     */
    namespace kw {
        BOOST_PARAMETER_KEYWORD(type, name) ///< Argument name
                                        /**< Sets the displayed name of the argument. */
        BOOST_PARAMETER_KEYWORD(type, description) ///< One-line argument description
                                        /**< This description is shown in the argument
                                             reference. If several overloads have same-named
                                             arguments, only one of them should be documented. This
                                             documentation then applies to all arguments of that
                                             name. */
        BOOST_PARAMETER_KEYWORD(type, default_value) ///< Argument default value
                                        /**< If a default value is specified for an argument, that
                                             argument is optional. If an overload is called with
                                             fewer arguments than defined, optional values will be
                                             used beginning at the last optional argument and going
                                             forward until all arguments have values assigned. E.g.,
                                             an overload with 5 parameters \a a - \a e with two
                                             defaults attached:
                                             <pre>
                                             command a:int [b:int] c:int [d:int] e:int
                                             </pre>
                                             When calling the overload, the arguments will be
                                             assigned in the following way:
                                             <table class="senf fixedwidth">
                                             <tr>
                                               <td style="whitespace:no-wrap"><tt>command 1 2</tt></td>
                                               <td colspan="5">SyntaxErrorException: invalid number of
                                                 arguments</td>
                                             </tr>
                                             <tr>
                                               <td style="white-space:nowrap"><tt>command 1 2 3</tt></td>
                                               <td style="width:6em">\a a = 1</td><td style="width:6em">\a b = \e default</td><td style="width:6em">\a c = 2</td><td style="width:6em">\a d = \e default</td><td style="width:6em">\a e = 3</td>
                                             </tr>
                                             <tr>
                                               <td style="white-space:nowrap"><tt>command 1 2 3 4</tt></td>
                                               <td>\a a = 1</td><td>\a b = 2</td><td>\a c = 3</td><td>\a d = \e default</td><td>\a e = 4</td>
                                             </tr>
                                             <tr>
                                               <td style="white-space:nowrap"><tt>command 1 2 3 4 5</tt></td>
                                               <td>\a a = 1</td><td>\a b = 2</td><td>\a c = 3</td><td>\a d = 4</td><td>\a e = 5</td>
                                             </tr>
                                             <tr>
                                               <td style="white-space:nowrap"><tt>command 1 2 3 4 5 6</tt></td>
                                               <td colspan="5">SyntaxErrorException: invalid number of
                                                 arguments</td>
                                             </tr>
                                             </table>
                                             So, if you use default values as you are used to,
                                             assigning default values to consecutive trailing
                                             arguments, they work like they do in C++ and most other
                                             languages */
        BOOST_PARAMETER_KEYWORD(type, type_name) ///< Type name of this arguments type
                                        /**< By default, the type of an argument is extracted from
                                             the C++ type name by taking the last component of the
                                             fully scoped name. This value can be changed by setting
                                             this attribute. */
        BOOST_PARAMETER_KEYWORD(type, default_doc) ///< String rep of default value
                                        /**< By default, the default value is documented by
                                             converting the value to it's string representation
                                             using \c boost::lexical_cast / \c iostreams. The
                                             displayed value can be changed by setting this
                                             attribute. */
        BOOST_PARAMETER_KEYWORD(type, parser) ///< Argument parser
                                        /**< The argument parser is used to convert the argument
                                             token list returned by the console/config parser into
                                             the appropriate value. If not set explicitly, this
                                             conversion is supplied by the ArgumentTraits
                                             class. 

                                             Setting the \a parser attribute allows to use a custom
                                             parser. The parser is an arbitrary callable object with
                                             the signature
                                             \code
                                             void parser(senf::console::ParseCommandInfo::TokensRange const & tokens, value_type & out);
                                             \endcode
                                             where \c value_type is the type of the overload
                                             parameter. The parser must read and parse the complete
                                             \a tokens range and return the parsed value in \a
                                             out. If the parser fails, it must raise a
                                             senf::console::SyntaxErrorException. */
    }

    /** \brief Derived class dependent ParsedCommandBase attributes

        Attributes for parsed commands are not set directly on the node. They are set via a special
        attributor temporary returned when adding a parsed command to the tree.
        
        This class adds all those members, which do not depend on any specific argument but which
        need to return the correct attributor type.

        \see \ref console_autoparse
     */
    template <class Overload, class Self, class ReturnType=typename Overload::traits::result_type>
    class ParsedArgumentAttributorBase
        : public ParsedCommandAttributor<Overload>
    {
    public:
        Self doc(std::string const & doc) const; ///< Set documentation for all overloads
        Self overloadDoc(std::string const & doc) const; ///< Set overload specific documentation
        Self formatter(typename Overload::Formatter formatter) const; 
                                        ///< Set return value formatter
                                        /**< This member is only available, if the \a ReturnType of
                                             the installed callback is not \c void.

                                             If \a ReturnType is not \c void, the \a formatter must
                                             be a callable with a signature compatible with
                                             \code
                                             void formatter(ReturnType const & value, std::ostream & os);
                                             \endcode
                                             The \a formatter takes the return value of the call \a
                                             value and writes it properly formated to \a os. */

    protected:
        ParsedArgumentAttributorBase(Overload & overload, unsigned index);

    private:
    };

#ifndef DOXYGEN

    template <class Overload, class Self>
    class ParsedArgumentAttributorBase<Overload, Self, void>
        : public ParsedCommandAttributor<Overload>
    {
    public:
        Self doc(std::string const & doc) const; ///< Set documentation for all overloads
        Self overloadDoc(std::string const & doc) const; ///< Set overload specific documentation

    protected:
        ParsedArgumentAttributorBase(Overload & overload, unsigned index);

    private:
    };

#endif
    
    /** \brief Argument dependent ParsedCommandBase attributes

        Attributes for parsed commands are not set directly on the node. They are set via a special
        attributor temporary returned when adding a parsed command to the tree.
        
        This class adds all those members, which depend on a specific argument. Each call to \c arg
        will advance to the next argument.

        \see \ref console_autoparse
     */
    template < class Overload, unsigned index, bool flag>
    class ParsedArgumentAttributor
        : public ParsedArgumentAttributorBase< Overload, 
                                                ParsedArgumentAttributor<Overload, index, flag> >
    {
        typedef boost::parameter::parameters<
            kw::type::name,
            kw::type::description,
            kw::type::default_value,
            kw::type::type_name,
            kw::type::default_doc,
            kw::type::parser> arg_params;

    public:
        typedef OverloadedCommandNode node_type;
        typedef ParsedArgumentAttributor return_type;

        typedef typename senf::function_traits_arg_type< 
            typename Overload::traits, int(index) >::type arg_type;
        typedef typename senf::remove_cvref< arg_type >::type value_type;
        typedef ParsedArgumentAttributor<Overload, index+1> next_type;

        next_type arg() const;          ///< Set argument attributes
                                        /**< This member changes the attributes for the current
                                             argument. The attributes are passed to arg() as keyword
                                             arguments using the <a
                                             href="http://www.boost.org/doc/libs/1_33_1/libs/parameter/doc/html/index.html">Boost.Parameter</a>
                                             library. 
                                             \code
                                             ...
                                                 .arg( kw::name          = "name", 
                                                       kw::default_value = 1 )
                                             ...
                                             \endcode
                                             The valid keywords are defined in the senf::console::kw
                                             namespace.
                                             
                                             Each call to arg() will increment the argument index
                                             and advance to the next argument. This member is only
                                             present, if there is an argument at the current
                                             index. */

#ifndef DOXYVEN

#       define BOOST_PP_ITERATION_PARAMS_1                                                        \
            (4, (1, BOOST_PARAMETER_MAX_ARITY,                                                    \
                 SENF_ABSOLUTE_INCLUDE_PATH(Console/ParsedCommand.mpp),                           \
                 5))
#       include BOOST_PP_ITERATE()

#endif

    private:
        explicit ParsedArgumentAttributor(Overload & overload);

        template <class ArgumentPack>
        next_type argInfo(ArgumentPack const & args) const;
        template <class Kw, class ArgumentPack>
        void argInfo(Kw const &, ArgumentPack const &, boost::mpl::false_) 
            const;

        template <class ArgumentPack>
        void argInfo(boost::parameter::keyword<kw::type::name> const &, 
                     ArgumentPack const & args, boost::mpl::true_) 
            const;
        template <class ArgumentPack>
        void argInfo(boost::parameter::keyword<kw::type::description> const &, 
                     ArgumentPack const & args, boost::mpl::true_) 
            const;
        template <class ArgumentPack>
        void argInfo(boost::parameter::keyword<kw::type::default_value> const &, 
                     ArgumentPack const & args, boost::mpl::true_) 
            const;
        template <class ArgumentPack>
        void argInfo(boost::parameter::keyword<kw::type::type_name> const &, 
                     ArgumentPack const & args, boost::mpl::true_) 
            const;
        template <class ArgumentPack>
        void argInfo(boost::parameter::keyword<kw::type::default_doc> const &, 
                     ArgumentPack const & args, boost::mpl::true_) 
            const;
        template <class ArgumentPack>
        void argInfo(boost::parameter::keyword<kw::type::parser> const &, 
                     ArgumentPack const & args, boost::mpl::true_) 
            const;

        next_type next() const;

        void defaultValue(value_type const & value) const;
        template <class Fn> void parser(Fn fn) const;

        template <class O, unsigned i, bool f>
        friend class ParsedArgumentAttributor;

        friend class detail::ParsedCommandAddNodeAccess;
    };

#ifndef DOXYGEN

    template <class Overload, unsigned index>
    class ParsedArgumentAttributor<Overload, index, false>
        : public ParsedArgumentAttributorBase< Overload, 
                                                ParsedArgumentAttributor<Overload, index, false> >
    {
    public:
        typedef OverloadedCommandNode node_type;
        typedef ParsedArgumentAttributor return_type;

    private:
        explicit ParsedArgumentAttributor(Overload & overload);

        template <class O, unsigned i, bool f>
        friend class ParsedArgumentAttributor;

        friend class detail::ParsedCommandAddNodeAccess;
    };

    template <class Function>
    typename detail::ParsedCommandTraits<Function>::Attributor
    senf_console_add_node(DirectoryNode & node, std::string const & name, Function fn, int);

    template <class Signature>
    typename detail::ParsedCommandTraits<Signature>::Attributor
    senf_console_add_node(DirectoryNode & node, std::string const & name, boost::function<Signature> fn, int);

    template <class Owner, class Function>
    typename detail::ParsedCommandTraits<Function>::Attributor
    senf_console_add_node(DirectoryNode & node, Owner & owner, std::string const & name,
                          Function fn, int,
                          typename boost::enable_if_c<
                              detail::ParsedCommandTraits<Function>::is_member>::type * = 0);

#endif

}}

#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

BOOST_TYPEOF_REGISTER_TEMPLATE(senf::console::ParsedCommandOverload, (class,unsigned))
BOOST_TYPEOF_REGISTER_TEMPLATE(senf::console::ParsedArgumentAttributor, (class, unsigned, bool))
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
