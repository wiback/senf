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

#ifndef HH_SENF_Socket_Protocols_DVB_DVBFrontendHandle_
#define HH_SENF_Socket_Protocols_DVB_DVBFrontendHandle_ 1

// Custom includes
#include <boost/cstdint.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <linux/dvb/frontend.h>
#include "../../../Socket/FramingPolicy.hh"
#include "../../../Socket/CommunicationPolicy.hh"
#include "../../../Socket/ReadWritePolicy.hh"
#include "../../../Socket/ProtocolClientSocketHandle.hh"
#include "../../../Socket/SocketProtocol.hh"
#include <string>
#include <fstream>



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
     */
    
    class DVBFrontendSocketProtocol
        : public ConcreteSocketProtocol<DVBFrontend_Policy, DVBFrontendSocketProtocol>
    {
    private: 
        dvb_frontend_event tune(const struct dvb_frontend_parameters & frontend);
        struct dvb_frontend_event waitTune(const struct dvb_frontend_parameters & frontend);
        struct dvb_frontend_event asyncTune(const struct dvb_frontend_parameters & frontend);
        boost::function<struct dvb_frontend_event (const struct dvb_frontend_parameters & frontend)> cb2_X_Tune;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        ///@{
        void init_client(unsigned const short adapter = 0, unsigned const short device = 0);
                                        ///< Opens the specified frontend device in read-only mode.
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        ///@}
        
        void tuneDVB_S(unsigned int frequency, fe_spectral_inversion_t inversion, unsigned int symbole_rate, fe_code_rate_t code_rate);
                                                                        ///< Tunes a DVB-C device
                                                                        /**< Tunes a DVB-C device. Needs full configuration */
        void tuneDVB_T(unsigned int frequency, 
                fe_spectral_inversion_t inversion,
                fe_bandwidth_t bandwidth, 
                fe_code_rate_t code_rate_HP, /* high priority stream code rate */
                fe_code_rate_t code_rate_LP, /* low priority stream code rate */
                fe_modulation_t constellation, /* modulation type (see above) */
                fe_transmit_mode_t transmission_mode, 
                fe_guard_interval_t guard_interval,
                fe_hierarchy_t hierarchy_information
                );                                                      ///< Tunes a DVB-T device
                                                                        /**< Tunes a DVB-T device. Needs full configuration */
        void tuneDVB_C(unsigned int frequency, 
                fe_spectral_inversion_t inversion,
                unsigned int symbol_rate,
                fe_code_rate_t fec_inner,
                fe_modulation_t modulation);        
                                                                        ///< Tunes a DVB-C device
                                                                        /**< Tunes a DVB-C device. Needs full configuration */
        struct dvb_frontend_info getInfo() const;                     ///< Returns information struct.
                                                                        /**< Returns information struct, which contains information 
                                                                             about the device which is associated with the current frontend.*/
        struct dvb_frontend_parameters getFrontendParam() const;        ///< Returns dvb_frontend_parameters struct.
                                                                        /**< Returns dvb_frontend_parameters struct, which contains the actual 
                                                                             configuration of the device.*/
        void setFrequency(unsigned int frequency);                      ///< Sets frequency
                                                                        /**< Sets frequency. This can be done for all device types.*/
        void setInversion(fe_spectral_inversion_t inversion);           ///< Sets inversion
                                                                        /**< Sets inversion. This can be done for all device types.*/
        void setCodeRate(fe_code_rate_t fec_inner);                     ///< Sets code rate
                                                                        /**< Sets code rate. This can be done for all device types. Attention 
                                                                             for DVB-T devices the high and low priority stream code rate will be set to 
                                                                             the given value.*/
        void setSymbolRate(unsigned int symbol_rate);                   ///< Sets symbol rate
                                                                        /**< Sets symbol rate. This can only be done for DVB-S or DVB-C devices.
                                                                             Other attempts will throw an exception.*/
        void setModulation(fe_modulation_t modulation);                 ///< Sets modulation
                                                                        /**< Sets modulation. This can only be done for DVB-T or DVB-C devices.
                                                                             Other attempts will throw an exception.*/
        void setBandwidth(fe_bandwidth_t bandwidth);                    ///< Sets bandwidth
                                                                        /**< Sets bandwidth. This can only be done for DVB-T devices.
                                                                             Other attempts will throw an exception.*/
        void setHighPriorityCodeRate(fe_code_rate_t code_rate_HP);      ///< Sets high priority stream code rate
                                                                        /**< Sets high priority stream code rate. This can only be done for DVB-T devices.
                                                                             Other attempts will throw an exception.*/
        void setLowPriorityCodeRate(fe_code_rate_t code_rate_LP);       ///< Sets low priority stream code rate
                                                                        /**< Sets low priority stream code rate. This can only be done for DVB-T devices.
                                                                             Other attempts will throw an exception.*/
        void setGuardInterval(fe_guard_interval_t guard_interval);      ///< Sets guard interval
                                                                        /**< Sets guard interval. This can only be done for DVB-T devices.
                                                                             Other attempts will throw an exception.*/
        void setHierarchyInformation(fe_hierarchy_t hierarchy_information);  ///< Sets hierarchy information
                                                                            /**< Sets hierarchy information. This can only be done for DVB-T devices.
                                                                                 Other attempts will throw an exception.*/
        
        
        ///\name Abstract Interface Implementation
        ///@{

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
