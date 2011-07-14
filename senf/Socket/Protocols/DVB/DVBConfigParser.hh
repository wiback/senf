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
//   Anton Gillert <atx23@berlios.de>
/** \file
    \brief DVBDemuxHandles public header */

#ifndef HH_SENF_Socket_Protocols_DVB_DVBConfigParser_
#define HH_SENF_Socket_Protocols_DVB_DVBConfigParser_ 1

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
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
