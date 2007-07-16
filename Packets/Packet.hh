// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief Packet public header */

#ifndef HH_Packet_
#define HH_Packet_ 1

// Custom includes
#include <boost/operators.hpp>

#include "Utils/Exception.hh"
#include "Utils/SafeBool.hh"
#include "PacketInterpreter.hh"

//#include "Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    template <class PackeType> class ConcretePacket;
    
    /** \brief
      */
    class Packet
        : public SafeBool<Packet>,
          public boost::equality_comparable<Packet>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        typedef void type;
        typedef senf::detail::packet::iterator iterator;
        typedef senf::detail::packet::const_iterator const_iterator;
        typedef senf::detail::packet::size_type size_type;
        typedef senf::detail::packet::difference_type difference_type;
        typedef senf::detail::packet::byte byte;
        typedef PacketInterpreterBase::factory_t factory_t;

        enum NoInit_t { noinit };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // default copy constructor
        // default copy assignment
        // default destructor
        
        Packet();
        Packet clone() const;

        // conversion constructors

        template <class PacketType>
        Packet(ConcretePacket<PacketType> packet);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name Interpreter chain access
        ///@{

                                     Packet      next() const;
        template <class OtherPacket> OtherPacket next() const;
        template <class OtherPacket> OtherPacket next(NoThrow_t) const;
        template <class OtherPacket> OtherPacket findNext() const;
        template <class OtherPacket> OtherPacket findNext(NoThrow_t) const;

                                     Packet      prev() const;
        template <class OtherPacket> OtherPacket prev() const;
        template <class OtherPacket> OtherPacket prev(NoThrow_t) const;
        template <class OtherPacket> OtherPacket findPrev() const;
        template <class OtherPacket> OtherPacket findPrev(NoThrow_t) const;

                                     Packet      first() const;
        template <class OtherPacket> OtherPacket first() const;
        template <class OtherPacket> OtherPacket first(NoThrow_t) const;

                                     Packet      last() const;
        template <class OtherPacket> OtherPacket last() const;
        template <class OtherPacket> OtherPacket last(NoThrow_t) const;


        template <class OtherPacket> OtherPacket parseNextAs() const;
                                     Packet      parseNextAs(factory_t factory) const;
        template <class OtherPacket> bool        is() const;
        template <class OtherPacket> OtherPacket as() const;

        Packet append(Packet packet) const;

        ///@}

        ///\name Data access
        ///@{

        PacketData & data() const;
        size_type size() const;
        
        ///@}

        ///\name Other methods
        ///@{

        bool operator==(Packet other) const;
        bool boolean_test() const;

        void finalize() const;

        void dump(std::ostream & os) const;

        TypeIdValue typeId() const;
        factory_t factory() const;
        
        ///@}

    protected:
        explicit Packet(PacketInterpreterBase::ptr packet);

        PacketInterpreterBase::ptr ptr() const;

    private:
        Packet checkNext() const;
        Packet checkLast() const;
        
        PacketInterpreterBase::ptr packet_;
        
        template <class PacketType>
        friend class ConcretePacket;
    };

    /** \brief
      */
    template <class PacketType>
    class ConcretePacket 
        : public Packet
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        typedef PacketType type;
        typedef PacketInterpreter<PacketType> interpreter;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // default copy constructor
        // default copy assignment
        // default destructor
        // no conversion constructors

        ConcretePacket();

        static factory_t factory();

        // Create completely new packet

        static ConcretePacket create();
        static ConcretePacket create(NoInit_t);
        static ConcretePacket create(size_type size);
        static ConcretePacket create(size_type size, NoInit_t);
        template <class ForwardReadableRange>
        static ConcretePacket create(ForwardReadableRange const & range);

        // Create packet as new packet after a given packet

        static ConcretePacket createAfter(Packet packet);
        static ConcretePacket createAfter(Packet packet, NoInit_t);
        static ConcretePacket createAfter(Packet packet, size_type size);
        static ConcretePacket createAfter(Packet packet, size_type size, NoInit_t);
        template <class ForwardReadableRange>
        static ConcretePacket createAfter(Packet packet, 
                                          ForwardReadableRange const & range);

        // Create packet as new packet (header) before a given packet

        static ConcretePacket createBefore(Packet packet);
        static ConcretePacket createBefore(Packet packet, NoInit_t);

        // Create a clone of the current packet

        ConcretePacket clone() const;

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        // Field access

        typename interpreter::parser * operator->() const;

    protected:

    private:
        ConcretePacket(typename interpreter::ptr packet_);
        
        typename interpreter::ptr ptr() const;

        friend class Packet;
        friend class PacketInterpreter<PacketType>;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#include "Packet.cci"
#include "Packet.ct"
#include "Packet.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
