// $Id: main.test.cc 206 2008-06-08 14:20:52Z pug $
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <Philipp.Batroff@fokus.fraunhofer.de>
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

#ifndef HH_ICMPV6TYPEPacket
#define HH_ICMPV6TYPEPacket
// Custom includes
#include "../../Packets/Packets.hh"
#include "ICMPv6Packet.hh"

namespace senf{
    //#############################################################
    //ICMPv6 Echo Request
    //#############################################################
    struct ICMPV6EchoRequestParser : public senf::PacketParserBase
    {
    #       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD    ( identifier, senf::UInt16Parser );
        SENF_PARSER_FIELD    ( seqNr, senf::UInt16Parser );
        
        SENF_PARSER_FINALIZE ( ICMPV6EchoRequestParser );
    };
    
    struct ICMPV6EchoRequestType
            : public senf::PacketTypeBase,
            public senf::PacketTypeMixin<ICMPV6EchoRequestType>
    {
        typedef senf::PacketTypeMixin<ICMPV6EchoRequestType> mixin;
        typedef senf::ConcretePacket<ICMPV6EchoRequestType> packet;
        typedef ICMPV6EchoRequestParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    
    typedef ICMPV6EchoRequestType::packet ICMPv6EchoReq;
    
    //#############################################################
    //ICMPv6 Echo Reply
    //#############################################################
    struct ICMPV6EchoReplyParser : public senf::PacketParserBase
    {
    #       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD    ( identifier, senf::UInt16Parser );
        SENF_PARSER_FIELD    ( seqNr, senf::UInt16Parser );
        
        SENF_PARSER_FINALIZE ( ICMPV6EchoReplyParser );
    };
    
    struct ICMPV6EchoReplyType
            : public senf::PacketTypeBase,
            public senf::PacketTypeMixin<ICMPV6EchoReplyType>
    {
        typedef senf::PacketTypeMixin<ICMPV6EchoReplyType> mixin;
        typedef senf::ConcretePacket<ICMPV6EchoReplyType> packet;
        typedef ICMPV6EchoReplyParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    typedef ICMPV6EchoReplyType::packet ICMPv6EchoReply;
    
    //#############################################################
    //ICMPv6 Error Destination Unreachable Message
    //#############################################################
    struct ICMPV6ErrDestUnreachableParser : public senf::PacketParserBase
    {
    #       include SENF_FIXED_PARSER()
        //should be set static 0 by sender and ignored by receiver
        SENF_PARSER_PRIVATE_FIELD    ( unused, senf::UInt32Parser );
        
        SENF_PARSER_INIT() { unused() = 0; }
        /*   Code     0 - No route to destination
                    1 - Communication with destination
                            administratively prohibited
                    2 - Beyond scope of source address
                    3 - Address unreachable
                    4 - Port unreachable
                    5 - Source address failed ingress/egress policy
                    6 - Reject route to destination   */
        void setErrCode(int code){
            ICMPv6Packet icmpv6 (senf::Packet().rfind<ICMPv6Packet>(senf::nothrow));
            icmpv6->code() = code;
        }
        
        SENF_PARSER_FINALIZE ( ICMPV6ErrDestUnreachableParser );
    };
    
    struct ICMPV6ErrDestUnreachableType
            : public senf::PacketTypeBase,
            public senf::PacketTypeMixin<ICMPV6ErrDestUnreachableType>
    {
        typedef senf::PacketTypeMixin<ICMPV6ErrDestUnreachableType> mixin;
        typedef senf::ConcretePacket<ICMPV6ErrDestUnreachableType> packet;
        typedef ICMPV6ErrDestUnreachableParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    typedef ICMPV6ErrDestUnreachableType::packet ICMPV6ErrDestUnreachable;
    
    //#############################################################
    //ICMPv6 Error Packet Too Big Message
    //#############################################################
    struct ICMPV6ErrTooBigParser : public senf::PacketParserBase
    {
    #       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD    ( MTU, senf::UInt32Parser );
    
        /*   Code     static set to 0       */
    //     SENF_PARSER_INIT() {
    //         ICMPv6Packet icmpv6 (senf::Packet().rfind<ICMPv6Packet>(senf::nothrow));
    //         icmpv6->code() = 0;
    //     }
        
        SENF_PARSER_FINALIZE ( ICMPV6ErrTooBigParser );
    };
    
    struct ICMPV6ErrTooBigType
            : public senf::PacketTypeBase,
            public senf::PacketTypeMixin<ICMPV6ErrTooBigType>
    {
        typedef senf::PacketTypeMixin<ICMPV6ErrTooBigType> mixin;
        typedef senf::ConcretePacket<ICMPV6ErrTooBigType> packet;
        typedef ICMPV6ErrTooBigParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    typedef ICMPV6ErrTooBigType::packet ICMPV6ErrTooBig;
    
    //#############################################################
    //ICMPv6 Error Time Exceeded Message
    //#############################################################
    struct ICMPV6ErrTimeExceededParser : public senf::PacketParserBase
    {
    #       include SENF_FIXED_PARSER()
        //should be set static 0 by sender and ignored by receiver
        SENF_PARSER_FIELD    ( unused, senf::UInt32Parser );
        /*  Code    0 - Hop limit exceeded in transit
                    1 - Fragment reassembly time exceeded   */
        void setErrCode(int code){
            ICMPv6Packet icmpv6 (senf::Packet().rfind<ICMPv6Packet>(senf::nothrow));
            icmpv6->code() = code;
        }
        
        SENF_PARSER_FINALIZE ( ICMPV6ErrTimeExceededParser );
    };
    
    struct ICMPV6ErrTimeExceededType
            : public senf::PacketTypeBase,
            public senf::PacketTypeMixin<ICMPV6ErrTimeExceededType>
    {
        typedef senf::PacketTypeMixin<ICMPV6ErrTimeExceededType> mixin;
        typedef senf::ConcretePacket<ICMPV6ErrTimeExceededType> packet;
        typedef ICMPV6ErrTimeExceededParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    typedef ICMPV6ErrTimeExceededType::packet ICMPV6ErrTimeExceeded;
    
    //#############################################################
    //ICMPv6 Error Parameter Problem Message
    //#############################################################
    struct ICMPV6ErrParamProblemParser : public senf::PacketParserBase
    {
    #       include SENF_FIXED_PARSER()
        //should be set static 0 by sender and ignored by receiver
        SENF_PARSER_FIELD    ( pointer, senf::UInt32Parser );
        /*  Code      0 - Erroneous header field encountered
                    1 - Unrecognized Next Header type encountered
                    2 - Unrecognized IPv6 option encountered          */
        
        void setErrCode(int code){
            ICMPv6Packet icmpv6 (senf::Packet().rfind<ICMPv6Packet>(senf::nothrow));
            icmpv6->code() = code;
        }
        SENF_PARSER_FINALIZE ( ICMPV6ErrParamProblemParser );
    };
    
    struct ICMPV6ErrParamProblemType
            : public senf::PacketTypeBase,
            public senf::PacketTypeMixin<ICMPV6ErrParamProblemType>
    {
        typedef senf::PacketTypeMixin<ICMPV6ErrParamProblemType> mixin;
        typedef senf::ConcretePacket<ICMPV6ErrParamProblemType> packet;
        typedef ICMPV6ErrParamProblemParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    typedef ICMPV6ErrParamProblemType::packet ICMPV6ErrParamProblem;
    
    //#############################################################
    //ICMPv6 MLDv2 (RFC 3810) Multicast Listener Query
    //#############################################################
    struct MLDv2ListenerQueryParser : public senf::PacketParserBase
    {
    #   include SENF_PARSER()
    //need a variant here
        // a.) maxResponseCode < 32768 =>Interger
        // b.) maxResponseCode >=32768 => float (is there a float parser???)
        /* 
        float value as followed: 
        0 1 2 3 4 5 6 7 8 9 A B C D E F
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |1| exp |          mant         |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
        
        SENF_PARSER_FIELD   ( maxResponseCode, senf::UInt16Parser );
        SENF_PARSER_FIELD   ( reserved, senf::UInt16Parser );   //set to zero by default
        SENF_PARSER_FIELD   ( mcAddress, senf::INet6AddressParser);
        SENF_PARSER_BITFIELD ( resv, 4, unsigned);              //set to zero by default
        SENF_PARSER_BITFIELD ( sFlag, 1, unsigned);
        SENF_PARSER_BITFIELD ( qrv, 3, unsigned);
        SENF_PARSER_FIELD ( qqic, senf::UInt8Parser );
        SENF_PARSER_PRIVATE_FIELD ( nrSources, senf::UInt16Parser );
        SENF_PARSER_VECTOR  (srcAddresses, nrSources, senf::INet6AddressParser );
        
        SENF_PARSER_INIT() 
        { 
            reserved() = 0; 
            resv() = 0;
        }
        SENF_PARSER_FINALIZE ( MLDv2ListenerQueryParser );
        
    };
    
    struct MLDv2ListenerQueryType
            : public senf::PacketTypeBase,
            public senf::PacketTypeMixin<MLDv2ListenerQueryType>
    {
        typedef senf::PacketTypeMixin<MLDv2ListenerQueryType> mixin;
        typedef senf::ConcretePacket<MLDv2ListenerQueryType> packet;
        typedef MLDv2ListenerQueryParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    typedef MLDv2ListenerQueryType::packet MLDv2ListenerQuery;
    
    //#############################################################
    //ICMPv6 MLDv2 (RFC 3810) Multicast Listener Report Message
    //#############################################################
    //First: the Multicast Address Record Parser
    struct MLDv2AddressRecordParser : public senf::PacketParserBase
    {
    #   include SENF_PARSER()
        SENF_PARSER_FIELD   ( recordType, senf::UInt8Parser );
        SENF_PARSER_PRIVATE_FIELD   ( auxDataLen, senf::UInt8Parser );
        SENF_PARSER_PRIVATE_FIELD   ( nrOfSrcs, senf::UInt16Parser );
        SENF_PARSER_FIELD   ( mcAddress, senf::INet6AddressParser);
        SENF_PARSER_VECTOR  ( srcAddresses, nrOfSrcs, senf::INet6AddressParser );
        SENF_PARSER_VECTOR  ( auxData, auxDataLen, senf::UInt32Parser );
        
        
        SENF_PARSER_FINALIZE ( MLDv2AddressRecordParser );
    };
    
    struct MLDv2ListenerReportParser : public senf::PacketParserBase
    {
    #   include SENF_PARSER()
        SENF_PARSER_FIELD   ( reserved, senf::UInt16Parser );   //set to zero by default
        SENF_PARSER_PRIVATE_FIELD   ( nrMcastAddrRecords_, senf::UInt16Parser );
        SENF_PARSER_LIST    ( mcastAddrRecords, nrMcastAddrRecords_, MLDv2AddressRecordParser );
        
        SENF_PARSER_INIT() { reserved() = 0; }
        
        SENF_PARSER_FINALIZE ( MLDv2ListenerReportParser );
    };
    
    struct MLDv2ListenerReportType
            : public senf::PacketTypeBase,
            public senf::PacketTypeMixin<MLDv2ListenerReportType>
    {
        typedef senf::PacketTypeMixin<MLDv2ListenerReportType> mixin;
        typedef senf::ConcretePacket<MLDv2ListenerReportType> packet;
        typedef MLDv2ListenerReportParser parser;
        
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    typedef MLDv2ListenerReportType::packet MLDv2ListenerReport;
}
#endif
