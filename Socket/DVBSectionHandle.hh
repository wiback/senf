// $Id: UDPSocketHandle.hh 296 2007-07-10 20:39:34Z g0dil $
//
// Copyright (C) 2007
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

/** \file
    \brief DVBSectionHandle
 */

#ifndef HH_DVBSectionHandle_
#define HH_DVBSectionHandle_ 1

// Custom includes
#include "BSDSocketProtocol.hh"
#include "FramingPolicy.hh"
#include "CommunicationPolicy.hh"
#include "ReadWritePolicy.hh"
#include "BufferingPolicy.hh"
#include "ProtocolClientSocketHandle.hh"
#include "DVBProtocol.hh"

//#include "DVBSocketHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    /// @{

    typedef MakeSocketPolicy<
    	NoAddressingPolicy,
        DatagramFramingPolicy,
        UnconnectedCommunicationPolicy,
        ReadablePolicy,
        NotWriteablePolicy,
        SocketBufferingPolicy
        >::policy DVBSection_Policy;   ///< Socket Policy for DVBSection

    /** \brief xxx
     */
    class DVBSectionProtocol
        : public ConcreteSocketProtocol<DVBSection_Policy>,
          public DVBProtocol
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        ///@{

        void init_client() const;       ///< xxx
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        ///@}
        ///\name Abstract Interface Implementation
        
        unsigned available() const;

        std::auto_ptr<SocketProtocol> clone() const;

        ///@}
        
        void setSectionFilter(unsigned short pid, unsigned char table_id) const;
    
    private:
        void setSectionFilter(struct dmx_sct_filter_params *filter) const;
        
    };

    typedef ProtocolClientSocketHandle<DVBSectionProtocol> DVBSectionHandle;


}

///////////////////////////////hh.e////////////////////////////////////////
//#include "DVBSectionHandle.cci"
//#include "DVBSectionHandle.ct"
//#include "DVBSectionHandle.cti"
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
