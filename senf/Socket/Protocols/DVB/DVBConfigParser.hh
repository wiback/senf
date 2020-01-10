//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief DVBDemuxHandles public header */

#ifndef HH_SENF_Socket_Protocols_DVB_DVBConfigParser_
#define HH_SENF_Socket_Protocols_DVB_DVBConfigParser_ 1

#include <string>
#include <map>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <linux/dvb/frontend.h>

namespace senf {

    class DVBConfigParser
    {
        struct DVBParams {
                std::map<std::string, fe_spectral_inversion_t> inversion;
                std::map<std::string, fe_bandwidth_t> bandwidth;
                std::map<std::string, fe_code_rate_t> code_rate;
                std::map<std::string, fe_guard_interval_t> guard_interval;
                std::map<std::string, fe_hierarchy_t> hierarchy;
                std::map<std::string, fe_modulation_t> modulation;
                std::map<std::string, fe_transmit_mode_t> transmit_mode;
                DVBParams();
        };
        static const DVBParams params;
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    public:
        DVBConfigParser(fe_type_t type_, const std::string & configFilePath = "");
        ~DVBConfigParser();

        std::string getConfigLine(std::string channel);

        dvb_frontend_parameters getFrontendParam(std::string configLine);

    private:
        fe_type_t type;
        std::string configFilePath;
        std::ifstream configFile;
        void initConfigFile(std::string _configFilePath);

        dvb_frontend_parameters getFrontendParamDVB_T( const tokenizer & tokens);
        dvb_frontend_parameters getFrontendParamDVB_C( const tokenizer & tokens);
        dvb_frontend_parameters getFrontendParamDVB_S( const tokenizer & tokens);

    };
}

#endif
