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
    \brief DynamicConnectorMixin public header */

#ifndef HH_SENF_PPI_DynamicConnectorMixin_
#define HH_SENF_PPI_DynamicConnectorMixin_ 1

// Custom includes
#include "../config.hh"
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/mpl/if.hpp>
#include "Connectors.hh"
#include "Setup.hh"

#include "DynamicConnectorMixin.mpp"
///////////////////////////////hh.p////////////////////////////////////////

#ifndef SENF_DYNAMIC_CONNECTOR_MAX_ARGS
#define SENF_DYNAMIC_CONNECTOR_MAX_ARGS 3
#endif

namespace senf {
namespace ppi {

#   define BOOST_PP_ITERATION_PARAMS_1 (4, ( \
            0, \
            SENF_DYNAMIC_CONNECTOR_MAX_ARGS, \
            SENF_ABSOLUTE_INCLUDE_PATH(PPI/DynamicConnectorMixin.mpp), \
            2 ))
#   include BOOST_PP_ITERATE()

namespace module {

namespace detail {
    template <class KeyType, class ConnectorType>
    struct DefaultDynamicContainer 
    { typedef boost::ptr_map<KeyType, ConnectorType> type; };

    template <class ConnectorType>
    struct DefaultDynamicContainer<void,ConnectorType> 
    { typedef boost::ptr_vector<ConnectorType> type; };

    template <class ConnectorType>
    struct DynamicDisableType
        : public boost::mpl::if_< 
              boost::is_base_of<connector::InputConnector, ConnectorType>,
              ppi::detail::DisableStandardInput, ppi::detail::DisableStandardOutput >
    {};
}

    /** \brief Dynamic connector management

        Provide modules with support for dynamic connectors.

        \li A module might have dynamic input or output connectors
        \li Creating a new connection might take an argument
        \li Connectors are stored either in a vector or a map
        
        Workflow:
        \li Connectors are created by the helper.
        \li Connector is passed to Self::connectorSetup. This returns either void or the map
            key. connectorSetup must setup internal routing and callbacks.
        \li Connector inserted into container.

        connectorSetup may take additional arguments besides reference to connector. These arguments
        are taken from the trailing ppi::connect call arguments.

        The list manager will insert the new connector at the end of the list BEFORE calling
        connetorSetup. This allows the setup routine to manipulate the position.
     */
    template <class Self, 
              class ConnectorType, 
              class KeyType=void, 
              class ContainerType=typename detail::DefaultDynamicContainer<
                                               KeyType,ConnectorType>::type>
    class DynamicConnectorMixin 
        : private detail::DynamicDisableType<ConnectorType>::type
    {
    public:
        typedef ConnectorType DynamicConnector;

    protected:
        typedef ContainerType DynamicConnectorContainer;
        ContainerType & connectors();

    private:
#       define BOOST_PP_ITERATION_PARAMS_1 (4, ( \
            0, \
            SENF_DYNAMIC_CONNECTOR_MAX_ARGS, \
            SENF_ABSOLUTE_INCLUDE_PATH(PPI/DynamicConnectorMixin.mpp), \
            1 ))
#       include BOOST_PP_ITERATE()
        
        ContainerType connectors_;
    };

    template <class Self,
              class ConnectorType,
              class ContainerType>
    class DynamicConnectorMixin<Self, ConnectorType, void, ContainerType>
        : private detail::DynamicDisableType<ConnectorType>::type
    {
    public:
        typedef ConnectorType DynamicConnector;
        
    protected:
        typedef ContainerType ConnectorContainer;
        ContainerType & connectors();

    private:
#       define BOOST_PP_ITERATION_PARAMS_1 (4, ( \
            0, \
            SENF_DYNAMIC_CONNECTOR_MAX_ARGS, \
            SENF_ABSOLUTE_INCLUDE_PATH(PPI/DynamicConnectorMixin.mpp), \
            1 ))
#       include BOOST_PP_ITERATE()
        
        ContainerType connectors_;
    };
        
}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "DynamicConnectorMixin.cci"
//#include "DynamicConnectorMixin.ct"
#include "DynamicConnectorMixin.cti"
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
