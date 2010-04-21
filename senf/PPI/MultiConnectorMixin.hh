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
    \brief MultiConnectorMixin public header */

#ifndef HH_SENF_PPI_MultiConnectorMixin_
#define HH_SENF_PPI_MultiConnectorMixin_ 1

// Custom includes
#include <senf/config.hh>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/mpl/if.hpp>
#include "Connectors.hh"
#include "Setup.hh"

#ifndef SENF_MULTI_CONNECTOR_MAX_ARGS
#define SENF_MULTI_CONNECTOR_MAX_ARGS 3
#endif

#include "MultiConnectorMixin.mpp"
#include "MultiConnectorMixin.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
    
#ifdef DOXYGEN

    // For exposition only.
    // Other implementations with 0..SENF_MULTI_CONNECTOR_MAX_ARGS arguments accordingly
    // The real implementation includes a boost::enable_if condition in the return value, which is
    // used to only enable the correct overload: The first overload, if the MultiConnector is an
    // output, otherwise the second overload.

    /** \brief Connect MultiConnector source to arbitrary target

        Additional implementations with 0..SENF_MULTI_CONNECTOR_MAX_ARGS arguments.

        \related module::MultiConnectorMixin
     */
    template <class MultiConnectorSource, class Target, class A1>
    MultiConnectorSource::ConnectorType & connect(
        MultiConnectorSource & source, A1 const & a1, Target & target);
    
    /** \brief Connect arbitrary source to MultiConnector target

        Additional implementations with 0..SENF_MULTI_CONNECTOR_MAX_ARGS arguments.

        \related module::MultiConnectorMixin
     */
    template <class Source, class MultiConnectorTarget, class A1>
    MultiConnectorTarget::ConnectorType & connect(
        Source & source, MultiConnectorTarget & target, A1 const & a1);

    template <class MultiConnectorSource, class MultiConnectorTarget, class A1, class A2>
    std::pair<MultiConnectorSource::ConnectorType &, MultiConnectorTarget::ConnectorType &>
    connect(
        MultiConnectorSource & source, MultiConnectorTarget & target, A1 const & a1, A2 const & a2);

#else

    // Include 'senf::ppi namespace member declarations' from MultiConnectorMixin.mpp
#   define BOOST_PP_ITERATION_PARAMS_1 (4, ( \
            0, \
            SENF_MULTI_CONNECTOR_MAX_ARGS, \
            SENF_ABSOLUTE_INCLUDE_PATH(PPI/MultiConnectorMixin.mpp), \
            2 ))
#   include BOOST_PP_ITERATE()

#endif

namespace module {

    namespace detail { class MultiConnectorMixinAccess; }

    /** \brief Multi-Connector management

        This mixin provides a module with support for a runtime configurable number of input or
        output connectors.
        \code
        class MyModule 
            : public senf::ppi::module::Module,
              public senf::ppi::module::MultiConnectorMixin<
                  MyModule, senf::ppi::connector::ActiveInput<> >
        {
            SENF_PPI_MODULE(MyModule);
        public:
            senf::ppi::connector::PassiveOutput<> output;

            // ...

        private:
            void connectorSetup(senf::ppi::connector::ActiveInput & input)
            {
                route(input, output);
                input.onThrottle(&MyModule::doThrottle);
            }
        
            // Optional
            void connectorDestroy(senf::ppi::connector::ActiveInput const & input)
            {
                 //  whatever
            }

            void doThrottle()
            { 
                // ...
            }

            friend class senf::ppi::module::MultiConnectorMixin<
                MyModule, senf::ppi::connector::ActiveInput<> >;
        }
        \endcode

        Using the MultiConnectorMixin consists of
        \li inheriting from MultiConnectorMixin
        \li defining a function \c connectorSetup
        \li declaring the mixin as a friend
        \li optionally defining \c connectorDestroy to be notified when connectors are disconnected

        The MultiConnectorMixin takes several template arguments
        \li the name of the derived class, \a Self_
        \li the type of connector, \a ConnectorType_
        \li an optional \a KeyType_ if a mapping container is required
        \li an optional \a ContainerType_ to replace the default \c boost::ptr_vector or \c
            boost::ptr_map.

        \section senf_ppi_multiconnector_sequence Sequence/vector based multi connector mixin

        If no \a KeyType_ is given (or the \a KeyType_ is \c void), the mixin will use a sequence
        container, by default a \c boost::ptr_vector to store the connectors. In this case, new
        connectors will be added to the end of the container. The \c connectorSetup() routine
        however may be used to move the inserted connector to some other location, e.g.
        \code
        container().insert(begin(), container().pop_back().release());
        \endcode
        which will move the new connector from the end to the beginning. If you want to abort adding
        the new connector, you may throw an exception.

        \par "Example:" senf::ppi::module::PriorityJoin

        \section senf_ppi_multiconnector_map Map based multi connector mixin

        If \a KeyType_ is given (and is not \c void), the mixin will use a mapping container, by
        default a \c boost::ptr_map to store the connectors. The \c connectorSetup() member function
        must return the key value under which the new connector will be stored. The new connector
        will this be written to the container only \e after \c connectorSetup() returns.

        When the returned key is not unique, the new connector will \e replace the old one. If this
        is not what you want, either check for an already existing member and throw an exception in
        \c connectorSetup() or replace the \c boost::ptr_map by a \c boost::ptr_multimap using the
        fourth template argument to MultiConnectorMixin.

        \par "Example:" senf::ppi::module::AnnotationRouter

        \section senf_ppi_multiconnector_connect Connect and additional connectorSetup() arguments

        When connecting to a module using the MultiConnectorMixin, every new connect call will
        allocate a new connector
        \code
        MyModule muModule;
        
        senf::ppi::connect(someModule, myModule);
        \endcode
        Some modules will expect additional arguments to be passed (see below)
        \code
        MyModule myModule;

        senf::ppi::connect(mod1, myModule);     // index = 0, see below
        senf::ppi::connect(mod2, myModule, 1);  // index = 1, see below
        \endcode
        To declare these additional arguments, just declare \c connectorSetup() with those
        additional arguments:
        \code
        void connectorSetup(MyModule::ConnectorType & input, int index=0)
        {
            container().insert(container().begin()+index, container().pop_back().release());
        }
        \endcode

        \par "Advanced note:" These additional arguments are always passed by const-reference. If
            you need to pass a non-const reference, declare the \c connectorSetup() argument as
            non-const reference and wrap the real argument using \c boost::ref() (The reason for
            this is known as 'The forwarding problem').

        \section senf_ppi_multiconnector_advanced Advanced usage: Managing your own container

        If you need to use a completely different type of container, you can take over the container
        management yourself. To do this, pass \c void as container type and change \c
        connectorSetup() to take an \c std::auto_ptr as argument. \c connectorSetup() must ensure to
        save this connector in some container or throw an exception.

        Implementing \c connectorDestroy now is \e mandatory. The signature is changed to take a
        pointer as argument
        \code
        class MyModule 
            : public senf::ppi::module::Module,
              public senf::ppi::module::MultiConnectorMixin<
                  MyModule, senf::ppi::connector::ActiveInput<>, void, void >
        {
            SENF_PPI_MODULE(MyModule);
        public:
            // ...

        private:
            void connectorSetup(std::auto_ptr<ConnectorType> conn, unsigned p)
            {
                if (p>connectors_.size())
                   throw SomeErrorException();
                route(*conn, output);
                connectors_.insert(connectors_.begin()+p,conn);
            }

            void connectorDestroy(ConnectorType const * conn)
            {
                using boost::lambda::_1;
                boost::ptr_vector<ConnectorType>::iterator i (
                    std::find_if(connectors_.begin(),connectors_.end(), &_1==conn))
                if (i != connectors_.end())
                    connectors_.erase(i);
            }

            boost::ptr_vector<ConnectorType> connectors_;
        };
        \endcode
        \warning You must make absolutely sure the connector does not get deleted when returning
            normally from \c connectorSetup(): The connector \e must be saved somewhere
            successfully, otherwise your code will break.

     */
    template <class Self_, 
              class ConnectorType_, 
              class KeyType_=void, 
              class ContainerType_=typename detail::MultiConnectorDefaultContainer<
                                               KeyType_,ConnectorType_>::type>
    class MultiConnectorMixin 
        : private detail::MultiConnectorSelectBase<ConnectorType_>::type
    {
    public:
        typedef ConnectorType_ ConnectorType; ///< Type of MultiConnector connector

    protected:
        typedef ContainerType_ ContainerType; ///< Type of connector container

        ContainerType_ & connectors();        ///< Get connector container
        ContainerType_ const & connectors() const; ///< Get connectors container (const)

        void connectorDestroy(ConnectorType const &);

    private:

#ifndef DOXYGEN

        // Include 'MultiConnectorMixin member declaration' from MultiConnectorMixin.mpp
#       define BOOST_PP_ITERATION_PARAMS_1 (4, ( \
            0, \
            SENF_MULTI_CONNECTOR_MAX_ARGS, \
            SENF_ABSOLUTE_INCLUDE_PATH(PPI/MultiConnectorMixin.mpp), \
            1 ))
#       include BOOST_PP_ITERATE()

#endif

        void disconnected(ConnectorType_ const & c);

        friend class detail::MultiConnectorMixinAccess;
        friend class detail::MultiConnectorWrapper<Self_,ConnectorType_>;
        
        ContainerType_ connectors_;
    };

#ifndef DOXYGEN

    template <class Self_,
              class ConnectorType_,
              class ContainerType_>
    class MultiConnectorMixin<Self_,ConnectorType_,void,ContainerType_>
        : private detail::MultiConnectorSelectBase<ConnectorType_>::type
    {
    public:
        typedef ConnectorType_ ConnectorType;
        
    protected:
        typedef ContainerType_ ContainerType;

        ContainerType_ & connectors();
        ContainerType_ const & connectors() const;

        void connectorDestroy(ConnectorType const &);

    private:

        // Include 'MultiConnectorMixin member declaration' from MultiConnectorMixin.mpp
#       define BOOST_PP_ITERATION_PARAMS_1 (4, ( \
            0, \
            SENF_MULTI_CONNECTOR_MAX_ARGS, \
            SENF_ABSOLUTE_INCLUDE_PATH(PPI/MultiConnectorMixin.mpp), \
            1 ))
#       include BOOST_PP_ITERATE()

        void disconnected(ConnectorType_ const & c);
        
        friend class detail::MultiConnectorMixinAccess;
        friend class detail::MultiConnectorWrapper<Self_,ConnectorType_>;

        ContainerType_ connectors_;
    };

    template <class Self_, 
              class ConnectorType_>
    class MultiConnectorMixin<Self_,ConnectorType_,void,void>
        : private detail::MultiConnectorSelectBase<ConnectorType_>::type
    {
    public:
        typedef ConnectorType_ ConnectorType;

    private:

        // Include 'MultiConnectorMixin member declaration' from MultiConnectorMixin.mpp
#       define BOOST_PP_ITERATION_PARAMS_1 (4, ( \
            0, \
            SENF_MULTI_CONNECTOR_MAX_ARGS, \
            SENF_ABSOLUTE_INCLUDE_PATH(PPI/MultiConnectorMixin.mpp), \
            1 ))
#       include BOOST_PP_ITERATE()

        void disconnected(ConnectorType_ const & c);

        friend class detail::MultiConnectorMixinAccess;
        friend class detail::MultiConnectorWrapper<Self_,ConnectorType_>;
    };

#endif
        
}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "MultiConnectorMixin.cci"
#include "MultiConnectorMixin.ct"
#include "MultiConnectorMixin.cti"
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
