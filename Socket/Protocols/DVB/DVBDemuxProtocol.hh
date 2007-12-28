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
    \brief DVBDemuxProtocol public header */

#ifndef HH_DVBDemuxProtocol_
#define HH_DVBDemuxProtocol_ 1

#include <linux/dvb/dmx.h> 

// Custom includes
#include "../../../Socket/SocketProtocol.hh"

//#include "DVBProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    /// @{

    /** xxx
     */
    class DVBDemuxProtocol
        : public virtual SocketProtocol
    {
    public:
        void setBufferSize(unsigned long size) const;
        
        void startFiltering() const;
        void stopFiltering() const;
        
        ///\name Abstract Interface Implementation
        ///@{
        
        bool eof() const;

        ///@}
    };
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "DVBDemuxProtocol.cci"
//#include "DVBDemuxProtocol.ct"
//#include "DVBDemuxProtocol.cti"
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
