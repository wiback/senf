// $Id$
//
// Copyright (C) 2007
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
    \brief Packet public header */

#ifndef HH_SENF_Packets_Packet_
#define HH_SENF_Packets_Packet_ 1

// Custom includes
#include <boost/operators.hpp>
#include <boost/utility.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <senf/Utils/Tags.hh>
#include <senf/Utils/safe_bool.hh>
#include "PacketInterpreter.hh"

//#include "Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \defgroup packet_module Packet Handling

        The basic groundwork of the %Packet library is the packet handling:

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

    /** \brief Main %Packet class

        %Packet is the main externally visible class of the packet library. %Packet is a handle into
        the internal packet representation. From %Packet you may access the data of that specific
        sub-packet/header/interpreter and navigate to the neighboring
        sub-packets/headers/interpreters.

        %Packet is protocol agnostic. This class only provides non-protocol dependent members. To
        access the protocol specific features of a packet (like header fields) the ConcretePacket
        class extending %Packet is provided.

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
            \ref ConcretePacket for the %type specific interface\n
            \ref PacketData for the sequence interface\n
            \ref packetparser for a specification of the parser interface
     */
    class Packet
        : public safe_bool<Packet>,
          public boost::equality_comparable<Packet>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef void type;              ///< Type of the packet.
        typedef senf::detail::packet::size_type size_type;
        ///< Unsigned type to represent packet size
        typedef PacketInterpreterBase::factory_t factory_t; ///< Packet factory type (see below)

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // default copy constructor
        // default copy assignment
        // default destructor

        Packet();                       ///< Create uninitialized packet handle
                                        /**< An uninitialized handle is in - valid(). It does not
                                             allow any operation except assignment and checking for
                                             validity. */
        Packet clone() const;           ///< Create copy packet
                                        /**< clone() will create a complete copy of \c this
                                             packet. The returned packet will have the same data and
                                             packet chain. It does however not share any data with
                                             the original packet. */

        // conversion constructors

        template <class PacketType>
        Packet(ConcretePacket<PacketType> const & packet);
                                        ///< Copy-construct Packet from ConcretePacket
                                        /**< This constructor allows to convert an arbitrary
                                             ConcretePacket into a general Packet, loosing the
                                             protocol specific interface. */

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name Interpreter chain access
        ///@{

        Packet      next() const;       ///< Get next packet in chain
                                        /**< \throws InvalidPacketChainException if no next packet
                                             exists */
        Packet      next(NoThrow_t) const; ///< Get next packet in chain
                                        /**< \returns in - valid() packet if no next packet
                                             exists */
        template <class OtherPacket> OtherPacket next() const;
                                        ///< Get next packet in chain and cast to \a OtherPacket
                                        /**< \throws std::bad_cast if the next() packet is not of
                                             type \a OtherPacket
                                             \throws InvalidPacketChainException if no next packet
                                                 exists */
        template <class OtherPacket> OtherPacket next(NoThrow_t) const;
                                        ///< Get next packet in chain and cast to \a OtherPacket
                                        /**< \returns in - valid() packet if no next packet
                                                 exists or if next() packet is not of
                                                 type \a OtherPacket */
        template <class OtherPacket> OtherPacket find() const;
                                        ///< Search chain forward for packet of type \a OtherPacket
                                        /**< The search will start with the current packet.
                                             \throws InvalidPacketChainException if no packet of
                                                 type \a OtherPacket can be found. */
        template <class OtherPacket> OtherPacket find(NoThrow_t) const;
                                        ///< Search chain forward for packet of type \a OtherPacket
                                        /**< The search will start with the current packet.
                                             \returns in - valid() packet if no packet of type \a
                                                 OtherPacket can be found. */

        Packet      prev() const;       ///< Get previous packet in chain
                                        /**< \throws InvalidPacketChainException if no previous
                                             packet exists */
        Packet      prev(NoThrow_t) const; ///< Get previous packet in chain
                                        /**< \returns in - valid() packet if no previous packet
                                             exists */
        template <class OtherPacket> OtherPacket prev() const;
                                        ///< Get previous packet in chain and cast to \a OtherPacket
                                        /**< \throws std::bad_cast, if the previous packet is not of
                                             type \a OtherPacket
                                             \throws InvalidPacketChainException if no previous
                                                 packet exists */
        template <class OtherPacket> OtherPacket prev(NoThrow_t) const;
                                        ///< Get previous packet in chain and cast to \a OtherPacket
                                        /**< \returns in - valid() packet if no previous packet
                                                 exists or if the previous packet is not of
                                                 type \a OtherPacket */
        template <class OtherPacket> OtherPacket rfind() const;
                                        ///< Search chain backwards for packet of type \a OtherPacket
                                        /**< The search will start with the current packet.
                                             \throws InvalidPacketChainException if no packet of
                                                 type \a OtherPacket can be found. */
        template <class OtherPacket> OtherPacket rfind(NoThrow_t) const;
                                        ///< Search chain backwards for packet of type \a OtherPacket
                                        /**< The search will start with the current packet.
                                             \returns in - valid() packet if no packet of type \a
                                                 OtherPacket can be found. */


        Packet      first() const;      ///< Return first packet in chain
        template <class OtherPacket> OtherPacket first() const;
                                        ///< Return first packet in chain and cast
                                        /**< \throws std::bad_cast if the first() packet is not of
                                             type \a OtherPacket */

        Packet      last() const;       ///< Return last packet in chain
        template <class OtherPacket> OtherPacket last() const;
                                        ///< Return last packet in chain and cast
                                        /**< \throws std::bad_cast if the last() packet is not of
                                             type \a OtherPacket  */


        template <class OtherPacket> OtherPacket parseNextAs() const;
                                        ///< Interpret payload of \c this as \a OtherPacket
                                        /**< parseNextAs() will throw away the packet chain after
                                             the current packet if necessary. It will then parse the
                                             payload section of \c this packet as given by \a
                                             OtherPacket. The new packet is added to the chain after
                                             \c this.
                                             \returns new packet instance sharing the same data and
                                                 placed after \c this packet in the chain.
                                             \throws InvalidPacketChainException if no next packet
                                                 header is allowed (viz. nextPacketRange() of the
                                                 the current PacketType returns no_range() ) */
        Packet      parseNextAs(factory_t factory) const;
                                        ///< Interpret payload of \c this as \a factory type packet
                                        /**< parseNextAs() will throw away the packet chain after
                                             the current packet if necessary. It will then parse the
                                             payload section of \c this packet as given by \a
                                             factory. The new packet is added to the chain after
                                             \c this.
                                             \returns new packet instance sharing the same data and
                                                 placed after \c this packet in the chain.
                                             \throws InvalidPacketChainException if no next packet
                                                 header is allowed (viz. nextPacketRange() of the
                                                 the current PacketType returns no_range() ) */

        template <class OtherPacket> bool        is() const;
                                        ///< Check, whether \c this packet is of the given type
        template <class OtherPacket> OtherPacket as() const;
                                        ///< Cast current packet to the given type
                                        /**< This operations returns a handle to the same packet
                                             header/interpreter however upcast to the given
                                             ConcretePacket type which have been instantiated
                                             before.
                                             \throws std::bad_cast if the current packet is not of
                                                 type \a OtherPacket */
        template <class OtherPacket> OtherPacket as(NoThrow_t) const;
                                        ///< Cast current packet to the given type
                                        /**< This operations returns a handle to the same packet
                                             header/interpreter however upcast to the given
                                             ConcretePacket type which have been instantiated
                                             before.
                                             \warning You must make absolutely sure that the packet
                                             is of the given type. If not, calling this member
                                             crashes your program in a unkindly way. */

        Packet append(Packet const & packet) const; ///< Append the given packet to \c this packet
                                        /**< This operation will replace the payload section of \c
                                             this packet with \a packet. This operation will replace
                                             the packet chain after \c this packet with a clone of
                                             \a packet and will replace the raw data of the payload
                                             of \c this with the raw data of \a packet. \c this
                                             packet will not share any data with \a packet.
                                             \returns Packet handle to the cloned \a packet, placed
                                                 after \c this in the packet/header/interpreter
                                                 chain. */

        void reparse() const;           ///< Reparse the payload the packet
                                        /**< This member will throw away the packet chain after the
                                             current packet. The payload will be reparsed
                                             automatically when calling next() */
        ///@}

        ///\name Data access
        ///@{

        PacketData & data() const;      ///< Access the packets raw data container
        size_type size() const;         ///< Return size of packet in bytes
                                        /**< This size does \e not include the size of any preceding
                                             headers/packets/interpreters. It does however include
                                             \c this packets payload. */
        ///@}

        ///\name Annotations
        ///@{

        template <class Annotation>
        Annotation & annotation();      ///< Get packet annotation
                                        /**< This member will retrieve an arbitrary packet
                                             annotation. Every annotation is identified by a unique
                                             \a Annotation type. This type should \e always be a \c
                                             struct.

                                             \code
                                             struct MyAnnotation {
                                                 int value;
                                             };

                                             senf::Packet p (...);

                                             p.annotation<MyAnnotation>().value = 1;
                                             \endcode

                                             Annotations are shared by all headers / interpreters
                                             within a single packet chain.

                                             If an annotation is \e not a POD type (more
                                             specifically, if it's constructor or destructor is not
                                             trivial including base classes and members), the \a
                                             Annotation type \e must inherit from
                                             senf::ComplexAnnotation. Failing to follow this rule
                                             will result in undefined behavior and will probably
                                             lead to a program crash.

                                             \code
                                             struct MyStringAnnotation : senf::ComplexAnnotation {
                                                 std::string value;
                                             };
                                             \endcode
                                             (This type is not POD since \c std::string is not POD)

                                             \see \ref packet_usage_annotation

                                             \implementation The annotation system is implemented
                                                 quite efficiently since annotations are stored
                                                 within a packet embedded vector of fixed size (the
                                                 size is determined automatically at runtime by the
                                                 number of different annotations
                                                 used). Additionally, non-complex small annotations
                                                 require no additional memory management (\c new /
                                                 \c delete).

                                             \idea Pool the annotation vectors: In the destructor
                                                 swap the vector into a vector graveyard (swapping
                                                 two vectors is an O(1) no allocation operation). In
                                                 the constructor, if there is a vector in the
                                                 graveyard, swap it in from there. Of course, it
                                                 would be better to do away with the vector and just
                                                 allocate the space together with the packet but
                                                 that looks quite complicated to do ... especially
                                                 considering that the packetimpl itself uses a pool.
                                          */

        template <class Annotation>
        Annotation const & annotation() const; ///< Get packet annotation
                                        /**< \see annotation() */

        void clearAnnotations();        ///< Clear all packet annotations
                                        /**< All packet annotations will be cleared. Afterwards
                                             the annotations equates to a new created %packet.
                                             \warning all references to existing complex
                                             annotations become invalid. */
        ///@}

        ///\name Other methods
        ///@{

        bool operator==(Packet const & other) const; ///< Check for packet identity
                                        /**< Two packet handles compare equal if they really are the
                                             same packet header in the same packet chain. */
        bool boolean_test() const;      ///< Check, whether the packet is valid()
                                        /**< \see valid() */
        bool valid() const;             ///< Check, whether the packet is valid()
                                        /**< An in - valid() packet does not allow any operation
                                             except checking for validity and assignment. in -
                                             valid() packets serve the same role as 0-pointers.

                                             This is an alias for boolean_test() which is called
                                             when using a packet in a boolean context. */

        void finalizeThis();            ///< Update calculated fields
                                        /**< The finalize() family of members will update
                                             calculated packet fields: checksums, size fields and so
                                             on. This includes any field, which can be set from
                                             other information in the packet. Each concrete packet
                                             type should document, which fields are set by
                                             finalize().

                                             finalizeThis() will \e only process the current
                                             header. Even if only changing fields in this protocol,
                                             depending on the protocol it may not be enough to
                                             finalize this header only. See the packet type
                                             documentation. */

        template <class Other>
        void finalizeTo();              ///< Update calculated fields
                                        /**< The finalize() family of members will update
                                             calculated packet fields: checksums, size fields and so
                                             on. This includes any field, which can be set from
                                             other information in the packet. Each concrete packet
                                             type should document, which fields are set by
                                             finalize().

                                             finalizeTo() will automatically process all
                                             packets/headers/interpreters from the \e first
                                             occurrence of packet type \a Other (beginning at \c
                                             this packet searching forward towards deeper nested
                                             packets) backwards up to \c this.

                                             This call is equivalent to
                                             \code
                                                 p.finalizeTo(p.next<Other>())
                                             \endcode */

        void finalizeTo(Packet const & other);  ///< Update calculated fields
                                        /**< The finalize() family of members will update
                                             calculated packet fields: checksums, size fields and so
                                             on. This includes any field, which can be set from
                                             other information in the packet. Each concrete packet
                                             type should document, which fields are set by
                                             finalize().

                                             finalizeTo(other) will automatically process all
                                             packets/headers/interpreters beginning at \a other
                                             backwards towards outer packets up to \c this. */

        void finalizeAll();             ///< Update calculated fields
                                        /**< The finalize() fammily of members will update
                                             calculated packet fields: checksums, size fields and so
                                             on. This includes any field, which can be set from
                                             other information in the packet. Each concrete packet
                                             type should document, which fields are set by
                                             finalize().

                                             finalizeAll() will automatically process all
                                             packets/headers/interpreters from the end of the chain
                                             (the most inner packet) backwards up to \c this.

                                             This call is equivalent to
                                             \code
                                                 p.finalizeTo(p.last())
                                             \endcode

                                             Beware, that finalizeAll() will \e not finalize any
                                             headers before \c this, it will \e only process inner
                                             headers. */

        void dump(std::ostream & os) const; ///< Write out a printable packet representation
                                        /**< This method is provided mostly to help debugging packet
                                             problems. Each concrete packet should implement a dump
                                             method writing out all fields of the packet in a
                                             readable representation. dump() will call this member
                                             for each packet/header/interpreter in the chain from \c
                                             this packet up to the end of the chain. */

        TypeIdValue typeId() const;     ///< Get type of \c this packet
                                        /**< This value is used e.g. in the packet registry to
                                             associate packet types with other information.
                                             \returns A type holding the same information as a
                                             type_info object, albeit assignable */
        factory_t factory() const;      ///< Return factory instance of \c this packet
                                        /**< The returned factory instance can be used to create new
                                             packets of the given type without knowing the concrete
                                             type of the packet. The value may be stored away for
                                             later use if needed. */

        unsigned long id() const;       ///< Unique packet id
                                        /**< Get a unique packet id. If two packets have the same
                                             id, they share the internal data representation. */

        bool is_shared() const;         ///< check if this packet shares data with any another packet handle.
                                        /**< This method returns true if there is any other packet
                                             handle pointing to any header in the packet chain. */

        ///@}

    protected:
        explicit Packet(PacketInterpreterBase::ptr const & packet);

        PacketInterpreterBase::ptr const & ptr() const;

    private:
        Packet getNext() const;
        Packet getLast() const;

        PacketInterpreterBase::ptr packet_;

        template <class PacketType>
        friend class ConcretePacket;
        friend class PacketParserBase;
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
            packet) \e after a given existing packet <em>thereby destroying and overwriting  any
            possibly existing packets and data after the given packet</em>.
        \li The createBefore() family of constructors will create new packets (again with new data)
            \e before a given existing packet <em>thereby destroying and overwriting any possibly
            existing packets and data before the given packet</em>.
        \li The createInsertBefore() family of constructors will create new packets \e before a
            given packet \e inserting them into the packet chain after any existing packets before
            the given packet.

        Whereas create() will create a completely new packet with it's own chain and data storage,
        createAfter(), createBefore() and createInsertBefore() extend a packet with additional
        headers/interpreters. createAfter() will set the payload of the given packet to the new
        packet whereas createBefore() and createInsertBefore() will create a new packet with the
        existing packet as it's payload.

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
        typedef typename PacketType::parser Parser;

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
        static ConcretePacket create(senf::NoInit_t); ///< Create uninitialized empty packet
                                        /**< This will create a completely empty and uninitialized
                                             packet with <tt>size() == 0</tt>.
                                             \param[in] senf::noinit This parameter must always have
                                                 the value \c senf::noinit. */
        static ConcretePacket create(size_type size); ///< Create default initialized packet
                                        /**< This member will create a default initialized packet
                                             with the given size. If the size parameter is smaller
                                             than the minimum allowed packet size an exception will
                                             be thrown.
                                             \param[in] size Size of the packet to create in bytes.
                                             \throws TruncatedPacketException if \a size is smaller
                                                 than the smallest permissible size for this type of
                                                 packet. */
        static ConcretePacket create(size_type size, senf::NoInit_t);
                                        ///< Create uninitialized packet
                                        /**< Creates an uninitialized (all-zero) packet of the exact
                                             given size.
                                             \param[in] size Size of the packet to create in bytes
                                             \param[in] senf::noinit This parameter must always have
                                                 the value \c senf::noinit. */
#ifndef DOXYGEN
        template <class ForwardReadableRange>
        static ConcretePacket create(
            ForwardReadableRange const & range,
            typename boost::disable_if< boost::is_integral<ForwardReadableRange> >::type * = 0);
#else
        template <class ForwardReadableRange>
        static ConcretePacket create(ForwardReadableRange const & range);
                                        ///< Create packet from given data
                                        /**< The packet will be created from a copy of the given
                                             data. The data from the range will be copied directly
                                             into the packet representation. The data will \e not be
                                             validated in any way.

                                             \param[in] range <a href="http://www.boost.org/doc/libs/release/libs/range/index.html">Boost.Range</a>
                                                 of data to construct packet from. */
#endif

        // Create packet as new packet after a given packet

        static ConcretePacket createAfter(Packet const & packet);
                                        ///< Create default initialized packet after \a packet
                                        /**< The packet will be initialized to it's default empty
                                             state. It will be appended as next header/interpreter
                                             after \a packet in that packets interpreter chain.
                                             \param[in] packet Packet to append new packet to. */
        static ConcretePacket createAfter(Packet const & packet, senf::NoInit_t);
                                        ///< Create uninitialized empty packet after\a packet
                                        /**< This will create a completely empty and uninitialized
                                             packet with <tt>size() == 0</tt>. It will be appended
                                             as next header/interpreter after \a packet in that
                                             packets interpreter chain.
                                             \param[in] packet Packet to append new packet to.
                                             \param[in] senf::noinit This parameter must always have
                                                 the value \c senf::noinit. */
        static ConcretePacket createAfter(Packet const & packet, size_type size);
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
        static ConcretePacket createAfter(Packet const & packet, size_type size, senf::NoInit_t);
                                        ///< Create uninitialized packet after \a packet
                                        /**< Creates an uninitialized (all-zero) packet of the exact
                                             given size.  It will be appended as next
                                             header/interpreter after \a packet in that packets
                                             interpreter chain.
                                             \param[in] packet Packet to append new packet to.
                                             \param[in] size Size of the packet to create in bytes
                                             \param[in] senf::noinit This parameter must always have
                                                 the value \c senf::noinit. */
#ifndef DOXYGEN
        template <class ForwardReadableRange>
        static ConcretePacket createAfter(
            Packet const & packet,
            ForwardReadableRange const & range,
            typename boost::disable_if< boost::is_integral<ForwardReadableRange> >::type * = 0);
#else
        template <class ForwardReadableRange>
        static ConcretePacket createAfter(Packet const & packet,
                                          ForwardReadableRange const & range);
                                        ///< Create packet from given data after \a packet
                                        /**< The packet will be created from a copy of the given
                                             data. The data from the range will be copied directly
                                             into the packet representation. The data will \e not be
                                             validated in any way.  It will be appended as next
                                             header/interpreter after \a packet in that packets
                                             interpreter chain.
                                             \param[in] packet Packet to append new packet to.
                                             \param[in] range <a href="http://www.boost.org/doc/libs/release/libs/range/index.html">Boost.Range</a>
                                                 of data to construct packet from. */
#endif

        // Create packet as new packet (header) before a given packet

        static ConcretePacket createBefore(Packet const & packet);
                                        ///< Create default initialized packet before \a packet
                                        /**< The packet will be initialized to it's default empty
                                             state. It will be prepended as previous
                                             header/interpreter before \a packet in that packets
                                             interpreter chain.
                                             \warning This constructor will destroy any existing
                                                 headers before \a packet and replace them with the
                                                 new header.
                                             \param[in] packet Packet to prepend new packet to. */
        static ConcretePacket createBefore(Packet const & packet, senf::NoInit_t);
                                        ///< Create uninitialized empty packet before \a packet
                                        /**< Creates a completely empty and uninitialized packet. It
                                             will be prepended as previous header/interpreter before
                                             \a packet in that packets interpreter chain.
                                             \warning This constructor will destroy any existing
                                                 headers before \a packet and replace them with the
                                                 new header.
                                             \param[in] packet Packet to prepend new packet to. */

        static ConcretePacket createInsertBefore(Packet const & packet);
                                        ///< Insert default initialized packet before \a packet
                                        /**< The new packet header will be initialized to it' s
                                             default empty state. It will be inserted into the
                                             packet chain before \a packet.
                                             \param[in] packet Packet before which to insert the new
                                                 packet */
        static ConcretePacket createInsertBefore(Packet const & packet, senf::NoInit_t);
                                        ///< Insert uninitialized empty packet before \a packet
                                        /**< Inserts a completely empty and unitialized packet
                                             before \a packet into the header/interpreter chain.
                                             \param[in] packet Packet before which to insert the new
                                                 packet */

        // Create a clone of the current packet

        ConcretePacket clone() const;

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        // Field access

        struct ParserProxy
        {
            ParserProxy(Parser const & p) : p_ (p) {}
            Parser * operator->() { return &p_; }
            Parser p_;
        };

        ParserProxy operator->() const;    ///< Access packet fields
                                        /**< This operator allows to access the parsed fields of the
                                             packet using the notation <tt>packet->field()</tt>. The
                                             fields of the packet are specified by the PacketType's
                                             \c parser member.

                                             The members are not strictly restricted to simple field
                                             access. The parser class may have any member which is
                                             needed for full packet access (e.g. checksum validation
                                             / recreation ...)
                                             \see \ref packetparser for the %parser interface. */

        Parser parser() const;          ///< Access packet field parser directly
                                        /**< Access the parser of the packet. This is the same
                                             object returned by the operator->() operator. The
                                             operator however does not allow to access this object
                                             itself, only it's members.
                                             \see \ref packetparser for the %parser interface */

    protected:

    private:
        typedef PacketInterpreter<PacketType> interpreter;

        ConcretePacket(typename interpreter::ptr const & packet_);

        interpreter * ptr() const;

        friend class Packet;
        friend class PacketInterpreter<PacketType>;
    };

    /** \brief Generic parser copying

        This operator allows to copy the value of identical parsers. This operation does \e not
        depend on the parsers detailed implementation, it will just replace the data bytes of the
        target parser with those from the source packet.
     */
    template <class PacketType, class Parser>
    Parser operator<<(Parser target, ConcretePacket<PacketType> const & packet);

    ///@}

}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_Packet_i_)
#define HH_SENF_Packets_Packet_i_
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

