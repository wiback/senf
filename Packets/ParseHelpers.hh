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
        typedef Parse_UIntField < 0,  3 > Parse_Priority;
        typedef Parse_Flag          < 3 > Parse_CFI;
        typedef Parse_UIntField < 4, 16 > Parse_VLanId;
        typedef Parse_UInt16              Parse_Type;

        SENF_PACKET_PARSER_INIT(Parse_EthVLan);

        SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
            ((OverlayField)( priority, Parse_Priority ))
            ((OverlayField)( cfi,      Parse_CFI      ))
            ((Field       )( vlanId,   Parse_VLanId   ))
            ((Field       )( type,     Parse_Type     )) );
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

#define SENF_FIXED_PARSER() SENF_ABSOLUTE_INCLUDE_PATH(Packets/parse_fixed_setup.hh)
#define SENF_PARSER()       SENF_ABSOLUTE_INCLUDE_PATH(Packets/parse_setup.hh)

#define SENF_FIXED_PARSER_INITIALIZE()                                                            \
    private:                                                                                      \
        SENF_MPL_SLOT_DEF_ZERO(index);                                                            \
        SENF_MPL_SLOT_DEF_ZERO(offset);                                                           \
        SENF_MPL_SLOT_DEF_ZERO(bit);                                                              \
        SENF_MPL_SLOT_DEF_ZERO(bitfield_size);                                                    \
        void init_chain(senf::mpl::rv<0>*) {}                                                     \
    public:

#define SENF_PARSER_INITIALIZE()                                                                  \
    private:                                                                                      \
        SENF_MPL_SLOT_DEF_ZERO(index);                                                            \
        SENF_MPL_SLOT_DEF_ZERO(init_bytes);                                                       \
        SENF_MPL_SLOT_DEF_ZERO(bit);                                                              \
        SENF_MPL_SLOT_DEF_ZERO(bitfield_size);                                                    \
        void init_chain(senf::mpl::rv<0>*) {}                                                     \
        size_type field_offset_(senf::mpl::rv<0>*) const { return 0; }                            \
    public:

#define SENF_PARSER_FIELD(name, type)              SENF_PARSER_FIELD_I(name, type, var, rw)
#define SENF_PARSER_FIELD_RO(name, type)           SENF_PARSER_FIELD_I(name, type, var, ro)
#define SENF_PARSER_CUSTOM_FIELD(name, type, size) SENF_PARSER_CUSTOM_FIELD_I(name, type, size, var)

#define SENF_PARSER_FIXED_FIELD(name, type)              SENF_PARSER_FIELD_I(name, type, fix, rw)
#define SENF_PARSER_FIXED_FIELD_RO(name, type)           SENF_PARSER_FIELD_I(name, type, fix, ro)
#define SENF_PARSER_FIXED_CUSTOM_FIELD(name, type, size) SENF_PARSER_CUSTOM_FIELD_I(name, type, size, fix)

#define SENF_PARSER_FIELD_AFTER(name, type, prev)              SENF_PARSER_FIELD_I_AFTER(name, type, prev, var, rw)
#define SENF_PARSER_FIELD_RO_AFTER(name, type, prev)           SENF_PARSER_FIELD_I_AFTER(name, type, prev, var, ro)
#define SENF_PARSER_CUSTOM_FIELD_AFTER(name, type, size, prev) SENF_PARSER_CUSTOM_FIELD_I_AFTER(name, type, size, prev, var)

#define SENF_PARSER_FIXED_FIELD_AFTER(name, type, prev)              SENF_PARSER_FIELD_I_AFTER(name, type, prev, fix, rw)
#define SENF_PARSER_FIXED_FIELD_RO_AFTER(name, type, prev)           SENF_PARSER_FIELD_I_AFTER(name, type, prev, fix, ro)
#define SENF_PARSER_FIXED_CUSTOM_FIELD_AFTER(name, type, size, prev) SENF_PARSER_CUSTOM_FIELD_I_AFTER(name, type, size, prev, fix)

#define SENF_PARSER_BITFIELD(name, bits, type)    SENF_PARSER_BITFIELD_I(name, bits, type, var, rw)
#define SENF_PARSER_BITFIELD_RO(name, bits, type) SENF_PARSER_BITFIELD_I(name, bits, type, var, ro)

#define SENF_PARSER_FIXED_BITFIELD(name, bits, type)    SENF_PARSER_BITFIELD_I(name, bits, type, fix, rw)
#define SENF_PARSER_FIXED_BITFIELD_RO(name, bits, type) SENF_PARSER_BITFIELD_I(name, bits, type, fix, ro)

#define SENF_PARSER_INIT()                                                                        \
     void init(int)

#define SENF_PARSER_FINALIZE_GENERIC(name, base)                                                  \
         void defaultInit() {                                                                     \
             init_chain(static_cast<senf::mpl::rv<SENF_MPL_SLOT_GET(index)>*>(0));                \
         }                                                                                        \
         name(data_iterator i, state_type s) : base(i,s) {}                                       \
    private:                                                                                      \
         template <class T> void init(T) { defaultInit(); }                                       \
    public:                                                                                       \
         void init() { init(0); }

#define SENF_PARSER_FINALIZE_INHERITED_DYNAMIC(name, base)                                        \
    SENF_PARSER_FINALIZE_GENERIC(name, base)                                                      \
    size_type bytes() const {                                                                     \
        return field_offset_(static_cast<senf::mpl::rv<SENF_MPL_SLOT_GET(index)>*>(0));           \
    }                                                                                             \
    static size_type const init_bytes = SENF_MPL_SLOT_GET(init_bytes)

#define SENF_PARSER_FINALIZE_INHERITED_FIXED(name, base)                                          \
    SENF_PARSER_FINALIZE_GENERIC(name, base)                                                      \
    static size_type const fixed_bytes = SENF_MPL_SLOT_GET(offset);

#define SENF_PARSER_FINALIZE_FIXED(name)                                                          \
    SENF_PARSER_FINALIZE_INHERITED_FIXED(name, senf::PacketParserBase)

#define SENF_PARSER_FINALIZE_DYNAMIC(name)                                                        \
    SENF_PARSER_FINALIZE_INHERITED_DYNAMIC(name, senf::PacketParserBase)

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
