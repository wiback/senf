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
    \brief DVBDemuxHandles public header */

#ifndef HH_SENF_Socket_Protocols_DVB_DVBDemuxHandles_
#define HH_SENF_Socket_Protocols_DVB_DVBDemuxHandles_ 1

// Custom includes
#include "../../../Socket/FramingPolicy.hh"
#include "../../../Socket/CommunicationPolicy.hh"
#include "../../../Socket/ReadWritePolicy.hh"
#include "../../../Socket/ProtocolClientSocketHandle.hh"
#include "DVBDemuxSocketProtocol.hh"

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
    class DVBDemuxSectionSocketProtocol
        : public ConcreteSocketProtocol<DVBDemux_Policy, DVBDemuxSectionSocketProtocol>,
          public DVBDemuxSocketProtocol
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        ///@{

        void init_client(unsigned short adapter=0, unsigned short device=0) const;       ///< xxx
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        ///@}
        ///\name Abstract Interface Implementation
        ///@{
        
        unsigned available() const;

        ///@}
        
        void setSectionFilter(unsigned short int pid, 
                unsigned int timeout, 
                unsigned int flags, 
                unsigned char filter, 
                unsigned char mask, 
                unsigned char mode) const;    
    };

    typedef ProtocolClientSocketHandle<DVBDemuxSectionSocketProtocol> DVBDemuxSectionHandle;
    
    // ----------------------------------------------------------------
    
    /** \brief xxx
     */
    class DVBDemuxPESSocketProtocol
        : public ConcreteSocketProtocol<DVBDemux_Policy,DVBDemuxPESSocketProtocol>,
          public DVBDemuxSocketProtocol
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        ///@{

        void init_client(unsigned short adapter=0, unsigned short device=0) const;       ///< xxx
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        ///@}
        ///\name Abstract Interface Implementation
        ///@{
        
        unsigned available() const;

        ///@}
        
        void setPESFilter(struct dmx_pes_filter_params *filter) const;    
    };

    typedef ProtocolClientSocketHandle<DVBDemuxPESSocketProtocol> DVBDemuxPESHandle;

    
    // ----------------------------------------------------------------
    
    
    /** \brief xxx
         */
    class DVBDvrSocketProtocol
        : public ConcreteSocketProtocol<DVBDemux_Policy, DVBDvrSocketProtocol>,
          public DVBDemuxSocketProtocol
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        ///@{

        void init_client(unsigned short adapter=0, unsigned short device=0) const;       ///< xxx
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        ///@}
        ///\name Abstract Interface Implementation
        ///@{
        
        unsigned available() const;

        ///@}
     };

     typedef ProtocolClientSocketHandle<DVBDvrSocketProtocol> DVBDvrHandle;

    ///@}
    
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
