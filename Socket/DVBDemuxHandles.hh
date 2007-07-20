// $Id$
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
    \brief DVBHandles
 */

#ifndef HH_DVBDemuxHandles_
#define HH_DVBDemuxHandles_ 1

// Custom includes
#include "FramingPolicy.hh"
#include "CommunicationPolicy.hh"
#include "ReadWritePolicy.hh"
#include "ProtocolClientSocketHandle.hh"
#include "DVBDemuxProtocol.hh"

//#include "DVBDemuxHandles.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    /// @{

    typedef MakeSocketPolicy<
    	NoAddressingPolicy,
        DatagramFramingPolicy,
        UnconnectedCommunicationPolicy,
        ReadablePolicy,
        NotWriteablePolicy
        >::policy DVBDemux_Policy;   ///< Socket Policy for xxxx

    /** \brief xxx
     */
    class DVBDemuxSectionProtocol
        : public ConcreteSocketProtocol<DVBDemux_Policy>,
          public DVBDemuxProtocol
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
        
        void setSectionFilter(struct dmx_sct_filter_params *filter) const;    
    };

    typedef ProtocolClientSocketHandle<DVBDemuxSectionProtocol> DVBDemuxSectionHandle;
    
    // ----------------------------------------------------------------
    
    /** \brief xxx
     */
    class DVBDemuxPESProtocol
        : public ConcreteSocketProtocol<DVBDemux_Policy>,
          public DVBDemuxProtocol
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
        
        void setPESFilter(struct dmx_pes_filter_params *filter) const;    
    };

    typedef ProtocolClientSocketHandle<DVBDemuxPESProtocol> DVBDemuxPESHandle;

    
    // ----------------------------------------------------------------
    
    
    /** \brief xxx
         */
    class DVBDvrProtocol
        : public ConcreteSocketProtocol<DVBDemux_Policy>,
          public DVBDemuxProtocol
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
   
     };

     typedef ProtocolClientSocketHandle<DVBDvrProtocol> DVBDvrHandle;
    
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "DVBDemuxHandles.cci"
//#include "DVBDemuxHandles.ct"
//#include "DVBDemuxHandles.cti"
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
