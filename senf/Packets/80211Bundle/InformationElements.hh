// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//   Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

/** \file
    \brief WLAN Information Elements public header */

#ifndef HH_SENF_Packets_80211Bundle_InformationElements_
#define HH_SENF_Packets_80211Bundle_InformationElements_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    struct WLANInfoElementParser
        : public PacketParserBase
    {
    #   include SENF_PARSER()
        SENF_PARSER_FIELD   ( type,   UInt8Parser   );
        SENF_PARSER_FIELD   ( length, UInt8Parser   );
        SENF_PARSER_FINALIZE( WLANInfoElementParser );

        typedef GenericTLVParserRegistry<WLANInfoElementParser> Registry;
    };

    typedef GenericTLVParserBase<WLANInfoElementParser> WLANGenericInfoElementParser;


    struct WLANSSIDInfoElementParser
        : public WLANInfoElementParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT ( WLANInfoElementParser            );
        // the StringParser includes the length field so we have to go back
        SENF_PARSER_GOTO    ( length                           );
        SENF_PARSER_FIELD   ( value, StringParser<UInt8Parser> );
        SENF_PARSER_FINALIZE( WLANSSIDInfoElementParser        );

        SENF_PARSER_INIT() {
            type() = typeId;
            length() = 0u;
        }
        static const type_t::value_type typeId = 0x00u;

        void dump(std::ostream & os) const;
    };


    struct WLANSupportedRatesInfoElementParser
        : public WLANInfoElementParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT ( WLANInfoElementParser               );
        // we just skip the value here. If somebody needs the information he has to implement
        // this strange information element himself.
        SENF_PARSER_SKIP    ( length(), 0                         );
        SENF_PARSER_FINALIZE( WLANSupportedRatesInfoElementParser );

        SENF_PARSER_INIT() {
            type() = typeId;
        }
        static const type_t::value_type typeId = 0x01u;

        void dump(std::ostream & os) const;
    };


    struct WLANPowerConstraintInfoElementParser
        : public WLANInfoElementParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT ( WLANInfoElementParser                );
        SENF_PARSER_FIELD   ( value, UInt8Parser                   );
        SENF_PARSER_FINALIZE( WLANPowerConstraintInfoElementParser );

        SENF_PARSER_INIT() {
            type() = typeId;
            length() = 1u;
        }
        static const type_t::value_type typeId = 0x20u;

        void dump(std::ostream & os) const;
    };


    struct WLANVendorSpecificInfoElementParser
        : public WLANInfoElementParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT ( WLANInfoElementParser               );
        SENF_PARSER_FIELD   ( oui, UInt24Parser                   );
        SENF_PARSER_SKIP    ( length()-3, 0                       );
        SENF_PARSER_FINALIZE( WLANVendorSpecificInfoElementParser );

        SENF_PARSER_INIT() {
            type() = typeId;
            length() = 3u;
        }
        static const type_t::value_type typeId = 0xddu;

        void dump(std::ostream & os) const;
    };


    struct WLANHTCapabilitiesInfoFieldParser
        : public PacketParserBase
    {
    public:
#       include SENF_FIXED_PARSER()
        // re-ordering of the fields due to the byte order
        SENF_PARSER_BITFIELD ( txSTBC,                    1, bool     );
        SENF_PARSER_BITFIELD ( shortGIfor40MHz,           1, bool     );
        SENF_PARSER_BITFIELD ( shortGIfor20MHz,           1, bool     );
        SENF_PARSER_BITFIELD ( htGreenfield,              1, bool     );
        SENF_PARSER_BITFIELD ( smPowerSave,               2, unsigned );
        SENF_PARSER_BITFIELD ( supportedChannelWidth,     1, bool     );
        SENF_PARSER_BITFIELD ( ldpcCodingCapability,      1, bool     );
        SENF_PARSER_BITFIELD ( lSIGTXOPProtectionSupport, 1, bool     );
        SENF_PARSER_BITFIELD ( fortyMHzIntolerant,        1, bool     );
        SENF_PARSER_SKIP_BITS(                            1           );  // reserved
        SENF_PARSER_BITFIELD ( dsss_cckModeIn40MHz,       1, bool     );
        SENF_PARSER_BITFIELD ( maxAMSDULength,            1, bool     );
        SENF_PARSER_BITFIELD ( delayedBlockAck,           1, bool     );
        SENF_PARSER_BITFIELD ( rxSTBC,                    2, unsigned );
        SENF_PARSER_FINALIZE ( WLANHTCapabilitiesInfoFieldParser      );
    };

    struct WLANAMPDUParametersParser
        : public PacketParserBase
    {
    public:
#       include SENF_FIXED_PARSER()
        // re-ordering of the fields due to the byte order
        SENF_PARSER_SKIP_BITS(                      3           );  // reserved
        SENF_PARSER_BITFIELD ( minMPDUStartSpacing, 3, unsigned );
        SENF_PARSER_BITFIELD ( maxAMPDULength,      2, unsigned );
        SENF_PARSER_FINALIZE ( WLANAMPDUParametersParser        );
    };

    struct WLANSupportedMCSSetParser
        : public PacketParserBase
    {
    public:
#       include SENF_FIXED_PARSER()
        // re-ordering of the fields due to the byte order
        typedef BitsetParser<77, MSB0Endianness> MCSBitmaskParser;
        SENF_PARSER_FIELD    ( rxMCSBitmask,            MCSBitmaskParser );  // the remaining 3 bits are already captured by the BitsetParser
        SENF_PARSER_SKIP_BITS(                               6           );  // reserved
        SENF_PARSER_BITFIELD ( rxHighestSupoortedDataRate,  10, unsigned );
        SENF_PARSER_SKIP_BITS(                               3           );  // reserved
        SENF_PARSER_BITFIELD ( txUnequalModulationSupported, 1, bool     );
        SENF_PARSER_BITFIELD ( txMaxStreamsSupported,        2, unsigned );
        SENF_PARSER_BITFIELD ( txRxMCSSetNotEqual,           1, bool     );
        SENF_PARSER_BITFIELD ( txMCSSetDefined,              1, bool     );
        SENF_PARSER_SKIP     ( 3                                         );  // reserved
        SENF_PARSER_FINALIZE ( WLANSupportedMCSSetParser                 );
    };

    struct WLANHTExtendedCapabilitiesParser
        : public PacketParserBase
    {
    public:
#       include SENF_FIXED_PARSER()
        // re-ordering of the fields due to the byte order
        SENF_PARSER_SKIP_BITS(                    5           );  // reserved
        SENF_PARSER_BITFIELD ( pcoTransitionTime, 2, unsigned );
        SENF_PARSER_BITFIELD ( pco,               1, bool     );
        SENF_PARSER_SKIP_BITS(                    4           );  // reserved
        SENF_PARSER_BITFIELD ( rdResponder,       1, bool     );
        SENF_PARSER_BITFIELD ( htcSupport,        1, bool     );
        SENF_PARSER_BITFIELD ( mcsFeedback,       2, unsigned );
        SENF_PARSER_FINALIZE ( WLANHTExtendedCapabilitiesParser );
    };

    struct WLANTxBeamformingCapabilitiesParser
        : public PacketParserBase
    {
    public:
#       include SENF_FIXED_PARSER()
        // if somebody needs the information he has to implement this himself. sorry.
        SENF_PARSER_SKIP( 4 );
        SENF_PARSER_FINALIZE( WLANTxBeamformingCapabilitiesParser );
    };

    struct WLANAntennaSelectionCapabilityParser
        : public PacketParserBase
    {
    public:
#       include SENF_FIXED_PARSER()
        // if somebody needs the information he has to implement this himself. sorry.
        SENF_PARSER_SKIP( 1 );
        SENF_PARSER_FINALIZE( WLANAntennaSelectionCapabilityParser );
    };

    struct WLANHTCapabilitiesInfoElementParser
        : public WLANInfoElementParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT ( WLANInfoElementParser                                            );
        SENF_PARSER_FIELD   ( capabilitiesInfo,           WLANHTCapabilitiesInfoFieldParser    );
        SENF_PARSER_FIELD   ( aMPDUParameters,            WLANAMPDUParametersParser            );
        SENF_PARSER_FIELD   ( supportedMCSSet,            WLANSupportedMCSSetParser            );
        SENF_PARSER_FIELD   ( extendedCapabilities,       WLANHTExtendedCapabilitiesParser     );
        SENF_PARSER_FIELD   ( txBeamformingCapabilities,  WLANTxBeamformingCapabilitiesParser  );
        SENF_PARSER_FIELD   ( antennaSelectionCapability, WLANAntennaSelectionCapabilityParser );
        SENF_PARSER_FINALIZE( WLANHTCapabilitiesInfoElementParser                              );

        SENF_PARSER_INIT() {
            type() = typeId;
            length() = 26u;
        }
        static const type_t::value_type typeId = 0x2du;

        void dump(std::ostream & os) const;
    };


    struct WLANHTOperationInfoFieldParser
        : public PacketParserBase
    {
    public:
#       include SENF_FIXED_PARSER()
        // re-ordering of the fields due to the byte order
        SENF_PARSER_SKIP_BITS(                             4           );  // reserved
        SENF_PARSER_BITFIELD ( rifsMode,                   1, bool     );
        SENF_PARSER_BITFIELD ( channelWidth,               1, bool     );
        SENF_PARSER_BITFIELD ( secondaryChannelOffset,     2, unsigned );
        SENF_PARSER_SKIP_BITS(                             3           );  // reserved
        SENF_PARSER_BITFIELD ( obssNonHTSTAsPresent,       1, bool     );
        SENF_PARSER_SKIP_BITS(                             1           );  // reserved
        SENF_PARSER_BITFIELD ( nonGreenfieldHTSTAsPresent, 1, bool     );
        SENF_PARSER_BITFIELD ( htProtection,               2, unsigned );
        SENF_PARSER_SKIP_BITS(                             8           );  // reserved
        SENF_PARSER_BITFIELD ( dualCTSProtection,          1, bool     );
        SENF_PARSER_BITFIELD ( dualBeacon,                 1, bool     );
        SENF_PARSER_SKIP_BITS(                             6           );  // reserved
        SENF_PARSER_SKIP_BITS(                             4           );  // reserved
        SENF_PARSER_BITFIELD ( pcoPhase,                   1, bool     );
        SENF_PARSER_BITFIELD ( pcoActive,                  1, bool     );
        SENF_PARSER_BITFIELD ( lsigTXOPProtectionSupport,  1, bool     );
        SENF_PARSER_BITFIELD ( stbcBeacon,                 1, bool     );
        SENF_PARSER_FINALIZE ( WLANHTOperationInfoFieldParser          );
    };

    struct WLANHTOperationInfoElementParser
        : public WLANInfoElementParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT ( WLANInfoElementParser                          );
        SENF_PARSER_FIELD   ( primaryChannel, UInt8Parser                    );
        SENF_PARSER_FIELD   ( operationInfo,  WLANHTOperationInfoFieldParser );
        SENF_PARSER_FIELD   ( basicMCSSet,    WLANSupportedMCSSetParser      );
        SENF_PARSER_FINALIZE( WLANHTOperationInfoElementParser               );

        SENF_PARSER_INIT() {
            type() = typeId;
            length() = 22u;
        }
        static const type_t::value_type typeId = 0x3du;

        void dump(std::ostream & os) const;
    };


    struct WLANMeshIdInfoElementParser
        : public WLANInfoElementParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT ( WLANInfoElementParser            );
        // the StringParser includes the length field so we have to go back
        SENF_PARSER_GOTO    ( length                           );
        SENF_PARSER_FIELD   ( value, StringParser<UInt8Parser> );
        SENF_PARSER_FINALIZE( WLANMeshIdInfoElementParser      );

        SENF_PARSER_INIT() {
            type() = typeId;
            length() = 0u;
        }
        static const type_t::value_type typeId = 0x72u;

        void dump(std::ostream & os) const;
    };


    struct WLANVHTCapabilitiesInfoFieldParser
        : public PacketParserBase
    {
    public:
#       include SENF_FIXED_PARSER()
        // re-ordering of the fields due to the byte order
        SENF_PARSER_BITFIELD ( txSTBC,                           1, bool     );
        SENF_PARSER_BITFIELD ( shortGIfor160_8080MHz,            1, bool     );
        SENF_PARSER_BITFIELD ( shortGIfor80MHz,                  1, bool     );
        SENF_PARSER_BITFIELD ( rxLDPC,                           1, bool     );
        SENF_PARSER_BITFIELD ( supportedChannelWidthSet,         2, unsigned );
        SENF_PARSER_BITFIELD ( maxMPDULength,                    2, unsigned );

        SENF_PARSER_BITFIELD ( compressedSteeringNrOfBFAntennas, 3, unsigned );
        SENF_PARSER_BITFIELD ( suBeamformee,                     1, bool     );
        SENF_PARSER_BITFIELD ( suBeamformer,                     1, bool     );
        SENF_PARSER_BITFIELD ( rxSTBC,                           3, unsigned );

        SENF_PARSER_PRIVATE_BITFIELD( maxAMPDULengthExponent_1,  1, unsigned );
        SENF_PARSER_BITFIELD ( htc_vhtCapable,                   1, bool     );
        SENF_PARSER_BITFIELD ( vhtTxOpPowerSave,                 1, bool     );
        SENF_PARSER_BITFIELD ( muBeamformee,                     1, bool     );
        SENF_PARSER_BITFIELD ( muBeamformer,                     1, bool     );
        SENF_PARSER_BITFIELD ( nrOfSoundingDimensions,           3, unsigned );

        SENF_PARSER_SKIP_BITS(                                   2           );  // reserved
        SENF_PARSER_BITFIELD ( txAntennaPatternConsistency,      1, bool     );
        SENF_PARSER_BITFIELD ( rxAntennaPatternConsistency,      1, bool     );
        SENF_PARSER_BITFIELD ( vhtLinkAdaptationCapable,         2, unsigned );
        SENF_PARSER_PRIVATE_BITFIELD( maxAMPDULengthExponent_2,  2, unsigned );

        SENF_PARSER_FINALIZE ( WLANVHTCapabilitiesInfoFieldParser            );

        boost::uint32_t maxAMPDULengthExponent() const {
        	return (maxAMPDULengthExponent_1().value() << 2) | maxAMPDULengthExponent_2().value();
        }
        void maxAMPDULengthExponent(boost::uint32_t v) const {
         	maxAMPDULengthExponent_1((v & 0x00000004) >> 2) ;
         	maxAMPDULengthExponent_2(v & 0x00000003);
         }
     };

    struct WLANVHTMCSMapParser
        : public PacketParserBase
    {
    public:
#       include SENF_FIXED_PARSER()
        // re-ordering of the fields due to the byte order
        SENF_PARSER_BITFIELD( maxMCS4SS, 2, unsigned );
        SENF_PARSER_BITFIELD( maxMCS3SS, 2, unsigned );
        SENF_PARSER_BITFIELD( maxMCS2SS, 2, unsigned );
        SENF_PARSER_BITFIELD( maxMCS1SS, 2, unsigned );
        SENF_PARSER_BITFIELD( maxMCS8SS, 2, unsigned );
        SENF_PARSER_BITFIELD( maxMCS7SS, 2, unsigned );
        SENF_PARSER_BITFIELD( maxMCS6SS, 2, unsigned );
        SENF_PARSER_BITFIELD( maxMCS5SS, 2, unsigned );
        SENF_PARSER_FINALIZE( WLANVHTMCSMapParser     );
    };

    struct WLANVHTHighestSupportedDataRateParser
        : public PacketParserBase
    {
    public:
#       include SENF_FIXED_PARSER()
        SENF_PARSER_PRIVATE_BITFIELD ( value_1, 8, unsigned );
        SENF_PARSER_SKIP_BITS(                  3           );
        SENF_PARSER_PRIVATE_BITFIELD ( value_2, 5, unsigned );
        SENF_PARSER_FINALIZE( WLANVHTHighestSupportedDataRateParser );

        boost::uint32_t value() const {
        	return (value_1().value() << 5) | value_2().value();
        }
        void value(boost::uint32_t v) const {
        	value_1((v & 0x00000004) >> 5) ;
        	value_2(v & 0x0000001f);
         }
    };

    struct WLANSupportedVHTMCSSetParser
        : public PacketParserBase
    {
    public:
#       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD   ( rxVHTMCSMap,                WLANVHTMCSMapParser                   );
        SENF_PARSER_FIELD   ( rxHighestSupportedDataRate, WLANVHTHighestSupportedDataRateParser );
        SENF_PARSER_FIELD   ( txVHTMCSMap,                WLANVHTMCSMapParser                   );
        SENF_PARSER_FIELD   ( txHighestSupportedDataRate, WLANVHTHighestSupportedDataRateParser );
        SENF_PARSER_FINALIZE( WLANSupportedVHTMCSSetParser                                      );
    };

    struct WLANVHTCapabilitiesInfoElementParser
        : public WLANInfoElementParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT ( WLANInfoElementParser                                );
        SENF_PARSER_FIELD   ( capabilitiesInfo, WLANVHTCapabilitiesInfoFieldParser );
        SENF_PARSER_FIELD   ( supportedMCSSet,  WLANSupportedVHTMCSSetParser       );
        SENF_PARSER_FINALIZE( WLANVHTCapabilitiesInfoElementParser                 );

        SENF_PARSER_INIT() {
            type() = typeId;
            length() = 12u;
        }
        static const type_t::value_type typeId = 0xbf;

        void dump(std::ostream & os) const;
    };


    struct WLANVHTOperationInfoFieldParser
		: public PacketParserBase
    {
    #   include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD   ( channelWidth,             UInt8Parser );
        SENF_PARSER_FIELD   ( channelCenterFrequency0,  UInt8Parser );
        SENF_PARSER_FIELD   ( channelCenterFrequency1,  UInt8Parser );
        SENF_PARSER_FINALIZE( WLANVHTOperationInfoFieldParser       );
    };

    struct WLANVHTOperationInfoElementParser
        : public WLANInfoElementParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT ( WLANInfoElementParser                           );
        SENF_PARSER_FIELD   ( operationInfo,  WLANVHTOperationInfoFieldParser );
        SENF_PARSER_FIELD   ( basicMCSSet,    WLANVHTMCSMapParser             );
        SENF_PARSER_FINALIZE( WLANVHTOperationInfoElementParser               );

        SENF_PARSER_INIT() {
            type() = typeId;
            length() = 5u;
        }
        static const type_t::value_type typeId = 0xc0;

        void dump(std::ostream & os) const;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "InformationElements.cci"
//#include "InformationElements.ct"
//#include "InformationElements.cti"
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
