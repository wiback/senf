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
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/ProtocolClientSocketHandle.hh>
#include "DVBDemuxSocketProtocol.hh"

//#include "DVBDemuxHandles.mpp"
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
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

    /** \brief Baseclass of various DVBProtocolWrappers which are defined in DVBProtocolWrapper.hh 
     */
    class DVBProtocolWrapper :public boost::enable_shared_from_this<DVBProtocolWrapper> {
        public:
            DVBProtocolWrapper() {}
            virtual ~DVBProtocolWrapper() {}
    };
    /** \brief Baseclass for DVBSocketProtocols which want use Wrappers for console. 
    */
    class DVBSocketProtocol : public virtual SocketProtocol {
private:
        boost::shared_ptr<DVBProtocolWrapper> wrap_;
public:
        DVBSocketProtocol() {}
        ~DVBSocketProtocol() {}
        
        void addWrapper(boost::shared_ptr<DVBProtocolWrapper> wrap)/**< Binds a wrapper to a DVBProtocol, 
                                                                        so if it is closed the functionality of the wrapper is automatically removed from console.*/ 
        {
            wrap_ = wrap;
        }
        virtual void close()
        {
            wrap_.reset();
            SocketProtocol::close();

        }
    };
    
    
    class DVBDemuxSectionSocketProtocol
        : public ConcreteSocketProtocol<DVBDemux_Policy, DVBDemuxSectionSocketProtocol>,
          public DVBDemuxSocketProtocol, 
          public DVBSocketProtocol
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
                unsigned char filter= 0x3e,
                unsigned int flags= DMX_IMMEDIATE_START | DMX_CHECK_CRC, 
                unsigned char mask = 0xff, 
                unsigned char mode =0x00,
                unsigned int timeout =0x00) const;
        
    };

    typedef ProtocolClientSocketHandle<DVBDemuxSectionSocketProtocol> DVBDemuxSectionHandle;
    
    // ----------------------------------------------------------------
    
    /** \brief xxx
     */
    class DVBDemuxPESSocketProtocol
        : public ConcreteSocketProtocol<DVBDemux_Policy,DVBDemuxPESSocketProtocol>,
          public DVBDemuxSocketProtocol,
          public DVBSocketProtocol
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
        
        void setPESFilter(unsigned short int pid, dmx_input_t input, dmx_output_t output, dmx_pes_type_t pesType, unsigned int flags)const;    
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
