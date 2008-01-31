// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS) 
// Competence Center NETwork research (NET), St. Augustin, GERMANY 
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief DVBFrontendHandle public header */

#ifndef HH_DVBFrontendHandle_
#define HH_DVBFrontendHandle_ 1

// Custom includes
#include <boost/cstdint.hpp>
#include <linux/dvb/frontend.h> 
#include "../../../Socket/FramingPolicy.hh"
#include "../../../Socket/CommunicationPolicy.hh"
#include "../../../Socket/ReadWritePolicy.hh"
#include "../../../Socket/ProtocolClientSocketHandle.hh"
#include "../../../Socket/SocketProtocol.hh"

//#include "DVBFrontendHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    /// @{

    typedef MakeSocketPolicy<
    	NoAddressingPolicy,
        DatagramFramingPolicy,
        UnconnectedCommunicationPolicy,
        NotReadablePolicy,
        NotWriteablePolicy
        >::policy DVBFrontend_Policy;   ///< Socket Policy for xxxx

    /** \brief xxx
     */
    class DVBFrontendProtocol
        : public ConcreteSocketProtocol<DVBFrontend_Policy, DVBFrontendProtocol>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        ///@{

        void init_client(boost::uint8_t adapter=0, boost::uint8_t device=0) const;       ///< xxx
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        ///@}
        ///\name Abstract Interface Implementation
        ///@{
        
        unsigned available() const;
        bool eof() const;

        ///@}
        
        void signalStrength(int16_t *strength) const;
    };

    typedef ProtocolClientSocketHandle<DVBFrontendProtocol> DVBFrontendHandle;

    ///@}
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "DVBFrontendHandle.cci"
//#include "DVBFrontendHandle.ct"
//#include "DVBFrontendHandle.cti"
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
