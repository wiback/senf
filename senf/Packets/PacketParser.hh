// $Id$
//
// Copyright (C) 2007
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
//   Stefan Bund <g0dil@berlios.be>

/** \file
    \brief PacketParser public header */

/** \defgroup packetparser The PacketParser facility

    The PacketParser facility provides a framework to implement very lightweight classes which parse
    the raw content of a packet into meaningful values. PacketParsers are always passed around
    <em>by value</em>, they can be understood as pointers into the packet data with added type
    information providing parsing functions.

    Packet parsers are \e only used within the packet framework. You should never allocate a new
    parser instance directly, you should the Packet library let that do for you (either by having
    the parser as a packet parser in a packet type or by having a member in the packet parser which
    allocates the parser as a sub-parser).

    Parsers are built hierarchically. A high-level parser will return other parsers when accessing
    an element (Example: Asking an EthernetParser for the ethertype field by calling the parsers \c
    type() member will return an \c UInt16 parser). The lowest level building blocks then return the
    values. This hierarchical structure greatly simplifies building complex parsers.

    Since parsers are very lightweight and are passed by value, packet fields are accessed using the
    corresponding accessor method:
    \code
      SomePacket p (...)
      SomePacket q (...)

      // Assign new value to an integer parser
      p->someField() = 10;

      // Write out above value
      std::cerr << p->someField() << "\n";

      // Use the generic parser-assignment operator '<<' to copy field values
      p->someVector()[1].someOtherField() << q->someField();
      p->someVector() << q->someVector()
    \endcode

    Here \c someField(), \c someOtherField() and \c someVector() are accessor methods named after
    the field name. Each returns a parser object. Simple parsers can be used like their
    corresponding basic type (e.g. a UInt16Parser field can be used like an unsigned integer), more
    complex parsers provide type specific access members. Assigning a value to a parser will change
    the underlying representation (the packet data).

    Parsers can be grouped into several categories. These categories are not all defined rigorously
    but are nevertheless helpful when working with the parsers:
    \li <em>\ref parserimpl_value</em> provide the lowest level parsers (e.g. senf::UInt16Parser which
        returns an integer value).
    \li <em>\ref parserimpl_collection</em> are parsers which model a collection of sub-elements like
        senf::ListParser or senf::VectorParser.
    \li <em>\ref parserimpl_composite</em> collect several fields of arbitrary type into a new
        parser. Parsers defined using the \ref packetparsermacros fall under this category.
    \li <em>\ref parserimpl_packet</em> are used to define a packet type.

    \warning Parsers are like iterators: They are invalidated <em>whenever the size of the packet's
    data is changed</em>. You should not store a parser anywhere. If you want to keep a parser
    reference, use the senf::SafePacketParserWrapper wrapper. You still will need to take extra care to
    ensure the parser is not invalidated.

    \section parserimpl Packet parser categories

    Every parser is derived from senf::PacketParserBase. This class provides the necessary
    housekeeping information and provides the parsers with access to the data. You may in principle
    define arbitrary methods as parser members (e.g. methods to calculate a checksum, methods
    processing fields in some way and so on). You should however be very wary to access data outside
    the range assigned to the packet (the range starting at \c i() and with a size of senf::bytes()
    bytes).

    Each parser type has specific features

    \subsection parserimpl_value Value parsers

    For a parser \a SomeParser to be a value parser, the following expressions must be valid:
    \code
    // SomeParser must have a 'value_type', The 'value_type' must be default constructible, copy
    // constructible and assignable
    SomeParser::value_type v;

    // An instance of 'SomeParser' must have a 'value' member which returns a value which may be
    // assigned to a variable of type 'value_type'
    v = p.someParserField().value()

    // It must be possible to assign a new value using the 'value' member
    p.someParserField().value(v)
    \endcode

    If at all possible, the 'value_type' should not reference the packet data using iterators or
    pointers, it should hold a copy of the value (it's Ok for \c value() to return such a reference
    as long as assigning it to a \c value_type variable will copy the value).

    \see parseint

    \subsection parserimpl_collection Collection parsers

    A collection parser \a SomeParser should model STL containers. The parsers themselves will
    probably only // provide a reduced interface, but the collection parser should have a \c
    collection member which is a wrapper providing the full interface.
    \code
    SomeParser::container c (p.someParserField());
    \endcode

    You will probably only very seldom need to implement a completely new collection
    parser. Instead, you can rely on senf::VectorParser or senf::ListParser and implement new
    policies.

    \see parsecollection

    \subsection parserimpl_composite Composite parsers

    If possible, composite parsers should be implemented using the \ref packetparsermacros. In
    addition to the normal parser requirements, these macros ensure, that for each field,
    <em>fieldname</em><tt>_t</tt> is a typedef for the fields parser and
    <em>fieldname</em><tt>_offset</tt> is the offset of the field in bytes from the beginning of the
    parser (either a constant for fixed size parsers or a member function for dynamically sized
    parsers). When defining composite parsers without the help of the \ref packetparsermacros, you
    should provide those same members.

    \subsection parserimpl_packet Protocol parsers

    Protocol parsers are composite parsers with relaxed requirements. Since a Protocol parser will
    never be used as a sub-parser (it will not be used within another composite parser or as value
    type in a collection parser), the value returned by senf::bytes for this parser must not
    necessarily cover the complete packet (e.g. if the packet has a trailer, the trailer will live
    outside the range given by senf::bytes). You may define any member you want to have in your
    packets field interface. These members may access the packet data in any way. You just need to
    ensure, that the integration into the packet-type is correct (the senf::PacketTypeMixin will by
    default use senf::bytes() to find the end of the header).

    <hr>
 */

#ifndef HH_SENF_Packets_PacketParser_
#define HH_SENF_Packets_PacketParser_ 1

// Custom includes
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/optional.hpp>
#include <senf/Utils/safe_bool.hh>
#include "PacketTypes.hh"
#include "PacketData.hh"
#include "ParseHelpers.hh"
#include "SafeIterator.hh"

//#include "PacketParser.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class Packet;

    /** \brief Parser Base class

        Parsers come in two flavors: fixed and dynamically sized parsers. A <em>fixed size
        parser</em> has a constant size, it will always parse a fixed number of bytes. The low-level
        'final'  parsers (like the integer parsers) are fixed size parsers as are composite parsers
        built up only of fixed-size fields.

        A <em>dynamically sized</em> parser on the other hand infers it's size from the contents of
        the data parsed. Any parser containing at least one dynamically sized sub-parser will itself
        be dynamically sized.

        Both kinds of parser need to derive from PacketParserBase and implement several required
        members. Which members to implement depends on the parsers flavor. There are two ways how to
        do this.
        \li If the parser just consists of sequence of consecutive fields (sub-parsers), the \ref
            packetparsermacros provide a simple yet flexible way to define a packet parser.
        \li In more complex cases, you need to implement the necessary members manually.

        This documentation is about the manual implementation. You should nevertheless read through
        this to understand, what above macros are doing.

        The following example documents the interface (which must be) provided by a parser:
        \code
          struct FooParser : public PacketParserBase
          {
              FooParser(data_iterator i, state_type s) : PacketParserBase(i,s) {}

              // If this parser has a fixed size, you must define this size here This definition
              // allows the parser to be used within the list, vector and array parsers static
              static const size_type fixed_bytes = some_constant_size;

              // If the parser does not have a fixed size, you must implement the bytes() member to
              // return the size. ONLY EVER DEFINE ONE OF fixed_bytes OR bytes().
              size_type bytes() const;

              // If you define bytes(), you also need to define the init_bytes. This is the number
              // of bytes to allocate when creating a new object
              static const size_type init_bytes = some_constant_size;

              // You also may define an init() member. This will be called to initialize a newly
              // created data object. The default implementation just does nothing.
              void init() const;

              //-////////////////////////////////////////////////////////////////////////

              // Add here members returning (sub-)parsers for the fields. The 'parse' member is
              // used to construct the sub-parsers. This member either takes an iterator to the
              // data to be parsed or just an offset in bytes.

              senf::UInt16Parser type() const { return parse<UInt16Parser>( 0 ); }
              senf::UInt16Parser size() const { return parse<UInt16Parser>( 2 ); }
          };
        \endcode

        You should never call the \c bytes() member of a parser directly. Instead you should use the
        freestanding senf::bytes() function. This function will return the correct size irrespective
        of the parsers flavor. You may access \c fixed_bytes directly, however be aware that this
        will restrict your code to fixed size parsers (which depending on the circumstances may be
        exactly what you want).

        In the same way, don't access \c init_bytes directly, always use the senf::init_bytes
        meta-function class which correctly supports fixed size parsers.

        \ingroup packetparser
      */
    class PacketParserBase
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef detail::packet::iterator data_iterator; ///< Raw data iterator type
        typedef detail::packet::size_type size_type; ///< Unsigned integral type
        typedef detail::packet::difference_type difference_type; ///< Signed integral type
        typedef detail::packet::byte byte; ///< Unsigned 8bit value, the raw value type
        typedef PacketData * state_type; ///< Type of the 'state' parameter
        typedef PacketParserBase parser_base_type; ///< Base type of the next parser

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        // no default constructor
        // default copy
        // default destructor
        // no conversion constructors

        //\}
        //-////////////////////////////////////////////////////////////////////////

        data_iterator i() const;        ///< Return beginning of data to parse
                                        /**< The parser is expected to interpret the data beginning
                                             here. The size of the interpreted is given by
                                             <tt>senf::bytes(</tt><em>parser
                                             instance</em><tt>)</tt>. */

        data_iterator i(size_type offset) const; ///< Return iterator \a offset bytes from the start
                                        /**< The return value is the same as i() + \a
                                             offset. However, the parser checks, that the iterator is
                                             still within range of the raw data
                                             container. Otherwise a TruncatedPacketException is
                                             thrown.

                                             \throws TruncatedPacketException if the raw data
                                                 container does not hold at least \a offset bytes
                                                 starting at i(). */

        state_type state() const;       ///< Return state of this parser
                                        /**< The value returned should be interpreted as an opaque
                                             value provided just to be forwarded to other
                                             parsers. */

        PacketData & data() const;      ///< Access the packets raw data container
                                        /**< This member will return the raw data container holding
                                             the data which is parsed by \c this parser. */

        void init() const;              ///< Default implementation
                                        /**< This is just an empty default
                                             implementation. Re-implement this member in your own
                                             parsers if needed. */

    private:
        struct ParserProtector {
            senf::safe_data_iterator safe_i_;
            mutable PacketParserBase const * parser_;

            ParserProtector(PacketParserBase const * parser);
            ParserProtector(ParserProtector const & other_);
            ~ParserProtector();
        };
    protected:
        ParserProtector protect() const;

        PacketParserBase(data_iterator i, state_type s); ///< Standard constructor
                                        /**< This is the constructor used by most parsers. The
                                             parameters are just forwarded from the derived classes
                                             constructor parameters. */

        PacketParserBase(data_iterator i, state_type s, size_type size);
                                        ///< Size checking constructor
                                        /**< In addition to the standard constructor, this
                                             constructor will validate, that there is enough data in
                                             the raw data container to parse \a size bytes after \a
                                             i.

                                             This constructor is called by all 'final' parsers
                                             (e.g. the integer parsers) and \e only by those
                                             parsers. Most parsers do \e not check the validity of
                                             the iterator, this is delayed until the very last
                                             parser. This allows to partial parse truncated
                                             packets.

                                             \throw TruncatedPacketException if the raw data
                                                 container does not hold at least \a size bytes
                                                 beginning at \a i. */

        bool check(size_type size) const; ///< Check size of data container
                                        /**< \returns \c true, if the data container holds at least
                                             \a size beginning at i(), \c false otherwise. */

        void validate(size_type size) const; ///< Validate size of data container
                                        /**< \throws TruncatedPacketException if the raw data
                                             container does not hold at least \a size bytes
                                             beginning at i(). */

        template <class Parser> Parser parse(data_iterator i) const; ///< Create sub-parser
                                        /**< Creates a new instance of \a Parser to parse data
                                             beginning at \a i. Automatically passes \a state() to
                                             the new parser. */

        template <class Parser, class Arg> Parser parse(Arg const & arg, data_iterator i) const;
                                        ///< Create sub-parser
                                        /**< This is like parse(data_iterator), however it passes
                                             the extra argument \a arg to the \a Parser
                                             constructor. */

        template <class Parser> Parser parse(size_type n) const; ///< Create sub-parser
                                        /**< Creates a new instance of \a Parser to parse data
                                         * beginning at i()<tt> + </tt>\a n. Automatically passes \a
                                             state() to the new parser. */

        template <class Parser, class Arg> Parser parse(Arg const & arg, size_type n) const;
                                        ///< Create sub-parser
                                        /**< This is like parse(size_type), however it passes the
                                             extra argument \a arg to the \a Parser constructor. */

        void defaultInit() const;       ///< Default implementation
                                        /**< This is just an empty default
                                             implementation. Re-implement this member in your own
                                             parsers if needed. */

        Packet packet() const;          ///< Get packet this parser is parsing from
                                        /**< \note This member should only be used from packet
                                             parsers when access to previous or following packets is
                                             needed e.g. for calculating checksums etc. */

        void resize(size_type oldSize, size_type newSize); ///< Resize data container
                                        /**< This command will erase or insert bytes from/into the
                                             data container at the end of the parser (at i() + \a
                                             newSize). If \a oldSize is > \a newSize, bytes will be
                                             removed, otherwise bytes will be inserted.

                                             \warning This may invalidate iterators and other
                                                 parsers. The current parser itself is automatically
                                                 updated */

    private:
        data_iterator end() const;

        data_iterator i_;
        PacketData * data_;

        template <class Parser> friend class SafePacketParserWrapper;
        friend struct ParserProtector;
    };

    /** \brief Return raw size parsed by the given parser object

        This function will either call <tt>p.bytes()</tt> or return <tt>Parser::fixed_bytes</tt>
        depending on the type of parser.

        The value returned does \e not take into account the amount of data actually available. So
        you always need to validate this value against the packet size if you directly access the
        data. The standard low-level parses all do this check automatically to guard against
        malformed packets.

        \param[in] p Parser object to check
        \returns number of bytes this parser expects to parser
        \ingroup packetparser
     */
    template <class Parser>
    PacketParserBase::size_type bytes(Parser const & p);

    namespace detail { template <class Parser> struct ParserInitBytes; }
    namespace detail { template <class Parser> struct ParserIsFixed; }

    /** \brief Return number of bytes to allocate to new object of given type

        This meta-function is called like
        \code
            senf::init_bytes<SomeParser>::value
        \endcode

        This expression evaluates to a compile-time constant integral expression of type
        senf::PacketParserBase::size_type. This meta-function will return \c Parser::fixed_bytes or
        \c Parser::init_bytes depending on the type of parser.

        \param[in] Parser The Parser to return init_bytes for
        \returns Number of bytes to allocate to the new object
        \ingroup packetparser
     */
    template <class Parser>
    struct init_bytes : public detail::ParserInitBytes<Parser>
    {};

    /** \brief Test, whether a parser is a fixed-size parser

        This meta-function is called like
        \code
            senf::is_fixed<SomeParser>::value
        \endcode

        This expression evaluates to a compile-time constant boolean expression which is \c true, if
        \a SomeParser is a fixed size parser, \c false otherwise

        \param[in] Parser The Parser to test
        \returns \c true, if \a Parser is fixed size, \c false otherwise
        \ingroup packetparser
     */
    template <class Parser>
    struct is_fixed : public detail::ParserIsFixed<Parser>
    {};

#   ifndef DOXYGEN
    template <class Parser>
    typename boost::enable_if<
        boost::is_base_of<PacketParserBase, Parser>,
        Parser >::type
    operator<<(Parser target, Parser source);
#   else
    /** \brief Generic parser copying


        This operator allows to copy the values of identical parsers. This operation does \e not
        depend on the parsers detailed implementation, it will just replace the data bytes of the
        target parser with those from the source parser. This allows to easily copy around complex
        packet substructures.

        This operation is different from the ordinary assignment operator: It does not change the \a
        target parser, it changes the data referenced by the \a target parser.

        \ingroup packetparser
     */
    template <class Parser>
    Parser operator<<(Parser target, Parser source);
#   endif

#   ifndef DOXYGEN
    template <class Parser, class Value>
    typename boost::enable_if_c <
        boost::is_base_of<PacketParserBase, Parser>::value
            && ! boost::is_base_of<PacketParserBase, Value>::value,
        Parser >::type
    operator<<(Parser target, Value const & value);
#   else
    /** \brief Generic parser value assignment

        This operator allows to assign a value to parsers which implement a <tt>value(</tt>\a
        value<tt>)</tt> member. This operator allows to use a common syntax for assigning values or
        parsers to a parser.

        \ingroup packetparser
     */
    template <class Parser, class Value>
    Parser operator<<(Parser target, Value const & value);
#   endif

#   ifndef DOXYGEN
    template <class Parser, class Value>
    typename boost::enable_if_c <
        boost::is_base_of<PacketParserBase, Parser>::value
            && ! boost::is_base_of<PacketParserBase, Value>::value,
        Parser >::type
    operator<<(Parser target, boost::optional<Value> const & value);
#   else
    /** \brief Generic parser value assignment

        This operator allows to assign a value to parsers which implement a <tt>value(</tt>\a
        value<tt>)</tt> member. This special version allows to assign optional values: IF the
        optional value is not set, the assignment will be skipped.

        This operator allows to use a common syntax for assigning values or parsers to a parser.

        \ingroup packetparser
     */
    template <class Parser, class Value>
    Parser operator<<(Parser target, boost::optional<Value> const & value);
#   endif

    /** \brief Default parser parsing nothing
     */
    struct VoidPacketParser
        : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        SENF_PARSER_FINALIZE(VoidPacketParser);
    };


    template <class Parser, typename ValueType, PacketParserBase::size_type FixedBytes>
    struct ValueParserBase : public PacketParserBase
    {
        typedef ValueParserBase<Parser, ValueType, FixedBytes> Base;

        static size_type const fixed_bytes = FixedBytes;
        typedef ValueType value_type;

        ValueParserBase(data_iterator i, state_type s);

        operator value_type () const;
        byte & operator[](size_type index);
        Parser const & operator= (value_type const & other);
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_PacketParser_i_)
#define HH_SENF_Packets_PacketParser_i_
#include "PacketParser.cci"
#include "PacketParser.ct"
#include "PacketParser.cti"
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

