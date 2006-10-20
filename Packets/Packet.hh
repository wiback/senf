// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// TODO: Implement assign() method akin to reinterpret(). However,
// instead of using the data already present, assign() will replace
// the date of the current packet with the given Packet.

// TODO: Implement wrapping-constructor. Somehow we want to have a
// constructor, which allows creating a chain of packet interpreters
// with as little overhead as possible.

// TODO: Document the additional concrete Packet facade requirements
// explicitly and not only within the Parser requirements (check(),
// bytes() and min_bytes() members ...)

// TODO: Implement special container replacing vector which manages
// some headroom to allow efficient insertion of elements at the
// beginning. This really is just another type of deque
// implementation.


/** \mainpage The SENF Packet Library

    \section arch Overall Architecture

    The general Architecture of the Packet Framework (pkf for short)
    is seperated into two components: The basic packet handling and
    the parser framework.

    The basic packet handling implements a packet interpreter
    chain. Every packet is represented as a chain of interpreters
    where each interpreter is a facade looking into the same
    packet. Each interpreter will interpret a specific header of a
    packet. For example, an ethernet frame might have an interpreter
    chain consisting of EthernetPacket, IPPacket, UDPPacket and
    DataPacket. Each of these interpreters will interpret a section of
    the raw data bytes. The interpreter ranges overlap since every
    packet also includes it's payload.

    The parser framework is used to interpret the raw bytes of a
    specific packet and parse the values present in that packet. For
    example, Parse_Ethernet will parse the ethernet source MAC,
    destination MAC and ethertype given any random access iterator to
    the first byte of the ethernet frame. Parsers are extremely light
    classes. They are temporary classes passed around by value. In
    most cases, they are just comprised of a single pointer adorned
    with type information.

    \section handling Packet Handling

    The packet handling is implemented within
    satcom::pkf::Packet. This class is the baseclass to all packet
    interpreter facades. To implement a new packet type, publically
    derive from satcom::pkf::Packet and implement the virtual
    interface (see the class documentation for details).

    \section framework Parser Framework

    The parser framework provides an abstract framwork to parse packet
    oriented data. A Parser is a template class taking an arbitrary
    iterator as input and allowing random access to data elements of
    the interpreted type, like source and destination MAC of an
    ethernet frame. The parser framework is to be used hierarchically
    and recursively, the parser methods should return further parsers
    which can return further parsers and so on.

    The parser framework contains some basic parsers to be used to
    build up more complex parsers:

     - ParseInt.hh: Lots of parsers for integer numbers like
       satcom::pkf::Parse_UInt8, for integer bitfields like
       satcom::pkf::Parse_UIntField and satcom::pkf::Parse_Flag to
       parse boolean flags.

     - ParseArray.hh: The satcom::pkf::Parse_Array parser to parse
       arbitrary fixed-size arrays of fixed-size elements (that is
       sub-parsers).

     - ParseVec.hh: The satcom::pkf::Parse_Vector parser to parse
       dynamically sized arrays of fixed-size elements (that is
       sub-parsers).

    See satcom::pkf::ParserBase for further information.

    \section stuff Other Utilities

    The pkf also comprises some additional utilities to support the
    development of packet classes. 

    The satcom::pkf::PacketRegistry implements a registry of packets
    keyed by an arbitrary type. The registry is used to find a packet
    type given some kind of id (like the ethertype value from the
    ethernet header). Together with it's support classes (especially
    satcom::pkf::PacketRegistryMixin) this class greatly simplifies
    implementing the needed table lookups.
 */

/** \file
    \brief Main packet interface
 */

#ifndef HH_Packet_
#define HH_Packet_ 1

// Custom includes
#include <boost/utility.hpp> // for boost::noncopyable
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <list>
#include <vector>
#include <iostream>

#include "Packet.mpp"
// ////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {
    
    namespace impl { template <class OtherPacket> class PkReg_EntryImpl; }
    namespace impl { class PacketImpl; }

    /** \brief Basic interface to all packet facades

        \section packet_overview Overview
        
        This class is the base class of all Packets. It implements the
        generic Packet interface and provides the packet management
        framework. satcom::pkf::Packet manages the necessary memory
        resources and controlls the chain of packet interpreters.

        The Packet user always interfaces with the pkf via a Packet
        derived class. This is the only external entity ever held by a
        library user. The interface is implemented using a reference
        counted smart pointer, so resource management is quasi
        automatic.

        \image html "../../structure.png" Overview
        
        Internally, every Packet references a PacketImpl instance which
        manages the raw packet data and the interpreter list. This raw
        data is interpreted by the concrete Packet derived class
        according to the definition of that derived class's packet
        type (i.e. EthernetPacket or UDPPacket).

        Packet provides several interfaces:
        
        - Creation of Packet instances: create()

        - Access to the chain of interpreters: next(), prev(), head(),
          last(), find_next(), find_prev(), get_next(), get_prev(),
          is(), as() and reinterpret()

        - Access to the raw packet data: begin(), end(), size(),
          insert() and erase()

        - An interface to the derived class: v_nextInterpreter(),
          v_finalize(), registerInterpreter()


        \section packet_der Implementing new Packet facades

        To implement a new Packet facade, publically derive from
        Packet. You need to implement the following minimal interface:

        - You need to provide a new #ptr typedef

        - You have to implement v_nextInterpreter() and v_finalize()

        - The constructor should be private

        - You must make Packet a \c friend of the new Packet facade

        - You must implement a static check() method which validates
          a byte region as your new Packet

        \code        
            class ExamplePacket 
                : public satcom::pkf::Packet
            {
            public:
                typedef ptr_t<ExamplePacket>::ptr ptr;

                static bool check(Packet::iterator begin, Packet::iterator end)
                {
                    // Validate, that the region [begin,end) can be
                    // interpreted as an ExamplePacket without risking
                    // memory access violations.
                }

            private:
                template <class Arg>
                ExamplePacket(Arg arg [, other args ... ])
                    : satcom::pkf::Packet(arg)
                {}

                virtual void v_nextInterpreter() const
                {
                    // NextPacketType and header_length of course
                    // depend on the packet type
                    registerInterpreter<NextPacketType>(begin()+header_length, end());
                }

                virtual void v_finalize()
                {
                    // calculate checksum etc
                }

                friend class satcom::pkf::Packet;
            };
        \endcode

        Please do not implement the methods inline to not clutter up
        the header file. This is done here in the example to simplify
        it. If a class is to be registered in some
        satcom:pkf::PacketRegistry, it must not take any additional
        constructor parameters.

        After having implemented the bare framework, the most comman
        way to implement access to the packets specific data is to use
        the parser framework by additionally inheriting a
        corresponding parser. This also automatically implements the
        check() method, which is provided by the Parser.

        In the following example we only show the differences from the
        previous example:

        \code
            class ExamplePacket
                : public satcom::pkf::Packet,
                  public Parse_Example<satcom::pkf::Packet::iterator,
                                       ExamplePacket>
            {

                // check does not need to be implemented here, it is
                // inherited from the parser 

            private:
                template <class InputIterator>
                ExamplePacket(InputIterator begin, InputIterator end)
                    : satcom::pkf::Packet(begin,end)
                {}
            };
        \endcode

        See the satcom::pkf::ParserBase Documentation for how to
        implement Parse_Example.

        The implementation of v_nextInterpreter most of the time
        relies on some packet registry. This is simplified using the
        satcom::pkf::PacketRegistryMixin class as follows. Again, we
        only show the differences from the preceding Example:

        \code
            struct ExampleRegistry {
                type boost::uint16_t key_t;
            };

            class ExamplePacket
                : public satcom::pkf::Packet,
                  public Parse_Example<satcom::pkf::Packet::iterator,
                                       ExamplePacket>,
                  public satcom::pkf::PacketRegistryMixin<ExampleRegistry,
                                                          ExamplePacket>
            {
                using satcom::pkf::Packet::registerInterpreter;
                using satcom::pkf::PacketRegsitryMixin<ExampleRegistry,ExamplePacket>::registerInterpreter;
            private:
                virtual void v_nextInterpreter() const
                {
                    // nextType() is defined in Parse_Example and
                    // returns the key in the ExampleRegistry of the
                    // next Packet.
                    registerInterpreter(nextType(),begin()+header_length, end());
                }
            };
        \endcode

        For further details on the packet registry, see
        satcom::pkf::PacketRegistry.

        \section packet_impl Implementation details

        The Packet interface is implemented to minimize overhead as
        far as possible without getting to complex. One area for
        improvement ist the container class used to hold the raw
        data. This currently is an \a std::vector. This could be
        imporved by either allocating some headroom/tailroom in the
        vector and using this when inserting data at the beginning or
        end. Alternatively, a new container class (like the
        satcom::lib::deque_list) could be used to support zero-copy
        semantics.

        At the moment, we leave the implementation at
        std::vector. This container is very simple and especially it
        can directly be sent out using the operating system since a \a
        vector stores data at contiguous memory locations. An \a
        std::deque could be used with \a writev(), however since we
        have no access to the implementation details of the \a deque,
        we cannot construct the \a writev() data structures.

        The interpreter list managed by Packet is lazy, meaning packet
        interpreter facades are added only when requestd by next(),
        last() or find_next(). v_nextInterpreter() is called if
        necessary by these methods to complete the interpreter chain.

        To implement the automatic memory management, every Packet
        facade is reference counted. Additionally, the number of
        (indirect) references to PacketImpl is counted. This allows to
        manage the PacketImpl instance automatically. To make this
        work, it is necessary to ensure throughout the Packet code,
        that the reference count of a Packet is not accidentally
        decremented to zero. Also, the internal pointers from the
        interpreter list to the Packet facades must not be
        counted. They are therefore implemented differently (
        boost::shared_ptr vs. boost::intrusive_ptr). The choice of
        boost::intrusive_ptr for the externaly visible smart pointer
        for all Packet facades is taken to reduce the overhead (an
        intrusive_ptr is only the size of an ordinary pointer, a
        smart_ptr has the size of two pointers).

        \nosubgrouping
      */
    class Packet : boost::noncopyable
    {
    public:
        ///\name Types
        ///@{
        typedef boost::uint8_t byte; //!< single byte datatype
        ///@}

    private:
        ///\name Implementation
        ///@{
        // These types are implementation details. They are however
        // needed to provide the correct typedefs for the user
        // interface. Hiding these classes would incur a huge
        // additional indirection overhead.

        typedef std::vector<byte> raw_container;
        typedef boost::shared_ptr<Packet> interpreter_list_ptr;
        typedef std::list<satcom::pkf::Packet::interpreter_list_ptr> interpreter_list;
        typedef unsigned refcount_t;

        ///@}

    public:

        ///////////////////////////////////////////////////////////////////////////
        ///\name Types
        ///@{
        
        /** \brief smart pointer template for all Packet classes
            
            This struct is just a template typedef. It defines the
            smart pointer used for all Packet classes.
         */
        template <class T> struct ptr_t { typedef boost::intrusive_ptr<T> ptr; };

        /** \brief smart pointer to the Packet facades

            Every derived class \e must redeclare this member for it's
            derived type:
            \code 
                typedef ptr_t<DerivedClass>::ptr ptr
            \endcode
         */
        typedef ptr_t<Packet>::ptr ptr; 
        typedef raw_container::iterator iterator; //!< raw data iterator
        typedef raw_container::size_type size_type;
        typedef raw_container::difference_type difference_type;

        ///@}

        // ////////////////////////////////////////////////////////////////////////

        ///\name Creating packets
        ///@{

        /** \brief create new Packet
            
            This method is used to create a new Packet. All Packet
            instances are created via this method, they are \e never
            created directly from the Packet derived class.

            \param OtherPacket Type of Packet to create, a Packet
                    derived class
            \param b begin iterator of byte range to create the Packet
                    from
            \param e corresponding end iterator
            \return smart pointer to new packet
            \throws TruncatedPacketException The data cannot be parsed
                    securely (the data might be trunctated or just
                    plain invalid)
         */
        template <class OtherPacket, class InputIterator>
        static typename ptr_t<OtherPacket>::ptr create(InputIterator b, InputIterator e);

        template <class OtherPacket>
        static typename ptr_t<OtherPacket>::ptr create();

        template <class OuterPacket>
        static typename ptr_t<OuterPacket>::ptr create(Packet::ptr payload);

        ///@}

        ///\name Interpreter chain
        ///@{
        
        /** \brief get next packet from the interpreter chain
            \return smart pointer to next packet or 0 if last packet */
        ptr next() const;
        /** \brief get previous packet from the interpreter chain
            \return smart pointer to previous packet or 0 if last packet */
        ptr prev() const;
        /** \brief first packet of the interpreter chain
            \return smart pointer to first packet */
        ptr head() const;
        /** \brief get last packet of the interpreter chain
            \return smart pointer to last packet */
        ptr last() const;
        
        /** \brief first packet of given type after the current packet
            \return smart pointer to first following packet of type \a
                OtherPacket or 0, if no such packet exists */
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr find_next() const;
        /** \brief first packet of given type before the current packet
            \return smart pointer to first preceding packet of type \a
                OtherPacket or 0, if no such packet exists */
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr find_prev() const;

        /** \brief first packet of given type after the current packet
            \return smart pointer to first following packet of type \a
            OtherPacket. \e Assert's, that a packet of this type exists */
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr get_next() const;
        /** \brief first packet of given type before the current packet
            \return smart pointer to first preceding packet of type \a
            OtherPacket. \e Assert's, that a packet of this type exists */
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr get_prev() const;

        /** \brief check, wether the packet is of the given type
            \return true, if packt is of type \a OtherPacket, false
                otherwise */
        template <class OtherPacket> bool is() const; 
        /** \brief cast packet pointer to the given type
            \return a properly cast smart pointer if packet is of type
                \a OtherPacket. Otherwise return 0 */
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr as(); 

        /** \brief replace current packet interpreter

            This method will \e replace the current packet facade in
            the interpreter list with a new interpreter given by \a
            OtherPacket. 

            \attention This invalidates the packet instance \e
            this</b>. You must ensure, not to use the Packet instance
            any further after this call

            \return smart pointer to a \e new packet facade
            \throws TruncatedPacketException there is not enough data
                to savely interpret the packet as the given type. The
                original packet is \e not invalidated
         */
        template <class OtherPacket>
        typename ptr_t<OtherPacket>::ptr reinterpret();

        ///@}

        ///\name Raw packet data
        ///@{

        /** \brief begin interator of raw packet data
            
            This iterator allows access to the raw data interpreted by
            the packet facade. This \e includes any header possibly
            interpreted by the derived packet instance. To access the
            payload of the packet, use next()->begin().

            \return random access iterator to the begin of the raw
                data */
        iterator begin() const;
        /** \brief past-the-end iterator of raw packet data
            
            This iterator allows access to the raw data interpreted by
            the packet facade. This \e includes any header possibly
            interpreted by the derived packet instance. To access the
            payload of the packet, use next()->end().

            \return random access past-the-end iterator of the raw
                data */
        iterator end() const;
        /** \brief raw data size of packet
            \return size of the raw data interpreted by this
                packet in bytes. This is \e not necessarily the size of
                the complete packet, use head()->size() for this. */
        size_t size() const;

        // Modifying the raw packet data

        // FIXME: Make all data mutators protected

        typedef enum { AUTO, BEFORE, INSIDE, OUTSIDE, AFTER } Whence;

        /** \brief insert single byte \a v before pos

            \attention The change will \e not be validated by the
            derived packet instance. This method is mostly to be used
            by the derived class implementation and their helper
            classes. */
        void insert(iterator pos, byte v, Whence whence = AUTO);
        /** \brief insert \a n copies of byte \a v before pos

            \attention The change will \e not be validated by the
            derived packet instance. This method is mostly to be used
            by the derived class implementation and their helper
            classes. */
        void insert(iterator pos, size_type n, byte v, Whence whence = AUTO);
        /** \brief insert a copy of the given range before pos

            \attention The change will \e not be validated by the
            derived packet instance. This method is mostly to be used
            by the derived class implementation and their helper
            classes. */
        template <class InputIterator> 
        void insert(iterator pos, InputIterator f, InputIterator l, Whence whence = AUTO);

        /** \brief erase single byte

            \attention The change will \e not be validated by the
            derived packet instance. This method is mostly to be used
            by the derived class implementation and their helper
            classes. */
        void erase(iterator pos);
        /** \brief erase range

            \attention The change will \e not be validated by the
            derived packet instance. This method is mostly to be used
            by the derived class implementation and their helper
            classes. */
        void erase(iterator first, iterator last);

        ///@}

        void dump(std::ostream & os) const;

    protected:
        ///\name Derived class interface
        ///@{

        /** \brief create new interpreter facade for an existing packet
            
            This constructor is called, when a new interpreter is to
            be added to the interpreter chain. The constructor is
            called indirectly from registerInterpreter() or
            reinterpret() via the derived classes template
            constructor.
         */
        template <class Operation>
        Packet(Operation const & arg);
        virtual ~Packet();
        
    private:
        /** \brief create next packet interpreter

            This method is called by next(), last() or find_next() to
            create any missing interpreters in the interpreter
            chain. This method must be overridden in the derived class
            to register the next packet interpreter in the interpreter
            chain with the packet framework.

            To register the new interpreter, use
            registerInterpreter() to create the new Packet
            instance. The new instance is automatically added to the
            interpreter chain after the current interpreter.

            See also satcom::pkf::PacketRegistryMixin on how to
            use a Registry to find the next interpreters implementing
            class.
         */
        virtual void v_nextInterpreter() const = 0;

        /** \brief finalize packet for sending

            This method is called by the packet framework to let the
            interpreter facade do some final calculations/packet
            cleanup before the packet is sent out or digested in some
            other way. This is the place to calcaulate checksums and
            such.

            This method is autmatically called for all interpreters on
            the interpreter chain.
         */
        virtual void v_finalize() = 0;

        virtual void v_dump(std::ostream & os) const = 0;

    protected:
        /** \brief add interpreter to interpreter chain

            This method is used by v_nextInterpreter() in the derived
            classes to add a new interpreter to the interpreter
            chain. This method will call \c OtherPacket's constructor
            with the correct arguments and insert the new interpreter
            into the interpreter list. This method is used, if no
            further arguments are to be passed to the \c OtherPacket
            constructor. If additional arguments are necessary, just
            add them after \c end. The compiler will then choose the
            correct overload to use.
         */
        template <class OtherPacket>
        typename ptr_t<OtherPacket>::ptr registerInterpreter(
            raw_container::iterator begin, raw_container::iterator end) const;
        template <class OtherPacket, class A0>
        typename ptr_t<OtherPacket>::ptr registerInterpreter(
            raw_container::iterator begin, raw_container::iterator end,
            A0 const & a0) const;

#       define BOOST_PP_ITERATION_PARAMS_1 (4, (2, 9, "Packets/Packet.mpp", 3))
#       include BOOST_PP_ITERATE()

        ///@}

    private:

        ///\name Implementation
        ///@{

        void add_ref() const;
        bool release();
        bool unlink();

	struct PacketOp_register;
	friend class PacketOp_register;
        void i_registerInterpreter(Packet * p) const;

	struct PacketOp_replace;
	friend class PacketOp_replace;
        void i_replaceInterpreter(Packet * p);

	struct PacketOp_set;
	friend class PacketOp_set;
        void i_setInterpreter(impl::PacketImpl * i);

    private:
        friend class impl::PacketImpl;
        template <class OtherPacket> friend class impl::PkReg_EntryImpl;

        impl::PacketImpl* impl_;
        size_type begin_;
        size_type end_;
        interpreter_list::iterator self_;
        mutable bool parsed_;
        mutable refcount_t refcount_;

        ///@}
    };

    /** \brief dump packet to stream
        \related Packet */
    // std::ostream & operator<<(std::ostream & os, Packet const & packet);

    /** \brief smart pointer handling
        \relates Packet */
    void intrusive_ptr_add_ref(Packet const *);
    /** \brief smart pointer handling
        \relates Packet */
    void intrusive_ptr_release(Packet *);

    struct TruncatedPacketException : public std::exception
    { virtual char const * what() const throw() { return "truncated packet"; } };

}}

// ////////////////////////////hh.e////////////////////////////////////////
#include "Packet.cci"
#include "Packet.ct"
#include "Packet.cti"

#include "Packet.mpp"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
