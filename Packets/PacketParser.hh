// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <g0dil@berlios.be>
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
    \brief PacketParser public header */

/** \defgroup packetparser The PacketParser facility
    
    The PacketParser facility provides a framework to implement very lightweight classes which parse
    the raw content of a packet into meaningful values. PacketParsers are always passed around
    <em>by value</em>, they can be understood as pointers into the packet data with added type
    information providing parsing functions.

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
    corresponding basic type (e.g. a Parse_UInt16 field can be used like an unsigned integer), more
    complex parsers provide type specific access members. Assigning a value to a parser will change
    the underlying representation (the packet data). 

    More complex parsers (especially those representing a collection of values) provide an
    additional wrapper class for mutating access (e.g. Parse_Vector provides a container wrapper
    with am STL compatible random-access sequence interface). See the documentation of the specific
    parser for the wrapper specification.

    Every parser is derived from senf::PacketParserBase. This class provides the necessary
    housekeeping information and provides the parsers with access to the data.

    \warning Parsers are like iterators: They are invalidated <em>whenever the size of the packet's
    data is changed</em>. You should not store a parser anywhere. If you want to keep a parser
    reference, use the senf::SafePacketParser wrapper. You still will need to take extra care to
    ensure the parser is not invalidated.
 */

#ifndef HH_PacketParser_
#define HH_PacketParser_ 1

// Custom includes
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/optional.hpp>
#include "Utils/SafeBool.hh"
#include "PacketTypes.hh"
#include "PacketData.hh"

#include "PacketParser.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
    
    /** \brief Parser Base class

        Parsers come in two favors: fixed and dynamically sized parsers. A <em>fixed size
        parser</em> has a constant size, it will always parse a fixed number of bytes. The low-level
        'final'  parsers (like the integer parsers) are fixed size parsers as are composite parsers
        built up only of fixed-size fields.

        A <em>dynamically sized</em> parser on the other hand infers it's size from the contents of
        the data parsed. Any parser containing at least one dynamically sized sub-parser will itself
        be dynamically sized.
        
        Both kinds of parser need to derive from PacketParserBase and implement several required
        members. Which members to implement depends on the parsers flavor. There are two ways how to
        do this.
        \li If the parser just consists of a simple sequence of consecutive fields (sub-parsers),
            the \ref SENF_PACKET_PARSER_DEFINE_FIELDS and \ref
            SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS macros provide a simple and convenient way to
            define the packet
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

              // ////////////////////////////////////////////////////////////////////////

              // Add here members returning (sub-)parsers for the fields. The 'parse' member is 
              // used to construct the sub-parsers. This member either takes an iterator to the
              // data to be parsed or just an offset in bytes.

              senf::Parse_UInt16 type() const { return parse<Parse_UInt16>( 0 ); }
              senf::Parse_UInt16 size() const { return parse<Parse_UInt16>( 2 ); }
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
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef detail::packet::iterator data_iterator; ///< Raw data iterator type
        typedef detail::packet::size_type size_type; ///< Unsigned integral type
        typedef detail::packet::difference_type difference_type; ///< Signed integral type
        typedef detail::packet::byte byte; ///< Unsigned 8bit value, the raw value type
        typedef PacketData * state_type; ///< Type of the 'state' parameter

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // default copy
        // default destructor
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        data_iterator i() const;        ///< Return beginning of data to parse
                                        /**< The parser is expected to interpret the data beginning
                                             here. The size of the interpreted is given by
                                             <tt>senf::bytes(</tt><em>parser
                                             instance</em><tt>)</tt>. */
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

    protected:
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

        bool check(size_type size);     ///< Check size of data container
                                        /**< \returns \c true, if the data container holds at least
                                             \a size beginning at i(), \c false otherwise. */
        void validate(size_type size);  ///< Validate size of data container
                                        /**< \throws TruncatedPacketException if the raw data
                                             container does not hold at least \a size bytes
                                             beginning at i(). */

        template <class Parser> Parser parse(data_iterator i) const; ///< Create sub-parser
                                        /**< Creates a new instance of \a Parser to parse data
                                             beginning at \a i. Automatically passes \a state() to
                                             the new parser. */
        template <class Parser> Parser parse(size_type n) const; ///< Create sub-parser
                                        /**< Creates a new instance of \a Parser to parse data
                                         * beginning at i()<tt> + </tt>\a n. Automatically passes \a
                                             state() to the new parser. */

        void defaultInit() const;       ///< Default implementation
                                        /**< This is just an empty default
                                             implementation. Re-implement this member in your own
                                             parsers if needed. */

    private:
        data_iterator end();

        data_iterator i_;
        PacketData * data_;

        template <class Parser> friend class SafePacketParser;
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
    PacketParserBase::size_type bytes(Parser p);
    
    namespace detail { template <class Parser> class ParserInitBytes; }

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

    /** \brief Generic parser copying

        This operator allows to copy the values of identical parsers. This operation does \e not
        depend on the parsers detailed implementation, it will just replace the data bytes of the
        target parser with those from the source parser. This allows to easily copy around complex
        packet substructures.

        This operation is different from the ordinary assignment operator: It does not change the \a
        target parser, it changes the data referenced by the \a target parser.

        \ingroup packetparser
     */
#   ifndef DOXYGEN
    template <class Parser>
    typename boost::enable_if< 
        boost::is_base_of<PacketParserBase, Parser>,
        Parser >::type
    operator<<(Parser target, Parser source);
#   else
    template <class Parser>
    Parser operator<<(Parser target, Parser source);
#   endif

    /** \brief Generic parser value assignment

        This operator allows to assign a value to parsers which implement a <tt>value(</tt>\a
        value<tt>)</tt> member. This operator allows to use a common syntax for assigning values or
        parsers to a parser. 

        \ingroup packetparser
     */
    template <class Parser, class Value>
    typename boost::enable_if_c < 
        boost::is_base_of<PacketParserBase, Parser>::value 
            && ! boost::is_base_of<PacketParserBase, Value>::value,
        Parser >::type
    operator<<(Parser target, Value const & value);

    /** \defgroup packetparsermacros Helper macros for defining new packet parsers
        
        To simplify the definition of simple packet parsers, several macros are provided. Before
        using these macros you should familiarize yourself with the packet parser interface as
        described in senf::PacketParserBase.

        These macros simplify providing the above defined interface. A typical packet declaration
        using these macros has the following form (This is a concrete example from the definition of
        the ethernet packet in <tt>DefaultBundle/EthernetPacket.hh</tt>)
    
        \dontinclude EthernetPacket.hh
        \skipline struct Parse_EthVLan : public PacketParserBase
        \until };

        The macros take care of the following:
        \li They define the accessor functions returning parsers of the given type.
        \li They automatically calculate the offset of the fields from the preceding fields.
        \li The macros provide a definition for \c init() 
        \li The macros define the \c bytes(), \c fixed_bytes and \c init_bytes members as needed.

        You may define either a fixed or a dynamically sized parser. Fixed size parsers are defined
        using \ref SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS, dynamically sized parsers are defined
        using \ref SENF_PACKET_PARSER_DEFINE_FIELDS. The different members are implemented such
        that:
        
        \li The needed parser constructor is defined
        \li \c init() calls \c defaultInit(). \c defaultInit() is defined to call \c init() on each
            of the fields.
        \li \c bytes() (on dynamically sized parser) respectively \c fixed_bytes (on fixed size
            parsers) is defined to return the sum of the sizes of all fields.
        \li On dynamically sized parsers, \c init_bytes is defined to return the sum of the
            \c init_size's of all fields

        The central definition macros are \ref SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS and \ref
        SENF_PACKET_PARSER_DEFINE_FIELDS. The argument to both has the same structure. It is a
        (boost preprocessor style) sequence of field definitions where each field definition
        provides the builder macro to use and the name and type of the field to define:
        \code
          SENF_PACKET_PARSER_DEFINE[_FIXED]_FIELDS(
              (( <builder> )( <name>, <type> ))
              ...
          )
        \endcode
        
        For each field, this command will define
        \li A method \a name() returning an instance of the \a type parser
        \li \a name<tt>_t</tt> as a typedef for \a type, the fields value
        \li \a name<tt>_offset</tt> to give the offset of the field from the beginning of the
            parser. If the parser is a fixed size parser, this will be a static constant, otherwise
            it will be a method.

        The \a builder argument selects, how the field is defined
        \li <tt>Field</tt> defines a field and increments the current position by the size of the
            field
        \li <tt>OverlayField</tt> defines a field like <tt>Field</tt> but does \e not increment the
            position. In the above example, this is used to overlay the different bitfield parsers:
            All overlaying bitfield parser except the last one (the one with the highest bit
            numbers) is marked as OverlayField.

        The \a name argument defines the name of the accessor method.

        The \a type argument is the parser to return for that field. Since none of the arguments may
        contain a comma, <em>This argument cannot be a multi-parameter template</em>. Always use
        typedefs to access templated parsers as shown above.

        The \ref SENF_PACKET_PARSER_INIT macro defines the constructor and the \c init() member. If
        you want to provide your own \c init() implementation, use \ref
        SENF_PACKET_PARSER_NO_INIT. The first statement in your init method should probably to call
        \c defaultInit(). This will call the \c init() member of all the fields. Afterwards you can
        set up the field values as needed:
        \code
          struct SomePacket : public senf::PacketParserBase
          {
              SENF_PACKET_PARSER_NO_INIT(SomePacket);
        
              typedef senf::Parse_UInt8 Parse_Type;
              typedef senf::Parse_Vector< senf::Parse_UInt32,
                                          senf::SimpleVectorSizer<senf::Parse_UInt16>
                                        > Parse_Elements;

              SENF_PACKET_PARSER_DEFINE_FIELDS(
                  ((Field)( type,     Parse_Type     ))
                  ((Field)( elements, Parse_Elements ))
              );

              void init() const {
                  defaultInit();
                  type() = 0x01;
                  elements().push_back(0x01020304u);
              }
          }
        \endcode
        
        \ingroup packetparser
     */

    /** \brief Define initialization members of a parser
        
        This macro defines the packet parser constructor and the \c init() member. \c init() is
        defined to just call \c defaultInit() which is defined by the other macros to call \c init()
        on each of the parsers fields.

        \ingroup packetparsermacros
        \hideinitializer
     */
#   define SENF_PACKET_PARSER_INIT(name)                                                          \
    name(data_iterator i, state_type s) : senf::PacketParserBase(i,s) {}                          \
    void init() const { defaultInit(); }

    /** \brief Define initialization members of a parser except init()
        
        This macro is like SENF_PACKET_PARSER_INIT but does \e not define \c init(). This allows you
        to provide your own implementation. You should call \c defaultInit() first before
        initializing your data fields.

        \ingroup packetparsermacros
        \hideinitializer
     */
#   define SENF_PACKET_PARSER_NO_INIT(name)                                                       \
    name(data_iterator i, state_type s) : senf::PacketParserBase(i,s) {}

    /** \brief Define fields for a dynamically sized parser

        Define the fields as specified in \a fields. This macro supports dynamically sized
        subfields, the resulting parser will be dynamically sized.

        \ingroup packetparsermacros
        \hideinitializer
     */
#   define SENF_PACKET_PARSER_DEFINE_FIELDS(fields)                                               \
    SENF_PACKET_PARSER_I_DEFINE_FIELDS(0,fields)
        
    /** \brief Define fields for a dynamically sized parser (with offset)

        Define the fields as specified in \a fields. This macro supports dynamically sized
        subfields, the resulting parser will be dynamically sized.

        The \a offset argument gives the byte offset at which to start parsing the fields. This
        helps defining extended parser deriving from a base parser:
        \code
           struct ExtendedParser : public BaseParser
           {
               ExtendedParser(data_iterator i, state_type s) : BaseParser(i,s) {}
        
               SENF_PACKET_PARSER_DEFINE_FIELDS_OFFSET(senf::bytes(BaseParser(*this)),
                 ( ... fields ... ) );

               void init() {
                   BaseParser::init();
                   defaultInit();
                   // other init code
               }
           }
        \endcode

        \ingroup packetparsermacros
        \hideinitializer
     */
#   define SENF_PACKET_PARSER_DEFINE_FIELDS_OFFSET(offset,fields)                                 \
    SENF_PACKET_PARSER_I_DEFINE_FIELDS(offset,fields)

    /** \brief Define fields for a fixed size parser

        Define the fields as specified in \a fields. This macro only supports fixed size
        subfields, the resulting parser will also be a fixed size parser.

        \ingroup packetparsermacros
        \hideinitializer
     */
#   define SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(fields)                                         \
    SENF_PACKET_PARSER_I_DEFINE_FIXED_FIELDS(0,fields)

    /** \brief Define fields for a fixed size parser

        Define the fields as specified in \a fields. This macro only supports fixed size
        subfields, the resulting parser will also be a fixed size parser.

        The \a offset argument gives the byte offset at which to start parsing the fields. This
        helps defining extended parser deriving from a base parser:
        \code
           struct ExtendedParser : public BaseParser
           {
               ExtendedParser(data_iterator i, state_type s) : BaseParser(i,s) {}

               SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS_OFFSET(BaseParser::fixed_bytes,
                 ( ... fields ... ) );

               void init() {
                   BaseParser::init();
                   defaultInit();
                   // other init code
               }
           }
        \endcode

        \ingroup packetparsermacros
        \hideinitializer
     */
#   define SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS_OFFSET(offset,fields)                           \
    SENF_PACKET_PARSER_I_DEFINE_FIXED_FIELDS(offset,fields)

    /** \brief Default parser parsing nothing
     */
    struct VoidPacketParser 
        : public PacketParserBase
    {
        SENF_PACKET_PARSER_INIT(VoidPacketParser);
    };

    /** \brief Iterator re-validating Parser wrapper

        An ordinary parser will be invalidated whenever the raw data container's size is
        changed. This can complicate some algorithms considerably.

        This wrapper will update the parsers iterator (the value returned by the i() member) on
        every access. This ensures that the iterator will stay valid.

        \attention Beware however, if you insert or remove data before the safe wrapper, the
            location will \e not be updated accordingly and therefore the parser will be
            invalid.

        Additionally a SafePacketParser has an uninitialized state. The only allowed operations in
        this state are the boolean test for validity and assigning another parser.

        \ingroup packetparser
      */
    template <class Parser>
    class SafePacketParser
        : public SafeBool< SafePacketParser<Parser> >
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // default copy constructor
        // default copy assignment
        // default destructor
        SafePacketParser();             ///< Create an empty uninitialized SafePacketParser

        // conversion constructors
        SafePacketParser(Parser parser); ///< Initialize SafePacketParser from \a parser

        SafePacketParser & operator=(Parser parser); ///< Assign \a parser to \c this

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        Parser operator*() const;       ///< Access the stored parser
                                        /**< On every access, the stored parsers iterator will be
                                             updated / re-validated. */
        Parser const * operator->() const; ///< Access the stored parser
                                        /**< On every access, the stored parsers iterator will be
                                             updated / re-validated. */
        bool boolean_test() const;      ///< Check validity

    protected:

    private:
        mutable boost::optional<Parser> parser_;
        senf::safe_data_iterator i_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(SENF_PACKETS_DECL_ONLY) && !defined(HH_PacketParser_i_)
#define HH_PacketParser_i_
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

