// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
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
    \brief 802.11 WLANPacket public header */

#ifndef HH_SENF_Packets_80211Bundle_WLANPacket_
#define HH_SENF_Packets_80211Bundle_WLANPacket_ 1

#include "../../Packets/Packets.hh"
#include "../DefaultBundle/EthernetPacket.hh"

namespace senf
{

    /** \brief Management frame parser
     * <b>Re-ordering of bits due to LSB byte order</b>

     */
    struct WLANPacket_MgtFrameParser : public senf::PacketParserBase
    {
    #   include SENF_FIXED_PARSER()
        
        SENF_PARSER_PRIVATE_BITFIELD ( subtype, 4,  unsigned                );
        //jump to fist address field
        SENF_PARSER_SKIP_BITS        (          28                          );
        SENF_PARSER_FIELD            ( destinationAddress, MACAddressParser );
        SENF_PARSER_FIELD            ( sourceAddress,      MACAddressParser );
        SENF_PARSER_FIELD            ( bssid,              MACAddressParser );

        //workaround since bitfield LSB parsers are not available
        SENF_PARSER_PRIVATE_BITFIELD ( seqNumber_1,    4, unsigned );
        SENF_PARSER_BITFIELD         ( fragmentNumber, 4, unsigned );
        SENF_PARSER_PRIVATE_FIELD    ( seqNumber_2,    UInt8Parser );

        SENF_PARSER_FINALIZE(WLANPacket_MgtFrameParser);

        //this is needed due to the goto in the WLANPacketParser. Don't know exactly why yet.
        SENF_PARSER_INIT() {}

        boost::uint16_t sequenceNumber() const {
            return (uint16_t)(seqNumber_2()) << 4 | seqNumber_1();
        };
    };

    /** \brief Control frame parser
     * <b>Re-ordering of bits due to LSB byte order</b>
     *
     * currently only CTS, RTS and ACK control frames are supported

     */
    struct WLANPacket_CtrlFrameParser : public senf::PacketParserBase
    {
    #   include SENF_PARSER()
        
        SENF_PARSER_PRIVATE_BITFIELD ( subtype,  4,  unsigned            );
        //jump to fist address field
        SENF_PARSER_SKIP_BITS        (          28                       );
        SENF_PARSER_FIELD            ( receiverAddress, MACAddressParser );

        //only RTS frame contains a source address field
        //variant is also needed to set correct subtype value
        SENF_PARSER_VARIANT ( subtype__, subtype,
                ( ids( na,            is_cts, set_cts, key(12, VoidPacketParser)) )
                ( ids( na,            is_ack, set_ack, key(13, VoidPacketParser)) )
                ( ids( sourceAddress, is_rts, set_rts, key(11, MACAddressParser)) ) );

        SENF_PARSER_FINALIZE(WLANPacket_CtrlFrameParser);

        //this is needed to due to the goto in the WLANPacketParser. Don't know exactly why yet.
        SENF_PARSER_INIT() {}
    };

    /** \brief Data frame parser
     * <b>Re-ordering of bits due to LSB byte order</b>

     */
    struct WLANPacket_DataFrameParser : public senf::PacketParserBase
    {
    #   include SENF_PARSER()
        SENF_PARSER_PRIVATE_BITFIELD ( subtype,  4,  unsigned  );
        //jump to 'toDS' and 'fromDS' bits
        //skip type, version and other flags
        SENF_PARSER_SKIP_BITS        ( 10                      );
        //needed in data frames due to the variable address fields
        SENF_PARSER_PRIVATE_BITFIELD ( dsBits,  2,  unsigned   );
        //skip duration field
        SENF_PARSER_SKIP             ( 2, 2                    );

        SENF_PARSER_PRIVATE_FIELD    ( addr1, MACAddressParser );
        SENF_PARSER_PRIVATE_FIELD    ( addr2, MACAddressParser );
        SENF_PARSER_PRIVATE_FIELD    ( addr3, MACAddressParser );

        //sequence Number and fragment number
        //shift bits manually due to LSB
        SENF_PARSER_PRIVATE_BITFIELD ( seqNumber_1,    4, unsigned );
        SENF_PARSER_BITFIELD         ( fragmentNumber, 4, unsigned );
        SENF_PARSER_PRIVATE_FIELD    ( seqNumber_2,    UInt8Parser );

        //TODO fourth address field in case of WDS
//        SENF_PARSER_PRIVATE_VARIANT (wds_, dsBits,
//                ( novalue ( disable_addr4,               VoidPacketParser                    ))
//                ( id      ( addr4,  key (3,                   MACAddressParser                     ))) );

        //QoS Filed
        SENF_PARSER_VARIANT ( qosField_, subtype,
                ( ids( na,       na,           set_data,        key(0, VoidPacketParser)) )
                ( ids( na,       na,           set_nullData,    key(4, VoidPacketParser)) )
                ( ids( qosField, has_qosField, set_qosData,     key(8, UInt16LSBParser )) )
                //we cannot parse qos Null (data) frames at the moment
                ( ids( na,       na,           set_qosNullData, key(12, UInt16LSBParser)) ) );

        SENF_PARSER_FINALIZE(WLANPacket_DataFrameParser);

        //this is needed to due to the goto in the WLANPacketParser. Don't know exactly why yet.
        SENF_PARSER_INIT() {}

        boost::uint16_t sequenceNumber() const {
            return (uint16_t)(seqNumber_2()) << 4 | seqNumber_1();
        };

        MACAddressParser receiverAddress() const    { return addr1(); }; //ra is always addr1
        MACAddressParser transmitterAddress() const { return addr2(); }; //ta is always addr2
        MACAddressParser sourceAddress() const;
        MACAddressParser destinationAddress() const;
        MACAddressParser bssid() const;
    };


    /** \brief 802.11 Frame parser
     * (see IEEE 802.11-2007 standard - Chapter 7 Frame formats)
     * <b>Re-ordering of bits due to LSB byte order</b>

     */
    struct WLANPacketParser : public senf::PacketParserBase
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

        SENF_PARSER_GOTO( subtype ); // subparsers need to know the subtype
        SENF_PARSER_VARIANT ( frameType_,  type,
                (     ids( mgtFrame,  is_mgtFrame,  init_mgtFrame,  WLANPacket_MgtFrameParser  ))
                (     ids( ctrlFrame, is_ctrlFrame, init_ctrlFrame, WLANPacket_CtrlFrameParser ))
                (     ids( dataFrame, is_dataFrame, init_dataFrame, WLANPacket_DataFrameParser ))
                ( novalue( reserved,                                WLANPacket_CtrlFrameParser )) );

        SENF_PARSER_CUSTOM_FIELD( fcs, senf::UInt32Parser, fcs_t::fixed_bytes, fcs_t::fixed_bytes) {
          return parse<UInt32Parser>( data().size()-4 ); }

        SENF_PARSER_FINALIZE(WLANPacketParser);

        SENF_PARSER_INIT() {
            version() = 0;
        }

        //Problems can occur with old madwifi and ath5k. Some frames only
        //contains two byte FCS instead of four.
//        UInt32Parser fcs() const { return parse<UInt32Parser>( data().size()-4 ); }

    };

    /** \brief WLAN packet

        \par Packet type (typedef):
            \ref WLANPacket

        \par Fields:
            \ref WLANPacketParser
	    \image html WLANPacket.png

        \ingroup protocolbundle_80211
     */
    struct WLANPacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<WLANPacketType>
    {
        typedef senf::PacketTypeMixin<WLANPacketType> mixin;
        typedef senf::ConcretePacket<WLANPacketType> packet;
        typedef WLANPacketParser parser;

//        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
        using senf::PacketTypeBase::nextPacketRange;;

        static void dump(packet p, std::ostream &os);
//        static PacketParserBase::size_type initSize();
    };

    typedef senf::ConcretePacket<WLANPacketType> WLANPacket;
}


#endif /* HH_SENF_Packets_80211Bundle_WLANPacket_ */
