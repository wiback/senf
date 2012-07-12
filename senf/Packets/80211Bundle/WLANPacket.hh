// $Id$
//
// Copyright (C) 2008
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
//   Christian Niephaus <cni@berlios.de>

/** \file
    \brief 802.11 WLANPacket public header */

#ifndef HH_SENF_Packets_80211Bundle_WLANPacket_
#define HH_SENF_Packets_80211Bundle_WLANPacket_ 1

#include <senf/Packets/Packets.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/LlcSnapPacket.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf
{

    /** \brief 802.11 Frame parser
        (see IEEE 802.11-2007 standard - Chapter 7 Frame formats)
        <b>Re-ordering of bits due to host byte order</b>

        Frame base class.
     */
    struct WLANPacketParser : public PacketParserBase
    {
    #   include SENF_PARSER()

        /*
         * Frame control field
         * re-ordering of fields due to the byte order
         */
        SENF_PARSER_BITFIELD_RO ( subtype,        4,  unsigned );
        SENF_PARSER_BITFIELD_RO ( type,           2,  unsigned );
        SENF_PARSER_BITFIELD    ( version,        2,  unsigned );
        SENF_PARSER_BITFIELD    ( order,          1,  bool     );
        SENF_PARSER_BITFIELD    ( protectedFrame, 1,  bool     );
        SENF_PARSER_BITFIELD    ( moreData,       1,  bool     );
        SENF_PARSER_BITFIELD    ( pwrMgt,         1,  bool     );
        SENF_PARSER_BITFIELD    ( retry,          1,  bool     );
        SENF_PARSER_BITFIELD    ( moreFrag,       1,  bool     );
        SENF_PARSER_BITFIELD    ( fromDS,         1,  bool     );
        SENF_PARSER_BITFIELD    ( toDS,           1,  bool     );

        SENF_PARSER_FIELD       ( duration,       UInt16LSBParser );

        SENF_PARSER_FINALIZE(WLANPacketParser);
    };

    //-////////////////////////////////////////////////////////////////////////

    /** \brief Management frame parser
        <b>Re-ordering of bits due to LSB byte order</b>
     */
    struct WLANPacket_MgtFrameParser : public WLANPacketParser
    {
    #   include SENF_PARSER()

        SENF_PARSER_INHERIT(WLANPacketParser);

        SENF_PARSER_FIELD            ( destinationAddress, MACAddressParser );
        SENF_PARSER_FIELD            ( sourceAddress,      MACAddressParser );
        SENF_PARSER_FIELD            ( bssid,              MACAddressParser );

        //workaround since bitfield LSB parsers are not available
        SENF_PARSER_PRIVATE_BITFIELD ( seqNumber_1,    4, unsigned );
        SENF_PARSER_BITFIELD         ( fragmentNumber, 4, unsigned );
        SENF_PARSER_PRIVATE_FIELD    ( seqNumber_2,    UInt8Parser );

        SENF_PARSER_INIT() { type_() = 0; }

        SENF_PARSER_FINALIZE(WLANPacket_MgtFrameParser);

        boost::uint16_t sequenceNumber() const {
            return (uint16_t)(seqNumber_2()) << 4 | seqNumber_1();
        };

        void sequenceNumber(boost::uint16_t sn);
    };

    /** \brief WLAN Management frame packet

        \par Packet type (typedef):
            \ref WLANPacket_MgtFrame

        \par Fields:
            \ref WLANPacket_MgtFrameParser
            \image html WLANPacket_MgtFrame.png

        \ingroup protocolbundle_80211
     */
    struct WLANPacket_MgtFrameType
        : public PacketTypeBase,
          public PacketTypeMixin<WLANPacket_MgtFrameType>
    {
        typedef PacketTypeMixin<WLANPacket_MgtFrameType> mixin;
        typedef ConcretePacket<WLANPacket_MgtFrameType> packet;
        typedef WLANPacket_MgtFrameParser parser;

        using mixin::init;
        using mixin::initSize;
        using PacketTypeBase::nextPacketRange;

        static void dump(packet p, std::ostream & os);
    };

    /** \brief WLAN Management frame packet typedef
        \ingroup protocolbundle_80211
     */
    typedef WLANPacket_MgtFrameType::packet WLANPacket_MgtFrame;

    //-////////////////////////////////////////////////////////////////////////

    /** \brief Control frame parser
        <b>Re-ordering of bits due to LSB byte order</b>

        currently only CTS, RTS and ACK control frames are supported
     */
    struct WLANPacket_CtrlFrameParser : public WLANPacketParser
    {
    #   include SENF_PARSER()

        SENF_PARSER_INHERIT(WLANPacketParser);

        SENF_PARSER_FIELD   ( receiverAddress, MACAddressParser );

        //only RTS frame contains a source address field
        //variant is also needed to set correct subtype value
        SENF_PARSER_VARIANT ( subtype__, subtype,
                ( ids( na,            is_cts, set_cts, key(12, VoidPacketParser)) )
                ( ids( na,            is_ack, set_ack, key(13, VoidPacketParser)) )
                ( ids( sourceAddress, is_rts, set_rts, key(11, MACAddressParser)) ) );

        SENF_PARSER_INIT() { type_() = 1; }

        SENF_PARSER_FINALIZE(WLANPacket_CtrlFrameParser);
    };

    /** \brief WLAN Control frame packet

        \par Packet type (typedef):
            \ref WLANPacket_CtrlFrame

        \par Fields:
            \ref WLANPacket_CtrlFrameParser
            \image html WLANPacket_CtrlFrame.png

        \ingroup protocolbundle_80211
     */
    struct WLANPacket_CtrlFrameType
        : public PacketTypeBase,
          public PacketTypeMixin<WLANPacket_CtrlFrameType>
    {
        typedef PacketTypeMixin<WLANPacket_CtrlFrameType> mixin;
        typedef ConcretePacket<WLANPacket_CtrlFrameType> packet;
        typedef WLANPacket_CtrlFrameParser parser;

        using mixin::init;
        using mixin::initSize;
        using PacketTypeBase::nextPacketRange;

        static void dump(packet p, std::ostream & os);
    };

    /** \brief WLAN Control frame packet typedef
        \ingroup protocolbundle_80211
     */
    typedef WLANPacket_CtrlFrameType::packet WLANPacket_CtrlFrame;

    //-////////////////////////////////////////////////////////////////////////

    /** \brief Data frame parser
        <b>Re-ordering of bits due to LSB byte order</b>
     */
    struct WLANPacket_DataFrameParser : public WLANPacketParser
    {
    #   include SENF_PARSER()

        SENF_PARSER_INHERIT(WLANPacketParser);

    protected:
        typedef UIntFieldParser<6, 6+2> dsBits_t;
        dsBits_t::value_type dsBits() const { return parse<dsBits_t>( 1); }

        MACAddressParser addr1() const { return parse<MACAddressParser>(  4); }
        MACAddressParser addr2() const { return parse<MACAddressParser>( 10); }
        MACAddressParser addr3() const { return parse<MACAddressParser>( 16); }

        //sequence Number and fragment number
        //shift bits manually due to LSB

        typedef UIntFieldParser<0, 0+4> seqNumber_1_t;
        seqNumber_1_t seqNumber_1() const { return parse<seqNumber_1_t>( 22); }
        UInt8Parser seqNumber_2() const { return parse<UInt8Parser>( 23); }

    public:
        typedef UIntFieldParser<4, 4+4> fragmentNumber_t;
        fragmentNumber_t fragmentNumber() const { return parse<fragmentNumber_t>( 22); }

        boost::uint16_t sequenceNumber() const {
            return (uint16_t)(seqNumber_2()) << 4 | seqNumber_1();
        };

        void sequenceNumber(boost::uint16_t sn);

        SENF_PARSER_GOTO_OFFSET( 24, 24);

        // TODO fourth address field in case of WDS
        // SENF_PARSER_PRIVATE_VARIANT (wds_, dsBits,
        //     ( novalue ( disable_addr4,               VoidPacketParser ))
        //     ( id      ( addr4,  key (3,              MACAddressParser ))) );

        //QoS Field
        SENF_PARSER_VARIANT ( qosField_, subtype,
                ( ids( na,       na,           set_data,        key(0, VoidPacketParser)) )
                ( ids( na,       na,           set_nullData,    key(4, VoidPacketParser)) )
                ( ids( qosField, has_qosField, set_qosData,     key(8, UInt16LSBParser )) )
                //we cannot parse qos Null (data) frames at the moment
                ( ids( na,       na,           set_qosNullData, key(12, UInt16LSBParser)) ) );

        SENF_PARSER_INIT() { type_() = 2; }

        SENF_PARSER_FINALIZE(WLANPacket_DataFrameParser);

        MACAddressParser receiverAddress() const    { return addr1(); }; //ra is always addr1
        MACAddressParser transmitterAddress() const { return addr2(); }; //ta is always addr2
        MACAddressParser sourceAddress() const;
        MACAddressParser destinationAddress() const;
        MACAddressParser bssid() const;

        friend struct WLANPacket_DataFrameType;
    };

    /** \brief WLAN Data frame packet

        \par Packet type (typedef):
            \ref WLANPacket_DataFrame

        \par Fields:
            \ref WLANPacket_DataFrameParser
            \image html WLANPacket_DataFrame.png

        \ingroup protocolbundle_80211
     */
    struct WLANPacket_DataFrameType
        : public PacketTypeBase,
          public PacketTypeMixin<WLANPacket_DataFrameType>
    {
        typedef PacketTypeMixin<WLANPacket_DataFrameType> mixin;
        typedef ConcretePacket<WLANPacket_DataFrameType> packet;
        typedef WLANPacket_DataFrameParser parser;

        using mixin::init;
        using mixin::initSize;
        using mixin::nextPacketRange;

        static factory_t nextPacketType(packet p) {
            return p->subtype() == 0 || p->subtype() == 8
                ? LlcSnapPacket::factory()
                : no_factory();
        }

        static void dump(packet p, std::ostream & os);
    };

    /** \brief WLAN Data frame packet typedef
        \ingroup protocolbundle_80211
     */
    typedef WLANPacket_DataFrameType::packet WLANPacket_DataFrame;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "WLANPacket.cci"
//#include "WLANPacket.ct"
//#include "WLANPacket.cti"
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
