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

#define SENF_FIXED_PARSER()     SENF_ABSOLUTE_INCLUDE_PATH(Packets/parse_fixed_setup.hh)
#define SENF_PARSER()           SENF_ABSOLUTE_INCLUDE_PATH(Packets/parse_setup.hh)

#define SENF_PARSER_INHERIT     BOOST_PP_CAT( SENF_PARSER_INHERIT_     , SENF_PARSER_TYPE )

#define SENF_PARSER_FIELD        BOOST_PP_CAT( SENF_PARSER_FIELD_        , SENF_PARSER_TYPE )
#define SENF_PARSER_FIELD_RO     BOOST_PP_CAT( SENF_PARSER_FIELD_RO_     , SENF_PARSER_TYPE )
#define SENF_PARSER_BITFIELD     BOOST_PP_CAT( SENF_PARSER_BITFIELD_     , SENF_PARSER_TYPE )
#define SENF_PARSER_BITFIELD_RO  BOOST_PP_CAT( SENF_PARSER_BITFIELD_RO_  , SENF_PARSER_TYPE )
#define SENF_PARSER_CUSTOM_FIELD BOOST_PP_CAT( SENF_PARSER_CUSTOM_FIELD_ , SENF_PARSER_TYPE )

#define SENF_PARSER_PRIVATE_FIELD        BOOST_PP_CAT( SENF_PARSER_P_FIELD_        , SENF_PARSER_TYPE )
#define SENF_PARSER_PRIVATE_FIELD_RO     BOOST_PP_CAT( SENF_PARSER_P_FIELD_RO_     , SENF_PARSER_TYPE )
#define SENF_PARSER_PRIVATE_BITFIELD     BOOST_PP_CAT( SENF_PARSER_P_BITFIELD_     , SENF_PARSER_TYPE )
#define SENF_PARSER_PRIVATE_BITFIELD_RO  BOOST_PP_CAT( SENF_PARSER_P_BITFIELD_RO_  , SENF_PARSER_TYPE )

#define SENF_PARSER_SKIP        BOOST_PP_CAT( SENF_PARSER_SKIP_        , SENF_PARSER_TYPE )
#define SENF_PARSER_SKIP_BITS   BOOST_PP_CAT( SENF_PARSER_SKIP_BITS_   , SENF_PARSER_TYPE )
#define SENF_PARSER_GOTO        BOOST_PP_CAT( SENF_PARSER_GOTO_        , SENF_PARSER_TYPE )
#define SENF_PARSER_GOTO_OFFSET BOOST_PP_CAT( SENF_PARSER_GOTO_OFFSET_ , SENF_PARSER_TYPE )
#define SENF_PARSER_LABEL       BOOST_PP_CAT( SENF_PARSER_LABEL_       , SENF_PARSER_TYPE )

#define SENF_PARSER_INIT()      void init(int)

#define SENF_PARSER_FINALIZE    BOOST_PP_CAT( SENF_PARSER_FINALIZE_    , SENF_PARSER_TYPE )

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
