//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#ifndef HH_SENF_Packets_DefaultBundle_ICMPv6TypePacket_
#define HH_SENF_Packets_DefaultBundle_ICMPv6TypePacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include "NDPOptions.hh"
#include "ICMPv6Packet.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {

    //#############################################################
    //ICMPv6 Echo Request
    //#############################################################
    struct ICMPv6EchoRequestParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD( identifier, UInt16Parser );
        SENF_PARSER_FIELD( seqNr,      UInt16Parser );

        SENF_PARSER_FINALIZE( ICMPv6EchoRequestParser );
    };

    /** \brief ICMPv6 Echo Request

        \par Packet type (typedef):
            \ref ICMPv6EchoRequest

        \par Fields:
            \ref ICMPv6EchoRequestParser
            \image html ICMPv6EchoRequest.png

        \ingroup protocolbundle_default
     */
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

        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<ICMPv6EchoRequestType> ICMPv6EchoRequest;

    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( ICMPv6EchoRequest );

    //#############################################################
    //ICMPv6 Echo Reply
    //#############################################################
    struct ICMPv6EchoReplyParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD( identifier, UInt16Parser );
        SENF_PARSER_FIELD( seqNr,      UInt16Parser );

        SENF_PARSER_FINALIZE( ICMPv6EchoReplyParser );
    };

    /** \brief ICMPv6 Echo Reply

        \par Packet type (typedef):
            \ref ICMPv6EchoReply

        \par Fields:
            \ref ICMPv6EchoReplyParser
            \image html ICMPv6EchoReply.png

        \ingroup protocolbundle_default
     */
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

        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<ICMPv6EchoReplyType> ICMPv6EchoReply;

    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( ICMPv6EchoReply );

    //#############################################################
    //ICMPv6 Error Destination Unreachable Message
    //#############################################################
    struct ICMPv6ErrDestUnreachableParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        //should be set static 0 by sender and ignored by receiver
        SENF_PARSER_PRIVATE_FIELD( unused, UInt32Parser );

        SENF_PARSER_FINALIZE( ICMPv6ErrDestUnreachableParser );

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
        void setErrCode(int code);
    };

    /** \brief ICMPv6 Destination unreachable

        \par Packet type (typedef):
            \ref ICMPv6ErrDestUnreachable

        \par Fields:
            \ref ICMPv6ErrDestUnreachableParser
            \image html ICMPv6ErrDestUnreachable.png

        \ingroup protocolbundle_default
    */
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

        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<ICMPv6ErrDestUnreachableType> ICMPv6ErrDestUnreachable;

    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( ICMPv6ErrDestUnreachable );

    //#############################################################
    //ICMPv6 Error Packet Too Big Message
    //#############################################################
    struct ICMPv6ErrTooBigParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD( mtu, UInt32Parser );
        SENF_PARSER_FINALIZE( ICMPv6ErrTooBigParser );

        /*   Code     static set to 0       */
        SENF_PARSER_INIT() {
            ICMPv6Packet icmpv6 (packet().rfind<ICMPv6Packet>(senf::nothrow));
            icmpv6->code() = 0;
        }
    };

    /** \brief ICMPv6 Packet to big

        \par Packet type (typedef):
            \ref ICMPv6ErrTooBig

        \par Fields:
            \ref ICMPv6ErrTooBigParser
            \image html ICMPv6ErrTooBig.png

        \ingroup protocolbundle_default
     */
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

        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<ICMPv6ErrTooBigType> ICMPv6ErrTooBig;

    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( ICMPv6ErrTooBig );

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
        void setErrCode(int code);

        SENF_PARSER_FINALIZE ( ICMPv6ErrTimeExceededParser );
    };

    /** \brief ICMPv6 Time exceeded

        \par Packet type (typedef):
            \ref ICMPv6ErrTimeExceeded

        \par Fields:
            \ref ICMPv6ErrTimeExceededParser
            \image html ICMPv6ErrTimeExceeded.png

        \ingroup protocolbundle_default
     */
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

        static void dump(packet p, std::ostream & os);
    };
    typedef ConcretePacket<ICMPv6ErrTimeExceededType> ICMPv6ErrTimeExceeded;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( ICMPv6ErrTimeExceeded );

    //#############################################################
    //ICMPv6 Error Parameter Problem Message
    //#############################################################
    struct ICMPv6ErrParamProblemParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        //should be set static 0 by sender and ignored by receiver
        SENF_PARSER_FIELD( pointer, UInt32Parser );
        /*  Code    0 - Erroneous header field encountered
                    1 - Unrecognized Next Header type encountered
                    2 - Unrecognized IPv6 option encountered          */

        void setErrCode(int code);

        SENF_PARSER_FINALIZE( ICMPv6ErrParamProblemParser );
    };

    /** \brief ICMPv6 Parameter problem

        \par Packet type (typedef):
            \ref ICMPv6ErrParamProblem

        \par Fields:
            \ref ICMPv6ErrParamProblemParser
            \image html ICMPv6ErrParamProblem.png

        \ingroup protocolbundle_default
     */
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

        static void dump(packet p, std::ostream & os);
    };
    typedef ConcretePacket<ICMPv6ErrParamProblemType> ICMPv6ErrParamProblem;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( ICMPv6ErrParamProblem );

    //#############################################################
    //ICMPv6 MLDv2 (RFC 3810) Multicast Listener Query
    //#############################################################
    struct MLDv2ListenerQueryParser : public PacketParserBase
    {
#      include SENF_PARSER()
        //need a variant here
        // a.) maxResponseCode < 32768 =>Integer
        // b.) maxResponseCode >=32768 => float (is there a float parser???)
        /*
        float value as followed:
        0 1 2 3 4 5 6 7 8 9 A B C D E F
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |1| exp |          mant         |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */

        SENF_PARSER_FIELD( maxResponseCode, UInt16Parser       );
        SENF_PARSER_FIELD( reserved,        UInt16Parser       ); // set to zero by default
        SENF_PARSER_FIELD( mcAddress,       INet6AddressParser );
        SENF_PARSER_BITFIELD( resv,  4, unsigned ); // set to zero by default
        SENF_PARSER_BITFIELD( sFlag, 1, unsigned );
        SENF_PARSER_BITFIELD( qrv,   3, unsigned );
        SENF_PARSER_FIELD        ( qqic, UInt8Parser       );
        SENF_PARSER_PRIVATE_FIELD( nrSources, UInt16Parser );
        SENF_PARSER_VECTOR( srcAddresses, nrSources, INet6AddressParser );

        SENF_PARSER_FINALIZE ( MLDv2ListenerQueryParser );

        SENF_PARSER_INIT() {
            reserved() = 0;
            resv() = 0;
        }
    };

    /** \brief MLDv2 Listener query

        \par Packet type (typedef):
            \ref MLDv2ListenerQuery

        \par Fields:
            \ref MLDv2ListenerQueryParser
            \image html MLDv2ListenerQuery.png

        \ingroup protocolbundle_default
     */
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

        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<MLDv2ListenerQueryType> MLDv2ListenerQuery;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( MLDv2ListenerQuery );

    //#############################################################
    //ICMPv6 MLDv2 (RFC 3810) Multicast Listener Report Message
    //#############################################################
    //First: the Multicast Address Record Parser
    struct MLDv2AddressRecordParser : public PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_FIELD        ( recordType, UInt8Parser        );
        SENF_PARSER_PRIVATE_FIELD( auxDataLen, UInt8Parser        );
        SENF_PARSER_PRIVATE_FIELD( nrOfSrcs,   UInt16Parser       );
        SENF_PARSER_FIELD        ( mcAddress,  INet6AddressParser );
        SENF_PARSER_VECTOR( srcAddresses, nrOfSrcs,   INet6AddressParser );
        SENF_PARSER_VECTOR( auxData,      auxDataLen, UInt32Parser       );

        SENF_PARSER_FINALIZE( MLDv2AddressRecordParser );
    };

    struct MLDv2ListenerReportParser : public PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_FIELD        ( reserved,            UInt16Parser );   //set to zero by default
        SENF_PARSER_PRIVATE_FIELD( nrMcastAddrRecords_, UInt16Parser );
        SENF_PARSER_LIST( mcastAddrRecords, nrMcastAddrRecords_, MLDv2AddressRecordParser );

        SENF_PARSER_FINALIZE( MLDv2ListenerReportParser );

        SENF_PARSER_INIT() {
            reserved() = 0;
        }
    };

    /** \brief MLDv2 Listener report

        \par Packet type (typedef):
            \ref MLDv2ListenerReport

        \par Fields:
            \ref MLDv2ListenerReportParser
            \image html MLDv2ListenerReport.png
            \image html MLDv2AddressRecord.png

        \ingroup protocolbundle_default
     */
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

        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<MLDv2ListenerReportType> MLDv2ListenerReport;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( MLDv2ListenerReport );

    //#############################################################
    //ICMPv6 Router Solicitation (RFC 4861) Message
    //#############################################################
    struct NDPRouterSolicitationParser : public PacketParserBase
    {
#      include SENF_PARSER()
        SENF_PARSER_BITFIELD( reserved, 32, unsigned );// set to zero by default
        SENF_PARSER_TLV_LIST( options, packetSize(), NDPGenericOptionParser );
        SENF_PARSER_FINALIZE( NDPRouterSolicitationParser );

        SENF_PARSER_INIT() {
            reserved() = 0;
        }
    };

    /** \brief Router Solicitation Message

        \par Packet type (typedef):
            \ref NDPRouterSolicitationMessage

        \par Fields:
            \ref NDPRouterSolicitationParser
            \image html NDPRouterSolicitationMessage.png

        \ingroup protocolbundle_default
     */
    struct NDPRouterSolicitationMessageType
        : public PacketTypeBase,
          public PacketTypeMixin<NDPRouterSolicitationMessageType>
    {
        typedef PacketTypeMixin<NDPRouterSolicitationMessageType> mixin;
        typedef ConcretePacket<NDPRouterSolicitationMessageType> packet;
        typedef NDPRouterSolicitationParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<NDPRouterSolicitationMessageType> NDPRouterSolicitationMessage;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( NDPRouterSolicitationMessage );

    //#############################################################
    //ICMPv6 Router Advertisement (RFC 4861) Message
    //#############################################################
    struct NDPRouterAdvertisementParser : public PacketParserBase
    {
#      include SENF_PARSER()
        SENF_PARSER_FIELD( curHopLimit, UInt8Parser );
        SENF_PARSER_BITFIELD( m,        1, bool     );
        SENF_PARSER_BITFIELD( o,        1, bool     );
        SENF_PARSER_BITFIELD( reserved, 6, unsigned ); // set to zero by default
        SENF_PARSER_FIELD( routerLifetime, UInt16Parser );
        SENF_PARSER_FIELD( reachableTime,  UInt32Parser );
        SENF_PARSER_FIELD( retransTimer,   UInt32Parser );
        SENF_PARSER_TLV_LIST( options, packetSize(), NDPGenericOptionParser );
        SENF_PARSER_FINALIZE( NDPRouterAdvertisementParser );

        SENF_PARSER_INIT() {
            reserved() = 0;
        }
    };

    /** \brief Router Advertisement Message

        \par Packet type (typedef):
            \ref NDPRouterAdvertisementMessage

        \par Fields:
            \ref NDPRouterAdvertisementParser
            \imageMessage html NDPRouterAdvertisementMessage.png

        \ingroup protocolbundle_default
     */
    struct NDPRouterAdvertisementMessageType
        : public PacketTypeBase,
          public PacketTypeMixin<NDPRouterAdvertisementMessageType>
    {
        typedef PacketTypeMixin<NDPRouterAdvertisementMessageType> mixin;
        typedef ConcretePacket<NDPRouterAdvertisementMessageType> packet;
        typedef NDPRouterAdvertisementParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<NDPRouterAdvertisementMessageType> NDPRouterAdvertisementMessage;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( NDPRouterAdvertisementMessage );

    //#############################################################
    //ICMPv6 Neighbor Solicitation (RFC 4861) Message
    //#############################################################
    struct NDPNeighborSolicitationParser : public PacketParserBase
    {
#      include SENF_PARSER()
        SENF_PARSER_BITFIELD( reserved, 32, unsigned ); // set to zero by default
        SENF_PARSER_FIELD   ( target, INet6AddressParser );
        SENF_PARSER_TLV_LIST( options, packetSize(), NDPGenericOptionParser );
        SENF_PARSER_FINALIZE( NDPNeighborSolicitationParser );

        SENF_PARSER_INIT() {
            reserved() = 0;
        }
    };

    /** \brief Neighbor Solicitation Message

        \par Packet type (typedef):
            \ref NDPNeighborSolicitationMessage

        \par Fields:
            \ref NDPNeighborSolicitationParser
            \imageMessage html NDPNeighborSolicitationMessage.png

        \ingroup protocolbundle_default
     */
    struct NDPNeighborSolicitationMessageType
        : public PacketTypeBase,
          public PacketTypeMixin<NDPNeighborSolicitationMessageType>
    {
        typedef PacketTypeMixin<NDPNeighborSolicitationMessageType> mixin;
        typedef ConcretePacket<NDPNeighborSolicitationMessageType> packet;
        typedef NDPNeighborSolicitationParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<NDPNeighborSolicitationMessageType> NDPNeighborSolicitationMessage;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( NDPNeighborSolicitationMessage );

    //#############################################################
    //ICMPv6 Neighbor Advertisement (RFC 4861) Message
    //#############################################################
    struct NDPNeighborAdvertisementParser : public PacketParserBase
    {
#      include SENF_PARSER()
        SENF_PARSER_BITFIELD( r,         1, bool     );
        SENF_PARSER_BITFIELD( s,         1, bool     );
        SENF_PARSER_BITFIELD( o,         1, bool     );
        SENF_PARSER_BITFIELD( reserved, 29, unsigned ); // set to zero by default
        SENF_PARSER_FIELD   ( target, INet6AddressParser );
        SENF_PARSER_TLV_LIST( options, packetSize(), NDPGenericOptionParser );
        SENF_PARSER_FINALIZE( NDPNeighborAdvertisementParser );

        SENF_PARSER_INIT() {
            reserved() = 0;
        }
    };

    /** \brief Neighbor Advertisement Message

        \par Packet type (typedef):
            \ref NDPNeighborAdvertisementParser

        \par Fields:
            \ref NDPNeighborAdvertisementParser
            \imageMessage html NDPNeighborAdvertisementMessage.png

        \ingroup protocolbundle_default
     */
    struct NDPNeighborAdvertisementMessageType
        : public PacketTypeBase,
          public PacketTypeMixin<NDPNeighborAdvertisementMessageType>
    {
        typedef PacketTypeMixin<NDPNeighborAdvertisementMessageType> mixin;
        typedef ConcretePacket<NDPNeighborAdvertisementMessageType> packet;
        typedef NDPNeighborAdvertisementParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<NDPNeighborAdvertisementMessageType> NDPNeighborAdvertisementMessage;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( NDPNeighborAdvertisementMessage );

    //#############################################################
    //ICMPv6 Redirect (RFC 4861) Message
    //#############################################################
    struct  NDPRedirectParser : public PacketParserBase
    {
#      include SENF_PARSER()
        SENF_PARSER_BITFIELD( reserved, 32, unsigned ); // set to zero by default
        SENF_PARSER_FIELD   ( target, INet6AddressParser );
        SENF_PARSER_FIELD   ( destination, INet6AddressParser );
        SENF_PARSER_TLV_LIST( options, packetSize(), NDPGenericOptionParser );
        SENF_PARSER_FINALIZE( NDPRedirectParser );

        SENF_PARSER_INIT() {
            reserved() = 0;

        }
    };

    /** \brief Redirect Message

        \par Packet type (typedef):
            \ref NDPRedirectParser

        \par Fields:
            \ref NDPRedirectParser
            \imageMessage html NDPRedirectMessage.png

        \ingroup protocolbundle_default
     */
    struct NDPRedirectMessageType
        : public PacketTypeBase,
          public PacketTypeMixin<NDPRedirectMessageType>
    {
        typedef PacketTypeMixin<NDPRedirectMessageType> mixin;
        typedef ConcretePacket<NDPRedirectMessageType> packet;
        typedef NDPRedirectParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<NDPRedirectMessageType> NDPRedirectMessage;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( NDPRedirectMessage );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "ICMPv6TypePacket.cci"
//#include "ICMPv6TypePacket.ct"
//#include "ICMPv6TypePacket.cti"
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
