// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#include "RTPPacket.hh"
//#include "RTPPacket.ih"

// Custom includes

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////


prefix_ void senf::RTPPacket::v_nextInterpreter()
    const
{

    if(extension()){
        Packet::registerInterpreter<RTPUnknownExtensionPacket>(begin()+bytes(),end());
    }else{
	
	int paddingOctets = 0;
        if(padding()){
	    paddingOctets = paddingOctet();
	}
        registerInterpreter(payloadType(),begin()+bytes(),end()-paddingOctets);
    }
}

prefix_ void senf::RTPPacket::v_finalize()
{}

prefix_ void senf::RTPPacket::v_dump(std::ostream & os)
    const
{
    os << "RTP:\n"
       << "  version       : " << version() << "\n"
       << "  padding       : " << padding() << "\n"
       << "  extension     : " << extension() << "\n"
       << "  csrc count    : " << csrcCount() << "\n"
       << "  marker        : " << marker() << "\n"
       << "  payload type  : " << payloadType() << "\n"
       << "  sequence nr   : " << seqNumber() << "\n"
       << "  timestamp     : " << timestamp() << "\n"
       << "  ssrc          : " << ssrc() << "\n"
       << "  csrc list     : <not shown>\n";
}

prefix_ void senf::RTPExtensionBasePacket::v_nextInterpreter()
    const
{

    // We don't want to inherit Parse_RTPExtensionBase to avoid
    // virtual inheritance problems. Since we need the size of the
    // extension, we just allocate ourselves a ExtensionBase parser

    Parse_RTPExtensionBase<iterator> p (begin());
    if (!p.check(begin(),end()))
        throw TruncatedPacketException();

    int paddingOctets = 0;
    if(get_prev<RTPPacket>()->padding()){
	paddingOctets = get_prev<RTPPacket>()->paddingOctet();
    }
    registerInterpreter(get_prev<RTPPacket>()->payloadType(),begin()+p.bytes(),end()-paddingOctets);
}

prefix_ void senf::RTPExtensionBasePacket::v_dump(std::ostream & os)
    const
{
    os << "RTP extension packet:\n"
       << "  content not shown\n";
}

prefix_ void senf::RTPUnknownExtensionPacket::v_finalize()
{}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
