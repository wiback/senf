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

// Custom includes
#include <senf/Packets/Packets.hh>
#include "ICMPv6Packet.hh"
#include "ICMPv6TypePacket.hh"

#define prefix_
namespace {
//Implementing the ICMPv6 Type registry
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::ICMPv6ErrDestUnreachable>
        registerICMPv6ErrDestUnreachable (1);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::ICMPv6ErrTooBig>
        registerICMPv6ErrTooBig (2);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::ICMPv6ErrTimeExceeded>
        registerICMPv6ErrTimeExceeded (3);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::ICMPv6ErrParamProblem>
        registerICMPv6ErrParamProblem (4);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::ICMPv6EchoRequest>
        registerICMPv6EchoRequest (128);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::ICMPv6EchoReply>
        registerICMPv6EchoReply (129);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::MLDv2ListenerQuery>
        registerMLDv2ListenerQuery (130);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::MLDv2ListenerReport>
        registerMLDv2ListenerReport (143);
}

prefix_ void senf::ICMPv6EchoRequestType::dump(packet p, std::ostream & os){
    os << "ICMPv6 Echo Request:\n"
        <<"  Identifier              : " << unsigned(p->identifier() )
        <<"\n  SequenceNumber          : " << unsigned(p->seqNr() ) << "\n";
}

prefix_ void senf::ICMPv6EchoReplyType::dump(packet p, std::ostream & os){
    os << "ICMPv6 Echo Reply:\n"
        <<"  Identifier              : " << unsigned(p->identifier() )
        <<"\n  SequenceNumber          : " << unsigned(p->seqNr() ) << "\n";
}

prefix_ void senf::ICMPv6ErrDestUnreachableType::dump(packet p, std::ostream & os){
    os << "ICMPv6 Error Destination Unreachable (no further fields available here)\n";    
}

prefix_ void senf::ICMPv6ErrTooBigType::dump(packet p, std::ostream & os){
    os << "ICMPv6 Error Packet Too Big:\n"
        <<"  MTU                     : " << unsigned(p->mtu() ) << "\n";    
}

prefix_ void senf::ICMPv6ErrTimeExceededType::dump(packet p, std::ostream & os){
    os << "ICMPv6 Error Time Exceeded:\n"
        <<"  Unused(32Bit)               : " << unsigned(p->unused() ) << "\n";    
}

prefix_ void senf::ICMPv6ErrParamProblemType::dump(packet p, std::ostream & os){
    os << "ICMPv6 Error Parameter Problem:\n"
        <<"  Pointer                     : " << unsigned(p->pointer() ) << "\n";    
}

prefix_ void senf::MLDv2ListenerQueryType::dump(packet p, std::ostream & os){
    os << "ICMPv6 Multicast Listener Query:\n"
        <<"  Max. ResponseCode            : " << unsigned(p->maxResponseCode() ) 
        <<"  \nReserved(16Bit)             : " << unsigned(p->reserved() )
        <<"  \nMulticast Address           : " << p->mcAddress()
        <<"  \nReserver(4Bit)              : " << unsigned(p->resv() )
        <<"  \nSuppress Router-Side Processing: " << unsigned(p->sFlag() )
        <<"  \nQuerier's Robustness Variable: " << unsigned(p->qrv() )
        <<"  \nQuerier's Query Interval Code: " << unsigned(p->qqic() )    
        <<"  \nSource Addresses: " << "\n";    
        senf::MLDv2ListenerQuery::Parser::srcAddresses_t::container c (p->srcAddresses() );
        senf::MLDv2ListenerQuery::Parser::srcAddresses_t::container::iterator i (c.begin() );
        for (unsigned int nr =1; i != c.end(); ++i, ++nr)
            os << "    " << nr << ".) " <<*i << "\n";    
        os << "\n";    
}

prefix_ void senf::MLDv2ListenerReportType::dump(packet p, std::ostream & os){
    os << "ICMPv6 Multicast Listener Report Message:\n"
        <<"  Reserved                    : " << unsigned(p->reserved() ) 
        <<"  Multicast Address Records:\n";
    
    senf::MLDv2ListenerReport::Parser::mcastAddrRecords_t::container cAddrR (p->mcastAddrRecords() );
    senf::MLDv2ListenerReport::Parser::mcastAddrRecords_t::container::iterator iAddrR (cAddrR.begin() );
    for (; iAddrR != cAddrR.end(); ++iAddrR){
        os << "    Record Type           : " << unsigned(iAddrR->recordType() )
            <<"    \nMulticast Address     : " << iAddrR->mcAddress() 
            <<"    \nSource Addresses      :\n";
            
        senf::MLDv2AddressRecordParser::srcAddresses_t::container cSrcAddr (iAddrR->srcAddresses() );
        senf::MLDv2AddressRecordParser::srcAddresses_t::container::iterator iSrcAddr ( cSrcAddr.begin() );
        for (;iSrcAddr != cSrcAddr.end();++iSrcAddr)
            os <<"      " << *iSrcAddr << "\n";
        os << "    Auxiliary Data:\n";
        senf::MLDv2AddressRecordParser::auxData_t::container cAuxD ( iAddrR->auxData() );
        senf::MLDv2AddressRecordParser::auxData_t::container::iterator iAuxD (cAuxD.begin() );
        for (;iAuxD != cAuxD.end(); ++iAuxD)
            os <<"      " << *iAuxD << "\n";
    }
}






#undef prefix_