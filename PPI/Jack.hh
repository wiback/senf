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
    \brief Jack public header */

#ifndef HH_SENF_PPI_Jack_
#define HH_SENF_PPI_Jack_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/type_traits.hpp>
#include "Connectors.hh"
#include "Setup.hh"

//#include "Jack.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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

    private:
        GenericActiveInput & input_;
    };

    /** \brief Jack referencing an ActiveOutput
        \see \ref ppi_jacks */
    class GenericActiveOutputJack
        : public Jack
    {
    public:
        explicit GenericActiveOutputJack(GenericActiveOutput & output);

        GenericActiveOutput & connector(); ///< Get referenced connector

    private:
        GenericActiveOutput & output_;
    };

    /** \brief Jack referencing a PassiveInput
        \see \ref ppi_jacks */
    class GenericPassiveInputJack
        : public Jack
    {
    public:
        explicit GenericPassiveInputJack(GenericPassiveInput & input);

        GenericPassiveInput & connector(); ///< Get referenced connector

    private:
        GenericPassiveInput & input_;
    };
    
    /** \brief Jack referencing a PassiveOutput
        \see \ref ppi_jacks */
    class GenericPassiveOutputJack
        : public Jack
    {
    public:
        explicit GenericPassiveOutputJack(GenericPassiveOutput & output);

        GenericPassiveOutput & connector(); ///< Get referenced connector

    private:
        GenericPassiveOutput & output_;
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
    };

#ifndef DOXYGEN

    template <>
    class ActiveInputJack<Packet>
        : public GenericActiveInputJack
    {
    public:
        explicit ActiveInputJack(GenericActiveInput & input);
        explicit ActiveInputJack(GenericActiveInputJack input);
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
    };

#ifndef DOXYGEN

    template <>
    class ActiveOutputJack<Packet>
        : public GenericActiveOutputJack
    {
    public:
        explicit ActiveOutputJack(GenericActiveOutput & output);
        explicit ActiveOutputJack(GenericActiveOutputJack & output);
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
    };

#ifndef DOXYGEN

    template <>
    class PassiveInputJack<Packet>
        : public GenericPassiveInputJack
    {
    public:
        explicit PassiveInputJack(GenericPassiveInput & input);
        explicit PassiveInputJack(GenericPassiveInputJack & input);
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
    };

#ifndef DOXYGEN

    template <>
    class PassiveOutputJack<Packet>
        : public GenericPassiveOutputJack
    {
    public:
        explicit PassiveOutputJack(GenericPassiveOutput & output);
        explicit PassiveOutputJack(GenericPassiveOutputJack & output);
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

///////////////////////////////hh.e////////////////////////////////////////
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
