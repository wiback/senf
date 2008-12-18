// $Id: DVBConfigParser.hh 965 2008-11-18 16:04:20Z g0dil $
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Anton Gillert <atx23@berlios.de>
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

#ifndef DVBCONFIGPARSER_HH_
#define DVBCONFIGPARSER_HH_

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
                DVBParams(); };
    static const DVBParams params;
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
 

public:
	DVBConfigParser(fe_type_t type_, const std::string & configFilePath = "");
	~DVBConfigParser();
	
	std::string getConfigLine(std::string channel);
	    
	dvb_frontend_parameters getFrontendParam(std::string configLine);
	
private:
	
	fe_type_t type;
	std::ifstream configFile;
	void initConfigFile(std::string _configFilePath);
	
	dvb_frontend_parameters getFrontendParamDVB_T( const tokenizer & tokens);
	dvb_frontend_parameters getFrontendParamDVB_C( const tokenizer & tokens);
	dvb_frontend_parameters getFrontendParamDVB_S( const tokenizer & tokens);
	
};
}
#endif /*DVBCONFIGPARSER_H_*/
