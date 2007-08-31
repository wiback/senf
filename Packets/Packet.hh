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

#include "../Utils/Exception.hh"
#include "../Utils/SafeBool.hh"
#include "PacketInterpreter.hh"

//#include "Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \defgroup packet_module Packet Handling

        The basic groundwork of the Packet library is the packet handling:

        \li The packet classes provide access to a chain of packet headers (more generically called
            interpreters).
        \li They automatically manage the required memory resources and the shared packet data.

        \section packet_module_chain The Interpreter Chain

        The central data structure for a packet is the interpreter chain

        \image html structure.png The Interpreter Chain

        This image depicts a packet with several headers. Each interpreter is responsible for a
        specific sub-range of the complete packet. This range always \e includes the packets payload
        (This is, why we call the data structure interpreter and not header: The interpreter is
        responsible for interpreting a range of the packet according to a specific protocol), the
        packet interpreters are nested inside each other.
    
        For each interpreter, this structure automatically divides the packet into three areas (each
        of which are optional): The header, the payload and the trailer. Every packet will have
        either a header or a payload section while most don't have a trailer.

        As user of the library you always interact with the chain through one (or more) of the
        interpreters. The interpreter provides methods to traverse to the following or preceding
        header (interpreter) and provides two levels of access to the packet data: Generic low-level
        access in the form of an STL compatible sequence and access to the parsed fields which are
        provided by the parser associated with the concrete packet type.

        \section packet_module_management Resource Management

        The interface to the packet library is provided using a handle class (\ref Packet for
        generic, protocol agnostic access and \ref ConcretePacket derived from \ref Packet to access
        a specific protocol). This handle automatically manages the resources associated with the
        packet (the interpreter chain and the data storage holding the packet data). The resources
        are automatically released when the last packet handle referencing a specific packet is
        destroyed.

        \implementation The packet chain is provided on two levels: The internal representation \ref
            PacketInterpreterBase and \ref PacketInterpreter which are referenced by the Handle
            classes \ref Packet and \ref ConcretePacket. \n
            The internal representation classes are pertinent in the sense, that they exist
            regardless of the existence of a handle referencing them (as long as the packet
            exists). Still the interpreter chain is lazy and packet interpreters beside the first
            are only created dynamically when accessed (this is implemented in the handle not in the
            internal representation). \n
            The packet interpreters make use of a pool allocator. This provides extremely efficient
            creation and destruction of packet interpreter's and removes the dynamic memory
            management overhead from the packet interpreter management. The packet implementation
            class (\ref PacketImpl which holds the packet data itself) however is still dynamically
            managed (however there is only a single instance for each packet).
     */

    template <class PackeType> class ConcretePacket;

    ///\addtogroup packet_module
    ///@{
    
    /** \brief Main Packet class

        Packet is the main externally visible class of the packet library. Packet is a handle into
        the internal packet representation. From Packet you may access the data of that specific
        sub-packet/header/interpreter and navigate to the neighboring
        sub-packets/headers/interpreters.

        Packet is protocol agnostic. This class only provides non-protocol dependent members. To
        access the protocol specific features of a packet (like header fields) the ConcretePacket
        class extending Packet is provided.

        \section packet_semantics Semantics
        
        All operations accessing the data of \c this packet in some way will ignore any preceding
        packets/headers/interpreters in the chain. It does not matter, whether a given packet is
        taken from the middle or the beginning of the chain, all operations (except those explicitly
        accessing the chain of course) should work the same.
        
        This especially includes members like clone() or append(): clone() will clone \e only from
        \c this packet until the end of the chain, append() will append the given packet \e ignoring
        any possibly preceding packets/headers/interpreters.

        In the same way, the data() member provides an STL-sequence compatible view of the packet
        data. This only includes the data which is part of \c this packet including header, trailer
        \e and payload but \e not the headers or trailers of packets \e before \c this packet in the
        packet/header/interpreter chain (nonetheless, this data overlaps with the data of other
        packets).

        Several members are member templates taking an \a OtherPacket template parameter. This
        parameter must be the ConcretePacket instantiation associated with some concrete packet type
        (protocol). For each implemented protocol, typedefs should be provided for these
        instantiations (Example: \ref EthernetPacket is a typedef for
        \ref ConcretePacket < \ref EthernetPacketType >).

        \see 
            \ref ConcretePacket for the type specific interface\n
            \ref PacketData for the sequence interface\n
            \ref packetparser for a specification of the parser interface
      */
    class Packet
        : public SafeBool<Packet>,
          public boost::equality_comparable<Packet>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        typedef void type;              ///< Type of the packet.
        typedef senf::detail::packet::size_type size_type; ///< Unsigned type to represent packet size
        typedef PacketInterpreterBase::factory_t factory_t; ///< Packet factory type (see below)

                                        /// Special argument flag
                                        /** Used in some ConcretePacket constructors */
        enum NoInit_t { noinit };       

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // default copy constructor
        // default copy assignment
        // default destructor
        
        Packet();                       ///< Create uninitialized packet handle
                                        /**< An uninitialized handle is not valid(). It does not
                                             allow any operation except assignment and checking for
                                             validity. */
        Packet clone() const;           ///< Create copy packet
                                        /**< clone() will create a complete copy the packet. The
                                             returned packet will have the same data and packet
                                             chain. It does however not share any data with the
                                             original packet. */

        // conversion constructors

        template <class PacketType>     
        Packet(ConcretePacket<PacketType> packet); ///< Copy-construct Packet from ConcretePacket
                                        /**< This constructor allows to convert an arbitrary
                                             ConcretePacket into a general Packet, loosing the
                                             protocol specific interface. */

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name Interpreter chain access
        ///@{

                                     Packet      next() const; 
                                        ///< Get next packet in chain
        template <class OtherPacket> OtherPacket next() const; 
                                        ///< Get next packet of given type in chain
                                        /**< \throws InvalidPacketChainException if no such packet
                                             is found */
        template <class OtherPacket> OtherPacket next(NoThrow_t) const; 
                                        ///< Get next packet of given type in chain
                                        /**< \param[in] nothrow This argument always has the value
                                             \c senf::nothrow
                                             \returns in-valid() packet, if no such packet is found */
        template <class OtherPacket> OtherPacket findNext() const;
                                        ///< Find next packet of given type in chain
                                        /**< findNext() is like next(), it will however return \c
                                             *this if it is of the given type. 
                                             \throws InvalidPacketChainException if no such packet
                                                 is found */
        template <class OtherPacket> OtherPacket findNext(NoThrow_t) const;
                                        ///< Find next packet of given type in chain
                                        /**< findNext() is like next(), it will however return \c
                                             *this if it is of the given type.
                                             \param[in] nothrow This argument always has the value
                                             \c senf::nothrow
                                             \returns in-valid() packet, if no such packet is found */
        

                                     Packet      prev() const; 
                                        ///< Get previous packet in chain
        template <class OtherPacket> OtherPacket prev() const; 
                                        ///< Get previous packet of given type in chain
                                        /**< \throws InvalidPacketChainException if no such packet
                                             is found */
        template <class OtherPacket> OtherPacket prev(NoThrow_t) const;
                                        ///< Get previous packet of given type in chain
                                        /**< \param[in] nothrow This argument always has the value
                                             \c senf::nothrow
                                             \returns in-valid() packet, if no such packet is found */
        template <class OtherPacket> OtherPacket findPrev() const;
                                        ///< Find previous packet of given type in chain
                                        /**< findPrev() is like prev(), it will however return \c
                                             *this if it is of the type 
                                             \throws InvalidPacketChainException if no such packet
                                                 is found */
        template <class OtherPacket> OtherPacket findPrev(NoThrow_t) const;
                                        ///< Find previous packet of given type in chain
                                        /**< findPrev() is like prev(), it will however return \c
                                             *this if it is of the type 
                                             \param[in] nothrow This argument always has the value
                                             \c senf::nothrow
                                             \returns in-valid() packet, if no such packet is found */


                                     Packet      first() const;
                                        ///< Return first packet in chain
        template <class OtherPacket> OtherPacket first() const;
                                        ///< Return first packet of given type in chain
                                        /**< \throws InvalidPacketChainException if no such packet
                                             is found */
        template <class OtherPacket> OtherPacket first(NoThrow_t) const;
                                        ///< Return first packet of given type in chain
                                        /**< \param[in] nothrow This argument always has the value
                                             \c senf::nothrow
                                             \returns in-valid() packet, if no such packet is found */

                                     Packet      last() const;
                                        ///< Return last packet in chain
        template <class OtherPacket> OtherPacket last() const;
                                        ///< Return last packet of given type in chain
                                        /**< \throws InvalidPacketChainException if no such packet
                                             is found */
        template <class OtherPacket> OtherPacket last(NoThrow_t) const;
                                        ///< Return last packet of given type in chain
                                        /**< \param[in] nothrow This argument always has the value
                                             \c senf::nothrow
                                             \returns in-valid() packet, if no such packet is found */


        template <class OtherPacket> OtherPacket parseNextAs() const;
                                        ///< Parse payload as given by \a OtherPacket and add packet
                                        /**< parseNextAs() will throw away the packet chain after
                                             the current packet if necessary. It will then parse the
                                             payload section of \c this packet as given by \a
                                             OtherPacket. The new packet is added to the chain after
                                             \c this.
                                             \returns new packet instance sharing the same data and
                                                 placed after \c this packet in the chain. */
                                     Packet      parseNextAs(factory_t factory) const;
                                        ///< Parse payload as given by \a factory and add packet
                                        /**< parseNextAs() will throw away the packet chain after
                                             the current packet if necessary. It will then parse the
                                             payload section of \c this packet as given by \a
                                             OtherPacket. The new packet is added to the chain after
                                             \c this.
                                             \returns new packet instance sharing the same data and
                                                 placed after \c this packet in the chain. */
        template <class OtherPacket> bool        is() const;
                                        ///< Check, whether \c this packet is of the given type
        template <class OtherPacket> OtherPacket as() const;
                                        ///< Cast current packet to the given type
                                        /**< This operations returns a handle to the same packet
                                             header/interpreter however cast to the given
                                             ConcretePacket type. <b>This conversion is
                                             unchecked</b>. If the packet really is of a different
                                             type, this will wreak havoc with the packet
                                             data-structures. You can validate whether the
                                             conversion is valid using is(). */

        Packet append(Packet packet) const; ///< Append the given packet to \c this packet
                                        /**< This operation will replace the payload section of \c
                                             this packet with \a packet. This operation will replace
                                             the packet chain after \c this packet with a clone of
                                             \a packet and will replace the raw data of the payload
                                             of \c this with the raw data if \a packet.
                                             \returns Packet handle to the cloned \a packet, placed
                                                 after \c this in the packet/header/interpreter
                                                 chain. */

        ///@}

        ///\name Data access
        ///@{

        PacketData & data() const;      ///< Access the packets raw data container
        size_type size() const;         ///< Return size of packet in bytes
                                        /**< This size does \e not include the size of any preceding
                                             headers/packets/interpreters. It does however include
                                             \c this packets payload. */
        
        ///@}

        ///\name Other methods
        ///@{

        bool operator==(Packet other) const; ///< Check for packet identity
                                        /**< Two packet handles compare equal if they really are the
                                             same packet header in the same packet chain. */
        bool boolean_test() const;      ///< Check, whether the packet is valid()
                                        /**< \see valid() */
        bool valid() const;             ///< Check, whether the packet is valid()
                                        /**< An in-valid() packet does not allow any operation
                                             except checking for validity and assignment. in-valid()
                                             packets serve the same role as 0-pointers. */
        

        void finalize() const;          ///< Update calculated fields
                                        /**< This call will update all calculated fields of the
                                             packet after it has been created or changed. This
                                             includes checksums, payload size fields or other
                                             fields, which can be set from other information in the
                                             packet. Each concrete packet type should document,
                                             which fields are set by finalize().

                                             finalize() will automatically process all
                                             packets/headers/interpreters from the end of the chain
                                             backwards up to \c this. */

        void dump(std::ostream & os) const; ///< Write out a printable packet representation
                                        /**< This method is provided mostly to help debugging packet
                                             problems. Each concrete packet should implement a dump
                                             method writing out all fields of the packet in a
                                             readable representation. dump() will call this member
                                             for each packet/header/interpreter in the chain from \c
                                             this packet up to the end of the chain. */

        TypeIdValue typeId() const;     ///< Get id of \c this packet
                                        /**< This value is used e.g. in the packet registry to
                                             associate packet types with other information.
                                             \returns A type holding the same information as a
                                                 type_info object, albeit assignable */
        factory_t factory() const;      ///< Return factory instance of \c this packet
                                        /**< The returned factory instance can be used to create new
                                             packets of the given type without knowing the concrete
                                             type of the packet. The value may be stored away for
                                             later use if needed. */
        
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

    /** \brief Protocol specific packet handle

        The ConcretePacket template class extends Packet to provide protocol/packet type specific
        aspects. These are packet constructors and access to the parsed packet fields.

        The \c PacketType template argument to ConcretePacket is a protocol specific and internal
        policy class which defines the protocol specific behavior. To access a specific type of
        packet, the library provides corresponding typedefs of ConcretePacket < \a SomePacketType >
        (e.g. \ref EthernetPacket as typedef for \ref ConcretePacket < \ref EthernetPacketType >).

        The new members provided by ConcretePacket over packet are mostly comprised of the packet
        constructors. These come in three major flavors:
        
        \li The create() family of constructors will create completely new packets.
        \li The createAfter() family of constructors will create new packets (with new data for the
            packet) \e after a given existing packet.
        \li The createBefore()  family of constructors will create new packets (again with new data)
            \e before a given existing packet.
        
        Whereas create() will create a completely new packet with it's own chain and data storage,
        createAfter() and createBefore() extend a packet with additional
        headers/interpreters. createAfter() will set the payload of the given packet to the new
        packet whereas createBefore() will create a new packet with the existing packet as it's
        payload. 

        createAfter() differs from Packet::parseNextAs() in that the former creates a new packet \e
        replacing any possibly existing data whereas the latter will interpret the already \e
        existing data as given by the type argument.
        
        \see \ref PacketTypeBase for a specification of the interface to be provided by the \a
            PacketType policy class.
      */
    template <class PacketType>
    class ConcretePacket 
        : public Packet
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        typedef PacketType type;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // default copy constructor
        // default copy assignment
        // default destructor
        // no conversion constructors

        ConcretePacket();               ///< Create uninitialized packet handle
                                        /**< An uninitialized handle is not valid(). It does not
                                             allow any operation except assignment and checking for
                                             validity. */

        static factory_t factory();     ///< Return factory for packets of specific type
                                        /**< This \e static member is like Packet::factory() for a
                                             specific packet of type \a PacketType */

        // Create completely new packet

        static ConcretePacket create(); ///< Create default initialized packet
                                        /**< The packet will be initialized to it's default empty
                                             state. */
        static ConcretePacket create(NoInit_t); ///< Create uninitialized empty packet
                                        /**< This will create a completely empty and uninitialized
                                             packet with <tt>size() == 0</tt>.
                                             \param[in] noinit This parameter must always have the
                                                 value \c senf::noinit. */
        static ConcretePacket create(size_type size); ///< Create default initialized packet
                                        /**< This member will create a default initialized packet
                                             with the given size. If the size parameter is smaller
                                             than the minimum allowed packet size an exception will
                                             be thrown.
                                             \param[in] size Size of the packet to create in bytes.
                                             \throws TruncatedPacketException if \a size is smaller
                                                 than the smallest permissible size for this type of
                                                 packet. */
        static ConcretePacket create(size_type size, NoInit_t); ///< Create uninitialized packet
                                        /**< Creates an uninitialized (all-zero) packet of the exact
                                             given size. 
                                             \param[in] size Size of the packet to create in bytes
                                             \param[in] noinit This parameter must always have the
                                                 value \c senf::noinit. */
        template <class ForwardReadableRange>
        static ConcretePacket create(ForwardReadableRange const & range); 
                                        ///< Create packet from given data
                                        /**< The packet will be created from a copy of the given
                                             data. The data from the range will be copied directly
                                             into the packet representation. The data will \e not be
                                             validated in any way.
                                             \param[in] range <a
                                                 href="http://www.boost.org/libs/range/index.html">Boost.Range</a> 
                                                 of data to construct packet from. */

        // Create packet as new packet after a given packet

        static ConcretePacket createAfter(Packet packet); 
                                        ///< Create default initialized packet after \a packet
                                        /**< The packet will be initialized to it's default empty
                                             state. It will be appended as next header/interpreter
                                             after \a packet in that packets interpreter chain.
                                             \param[in] packet Packet to append new packet to. */
        static ConcretePacket createAfter(Packet packet, NoInit_t);
                                        ///< Create uninitialized empty packet after\a packet
                                        /**< This will create a completely empty and uninitialized
                                             packet with <tt>size() == 0</tt>. It will be appended
                                             as next header/interpreter after \a packet in that
                                             packets interpreter chain.
                                             \param[in] packet Packet to append new packet to.
                                             \param[in] noinit This parameter must always have the
                                                 value \c senf::noinit. */
        static ConcretePacket createAfter(Packet packet, size_type size);
                                        ///< Create default initialized packet after \a packet
                                        /**< This member will create a default initialized packet
                                             with the given size. If the size parameter is smaller
                                             than the minimum allowed packet size an exception will
                                             be thrown. It will be appended as next
                                             header/interpreter after \a packet in that packets
                                             interpreter chain.
                                             \param[in] packet Packet to append new packet to.
                                             \param[in] size Size of the packet to create in bytes.
                                             \throws TruncatedPacketException if \a size is smaller
                                                 than the smallest permissible size for this type of
                                                 packet. */
        static ConcretePacket createAfter(Packet packet, size_type size, NoInit_t);
                                        ///< Create uninitialized packet after \a packet
                                        /**< Creates an uninitialized (all-zero) packet of the exact
                                             given size.  It will be appended as next
                                             header/interpreter after \a packet in that packets
                                             interpreter chain.
                                             \param[in] packet Packet to append new packet to.
                                             \param[in] size Size of the packet to create in bytes
                                             \param[in] noinit This parameter must always have the
                                                 value \c senf::noinit. */
        template <class ForwardReadableRange>
        static ConcretePacket createAfter(Packet packet, 
                                          ForwardReadableRange const & range);
                                        ///< Create packet from given data after \a packet
                                        /**< The packet will be created from a copy of the given
                                             data. The data from the range will be copied directly
                                             into the packet representation. The data will \e not be
                                             validated in any way.  It will be appended as next
                                             header/interpreter after \a packet in that packets
                                             interpreter chain.
                                             \param[in] packet Packet to append new packet to.
                                             \param[in] range <a
                                                 href="http://www.boost.org/libs/range/index.html">Boost.Range</a> 
                                                 of data to construct packet from. */

        // Create packet as new packet (header) before a given packet

        static ConcretePacket createBefore(Packet packet); 
                                        ///< Create default initialized packet before \a packet
                                        /**< The packet will be initialized to it's default empty
                                             state. It will be prepended as previous
                                             header/interpreter before \a packet in that packets
                                             interpreter chain.
                                             \param[in] packet Packet to prepend new packet to. */
        static ConcretePacket createBefore(Packet packet, NoInit_t);
                                        ///< Create uninitialized empty packet before \a packet
                                        /**< Creates a completely empty and uninitialized packet. It
                                             will be prepended as previous header/interpreter before
                                             \a packet in that packets interpreter chain.
                                             \param[in] packet Packet to prepend new packet to. */
        
        // Create a clone of the current packet

        ConcretePacket clone() const;

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        // Field access

        typename type::parser * operator->() const; ///< Access packet fields
                                        /**< This operator allows to access the parsed fields of the
                                             packet using the notation <tt>packet->field()</tt>. The
                                             fields of the packet are specified by the PacketType's
                                             \c parser member. 

                                             The members are not strictly restricted to simple field
                                             access. The parser class may have any member which is
                                             needed for full packet access (e.g. checksum validation
                                             / recreation ...)
                                             \see \ref packetparser for the parser interface. */

    protected:

    private:
        typedef PacketInterpreter<PacketType> interpreter;

        ConcretePacket(typename interpreter::ptr packet_);
        
        typename interpreter::ptr ptr() const;

        friend class Packet;
        friend class PacketInterpreter<PacketType>;
    };

    ///@}

}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(SENF_PACKETS_DECL_ONLY) && !defined(HH_Packet_i_)
#define HH_Packet_i_
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
// comment-column: 40
// End:

