// $Id$
//
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
    \brief ParseHelpers public header */

#ifndef HH_ParseHelpers_
#define HH_ParseHelpers_ 1

#ifndef HH_Packets_
#error "Don't include 'ParseHelpers.hh' directly, include 'Packets.hh'"
#endif

// Custom includes

//#include "ParseHelpers.mpp"
#include "ParseHelpers.ih"
///////////////////////////////hh.p////////////////////////////////////////

/** \defgroup packetparsermacros Helper macros for defining new packet parsers

    To simplify the definition of simple packet parsers, several macros are provided. Before
    using these macros you should familiarize yourself with the packet parser interface as
    described in senf::PacketParserBase.

    These macros simplify providing the above defined interface. A typical packet declaration
    using these macros has the following form (This is a concrete example from the definition of
    the ethernet packet in <tt>DefaultBundle/EthernetPacket.hh</tt>)

    \code
    struct Parse_EthVLan : public PacketParserBase
    {
    #   include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( priority,  3, unsigned );
        SENF_PARSER_BITFIELD( cfi,       1, bool     );
        SENF_PARSER_BITFIELD( vlanId,   12, unsigned );

        SENF_PARSER_FIELD( type, Parse_UInt16 );

        SENF_PARSER_FINALIZE(Parse_EthVLan);
    };
    \endcode

    The macros take care of the following:
    \li They define the accessor functions returning parsers of the given type.
    \li They automatically calculate the offset of the fields from the preceding fields.
    \li The macros provide a definition for \c init() 
    \li The macros define the \c bytes(), \c fixed_bytes and \c init_bytes members as needed.

    You may define either a fixed or a dynamically sized parser. Fixed size parsers are defined by
    starting the packet with <tt>\#include SENF_FIXED_PARSER()</tt>, dynamically sized parsers start
    with <tt>\#include SENF_PARSER()</tt>. The different members are implemented such that:

    \li The needed parser constructor is defined
    \li \c init() calls \c defaultInit(). \c defaultInit() is defined to call \c init() on each
        of the fields.
    \li \c bytes() (on dynamically sized parser) respectively \c fixed_bytes (on fixed size
        parsers) is defined to return the sum of the sizes of all fields.
    \li On dynamically sized parsers, \c init_bytes is defined to return the sum of the
        \c init_byte's of all fields

    \section parserlanguage The Parser Macro micro-language

    The macros provided to help implement collection parsers implement a very small declarative
    language. This way of to think of the macros simplifies understanding, how the macros work.

    Central to this language is the concept of <em>current offset</em>. The current offset is the
    place (in bytes) from the beginning of the parser at which the next field will be added. Adding
    fields to the parser will advance this offset by the size of the field added. Additional
    commands allow to arbitrarily manipulate the current offset manually.

    For fixed size parsers, the current offset is a single constant integral value, the number of
    bytes from the parsers start. For dynamically sized parsers, the current offset really consists
    of two values: One is the number of bytes from the parsers start, which however needs not be a
    constant expression, the other value is the \c init_bytes value at this point, which is an
    integral constant.

    To demonstrate this functionality, here a more complex example (taken from MPEGDVBBundle and
    then simplified by removing some fields)
    \code
    struct Parse_DSMCCSection : public PacketParserBase
    {
    #   include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD    ( table_id            , Parse_UInt8   );
        
        SENF_PARSER_BITFIELD ( ssi                 ,  1 , bool     );
        SENF_PARSER_BITFIELD ( private_indicator   ,  1 , bool     );
        SENF_PARSER_SKIP_BITS( 2 );
        SENF_PARSER_BITFIELD ( sec_length          , 12 , unsigned );
        
        SENF_PARSER_FIELD    ( table_id_extension  , Parse_UInt16  );
        
        SENF_PARSER_FINALIZE( Parse_DSMCCSection );
    };

    struct Parse_DatagramSection : public Parse_DSMCCSection
    {
    #   include SENF_FIXED_PARSER()

        SENF_PARSER_INHERIT( Parse_DSMCCSection );

        SENF_PARSER_FIELD    ( mac_addr_4          , Parse_UInt8   );
        SENF_PARSER_FIELD    ( mac_addr_3          , Parse_UInt8   );

        SENF_PARSER_FINALIZE( Parse_DatagramSection );

        // Parse table_id_extension as two bytes
        SENF_PARSER_GOTO( table_id_extension );
        SENF_PARSER_FIELD    ( mac_addr_6          , Parse_UInt8   );
        SENF_PARSER_FIELD    ( mac_addr_5          , Parse_UInt8   );
    };
    \endcode

    This code defines two parsers, the second of which is based on the first. Both are fixed size
    parsers. The definition of \c Parse_DSMCCSection is straight forward (more on bit fields
    below). 

    The derived parser is a little bit more complex. It starts out the same defining itself as a
    fixed size parser. Then the base class is imported. Among other things, this call sets the
    current offset to the first byte after the base parser (the base parser need not be implemented
    using the packet parser macros, it just needs to be a valid parser). The additional fields \c
    mac_addr_4 and \c mac_addr_3 are defined. Then we finalize the parser declaration.

    <em>After</em> \ref SENF_PARSER_FINALIZE we add two more fields but not at the end of the
    parser. \ref SENF_PARSER_GOTO jumps back to a previously defined label or field. Since the base
    parser \c Parse_DSMCCSection is defined using the packet parser macros, we can even jump to
    labels or fields defined in the base parser. Here, we jump to the beginning of the \c
    table_id_extension field. \c mac_addr_6 and \c mac_addr_5 are therefore defined starting at that
    offset and therefore overlay the \c table_id_extension field.

    \section parsermacroinit Parser initialization

    \par ""
        \ref SENF_FIXED_PARSER(), \ref SENF_PARSER(), \ref SENF_PARSER_INHERIT(), \ref
        SENF_PARSER_INIT(), \ref SENF_PARSER_FINALIZE()

    Every parser using the parser macros starts either with <tt>\#include SENF_PARSER()</tt> or with
    <tt>\#include SENF_FIXED_PARSER()</tt>. This command sets the current offset to zero and defines
    the type of parser to define.

    A following optional \ref SENF_PARSER_INHERIT(\e base_class) is necessary if the parser does not
    derive from senf::PacketParserBase. This call sets the base class and advances the current
    offset to the end of the base parser.

    \ref SENF_PARSER_INIT() is used to define the parser initialization code (the \c init()
    member).

    \ref SENF_PARSER_FINALIZE(\e class_name) is called to define the parsers constructor, the \c
    init() member and to set the parsers size (\c fixed_bytes for fixed size parsers or \c bytes()
    and \c init_bytes for dynamically sized parsers). It is valid to define further fields \e after
    \ref SENF_PARSER_FINALIZE() has been called, however
    \li Fields defined after \ref SENF_PARSER_FINALIZE() will \e not be initialized by \c
        defaultInit() (and therefore not by the default provided \c init() member). This can be very
        helpful when defining overlaying fields to avoid initializing some bytes multiple times.
    \li The size of the parser is given by the current offset at the time of the \ref
        SENF_PARSER_FINALIZE() call.

    \section parsermacrofields Defining fields

    \par ""
        \ref SENF_PARSER_FIELD(), \ref SENF_PARSER_FIELD_RO(), \ref SENF_PARSER_PRIVATE_FIELD(),
        \ref SENF_PARSER_PRIVATE_FIELD_RO(), SENF_PARSER_CUSTOM_FIELD()

    There are quite a few commands available to define fields. All these macros do the same thing:
    they define a field accessor plus some auxiliary symbols for the field:
    \par ""
        <em>type</em> <em>name</em><tt>()</tt> <tt>const</tt><br>
        <tt>typedef</tt> <em>type</em> <em>name</em><tt>_t</tt><br>
        <tt>size_type</tt> <tt>const</tt> <em>name</em><tt>_offset</tt><br>
        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<em>or</em><br>
        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<tt>size_type</tt> <em>name</em><tt>_offset()</tt> <tt>const</tt>
    
    Here \e type is the type of the field defined (the return value of the \e name() accessor) and
    \e name is the field name. <em>name</em><tt>_offset</tt> gives the offset of the first byte of
    the field from the start of the parser. This member is a constant for fixed size parser and a
    member function for dynamically sized parsers. <em>name</em><tt>_t</tt> is just an alias for the
    fields type (more precisely the return type of the <em>name</em>() accessor).

    There are some properties the field defining macros might have. These properties are parts
    of the macro name:

    \par RO: Read only fields
        Macros with \c RO in their name define read only fields. This is only possible, if the
        field's parser is a value parser (that is, it must have a \c value_type typedef member and
        must provide the \c value() accessor member function). In this case, the value returned from
        the \e name() accessor member is not the parser but the parsers value and therefore it does
        not allow assignment to the field.

    \par PRIVATE: Fields private to the parser class 
        A private field will not be accessible from the outside (it is made \c private to the parser
        class). This is very handy when providing other accessor members to access a field in a
        manner more suitable for the specific field, when combining several fields into a single
        return value etc.

    The field defining macros come in groups which members only differ in their properties:

    \par Define standard fields:
        \ref SENF_PARSER_FIELD(), \ref SENF_PARSER_FIELD_RO(), \ref SENF_PARSER_PRIVATE_FIELD(),
        \ref SENF_PARSER_PRIVATE_FIELD_RO() define standard fields.

    \par Define arbitrary custom field:
        \ref SENF_PARSER_CUSTOM_FIELD()

    See the documentation of each of these macros for a detailed description of the macro arguments
    and usage. Bit-fields are handled in the following section.

    \subsection parsermacrobitfields Bit-fields

    \par "" 
        \ref SENF_PARSER_BITFIELD(), \ref SENF_PARSER_BITFIELD_RO(), \ref
        SENF_PARSER_PRIVATE_BITFIELD(), \ref SENF_PARSER_PRIVATE_BITFIELD_RO() \n

    Bit-fields play a special role. They are quite frequent in packet definitions but don't fit into
    the byte offset based parsing infrastructure defined so far. Since defining the correctly
    parameterized Parse_IntField, Parse_UIntField and Parse_Flag typedefs is quite tedious, these
    helper macros are provided.

    It is important to recognize, that the current offset does \e not include the current bit
    position. The current offset after defining a bit-field will be the first complete byte after
    that bit-field. Only the bit-field macros additionally take care of the current bit position
    which is reset automatically by any intervening non-bitfield command.

    So bit-field commands will come in groups. Any group of consecutive bitfield commands defines a
    set of consecutive bits. The group as a whole will always be considered to cover a fixed number
    of complete bytes. If the group does not cover those bytes completely (there are some bit's left
    at the end), those bit's will be skipped.

    Since consecutive bit-field commands are aggregated into a single bit-field group, the offset of
    all these bit-fields will be the offset of the \e beginning of the bit-field irrespective of the
    number of bits parsed so far. Changing the offset to some bitfield using \ref SENF_PARSER_GOTO()
    will therefore always go to the position at the beginning of this bitfield group. And since the
    current offset does not include the bit position, the bit position will be 0, the first bit. You
    may however break a bit-field group into two groups (at a byte boundary) by placing a \ref
    SENF_PARSER_LABEL() command between the two groups.

    The additional command \ref SENF_PARSER_SKIP_BITS() can be used to skip bit's between two
    bit-fields.

    \section parsermacrooffset Manipulating the current offset

    \par ""
        \ref SENF_PARSER_SKIP(), \ref SENF_PARSER_SKIP_BITS(), \ref SENF_PARSER_GOTO(), \ref
        SENF_PARSER_GOTO_OFFSET(), \ref SENF_PARSER_LABEL()

    To define more complex parsers, there are some macro commands which change the current offset. 

    \ref SENF_PARSER_SKIP(\e bytes) will skip the given number of bytes. \ref
    SENF_PARSER_SKIP_BITS(\e bits) will work within bitfield definition to skip that number of bits.

    \ref SENF_PARSER_GOTO(\e label_or_field) will change the offset to the given field or label. The
    following fields will therefore start at that offset and overlay any fields already defined.

    \ref SENF_PARSER_GOTO_OFFSET(\e offset) will jump to the given byte offset from the start of the
    parser.

    \ref SENF_PARSER_LABEL(\e name) will define \e name as a label for the current offset which can
    then later be referenced using \ref SENF_PARSER_GOTO(). This also defines
    <em>name</em><tt>_offset</tt> as a constant or member (for fixed respectively dynamically sized
    parsers).
    
    It is very important to recognize, that the size of the parser is defined by the current offset
    <em>at the time \ref SENF_PARSER_FINALIZE() is called</em>. This allows to arbitrarily
    manipulate the size of the parser by changing the current offset accordingly. For dynamically
    sized parsers, the offset can even be any expression involving member function calls. See the
    documentation of the respective macros for more details.

    \ingroup packetparser
 */
///\ingroup packetparsermacros
///\{

#define SENF_FIXED_PARSER()      SENF_ABSOLUTE_INCLUDE_PATH(Packets/parse_fixed_setup.hh)
#define SENF_PARSER()            SENF_ABSOLUTE_INCLUDE_PATH(Packets/parse_setup.hh)

#define SENF_PARSER_INHERIT      BOOST_PP_CAT(SENF_PARSER_INHERIT_,      SENF_PARSER_TYPE)

#define SENF_PARSER_FIELD        BOOST_PP_CAT(SENF_PARSER_FIELD_,        SENF_PARSER_TYPE)
#define SENF_PARSER_FIELD_RO     BOOST_PP_CAT(SENF_PARSER_FIELD_RO_,     SENF_PARSER_TYPE)
#define SENF_PARSER_BITFIELD     BOOST_PP_CAT(SENF_PARSER_BITFIELD_,     SENF_PARSER_TYPE)
#define SENF_PARSER_BITFIELD_RO  BOOST_PP_CAT(SENF_PARSER_BITFIELD_RO_,  SENF_PARSER_TYPE)
#define SENF_PARSER_CUSTOM_FIELD BOOST_PP_CAT(SENF_PARSER_CUSTOM_FIELD_, SENF_PARSER_TYPE)

#define SENF_PARSER_PRIVATE_FIELD       BOOST_PP_CAT(SENF_PARSER_P_FIELD_,       SENF_PARSER_TYPE)
#define SENF_PARSER_PRIVATE_FIELD_RO    BOOST_PP_CAT(SENF_PARSER_P_FIELD_RO_,    SENF_PARSER_TYPE)
#define SENF_PARSER_PRIVATE_BITFIELD    BOOST_PP_CAT(SENF_PARSER_P_BITFIELD_,    SENF_PARSER_TYPE)
#define SENF_PARSER_PRIVATE_BITFIELD_RO BOOST_PP_CAT(SENF_PARSER_P_BITFIELD_RO_, SENF_PARSER_TYPE)

#define SENF_PARSER_SKIP         BOOST_PP_CAT(SENF_PARSER_SKIP_,        SENF_PARSER_TYPE)
#define SENF_PARSER_SKIP_BITS    BOOST_PP_CAT(SENF_PARSER_SKIP_BITS_,   SENF_PARSER_TYPE)
#define SENF_PARSER_GOTO         BOOST_PP_CAT(SENF_PARSER_GOTO_,        SENF_PARSER_TYPE)
#define SENF_PARSER_GOTO_OFFSET  BOOST_PP_CAT(SENF_PARSER_GOTO_OFFSET_, SENF_PARSER_TYPE)
#define SENF_PARSER_LABEL        BOOST_PP_CAT(SENF_PARSER_LABEL_,       SENF_PARSER_TYPE)

/** \brief Define parser initialization routine

    \code
    SENF_PARSER_INIT() {
        defaultInit();
        foo() = 2;
    }
    \endcode
    Defining the initialization code manually skips the automatic call of defaultInit(), which may
    be performed manually. Should the initialization code be more complex, it should be placed into
    a non-inline private member which is called from \ref SENF_PARSER_INIT()
    
    \hideinitializer
 */
#define SENF_PARSER_INIT()       void init(int)

#define SENF_PARSER_FINALIZE     BOOST_PP_CAT(SENF_PARSER_FINALIZE_,    SENF_PARSER_TYPE)

///\}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_Packets__decls_) && !defined(HH_ParseHelpers_i_)
#define HH_ParseHelpers_i_
//#include "ParseHelpers.cci"
//#include "ParseHelpers.ct"
//#include "ParseHelpers.cti"
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
