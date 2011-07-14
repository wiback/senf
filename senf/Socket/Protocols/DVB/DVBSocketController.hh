// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Anton Gillert <atx@berlios.de>


#ifndef HH_SENF_Socket_Protocols_DVB_DVBSocketController_
#define HH_SENF_Socket_Protocols_DVB_DVBSocketController_ 1

// Custom includes
#include <string>
#include <linux/dvb/frontend.h>
#include "DVBFrontendHandle.hh"
#include "DVBDemuxHandles.hh"
#include "DVBConfigParser.hh"
#include <senf/Scheduler/FdEvent.hh>
#include <senf/Utils/Console/ScopedDirectory.hh>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {

#define MPE_TABLEID 62

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

    console::ScopedDirectory<DVBSocketController> dir;

    typedef boost::function<void (const struct dvb_frontend_event & )> Callback;
                                        ///< Callback which is called when an asynchronous tuning succeeds.

    DVBSocketController(DVBFrontendHandle frontendHandle_ = DVBFrontendHandle(0,0), const Callback & cb = NULL);
    ~DVBSocketController();

    DVBDemuxSectionHandle createDVBDemuxSectionHandle(  int adapternumber=0, int demuxnumber=0, bool addToConsole=false );
    DVBDemuxPESHandle createDVBDemuxPESHandle(  int adapternumber=0, int demuxnumber=0, bool addToConsole=false );

    void addToConsole(DVBDemuxSectionHandle sh);
                                        ///< Adds an DVBDemuxSectionHandle to the console
                                        /**< Allocates the functionality of DVBDemuxSectionProtocol
                                             into the folder of the DVBSocketController. If the
                                             protocol is closed, or all handles are discarded the
                                             console support will automatically removed.
                                             \param[in] sh handle of a protocol*/

    void addToConsole(DVBDemuxPESHandle sh);
                                        ///< Adds an DVBDemuxPESHandle to the console
                                        /**< Allocates the functionality of DVBDemuxPESProtocol into
                                             the folder of the DVBSocketController. If the protocol
                                             is closed, or all handles are discarded the console
                                             support will automatically removed.
                                             \param[in] sh handle of a protocol*/

    void tuneToCMD(std::string const & input, std::string const & mode = "async");
                                        ///< Tunes a DVB device given by the type of the DVBFrontendHandle
                                        /**< Tunes a DVB device by a channel name or complete
                                             configuration line. This method was created for use
                                             in the SENF-Console.
                                             \param[in] input A channel name or a complete
                                               configuration line. If a channel name is given
                                               it would be searched in the config file.
                                             \param[in] mode The mode in which it will tune
                                               "sync" or "async"*/
    void tuneTo(std::string const & channel);
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

    dvb_frontend_event tuneTo_sync(std::string const & channel );
                                        ///< Tunes a DVB device given by the type of the DVBFrontendHandle
                                        /**< Tunes a DVB device, given by the type of the
                                             DVBFrontendHandle, by a channel name in synchronous mode
                                             \param[in] channel A channel name which will be looked
                                                 up in config file.
                                             \returns dvb_frontend_event
                                             \note The member "dvb_frontend_event.status" should be
                                               correct by the most device driver implementations.
                                               But "dvb_frontend_event.parameters" maybe not and is
                                               definitely not set by: Cinergy T� (2.6.27),
                                               Terratec Cinergy DT USB XS Diversity (2.6.27) */

    dvb_frontend_event tuneDVB_S_sync(unsigned int frequency, fe_spectral_inversion_t inversion, unsigned int symbole_rate, fe_code_rate_t code_rate);
                                        ///< Tunes a DVB-S device
                                        /**< Tunes a DVB-S device in synchronous mode. Needs full configuration
                                             \returns dvb_frontend_event
                                             \note The member "dvb_frontend_event.status" should be
                                               correct by the most device driver implementations.
                                               But "dvb_frontend_event.parameters" maybe not and is
                                               definitely not set by: Cinergy T� (2.6.27),
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
                                               definitely not set by: Cinergy T� (2.6.27),
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
                                               definitely not set by: Cinergy T� (2.6.27),
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
                                             <br>"s" prints signal to noise ration (in hex)
                                             <br>"b" prints bit error rate (in hex)
                                             <br>"u" prints uncorrected blocks (in hex)
                                             <br>"f" prints readable overall status e.g. "Has Lock"<br>
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
    scheduler::FdEvent event;
    void readEvent(int i);
    void initConsole();
};

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "DVBSocketController.cci"
//#include "DVBSocketController.ct"
//#include "DVBSocketController.cti"
#endif
