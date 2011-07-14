// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief Jack public header */

#ifndef HH_SENF_PPI_Jack_
#define HH_SENF_PPI_Jack_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/type_traits.hpp>
#include "Connectors.hh"
#include "Setup.hh"

//#include "Jack.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace connector {

    /** \brief Connector Jack base class
        \see \ref ppi_jacks */
    class Jack
        : private boost::noncopyable, private ppi::detail::DisableStandardConnect
    {};

    /** \brief Jack referencing an ActiveInput
        \see \ref ppi_jacks */
    class GenericActiveInputJack
        : public Jack
    {
    public:
        explicit GenericActiveInputJack(GenericActiveInput & input);

        GenericActiveInput & connector(); ///< Get referenced connector

        void reset(GenericActiveInput & input); ///< Change connector
                                        /**< Will update any existing connection accordingly */

    private:
        GenericActiveInput * input_;
    };

    /** \brief Jack referencing an ActiveOutput
        \see \ref ppi_jacks */
    class GenericActiveOutputJack
        : public Jack
    {
    public:
        explicit GenericActiveOutputJack(GenericActiveOutput & output);

        GenericActiveOutput & connector(); ///< Get referenced connector

        void reset(GenericActiveOutput & output); ///< Change connector
                                        /**< Will update any existing connection accordingly */

    private:
        GenericActiveOutput * output_;
    };

    /** \brief Jack referencing a PassiveInput
        \see \ref ppi_jacks */
    class GenericPassiveInputJack
        : public Jack
    {
    public:
        explicit GenericPassiveInputJack(GenericPassiveInput & input);

        GenericPassiveInput & connector(); ///< Get referenced connector

        void reset(GenericPassiveInput & input); ///< Change connector
                                        /**< Will update any existing connection accordingly */

    private:
        GenericPassiveInput * input_;
    };

    /** \brief Jack referencing a PassiveOutput
        \see \ref ppi_jacks */
    class GenericPassiveOutputJack
        : public Jack
    {
    public:
        explicit GenericPassiveOutputJack(GenericPassiveOutput & output);

        GenericPassiveOutput & connector(); ///< Get referenced connector

        void reset(GenericPassiveOutput & output); ///< Change connector
                                        /**< Will update any existing connection accordingly */

    private:
        GenericPassiveOutput * output_;
    };

    /** \brief Jack with packet type referencing an ActiveInput
        \see \ref ppi_jacks */
    template <class PacketType=Packet>
    class ActiveInputJack
        : public GenericActiveInputJack
    {
    public:
        explicit ActiveInputJack(ActiveInput<PacketType> & input);
        explicit ActiveInputJack(ActiveInput<> & input);

        explicit ActiveInputJack(ActiveInputJack & input);
        explicit ActiveInputJack(ActiveInputJack<> & input);

        void reset(ActiveInput<PacketType> & input);
        void reset(ActiveInput<> & input);

        void reset(ActiveInputJack & input);
        void reset(ActiveInputJack<> & input);
    };

#ifndef DOXYGEN

    template <>
    class ActiveInputJack<Packet>
        : public GenericActiveInputJack
    {
    public:
        explicit ActiveInputJack(GenericActiveInput & input);
        explicit ActiveInputJack(GenericActiveInputJack input);

        void reset(GenericActiveInput & input);
        void reset(GenericActiveInputJack input);
    };

#endif

    /** \brief Jack with packet type referencing an ActiveOutput
        \see \ref ppi_jacks */
    template <class PacketType=Packet>
    class ActiveOutputJack
        : public GenericActiveOutputJack
    {
    public:
        explicit ActiveOutputJack(ActiveOutput<PacketType> & output);
        explicit ActiveOutputJack(ActiveOutput<> & output);

        explicit ActiveOutputJack(ActiveOutputJack & output);
        explicit ActiveOutputJack(ActiveOutputJack<> & output);

        void reset(ActiveOutput<PacketType> & output);
        void reset(ActiveOutput<> & output);

        void reset(ActiveOutputJack & output);
        void reset(ActiveOutputJack<> & output);
    };

#ifndef DOXYGEN

    template <>
    class ActiveOutputJack<Packet>
        : public GenericActiveOutputJack
    {
    public:
        explicit ActiveOutputJack(GenericActiveOutput & output);
        explicit ActiveOutputJack(GenericActiveOutputJack & output);

        void reset(GenericActiveOutput & output);
        void reset(GenericActiveOutputJack & output);
    };

#endif

    /** \brief Jack with packet type referencing a PassiveInput
        \see \ref ppi_jacks */
    template <class PacketType=Packet>
    class PassiveInputJack
        : public GenericPassiveInputJack
    {
    public:
        explicit PassiveInputJack(PassiveInput<PacketType> & input);
        explicit PassiveInputJack(PassiveInput<> & input);

        explicit PassiveInputJack(PassiveInputJack & input);
        explicit PassiveInputJack(PassiveInputJack<> & input);

        void reset(PassiveInput<PacketType> & input);
        void reset(PassiveInput<> & input);

        void reset(PassiveInputJack & input);
        void reset(PassiveInputJack<> & input);
    };

#ifndef DOXYGEN

    template <>
    class PassiveInputJack<Packet>
        : public GenericPassiveInputJack
    {
    public:
        explicit PassiveInputJack(GenericPassiveInput & input);
        explicit PassiveInputJack(GenericPassiveInputJack & input);

        void reset(GenericPassiveInput & input);
        void reset(GenericPassiveInputJack & input);
    };

#endif

    /** \brief Jack with packet type referencing a PassiveOutput
        \see \ref ppi_jacks */
    template <class PacketType=Packet>
    class PassiveOutputJack
        : public GenericPassiveOutputJack
    {
    public:
        explicit PassiveOutputJack(PassiveOutput<PacketType> & output);
        explicit PassiveOutputJack(PassiveOutput<> & output);

        explicit PassiveOutputJack(PassiveOutputJack & output);
        explicit PassiveOutputJack(PassiveOutputJack<> & output);

        void reset(PassiveOutput<PacketType> & output);
        void reset(PassiveOutput<> & output);

        void reset(PassiveOutputJack & output);
        void reset(PassiveOutputJack<> & output);
    };

#ifndef DOXYGEN

    template <>
    class PassiveOutputJack<Packet>
        : public GenericPassiveOutputJack
    {
    public:
        explicit PassiveOutputJack(GenericPassiveOutput & output);
        explicit PassiveOutputJack(GenericPassiveOutputJack & output);

        void reset(GenericPassiveOutput & output);
        void reset(GenericPassiveOutputJack & output);
    };

#endif
}

#ifndef DOXYGEN

    template <class T>
    void connect(connector::GenericActiveOutputJack & source, T & target,
                 typename boost::disable_if< boost::is_base_of<connector::Jack, T> >::type * = 0);
    template <class T>
    void connect(connector::GenericPassiveOutputJack & source, T & target,
                 typename boost::disable_if< boost::is_base_of<connector::Jack, T> >::type * = 0);

    template <class T>
    void connect(T & source, connector::GenericActiveInputJack & target,
                 typename boost::disable_if< boost::is_base_of<connector::Jack, T> >::type * = 0);
    template <class T>
    void connect(T & source, connector::GenericPassiveInputJack & target,
                 typename boost::disable_if< boost::is_base_of<connector::Jack, T> >::type * = 0);

    void connect(connector::GenericActiveOutputJack & source,
                 connector::GenericPassiveInputJack & target);
    void connect(connector::GenericPassiveOutputJack & source,
                 connector::GenericActiveInputJack & target);

    void connect(connector::GenericActiveOutputJack & source,
                 connector::GenericPassiveInput & target);
    void connect(connector::GenericPassiveOutputJack & source,
                 connector::GenericActiveInput & target);

    void connect(connector::GenericActiveOutput & source,
                 connector::GenericPassiveInputJack & target);
    void connect(connector::GenericPassiveOutput & source,
                 connector::GenericActiveInputJack & target);

#endif
}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Jack.cci"
//#include "Jack.ct"
#include "Jack.cti"
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
