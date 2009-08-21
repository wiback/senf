#include "DVBConfigParser.hh"
#include <boost/assign/std/map.hpp>
#include <senf/Utils/Exception.hh>
#include <senf/Utils/Logger.hh>
using namespace std;
#define prefix_

senf::DVBConfigParser::DVBParams const senf::DVBConfigParser::params;

prefix_ senf::DVBConfigParser::DVBParams::DVBParams()
{
    
    boost::assign::insert(inversion) 
    (   "INVERSION_OFF", INVERSION_OFF )
    (   "INVERSION_ON", INVERSION_ON ) 
    (   "INVERSION_AUTO", INVERSION_AUTO );
    boost::assign::insert(bandwidth) 
    (   "BANDWIDTH_6_MHZ", BANDWIDTH_6_MHZ)
    (   "BANDWIDTH_7_MHZ", BANDWIDTH_7_MHZ)
    (   "BANDWIDTH_8_MHZ", BANDWIDTH_8_MHZ);
    boost::assign::insert(code_rate) 
    (   "FEC_1_2", FEC_1_2)
    (   "FEC_2_3", FEC_2_3)
    (   "FEC_3_4", FEC_3_4)
    (   "FEC_4_5", FEC_4_5)
    (   "FEC_5_6", FEC_5_6)
    (   "FEC_6_7", FEC_6_7)
    (   "FEC_7_8", FEC_7_8)
    (   "FEC_8_9", FEC_8_9)
    (   "FEC_AUTO", FEC_AUTO)
    (   "FEC_NONE", FEC_NONE);
    boost::assign::insert(guard_interval) 
    (   "GUARD_INTERVAL_1_16", GUARD_INTERVAL_1_16)
    (   "GUARD_INTERVAL_1_32", GUARD_INTERVAL_1_32)
    (   "GUARD_INTERVAL_1_4", GUARD_INTERVAL_1_4)
    (   "GUARD_INTERVAL_1_8", GUARD_INTERVAL_1_8);
    boost::assign::insert(hierarchy) 
    (   "HIERARCHY_1", HIERARCHY_1)
    (   "HIERARCHY_2", HIERARCHY_2)
    (   "HIERARCHY_4", HIERARCHY_4)
    (   "HIERARCHY_NONE", HIERARCHY_NONE);
    boost::assign::insert(modulation) 
    (   "QPSK", QPSK)
    (   "QAM_128", QAM_128)
    (   "QAM_16", QAM_16)
    (   "QAM_256", QAM_256)
    (   "QAM_32", QAM_32)
    (   "QAM_64", QAM_64);
    boost::assign::insert(transmit_mode) 
    (   "TRANSMISSION_MODE_2K", TRANSMISSION_MODE_2K)
    (   "TRANSMISSION_MODE_8K", TRANSMISSION_MODE_8K);
}

senf::DVBConfigParser::DVBConfigParser(fe_type_t type_, const string & configFilePath_) :  
    type(type_), 
    configFile()
{
    initConfigFile(configFilePath_);
}

senf::DVBConfigParser::~DVBConfigParser()
{
    configFile.close();
}
prefix_ void senf::DVBConfigParser::initConfigFile(string configFilePath_){
    if (configFilePath_.size() == 0) {
        if ( !(::getenv ("HOME")) )
            SENF_THROW_SYSTEM_EXCEPTION("$HOME not set! You need it to use default configfile.");
        string configPath(::getenv ("HOME"));
        switch(type) {
            case FE_QPSK :
                configPath += string("/.szap/channels.conf");
                break;
            case FE_QAM :
                configPath += string("/.czap/channels.conf");
                break;
            case FE_OFDM :
                configPath += string("/.tzap/channels.conf");
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("Could not determine type of card.");
        }
        configFilePath_ = configPath;
    }
    configFilePath = configFilePath_;
    configFile.open( configFilePath.c_str(), ios_base::in);
    if(configFile.bad())
        SENF_LOG((senf::log::IMPORTANT)  ("Could not open channels file"<< configFilePath << "." ));
    configFile.close();
}
prefix_ string senf::DVBConfigParser::getConfigLine(string channel)
{
    string configLine;
    size_t pos;
    transform(channel.begin(), channel.end(), channel.begin(), ::toupper);
    
    configFile.open( configFilePath.c_str(), ios_base::in);
    if(configFile.bad())
           SENF_THROW_SYSTEM_EXCEPTION("Could not read channels file: ") << configFilePath << ".";
    
    while (configFile.good()){
        getline( configFile, configLine );
        SENF_LOG((senf::log::NOTICE)  ("configLine: " << configLine ));
        transform(configLine.begin(), configLine.end(), configLine.begin(), ::toupper);
        pos = configLine.find(channel);
        
        if(pos != string::npos && pos == 0){ // only first matching number should be interpreted as channel number 
            configFile.close();
            return configLine; // Line found!
        }
    }
    configFile.close();
    SENF_THROW_SYSTEM_EXCEPTION("Channel \"")<< channel << "\" not found!";
    return channel;
}

prefix_ dvb_frontend_parameters senf::DVBConfigParser::getFrontendParam(string configLine){
    struct dvb_frontend_parameters frontend;
    transform(configLine.begin(), configLine.end(), configLine.begin(), ::toupper);
    boost::char_separator<char> sep(":");
    tokenizer tokens(configLine, sep);
    switch (type) {
        case FE_QPSK:
            frontend = getFrontendParamDVB_S(tokens);
            break;
        case FE_QAM:
            frontend = getFrontendParamDVB_C(tokens);
            break;
        case FE_OFDM:
            frontend = getFrontendParamDVB_T(tokens);
            break;
        default:
            SENF_THROW_SYSTEM_EXCEPTION("Could not determine type of card.");
    }
    return frontend;
}

prefix_ dvb_frontend_parameters senf::DVBConfigParser::getFrontendParamDVB_T( const tokenizer & tokens){
    
    struct dvb_frontend_parameters frontend;
    istringstream isst;
    int number;
    enum { p_Frequency=1, p_Inversion, p_Bandwidth, p_hp_code_rate, p_lp_code_rate, p_Mudualtion, p_Transmission, p_guard, p_hierarchy};
    vector<string> words( tokens.begin(), tokens.end() );
    
    ::memset(&frontend, 0, sizeof(struct dvb_frontend_parameters));
    
    /*if(words.size() < 10)
        SENF_THROW_SYSTEM_EXCEPTION("Too few arguments! There must be at least 10, but there are only: ") << words.size();*/
    
    isst.str(words[p_Frequency]);
    isst >> number;
    if(isst.fail())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse frequency");
    frontend.frequency = number;
    
    if( params.inversion.find(words[p_Inversion]) == params.inversion.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse inversion");
    frontend.inversion = params.inversion.find(words[p_Inversion])->second;
    
    if( params.bandwidth.find(words[p_Bandwidth]) == params.bandwidth.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse bandwidth");
    frontend.u.ofdm.bandwidth = params.bandwidth.find(words[p_Bandwidth])->second;
    
    if( params.code_rate.find(words[p_hp_code_rate]) == params.code_rate.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse high priority stream code rate");
    frontend.u.ofdm.code_rate_HP = params.code_rate.find(words[p_hp_code_rate])->second;
    
    if( params.code_rate.find(words[p_lp_code_rate]) == params.code_rate.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse low priority stream code rate");
    frontend.u.ofdm.code_rate_LP = params.code_rate.find(words[p_lp_code_rate])->second;
    
    if( params.modulation.find(words[p_Mudualtion]) == params.modulation.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse modulation");
    frontend.u.ofdm.constellation = params.modulation.find(words[p_Mudualtion])->second;
    
    if( params.transmit_mode.find(words[p_Transmission]) == params.transmit_mode.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse transmission mode");
    frontend.u.ofdm.transmission_mode = params.transmit_mode.find(words[p_Transmission])->second;
    
    if( params.guard_interval.find(words[p_guard]) == params.guard_interval.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse guard interval");
    frontend.u.ofdm.guard_interval = params.guard_interval.find(words[p_guard])->second;  
    
    if( params.hierarchy.find(words[p_hierarchy]) == params.hierarchy.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse hierarchy");
    frontend.u.ofdm.hierarchy_information = params.hierarchy.find(words[p_hierarchy])->second;
    
    return frontend;
}

prefix_ dvb_frontend_parameters senf::DVBConfigParser::getFrontendParamDVB_S( const  tokenizer & tokens){
    struct dvb_frontend_parameters frontend;
    istringstream isst;
    int number;
    enum { p_Frequency=1, p_Inversion, p_Symbole, p_code_rate};
    vector<string> words( tokens.begin(), tokens.end() ) ;
    
    ::memset(&frontend, 0, sizeof(struct dvb_frontend_parameters));
    
    if(words.size() < 5)
                SENF_THROW_SYSTEM_EXCEPTION("Too few arguments! There must be at least 5, but there are only: ") << words.size();
    
    isst.str(words[p_Frequency]);
    isst >> number;
    if (isst.fail())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse frequency");
    frontend.frequency = number;
    
    if (params.inversion.find(words[p_Inversion]) == params.inversion.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse inversion");
    frontend.inversion = params.inversion.find(words[p_Inversion])->second;
    
    isst.str(words[p_Symbole]);
    isst >> number;
    if (isst.fail())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse symbole rate");
    frontend.u.qpsk.symbol_rate = number;

    if (params.code_rate.find(words[p_code_rate]) == params.code_rate.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse code rate");
    frontend.u.qpsk.fec_inner = params.code_rate.find(words[p_code_rate])->second;
    
    return frontend;
}

prefix_ dvb_frontend_parameters senf::DVBConfigParser::getFrontendParamDVB_C( const  tokenizer & tokens){
    struct dvb_frontend_parameters frontend;
    istringstream isst;
    int number;
    enum { p_Frequency=1, p_Inversion, p_Symbole, p_code_rate, p_modulation};
    vector<string> words( ++tokens.begin(), tokens.end() ) ; 
  
    ::memset(&frontend, 0, sizeof(struct dvb_frontend_parameters));
    
    if(words.size() < 6)
        SENF_THROW_SYSTEM_EXCEPTION("Too few arguments! There must be at least 6, but there are only: ") << words.size();
    
    isst.str(words[p_Frequency]);
    isst >> number;
    if (isst.fail())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse frequency");
    frontend.frequency = number;
    
    if (params.inversion.find(words[p_Inversion]) == params.inversion.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse inversion");
    frontend.inversion = params.inversion.find(words[p_Inversion])->second;
    
    isst.str(words[p_Symbole]);
    isst >> number;
    if (isst.fail())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse symbole rate");
    frontend.u.qam.symbol_rate = number;
    
    if (params.code_rate.find(words[p_code_rate]) == params.code_rate.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse code rate");
    frontend.u.qam.fec_inner = params.code_rate.find(words[p_code_rate])->second;
    
    if (params.modulation.find(words[p_modulation]) == params.modulation.end())
        SENF_THROW_SYSTEM_EXCEPTION("Cant parse modulation");
    frontend.u.qam.modulation = params.modulation.find(words[p_modulation])->second;
    
    return frontend;
}
