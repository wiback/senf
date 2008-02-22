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
    struct EthVLanPacketParser : public PacketParserBase
    {
    #   include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( priority,  3, unsigned );
        SENF_PARSER_BITFIELD( cfi,       1, bool     );
        SENF_PARSER_BITFIELD( vlanId,   12, unsigned );

        SENF_PARSER_FIELD( type, UInt16Parser );

        SENF_PARSER_FINALIZE(EthVLanPacketParser);
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

    The macros provided to help implement composite parsers implement a very small declarative
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

    To demonstrate this functionality, here a more complex example (taken from \c MPEGDVBBundle and
    then simplified by removing some fields)
    \code
    struct Parse_DSMCCSection : public PacketParserBase
    {
    #   include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD    ( table_id            , UInt8Parser   );
        
        SENF_PARSER_BITFIELD ( ssi                 ,  1 , bool     );
        SENF_PARSER_BITFIELD ( private_indicator   ,  1 , bool     );
        SENF_PARSER_SKIP_BITS( 2 );
        SENF_PARSER_BITFIELD ( sec_length          , 12 , unsigned );
        
        SENF_PARSER_FIELD    ( table_id_extension  , UInt16Parser  );
        
        SENF_PARSER_FINALIZE( Parse_DSMCCSection );
    };

    struct Parse_DatagramSection : public Parse_DSMCCSection
    {
    #   include SENF_FIXED_PARSER()

        SENF_PARSER_INHERIT( Parse_DSMCCSection );

        SENF_PARSER_FIELD    ( mac_addr_4          , UInt8Parser   );
        SENF_PARSER_FIELD    ( mac_addr_3          , UInt8Parser   );

        SENF_PARSER_FINALIZE( Parse_DatagramSection );

        // Parse table_id_extension as two bytes
        SENF_PARSER_GOTO( table_id_extension );
        SENF_PARSER_FIELD    ( mac_addr_6          , UInt8Parser   );
        SENF_PARSER_FIELD    ( mac_addr_5          , UInt8Parser   );
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
        \ref SENF_PARSER_CUSTOM_FIELD()

    There are quite a few commands available to define fields. All these macros do the same thing:
    they define a field accessor plus some auxiliary symbols. The accessor will use the parser type
    passed to the macro to parse the field. The current offset is adjusted according to the size of
    that parser. Normally, the parser will return an instance of the given parser type.

    There are some properties the field defining macros might have. These properties are parts
    of the macro name:

    \par \c RO: Read only fields
        Macros with \c RO in their name define read only fields. This is only possible, if the
        field's parser is a value parser (that is, it must have a \c value_type typedef member and
        must provide the \c value() accessor member function). In this case, the value returned from
        the \e name() accessor member is not the parser but the parsers value and therefore it does
        not allow assignment to the field.

    \par \c PRIVATE: Fields private to the parser class 
        A private field will not be accessible from the outside (it is made \c private to the parser
        class). This is very handy when providing other accessor members to access a field in a
        manner more suitable for the specific field, when combining several fields into a single
        return value etc.

    The field defining macros come in groups which members only differ in their properties:

    <dl><dt><em>Standard fields:</em><dt><dd>\ref SENF_PARSER_FIELD(), \ref SENF_PARSER_FIELD_RO(),
        \ref SENF_PARSER_PRIVATE_FIELD() define standard fields.</dd>

    <dt><em>Arbitrary custom field:</em><dt><dd>\ref SENF_PARSER_CUSTOM_FIELD()</dd></dl>

    See the documentation of each of these macros for a detailed description of the macro arguments
    and usage. Bit-fields are handled in the following section.

    There also some supplementary macros for defining fields of more complex composite types
    (e.g. vectors). See the list of 'Defines' further down this page.

    \subsection parsermacrosbitfields Bit-fields

    \par "" 
        \ref SENF_PARSER_BITFIELD(), \ref SENF_PARSER_BITFIELD_RO(), \ref
        SENF_PARSER_PRIVATE_BITFIELD()\n

    Bit-fields play a special role. They are quite frequent in packet definitions but don't fit into
    the byte offset based parsing infrastructure defined so far. Since defining the correctly
    parameterized senf::IntFieldParser, senf::UIntFieldParser and senf::FlagParser typedefs is quite
    tedious, these helper macros are provided.

    It is important to recognize, that the current offset does \e not include the current bit
    position. The current offset after defining a bit-field will be the first complete byte after
    that bit-field. Only the bit-field macros additionally take care of the current bit position
    which is reset automatically by any intervening non-bitfield command.

    So bit-field commands will come in groups. Any group of consecutive bitfield commands defines a
    set of consecutive bits. The group as a whole will always be considered to cover a fixed number
    of complete bytes. If the group does not cover those bytes completely (there are some bit's left
    at the end), those bit's will be skipped.

    Since consecutive bit-field commands are aggregated into a single bit-field group, the offset of
    all these bit-fields will be the offset of the \e beginning of the group irrespective of the
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

///\name Control information
///@{

/** \brief Define fixed size parser
    
    This macro must be called using \c \#include at the beginning of every fixed size parser using
    the packet parser helper macros:

    \code
    struct SomeParser : public senf::PacketParserBase
    {
    #   include SENF_FIXED_PARSER()
    \endcode

    The parser must directly or indirectly inherit from senf::PacketParserBase

    \hideinitializer
 */
#define SENF_FIXED_PARSER()      SENF_ABSOLUTE_INCLUDE_PATH(Packets/parse_fixed_setup.hh)

/** \brief Define dynamically sized parser
    
    This macro must be called using \c \#include at the beginning of every dynamically sized parser
    using the packet parser helper macros:

    \code
    struct SomeParser : public senf::PacketParserBase
    {
    #   include SENF_PARSER()
    \endcode

    The parser must directly or indirectly inherit from senf::PacketParserBase

    \hideinitializer
 */
#define SENF_PARSER()            SENF_ABSOLUTE_INCLUDE_PATH(Packets/parse_setup.hh)

/** \brief Define parser initialization routine

    This macro allows to replace the default initialization code. The default \c init()
    implementation will call \c defaultInit() which in turn will call \c init() of every field
    defined before \ref SENF_PARSER_FINALIZE().

    \ref SENF_PARSER_INIT() allows to replace \c init() with custom code:
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
#define SENF_PARSER_INIT()       void init(int) const

#ifdef DOXYGEN

/** \brief Define parser inheritance

    If the a parser does not directly inherit senf::PacketParserBase, \ref SENF_PARSER_INHERIT()
    must be called to define the parser's base-class. This call will additionally move the current
    offset to the end of the inherited parser so additional fields can be added.
    \code
    struct MyParser : public BaseParser
    {
    #   include SENF_FIXED_PARSER() // or SENF_PARSER()

        SENF_PARSER_INHERIT(BaseParser)
    \endcode

    \param[in] base name of base class
    \hideinitializer
 */
#define SENF_PARSER_INHERIT(base)

/** \brief Generate parser control members

    \ref SENF_PARSER_FINALIZE() will generate the necessary parser control members (default
    constructor, parser size, parser initialization). \ref SENF_PARSER_FINALIZE() needs not be the
    last macro command within the parser though it will often be the last command since \ref
    SENF_PARSER_FINALIZE() does not account for fields defined later.
    
    \ref SENF_PARSER_FINALIZE() uses the information from \ref SENF_PARSER_INHERIT() to construct
    the parsers base class (which must be a valid parser class). 

    \c defaultInit() is defined to initialize all fields <em>defined before the call to \ref
    SENF_PARSER_FINALIZE()</em>. Fields defined later will \e not be initialized. If \ref
    SENF_PARSER_INIT() is not used, \c init() is defined to call \c defaultInit().

    The parsers size (either \c fixed_bytes for fixed size parsers or \c bytes() and \c init_bytes
    for dynamically sized parsers) is set to the current offset. By manipulating the current offset
    before calling \ref SENF_PARSER_FINALIZE(), the parser size can therefore be arbitrarily
    manipulated. E.g., using \ref SENF_PARSER_GOTO_OFFSET() allows to set the size to an arbitrary
    value.

    \param[in] name name of the parser class currently being defined
 */
#define SENF_PARSER_FINALIZE(name)

///@}

///\name Parser fields
///@{

/** \brief Define normal parser field

    The family of \ref SENF_PARSER_FIELD() macros is used to define standard fields of a composite
    parser. Every field is accessed by an accessor method named after the \a name parameter. The
    field will be parsed using the \a type parser which must be a valid packet parser. If the
    current parser is defined as a fixed size parser, all sub parsers must also be fixed size,
    otherwise dynamically sized parser (e.g. collection parsers) are Ok.

    Defining a field will always define several members:

    <dl><dt><em>return_type</em> <em>name</em><tt>()</tt> <tt>const</tt></dt><dd>The accessor member
        will return the parsed value when called. For normal fields, <em>return_type</em> equals
        <em>type</em>, the type of the sub parser. This allows to change the value via the returned
        sub-parser. If the field is marked read-only (\ref SENF_PARSER_FIELD_RO()), the return type
        will be <em>type</em>::<tt>value_type</tt>.</dd>

    <dt><tt>typedef</tt> <em>type</em> <em>name</em><tt>_t</tt></dt><dd>This typedef symbol is an
        alias for the fields type.</dd>

    <dt><tt>size_type</tt> <tt>const</tt> <em>name</em><tt>_offset</tt></dt><dd>Defined only for
        fixed size parsers, this gives the fixed starting offset of the field from the beginning of
        the parser.</dd>

    <dt><tt>size_type</tt> <em>name</em><tt>_offset() const</tt></dt><dd>Defined only for
        dynamically sized parsers, this member function will return the dynamic offset of the field
        from the beginning of the parser.</dd></dl>

    \param[in] name field name
    \param[in] type parser type

    \see \ref SENF_PARSER_FIELD_RO(), \ref SENF_PARSER_PRIVATE_FIELD()
    \hideinitializer
 */
#define SENF_PARSER_FIELD(name, type) 

/** \brief Define parser field (read-only)
    
    Define read-only parser field. Read-only fields may only be defined for \a type's which are
    value parsers: The parser \a type must have a \c value_type typedef member and a \c value()
    member, which returns the current value of the field.

    Defining such a field really defines \e two accessors: A read/write \e private field and a
    read-only \e public accessor. The name of the private read/write field is given by adding a
    trailing '_' to \a name. The read-only public accessor is called \a name.
   
    \see SENF_PARSER_FIELD() 
    \hideinitializer
*/
#define SENF_PARSER_FIELD_RO(name, type) 

/** \brief Define parser field (private)

    Define a parser field which is marked as \c private and may only be accessed from the parser
    class itself.

    \see SENF_PARSER_FIELD()
    \hideinitializer
 */
#define SENF_PARSER_PRIVATE_FIELD(name, type) 

/** \brief Define custom field accessor

    This macro is used to define a field using a custom access method:
    \code
    // The following is the same as SENF_PARSER_FIELD( xyz, senf::UInt16Parser )
    // in a fixed size parser.

    SENF_PARSER_CUSTOM_FIELD(xyz, senf::UInt16Parser, xyz_t::fixed_bytes) {
        return parse<xyz_t>( xyz_offset );
    }
    \endcode
    
    The macro defines the same auxiliary symbols defined by \ref SENF_PARSER_FIELD(\a name, \a
    type), the accessor method however is provided by the user.

    \a size depends on the type of parser being defined: 

    \li If defining a fixed parser, \a size is a single value \a bytes which must be a constant
        integral expression giving the fixed size of the field.
    \li If defining a dynamically sized parser, \a size is given by two parameters \a bytes and \a
        init_bytes. \a bytes is an arbitrary (not necessarily constant) expression giving the
        dynamic size of the field whereas \a init_bytes is the constant initial size assigned to the
        field.

    \param[in] name name of the field to define
    \param[in] type return type of the accessor function
    \param[in] size size of the field, either a single value \a bytes for fixed size parsers or two
        separate arguments \a bytes and \a init_bytes for dynamically sized parsers
 */
#define SENF_PARSER_CUSTOM_FIELD(name, type, size) 

///@}

///\name Bit fields
///@{

/** \brief Define bit-field

    Bit fields are supported by a special family of parser macros. These macros simplify defining
    fields using the senf::Parse_Int, senf::Parse_UInt and senf::FlagParser parsers by keeping track
    of the current bit position and automatically creating the correct template parameters.
    
    The \a type parameter specifies the type of bitfield to define. This value is one of
    \li \c signed, for signed bit fields (senf::IntFieldParser)
    \li \c unsigned, for unsigned bit fields (senf::UIntFieldParser) or 
    \li \c bool, for single-bit flags (senf::FlagParser). 

    The \a bits parameter specifies the number of bits the field covers. For \c signed or \c
    unsigned fields, this value may be any numeric value from 1 to 32, for \c bool fields, this
    value \e must be 1.

    For more information see \ref parsermacrosbitfields

    \param[in] name name of the bit field
    \param[in] bits number of bits
    \param[in] type bit field type, one of \c signed, \c unsigned or \c bool

    \see \ref SENF_PARSER_BITFIELD_RO(), \ref SENF_PARSER_PRIVATE_BITFIELD()
    \hideinitializer
 */
#define SENF_PARSER_BITFIELD(name, bits, type) 

/** \brief Define bit-field (read-only) 

    Define read-only bit field. This is for bit-fields what \ref SENF_PARSER_FIELD_RO is for ordinary fields.

    \see \ref SENF_PARSER_BITFIELD() \n \ref SENF_PARSER_FIELD_RO()
    \hideinitializer
 */
#define SENF_PARSER_BITFIELD_RO(name, bits, type) 

/** \brief Define bit-field (private) 

    Define a bit field which is marked as \c private and may only be accessed from the parser class
    itself.

    \see \ref SENF_PARSER_BITFIELD()
    \hideinitializer
 */
#define SENF_PARSER_PRIVATE_BITFIELD(name, bits, type) 

///@}

///\name Current offset
///@{

/** \brief Skip bytes

    Moves the offset by the given distance (which may be negative). \a skip depends on the type of
    parser being defined and is either \a bytes or \a bytes, \a init_bytes.

    \li If defining a fixed parser, \a bytes must be a constant integral expression which will be
        added to the current offset
    \li If defining a dynamically sized parser, the macro really takes two arguments, \a bytes and
        \a init_bytes. \a bytes will adjust the current field offset whereas \a init_bytes will
        adjust the parsers \c init_bytes value. \a bytes is allowed to be any integral expression,
        and need \e not be constant. The second argument \a init_bytes on the other hand needs to be
        a constant integral expression.

    \param[in] bytes number of bytes to skip
    \param[in] init_bytes only for dynamically sized parsers, value to adjust the \c init_bytes value
        with.

    \hideinitializer
 */
#define SENF_PARSER_SKIP(skip)

/** \brief Skip bits within bitfield group

    This command will skip the given number of bits within a bitfield group. This command does \e
    only affect bitfield commands. Therefore, a SENF_PARSER_SKIP_BITS command which is not followed
    by a bitfield command will be ignored.

    \hideinitializer
 */
#define SENF_PARSER_SKIP_BITS(bits)

/** \brief Change current offset

    This command will change the current offset to the field or label \a name. Fields defined after
    this command will start at that position and will therefore overlay any fields defined earlier
    for these byte positions.

    \ref SENF_PARSER_GOTO() does \e not take into account the current bit position within bit
    fields. When passed the name of a field within a bit field group, this command will always jump
    to the beginning of the \e complete group (\e not the field within the bit field), even if the
    group covers multiple bytes before the bit field \a name.

    \param[in] name field or label to jump to
    \hideinitializer
 */
#define SENF_PARSER_GOTO(name)

/** \brief Change current offset to explicit value

    \ref SENF_PARSER_GOTO_OFFSET() allows to change the current offset manually to an arbitrary
    value. The \a offset parameter depends on the type of field currently being defined. 
   
    \li If defining a <em>fixed size parser</em>, the \a offset argument is a single \a bytes value
        which is an integral constant expression to which the offset will be set.
    \li If defining a <em>dynamically sized parser</em>, the \a offset argument is given by two
        parameters \a bytes and \a init_bytes. \a bytes can be any integral expression (not
        necessarily constant) giving the new byte position. \a init_bytes must be a constant
        integral expression and will set the current initial size of the packet to this value.

    \param[in] offset Depending on the parser type, either single \a bytes value or two arguments \a
        bytes and \a init_size.
    \hideinitializer
 */
#define SENF_PARSER_GOTO_OFFSET(offset)

/** \brief Define offset label

    This command defines \a name as a label for the current offset. The member
    <em>name</em><tt>_offset</tt> is defined (either as a constant for fixed size parsers or as a
    member function for dynamically sized parsers) to return the position at the point of label
    definition. 

    \ref SENF_PARSER_GOTO() can later be used to jump to a position which has previously been
    labeled with \ref SENF_PARSER_LABEL()

    \param[in] name label name
    \hideinitializer
 */
#define SENF_PARSER_LABEL(name)

/** \brief Get field offset

    This macro will return the offset of the given field or label. This macro may only be used
    while defining the parser, normally while defining inline functions.

    This macro will return the correct value when defining fixed or dynamically sized parsers.

    \param[in] name field or label name
    \returns offset of the field from parsers start
    \hideinitializer
 */
#define SENF_PARSER_OFFSET(name)

/** \brief Get fixed field offset, if possible

    This macro will return the fixed offset to the field \a name, a compile-time constant
    expression. This is identical to \ref SENF_PARSER_OFFSET() when defining a fixed size parser.

    Even in dynamically sized parsers this macro will work, if the field \a name is preceded by
    fixed size fields only. This macro does \e not validate this condition, it will return an
    arbitrary incorrect value otherwise.

    \pre Field \a name preceded by fixed size fields only
    \param[in] name field or label name
    \returns compile-time constant offset of the field from parsers start
    \hideinitializer
 */
#define SENF_PARSER_FIXED_OFFSET(name)

/** \brief Get current fixed offset, if possible
 
    This macro will return the current fixed offset, a compile-time constant expression. This is
    always possible when defining a fixed size parser.

    Even in dynamically sized parsers this macro will work, if all parser defined \e before the
    current position are fixed-size parsers. This macro does \e not validate this condition, it will
    return an arbitrary incorrect value otherwise.

    \pre Current position preceded by fixed-size parsers only
    \returns compile-time constant offset from parsers start
    \hideinitializer
 */
#define SENF_PARSER_CURRENT_FIXED_OFFSET()

///@}

#else

#define SENF_PARSER_INHERIT              BOOST_PP_CAT( SENF_PARSER_INHERIT_,              SENF_PARSER_TYPE )

#define SENF_PARSER_FIELD                BOOST_PP_CAT( SENF_PARSER_FIELD_,                SENF_PARSER_TYPE )
#define SENF_PARSER_FIELD_RO             BOOST_PP_CAT( SENF_PARSER_FIELD_RO_,             SENF_PARSER_TYPE )
#define SENF_PARSER_BITFIELD             BOOST_PP_CAT( SENF_PARSER_BITFIELD_,             SENF_PARSER_TYPE )
#define SENF_PARSER_BITFIELD_RO          BOOST_PP_CAT( SENF_PARSER_BITFIELD_RO_,          SENF_PARSER_TYPE )
#define SENF_PARSER_CUSTOM_FIELD         BOOST_PP_CAT( SENF_PARSER_CUSTOM_FIELD_,         SENF_PARSER_TYPE )

#define SENF_PARSER_PRIVATE_FIELD        BOOST_PP_CAT( SENF_PARSER_P_FIELD_,              SENF_PARSER_TYPE )
#define SENF_PARSER_PRIVATE_BITFIELD     BOOST_PP_CAT( SENF_PARSER_P_BITFIELD_,           SENF_PARSER_TYPE )

#define SENF_PARSER_SKIP                 BOOST_PP_CAT( SENF_PARSER_SKIP_,                 SENF_PARSER_TYPE )
#define SENF_PARSER_SKIP_BITS            BOOST_PP_CAT( SENF_PARSER_SKIP_BITS_,            SENF_PARSER_TYPE )
#define SENF_PARSER_GOTO                 BOOST_PP_CAT( SENF_PARSER_GOTO_,                 SENF_PARSER_TYPE )
#define SENF_PARSER_GOTO_OFFSET          BOOST_PP_CAT( SENF_PARSER_GOTO_OFFSET_,          SENF_PARSER_TYPE )
#define SENF_PARSER_LABEL                BOOST_PP_CAT( SENF_PARSER_LABEL_,                SENF_PARSER_TYPE )

#define SENF_PARSER_OFFSET               BOOST_PP_CAT( SENF_PARSER_OFFSET_,               SENF_PARSER_TYPE )
#define SENF_PARSER_FIXED_OFFSET         BOOST_PP_CAT( SENF_PARSER_FIXED_OFFSET_,         SENF_PARSER_TYPE )
#define SENF_PARSER_CURRENT_FIXED_OFFSET BOOST_PP_CAT( SENF_PARSER_CURRENT_FIXED_OFFSET_, SENF_PARSER_TYPE )

#define SENF_PARSER_FINALIZE             BOOST_PP_CAT( SENF_PARSER_FINALIZE_,             SENF_PARSER_TYPE )

#endif

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
