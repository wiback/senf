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
    \brief PacketParser public header */

/** \defgroup packetparser The PacketParser facility
    
    The PacketParser facility provides a framework to implement very lightweight classes which parse
    the raw content of a packet into meaningful values. PacketParsers are always passed around by
    value, they can be understood as pointers into the data structure with added type information
    providing parsing functions.

    Parsers are built hirarchically. A high-level parser will return other parsers when returning
    some element (Example: Asking an EthernetParser for the ethertype field by calling the parsers
    \c type() member will return an UInt16 parser). The lowest level building blocks then return the
    values. This hierarchical structure greatly simplifies building complex parsers.

    Every parser is derived from senf::PacketParserBase. This parser provides the necessary
    housekeeping information and provides the parsers with access to the data.

    The PacketParser facility predefines several parsers to be used as building blocks in defining
    more complex parsers (integer parsers, several parsers for repetitive constructs)
 */

#ifndef HH_PacketParser_
#define HH_PacketParser_ 1

// Custom includes
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/optional.hpp>
#include "Utils/SafeBool.hh"
#include "PacketTypes.hh"
#define HH_PacketData_DeclOnly
#include "PacketData.hh"
#undef HH_PacketData_DeclOnly

#include "PacketParser.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
    
    /** \brief Parser Baseclass

        To implement a packet parser, you need to derive from PacketParserBase and implement several
        required members. There are two ways how to do this.
        \li If the parser just consists of a simple sequence of consecutive fields, the
            SENF_PACKET_PARESR_DEFINE_FIELDS and SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS macros
            provide a simple an convenient way to define the packet
        \li In more complex cases, you need to implement the necessary members manually.

        The following example documents the interface (which must be) provided by a parser.
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

              // You also mey define an init() member. This will be called to initialize a newly
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
        freestanding senf::bytes() functon. This function will return the correct size even for
        fixed-size parsers. You may access \c fixed_bytes directly, however be aware that this will
        restrict your code to fixed size parsers (which depending on the circumstances may be
        exactly what you want).

        In the same way, dont access \c init_bytes directly, always use the senf::init_bytes
        metafunction class which will correctly support fixed size parsers.

        \ingroup packetparser
      */
    class PacketParserBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef detail::packet::iterator data_iterator;
        typedef detail::packet::size_type size_type;
        typedef detail::packet::difference_type difference_type;
        typedef detail::packet::byte byte;
        typedef PacketData * state_type;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // default copy
        // default destructor
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        data_iterator i() const;
        state_type state() const;
        PacketData & data() const;

        void init() const;

    protected:
        PacketParserBase(data_iterator i, state_type s);
        PacketParserBase(data_iterator i, state_type s, size_type size);

        bool check(size_type size);
        void validate(size_type size);

        template <class Parser> Parser parse(data_iterator i) const;
        template <class Parser> Parser parse(size_type n) const;

        void defaultInit() const;

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

        This metafcuntion is called like
        \code
            senf::init_bytes<SomeParser>::value
        \endcode

        This expression evaluates to a compile-time constant itegral expression of type
        senf::PacketParserBase::size_type. This metafunction will return \c Parser::fixed_bytes or
        \c Parser::init_bytes depending on the type of parser.

        \param[in] Parser Parser to return init_bytes for
        \returns Number of bytes to allocate to the new object
        \ingroup packetparser
     */
    template <class Parser>
    struct init_bytes : public detail::ParserInitBytes<Parser>
    {};

    template <class Parser>
    typename boost::enable_if< 
        boost::is_base_of<PacketParserBase, Parser>,
        Parser >::type
    operator<<(Parser target, Parser source);

    template <class Parser, class Value>
    typename boost::enable_if_c < 
        boost::is_base_of<PacketParserBase, Parser>::value 
            && ! boost::is_base_of<PacketParserBase, Value>::value,
        Parser >::type
    operator<<(Parser target, Value const & value);

    /** \defgroup packetparsermacros Helper macros for defining new packet parsers
        
        To simplify the definition of simple packet parsers, several macros are provided. Before
        using these macros you should familarize yourself with the packet parser interface as
        described in senf::PacketParserBase.

        These macros simplify providing the above defined interface. A typical packet declaration
        using these macros has the following form (This is a concrete example from the definition of
        the ethernet packet in <tt>DefaultBundle//EthernetPacket.hh</tt>)
        \code
            struct Parse_EthVLan : public senf::PacketParserBase
            {
                SENF_PACKET_PARSER_INIT(Parse_EthVLan);

                // ////////////////////////////////////////////////////////////////////////

                typedef senf::Parse_UIntField < 0,  3 > Parse_Priority;
                typedef senf::Parse_Flag          < 3 > Parse_CFI;
                typedef senf::Parse_UIntField < 4, 16 > Parse_VLanId;
                typedef senf::Parse_UInt16              Parse_Type;

                SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
                    ((OverlayField)( priority, Parse_Priority ))
                    ((OverlayField)( cfi,      Parse_CFI      ))
                    ((Field       )( vlanId,   Parse_VLanId   ))
                    ((Field       )( type,     Parse_Type     )) 
                );
            };
        \endcode

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
        
        The \a builder argument selects, how the field is defined
        \li <tt>Field</tt> defines a field and increments the current position by the size of the
            field
        \li <tt>OverlayField</tt> defines a field like <tt>Field</tt> but does \e not increment the
            position. In the above example, this is used to overlay the different bitfield parsers:
            All overlaying bitfield parser except the last one (the one with the highest bit
            numbers) is marked as OverlayField.

        The \a name argument defines the name of the accessor method.

        The \a type argument is the parser to return for that field. Since none of the arguments may
        contain a komma, <em>This argument cannot be a template</em>. Always use typedefs to access
        tempalte parsers as shown above.

        The \ref SENF_PACKET_PARSER_INIT makro defines the constructor and the \c init() member. If
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

    /** \brief Define initialization mebers of a parser except init()
        
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
    SENF_PACKET_PARSER_I_DEFINE_FIELDS(fields)
        
    /** \brief Define fields for a fixed size parser

        Define the fields as specified in \a fields. This macro only supports fixed size
        subfields, the resulting parser will also be a fixed size parser.

        \ingroup packetparsermacros
        \hideinitializer
     */
#   define SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(fields)                                         \
    SENF_PACKET_PARSER_I_DEFINE_FIXED_FIELDS(fields)

    struct VoidPacketParser 
        : public PacketParserBase
    {
        SENF_PACKET_PARSER_INIT(VoidPacketParser);
    };

    /** \brief
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

        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor
        SafePacketParser();

        // conversion constructors
        SafePacketParser(Parser parser);

        SafePacketParser & operator=(Parser parser);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        Parser operator*() const;
        Parser const * operator->() const;
        bool boolean_test() const;

    protected:

    private:
        mutable boost::optional<Parser> parser_;
        senf::safe_data_iterator i_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
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
