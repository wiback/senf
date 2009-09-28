// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
//     Anton Gillert <atx@berlios.de>
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

#ifndef HH_SENF_Socket_Protocols_DVB_DVBFrontendHandle_
#define HH_SENF_Socket_Protocols_DVB_DVBFrontendHandle_ 1

// Custom includes
#include <boost/cstdint.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <linux/dvb/frontend.h>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/ProtocolClientSocketHandle.hh>
#include <senf/Socket/SocketProtocol.hh>
#include <fcntl.h>
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
>    ::policy DVBFrontend_Policy; ///< Socket Policy for DVBFrontendSocketProtocol
        

    /** \brief SocketProtocol for the dvb frontend device

        The DVB frontend device controls the tuner and DVB demodulator hardware.
        ATTENTION!
        Some calls are not supported by real life drivers, known issues:
        
        Cinergy T� getParameter is not supported
        Cinergy T� in getEvent fe_status_t will be set but dvb_frontend_parameters will be stay untouched
        Cinergy DT XS bitErrorRate is not supported
        
        This dues to the lack of driver implementation. There could be restrictions also for other DVB devices!
     */
    
    class DVBFrontendSocketProtocol
        : public ConcreteSocketProtocol<DVBFrontend_Policy, DVBFrontendSocketProtocol>
    {
    private: 
        void tune(const struct dvb_frontend_parameters & frontend) const;
    public:
        
        
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        ///@{
        void init_client(unsigned short adapter = 0, unsigned short device = 0, int flags = (O_RDWR | O_NONBLOCK) ) const;
                                        ///< Opens the specified frontend device in read-only mode.
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        ///@}

        void setNonBlock(bool on = true) const;
       
        void tuneDVB_S(unsigned int frequency, fe_spectral_inversion_t inversion, unsigned int symbole_rate, fe_code_rate_t code_rate) const;
                                                                        ///< Tunes a DVB-S device
                                                                        /**< Tunes a DVB-S device. Needs full configuration */
        void tuneDVB_T(unsigned int frequency, 
                fe_spectral_inversion_t inversion,
                fe_bandwidth_t bandwidth, 
                fe_code_rate_t code_rate_HP, /* high priority stream code rate */
                fe_code_rate_t code_rate_LP, /* low priority stream code rate */
                fe_modulation_t constellation, /* modulation type (see above) */
                fe_transmit_mode_t transmission_mode, 
                fe_guard_interval_t guard_interval,
                fe_hierarchy_t hierarchy_information
                ) const;                                                ///< Tunes a DVB-T device
                                                                        /**< Tunes a DVB-T device. Needs full configuration */
        void tuneDVB_C(unsigned int frequency, 
                fe_spectral_inversion_t inversion,
                unsigned int symbol_rate,
                fe_code_rate_t fec_inner,
                fe_modulation_t modulation
                ) const;        
                                                                        ///< Tunes a DVB-C device
                                                                        /**< Tunes a DVB-C device. Needs full configuration */
        dvb_frontend_info getInfo() const;                              ///< Returns information struct.
                                                                        /**< Returns information struct, which contains information 
                                                                             about the device which is associated with the current frontend.*/
        struct dvb_frontend_parameters getFrontendParam() const;        ///< Returns dvb_frontend_parameters struct.
                                                                        /**< Returns dvb_frontend_parameters struct, which contains the actual 
                                                                             configuration of the device.*/
        ///\name Abstract Interface Implementation
        ///@{
        
        dvb_frontend_event getEvent() const;
        

        unsigned available() const;     ///< Returns always <tt>0</tt>
                                        /**< Returns always <tt>0</tt>, since the DVB frontend
                                             socket is not readable. */
        bool eof() const;               ///< Returns always <tt>false</tt>
                                        /**< Returns always <tt>false</tt>, since the DVB frontend
                                             socket does not support the notion of EOF. */

        ///@}

        int16_t signalStrength() const; ///< Returns current signal strength
                                        /**< Returns the signal strength value for the signal
                                             currently received by the front-end. For this method,
                                             read-only access to the device is sufficient.*/

        int16_t signalNoiseRatio() const;
                                        ///< Returns current signal-to-noise ratio
                                        /**< Returns the signal-to-noise ratio for the signal
                                             currently received by the front-end. For this method,
                                             read-only access to the device is sufficient. */

        uint32_t bitErrorRate() const;  ///< Returns the current bit error rate for the signal
                                        /**< Returns the bit error rate for the signal currently
                                             received/demodulated by the front-end. For this method,
                                             read-only access to the device is sufficient. */
        uint32_t uncorrectedBlocks() const; ///< Returns the number of uncorrected blocks 
                                            /**< Returns the number of uncorrected blocks 
                                             * detected by the device driver during its lifetime.
                                             *  For meaningful measurements, the increment in block 
                                             * count during a specific time interval should be calculated. 
                                             * For this command, read-only access to the device is sufficient.
                                             *  Note that the counter will wrap to zero after its maximum count 
                                             * has been reached.*/
        
        fe_status_t status() const;         ///< This ioctl call returns status information about the front-end.
                                            /**< This ioctl call returns status information about the 
                                             * front-end. This call only requires read-only access 
                                             * to the device.*/
               
    };

    typedef ProtocolClientSocketHandle<DVBFrontendSocketProtocol> DVBFrontendHandle;

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
