// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <pug@berlios.de>
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

// Definition of non-inline non-template functions

#ifndef HH_SENF_Packets_DefaultBundle_ICMPv6TypePacket_
#define HH_SENF_Packets_DefaultBundle_ICMPv6TypePacket_ 1

// Custom includes
#include "../../Packets/Packets.hh"
#include "ICMPv6Packet.hh"

namespace senf {

    //#############################################################
    //ICMPv6 Echo Request
    //#############################################################
    struct ICMPv6EchoRequestParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD ( identifier, UInt16Parser );
        SENF_PARSER_FIELD ( seqNr,      UInt16Parser );
        
        SENF_PARSER_FINALIZE ( ICMPv6EchoRequestParser );
    };
    
    struct ICMPv6EchoRequestType
        : public PacketTypeBase,
          public PacketTypeMixin<ICMPv6EchoRequestType>
    {
        typedef PacketTypeMixin<ICMPv6EchoRequestType> mixin;
        typedef ConcretePacket<ICMPv6EchoRequestType> packet;
        typedef ICMPv6EchoRequestParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    
    typedef ConcretePacket<ICMPv6EchoRequestType> ICMPv6EchoRequest;
    
    //#############################################################
    //ICMPv6 Echo Reply
    //#############################################################
    struct ICMPv6EchoReplyParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD ( identifier, UInt16Parser );
        SENF_PARSER_FIELD ( seqNr,      UInt16Parser );
        
        SENF_PARSER_FINALIZE ( ICMPv6EchoReplyParser );
    };
    
    struct ICMPv6EchoReplyType
        : public PacketTypeBase,
          public PacketTypeMixin<ICMPv6EchoReplyType>
    {
        typedef PacketTypeMixin<ICMPv6EchoReplyType> mixin;
        typedef ConcretePacket<ICMPv6EchoReplyType> packet;
        typedef ICMPv6EchoReplyParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
        
    typedef ConcretePacket<ICMPv6EchoReplyType> ICMPv6EchoReply;
    
    //#############################################################
    //ICMPv6 Error Destination Unreachable Message
    //#############################################################
    struct ICMPv6ErrDestUnreachableParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        
        //should be set static 0 by sender and ignored by receiver
        SENF_PARSER_PRIVATE_FIELD ( unused, UInt32Parser );
        
        SENF_PARSER_INIT() { 
            unused() = 0; 
        }
        /*   Code     0 - No route to destination
                    1 - Communication with destination
                            administratively prohibited
                    2 - Beyond scope of source address
                    3 - Address unreachable
                    4 - Port unreachable
                    5 - Source address failed ingress/egress policy
                    6 - Reject route to destination   */
        void setErrCode(int code) {
            ICMPv6Packet icmpv6 (senf::Packet().rfind<ICMPv6Packet>(senf::nothrow));
            icmpv6->code() = code;
        }
        
        SENF_PARSER_FINALIZE ( ICMPv6ErrDestUnreachableParser );
    };
    
    struct ICMPv6ErrDestUnreachableType
        : public PacketTypeBase,
          public PacketTypeMixin<ICMPv6ErrDestUnreachableType>
    {
        typedef PacketTypeMixin<ICMPv6ErrDestUnreachableType> mixin;
        typedef ConcretePacket<ICMPv6ErrDestUnreachableType> packet;
        typedef ICMPv6ErrDestUnreachableParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
        
    typedef ConcretePacket<ICMPv6ErrDestUnreachableType> ICMPv6ErrDestUnreachable;
    
    //#############################################################
    //ICMPv6 Error Packet Too Big Message
    //#############################################################
    struct ICMPv6ErrTooBigParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD ( mtu, UInt32Parser );
    
        /*   Code     static set to 0       */
    //     SENF_PARSER_INIT() {
    //         ICMPv6Packet icmpv6 (senf::Packet().rfind<ICMPv6Packet>(senf::nothrow));
    //         icmpv6->code() = 0;
    //     }
        
        SENF_PARSER_FINALIZE ( ICMPv6ErrTooBigParser );
    };
    
    struct ICMPv6ErrTooBigType
        : public PacketTypeBase,
          public PacketTypeMixin<ICMPv6ErrTooBigType>
    {
        typedef PacketTypeMixin<ICMPv6ErrTooBigType> mixin;
        typedef ConcretePacket<ICMPv6ErrTooBigType> packet;
        typedef ICMPv6ErrTooBigParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };

    typedef ConcretePacket<ICMPv6ErrTooBigType> ICMPv6ErrTooBig;
    
    //#############################################################
    //ICMPv6 Error Time Exceeded Message
    //#############################################################
    struct ICMPv6ErrTimeExceededParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        //should be set static 0 by sender and ignored by receiver
        SENF_PARSER_FIELD    ( unused, UInt32Parser );
        /*  Code    0 - Hop limit exceeded in transit
                    1 - Fragment reassembly time exceeded   */
        void setErrCode(int code){
            ICMPv6Packet icmpv6 (senf::Packet().rfind<ICMPv6Packet>(senf::nothrow));
            icmpv6->code() = code;
        }
        
        SENF_PARSER_FINALIZE ( ICMPv6ErrTimeExceededParser );
    };
    
    struct ICMPv6ErrTimeExceededType
        : public PacketTypeBase,
          public PacketTypeMixin<ICMPv6ErrTimeExceededType>
    {
        typedef PacketTypeMixin<ICMPv6ErrTimeExceededType> mixin;
        typedef ConcretePacket<ICMPv6ErrTimeExceededType> packet;
        typedef ICMPv6ErrTimeExceededParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    typedef ConcretePacket<ICMPv6ErrTimeExceededType> ICMPv6ErrTimeExceeded;
    
    //#############################################################
    //ICMPv6 Error Parameter Problem Message
    //#############################################################
    struct ICMPv6ErrParamProblemParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        //should be set static 0 by sender and ignored by receiver
        SENF_PARSER_FIELD ( pointer, UInt32Parser );
        /*  Code      0 - Erroneous header field encountered
                    1 - Unrecognized Next Header type encountered
                    2 - Unrecognized IPv6 option encountered          */
        
        void setErrCode(int code){
            ICMPv6Packet icmpv6 (senf::Packet().rfind<ICMPv6Packet>(senf::nothrow));
            icmpv6->code() = code;
        }
        SENF_PARSER_FINALIZE ( ICMPv6ErrParamProblemParser );
    };
    
    struct ICMPv6ErrParamProblemType
        : public PacketTypeBase,
          public PacketTypeMixin<ICMPv6ErrParamProblemType>
    {
        typedef PacketTypeMixin<ICMPv6ErrParamProblemType> mixin;
        typedef ConcretePacket<ICMPv6ErrParamProblemType> packet;
        typedef ICMPv6ErrParamProblemParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    typedef ConcretePacket<ICMPv6ErrParamProblemType> ICMPv6ErrParamProblem;
    
    //#############################################################
    //ICMPv6 MLDv2 (RFC 3810) Multicast Listener Query
    //#############################################################
    struct MLDv2ListenerQueryParser : public PacketParserBase
    {
#      include SENF_PARSER()
        //need a variant here
        // a.) maxResponseCode < 32768 =>Interger
        // b.) maxResponseCode >=32768 => float (is there a float parser???)
        /* 
        float value as followed: 
        0 1 2 3 4 5 6 7 8 9 A B C D E F
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |1| exp |          mant         |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
        
        SENF_PARSER_FIELD ( maxResponseCode, UInt16Parser       );
        SENF_PARSER_FIELD ( reserved,        UInt16Parser       ); // set to zero by default
        SENF_PARSER_FIELD ( mcAddress,       INet6AddressParser );
        SENF_PARSER_BITFIELD ( resv,  4, unsigned ); // set to zero by default
        SENF_PARSER_BITFIELD ( sFlag, 1, unsigned );
        SENF_PARSER_BITFIELD ( qrv,   3, unsigned );
        SENF_PARSER_FIELD    ( qqic, UInt8Parser );
        SENF_PARSER_PRIVATE_FIELD ( nrSources, UInt16Parser );
        SENF_PARSER_VECTOR   (srcAddresses, nrSources, INet6AddressParser );
        
        SENF_PARSER_FINALIZE ( MLDv2ListenerQueryParser );
        
        SENF_PARSER_INIT() { 
            reserved() = 0; 
            resv() = 0;
        }
    };
    
    struct MLDv2ListenerQueryType
        : public PacketTypeBase,
          public PacketTypeMixin<MLDv2ListenerQueryType>
    {
        typedef PacketTypeMixin<MLDv2ListenerQueryType> mixin;
        typedef ConcretePacket<MLDv2ListenerQueryType> packet;
        typedef MLDv2ListenerQueryParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
        
    typedef ConcretePacket<MLDv2ListenerQueryType> MLDv2ListenerQuery;
    
    //#############################################################
    //ICMPv6 MLDv2 (RFC 3810) Multicast Listener Report Message
    //#############################################################
    //First: the Multicast Address Record Parser
    struct MLDv2AddressRecordParser : public PacketParserBase
    {
#       include SENF_PARSER()
        
        SENF_PARSER_FIELD   ( recordType, UInt8Parser );
        SENF_PARSER_PRIVATE_FIELD   ( auxDataLen, UInt8Parser );
        SENF_PARSER_PRIVATE_FIELD   ( nrOfSrcs, UInt16Parser );
        SENF_PARSER_FIELD   ( mcAddress, INet6AddressParser);
        SENF_PARSER_VECTOR  ( srcAddresses, nrOfSrcs, INet6AddressParser );
        SENF_PARSER_VECTOR  ( auxData, auxDataLen, UInt32Parser );
        
        SENF_PARSER_FINALIZE ( MLDv2AddressRecordParser );
    };
    
    struct MLDv2ListenerReportParser : public PacketParserBase
    {
#       include SENF_PARSER()
        
        SENF_PARSER_FIELD   ( reserved, UInt16Parser );   //set to zero by default
        SENF_PARSER_PRIVATE_FIELD   ( nrMcastAddrRecords_, UInt16Parser );
        SENF_PARSER_LIST    ( mcastAddrRecords, nrMcastAddrRecords_, MLDv2AddressRecordParser );
        
        SENF_PARSER_FINALIZE ( MLDv2ListenerReportParser );
        
        SENF_PARSER_INIT() { 
            reserved() = 0; 
        }
    };
    
    struct MLDv2ListenerReportType
        : public PacketTypeBase,
          public PacketTypeMixin<MLDv2ListenerReportType>
    {
        typedef PacketTypeMixin<MLDv2ListenerReportType> mixin;
        typedef ConcretePacket<MLDv2ListenerReportType> packet;
        typedef MLDv2ListenerReportParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
        
    typedef ConcretePacket<MLDv2ListenerReportType> MLDv2ListenerReport;
}

#endif