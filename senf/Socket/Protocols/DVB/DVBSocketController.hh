// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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


#ifndef HH_SENF_Socket_Protocols_DVB_DVBSocketController_
#define HH_SENF_Socket_Protocols_DVB_DVBSocketController_ 1

// Custom includes
#include <string>
#include <linux/dvb/frontend.h>
#include "DVBFrontendHandle.hh"
#include "DVBDemuxHandles.hh"
#include "DVBConfigParser.hh"
#include "DVBProtocolWrapper.hh"
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/Console/Console.hh>

#define MPE_TABLEID 62

namespace senf {

/** \brief Helperclass for configuration and controlling DVB devices.

     The DVB API provides two methods for tuning. The first method is
     synchronous, which means e.g. "tuneTo_sync" will return when the tuning operation on the card
     succeed, no matter how long it takes. The second (preferred) method is to tune asynchronous.
     The call "tuneTo" will return immediately and (if a callback was set) call the callback when
     the tuning operation succeeds. <br>

     Note that you may have to take draw backs in using this API. This dues to the implementation
     of the DVB device driver which you or your kernel choose. It has shown that the completeness
     of implementations vary from driver to driver. This wrapper provides the full Frontend API which
     is defined by <a href="http://www.linuxtv.org/docs/dvbapi/DVB_Frontend_API.html">Convergence</a>.
     You have to find out which parts of these functionality are implemented by your preferred
     device driver by your own.
*/
    
    
    
class DVBSocketController : boost::noncopyable
{
public:

    senf::console::ScopedDirectory<DVBSocketController> dir;

    typedef boost::function<void (const struct dvb_frontend_event & )> Callback;
                                        ///< Callback which is called when an asynchronous tuning succeeds.

    DVBSocketController(DVBFrontendHandle frontendHandle_ = DVBFrontendHandle(0,0), const Callback & cb = NULL);
    ~DVBSocketController();
    
    senf::DVBDemuxSectionHandle createDVBDemuxSectionHandle(  int adapternumber=0, int demuxnumber=0, bool addToConsole=false );
    senf::DVBDemuxPESHandle createDVBDemuxPESHandle(  int adapternumber=0, int demuxnumber=0, bool addToConsole=false );

    void addToConsole(senf::DVBDemuxSectionHandle sh);
    ///< Adds an DVBDemuxSectionHandle to the console 
    /**< Allocates the functionality of DVBDemuxSectionProtocol into the folder 
         of the DVBSocketController. If the protocol is closed, or all handles are 
         discarded the console support will automatically removed.
         \param[in] handle of a protocol*/
    void addToConsole(senf::DVBDemuxPESHandle sh);
    ///< Adds an DVBDemuxPESHandle to the console 
    /**< Allocates the functionality of DVBDemuxPESProtocol into the folder 
         of the DVBSocketController. If the protocol is closed, or all handles are 
         discarded the console support will automatically removed.
         \param[in] handle of a protocol*/
    
    void tuneToCMD( const std::string & input, const std::string & mode = "async");
                                        ///< Tunes a DVB device given by the type of the DVBFrontendHandle
                                        /**< Tunes a DVB device by a channel name or complete
                                             configuration line. This method was created for use
                                             in the SENF-Console.
                                             \param[in] input A channel name or a complete
                                               configuration line. If a channel name is given
                                               it would be searched in the config file.
                                             \param[in] mode The mode in which it will tune
                                               "sync" or "async"*/
    void tuneTo(const std::string & channel);
                                        ///< Tunes a DVB device to a channel
                                        /**< Tunes a DVB device to a channel whose parameters
                                             are stored in a config file. The method determines
                                             the type of DVB device by it self.
                                             \param[in] channel A channel name which will be
                                                 looked up in config file.*/

    void tuneDVB_S(unsigned int frequency, fe_spectral_inversion_t inversion, unsigned int symbole_rate, fe_code_rate_t code_rate);
                                        ///< Tunes a DVB-S device
                                        /**< Tunes a DVB-S device in asynchronous mode and calls
                                             the callback if existing. Needs full configuration */

    void tuneDVB_T(unsigned int frequency,
                    fe_spectral_inversion_t inversion, fe_bandwidth_t bandwidth,
                    fe_code_rate_t code_rate_HP, /* high priority stream code rate */
                    fe_code_rate_t code_rate_LP, /* low priority stream code rate */
                    fe_modulation_t constellation, /* modulation type (see above) */
                    fe_transmit_mode_t transmission_mode,
                    fe_guard_interval_t guard_interval,
                    fe_hierarchy_t hierarchy_information);
                                        ///< Tunes a DVB-T device
                                        /**< Tunes a DVB-T device in asynchronous mode and calls
                                             the callback if existing. Needs full configuration. */

    void tuneDVB_C(unsigned int frequency,
                    fe_spectral_inversion_t inversion, unsigned int symbol_rate,
                    fe_code_rate_t fec_inner, fe_modulation_t modulation);
                                        ///< Tunes a DVB-C device
                                        /**< Tunes a DVB-C device in asynchronous mode and calls
                                             the callback if existing. Needs full configuration. */

    dvb_frontend_event tuneTo_sync( const std::string & channel );
                                        ///< Tunes a DVB device given by the type of the DVBFrontendHandle
                                        /**< Tunes a DVB device, given by the type of the
                                             DVBFrontendHandle, by a channel name in synchronous mode
                                             \param[in] channel A channel name which will be looked
                                                 up in config file.
                                             \returns dvb_frontend_event
                                             \note The member "dvb_frontend_event.status" should be
                                               correct by the most device driver implementations.
                                               But "dvb_frontend_event.parameters" maybe not and is
                                               definitly not set by: Cinergy T� (2.6.27),
                                               Terratec Cinergy DT USB XS Diversity (2.6.27) */

    dvb_frontend_event tuneDVB_S_sync(unsigned int frequency, fe_spectral_inversion_t inversion, unsigned int symbole_rate, fe_code_rate_t code_rate);
                                        ///< Tunes a DVB-S device
                                        /**< Tunes a DVB-S device in synchronous mode. Needs full configuration
                                             \returns dvb_frontend_event
                                             \note The member "dvb_frontend_event.status" should be
                                               correct by the most device driver implementations.
                                               But "dvb_frontend_event.parameters" maybe not and is
                                               definitly not set by: Cinergy T� (2.6.27),
                                               Terratec Cinergy DT USB XS Diversity (2.6.27) */

    dvb_frontend_event tuneDVB_T_sync(unsigned int frequency,
                fe_spectral_inversion_t inversion, fe_bandwidth_t bandwidth,
                fe_code_rate_t code_rate_HP, /* high priority stream code rate */
                fe_code_rate_t code_rate_LP, /* low priority stream code rate */
                fe_modulation_t constellation, /* modulation type (see above) */
                fe_transmit_mode_t transmission_mode,
                fe_guard_interval_t guard_interval,
                fe_hierarchy_t hierarsourcechy_information);
                                        ///< Tunes a DVB-T device
                                        /**< Tunes a DVB-T device in synchronous mode. Needs full
                                             configuration
                                             \returns dvb_frontend_event
                                             \note The member "dvb_frontend_event.status" should be
                                               correct by the most device driver implementations.
                                               But "dvb_frontend_event.parameters" maybe not and is
                                               definitly not set by: Cinergy T� (2.6.27),
                                               Terratec Cinergy DT USB XS Diversity (2.6.27) */

    dvb_frontend_event tuneDVB_C_sync(unsigned int frequency,
                fe_spectral_inversion_t inversion, unsigned int symbol_rate,
                fe_code_rate_t fec_inner, fe_modulation_t modulation);
                                        ///< Tunes a DVB-C device
                                        /**< Tunes a DVB-C device in synchronous mode. Needs full configuration
                                             \returns dvb_frontend_event
                                             \note The member "dvb_frontend_event.status" should be
                                               correct by the most device driver implementations.
                                               But "dvb_frontend_event.parameters" maybe not and is
                                               definitly not set by: Cinergy T� (2.6.27),
                                               Terratec Cinergy DT USB XS Diversity (2.6.27) */

    fe_type_t getType();                ///< Returns the type of the card. The type is defined in frontend.h

    std::string getTypeString();        ///< Returns the type of the card.
                                        /**< Returns the type of the card but human readable e.g.
                                             "DVB-S", "DVB-T" or "DVB-C"*/

    std::string status2String(fe_status_t status);
                                        ///< Returns a human readable status information
                                        /**< Returns a human readable version of fe_status_t
                                             (defined by frontend.h)*/

    unsigned int bitErrorRate();        ///< Returns the bit error rate
                                        /**< \note This function may not be implemented by your
                                             specific driver implementation. In this case the
                                             output is random. This is true for:
                                               Cinergy T� (2.6.27),
                                               Terratec Cinergy DT USB XS Diversity (2.6.27) */

    unsigned int signalToNoiseRatio();  ///< Returns the signal to noise ratio
                                        /**< \note This function may not be implemented by your
                                             specific driver implementation. In this case the
                                             output is random. This is true for:
                                               Cinergy T� (2.6.27),
                                               Terratec Cinergy DT USB XS Diversity (2.6.27) */

    unsigned int signalStrength();      ///< Returns the signal strength
                                        /**< Returns the signal to noise ratio
                                             \note This function may not be implemented by your
                                             specific driver implementation. In this case the output is
                                             random.*/

    std::string getTuneInfo(const std::string & conf ="Ssbuf");
                                        ///< Returns a string which shows actual tuning status
                                        /**< <br>"S" prints signal strength (in hex)
                                             <br>"s" prints singal to noise ration (in hex)
                                             <br>"b" prints bit error rate (in hex)
                                             <br>"u" prints uncorrected blocks (in hex)
                                             <br>"f" prints readable overal status e.g. "Has Lock"<br>
                                             These characters can be used to form the output. Be
                                             aware, some features may not be supported be your
                                             current driver implementation and could end in throwing
                                             an exception! */
private:
    DVBFrontendHandle frontendHandle;
    fe_type_t type;
    DVBConfigParser parser;
    Callback cb;
    static unsigned int controllerNr;
    unsigned int sectionNr;
    unsigned int pesNr;
    senf::scheduler::FdEvent event;
    void readEvent(int i);
    void initConsole();
};

}
#endif
