#ifndef DVBSOCKETCONTROLLER_HH_
#define DVBSOCKETCONTROLLER_HH_

#include "DVBFrontendHandle.hh"
#include "DVBDemuxHandles.hh"
// Custom includes
#include <string>
#include <linux/dvb/frontend.h>
#include <senf/Scheduler/Scheduler.hh>
#include "DVBConfigParser.hh"
#include <senf/Console.hh>
namespace senf {
    std::string status2String(fe_status_t status);
class DVBSocketController : boost::noncopyable
{
public:
    senf::console::ScopedDirectory<DVBSocketController> dir;
    
    typedef boost::function<void (const struct dvb_frontend_event & )> Callback;
        
    DVBSocketController(DVBFrontendHandle frontendHandle_ = DVBFrontendHandle(0,0), DVBDemuxSectionHandle sectionHandle_ = DVBDemuxSectionHandle(0,0), const Callback & cb = NULL);
	~DVBSocketController();
	
	std::string tuneToCMD( const std::string & channel, const std::string & mode = "async");
	
	void tuneTo(const std::string & channel);
	
	void tuneDVB_S(unsigned int frequency, fe_spectral_inversion_t inversion, unsigned int symbole_rate, fe_code_rate_t code_rate);
	                                                                                ///< Tunes a DVB-S device
	                                                                                /**< Tunes a DVB-S device. Needs full configuration */
	void tuneDVB_T(unsigned int frequency,
	                fe_spectral_inversion_t inversion, fe_bandwidth_t bandwidth,
	                fe_code_rate_t code_rate_HP, /* high priority stream code rate */
	                fe_code_rate_t code_rate_LP, /* low priority stream code rate */
	                fe_modulation_t constellation, /* modulation type (see above) */
	                fe_transmit_mode_t transmission_mode,
	                fe_guard_interval_t guard_interval,
	                fe_hierarchy_t hierarchy_information);                                                      ///< Tunes a DVB-T device
	                                                                                /**< Tunes a DVB-T device. Needs full configuration */
	void tuneDVB_C(unsigned int frequency,
	                fe_spectral_inversion_t inversion, unsigned int symbol_rate,
	                fe_code_rate_t fec_inner, fe_modulation_t modulation); 
	
	dvb_frontend_event tuneTo_sync( const std::string & channel );
	
	dvb_frontend_event tuneDVB_S_sync(unsigned int frequency, fe_spectral_inversion_t inversion, unsigned int symbole_rate, fe_code_rate_t code_rate);
                                                                                ///< Tunes a DVB-S device
                                                                                /**< Tunes a DVB-S device. Needs full configuration */
    dvb_frontend_event tuneDVB_T_sync(unsigned int frequency,
                fe_spectral_inversion_t inversion, fe_bandwidth_t bandwidth,
                fe_code_rate_t code_rate_HP, /* high priority stream code rate */
                fe_code_rate_t code_rate_LP, /* low priority stream code rate */
                fe_modulation_t constellation, /* modulation type (see above) */
                fe_transmit_mode_t transmission_mode,
                fe_guard_interval_t guard_interval,
                fe_hierarchy_t hierarsourcechy_information);                                                      ///< Tunes a DVB-T device
                                                                                /**< Tunes a DVB-T device. Needs full configuration */
    dvb_frontend_event tuneDVB_C_sync(unsigned int frequency,
                fe_spectral_inversion_t inversion, unsigned int symbol_rate,
                fe_code_rate_t fec_inner, fe_modulation_t modulation); 
    
    fe_type_t getType();
    
    std::string getTypeString();
    
    std::string status2String(fe_status_t status);
    unsigned int bitErrorRate();
    unsigned int signalToNoiseRatio();
    unsigned int signalStrength();
    
    void setSectionFilter(unsigned short int pid, 
               unsigned char filter = 62,
               unsigned int flags = DMX_IMMEDIATE_START | DMX_CHECK_CRC,
               unsigned char mask = 0xff, 
               unsigned char mode = 0,
               unsigned int timeout = 0);
    
    void setBufferSize(unsigned long size); 
                            ///< set the size of the circular buffer used for filtered data.
                            /**< The default size is two maximum sized sections, i.e. if this
                                 function is not called a buffer size of 2 * 4096 bytes will
                                 be used.
                                 \param[in] size Size of circular buffer. */
    
    void startFiltering();
    
    void stopFiltering();
    
    std::string getTuneInfo(const std::string & conf ="Ssbuf"); ///< Returns a string which shows actual tuning status
                                                                /**< Returns a string which shows actual tuning status. 
                                                                "S" prints signal strength (in hex)
                                                                "s" prints singal to noise ration (in hex)
                                                                "b" prints bit error rate (in hex)
                                                                "u" prints uncorrected blocks (in hex) 
                                                                "f" prints readable overal status e.g. "Has Lock"
                                                                These characters can be used to form the output. Be aware, some 
                                                                features may not be supported be your current driver implementation and 
                                                                could end in throwing an exception!*/
private:
    DVBFrontendHandle frontendHandle;
    senf::DVBDemuxSectionHandle sectionHandle;
    fe_type_t type;
    DVBConfigParser parser;
    Callback cb;
    senf::scheduler::FdEvent event;
    
    void readEvent(int i);
    void initConsole();
};

}
#endif /*DVBSOCKETCONTROLLER_HH_*/
