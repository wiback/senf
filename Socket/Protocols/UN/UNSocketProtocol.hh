// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     David Wagner <dw6@berlios.de>
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
    \brief UNSocketProtocol public header */

#ifndef HH_UNSocketProtocol_
#define HH_UNSocketProtocol_ 1

// Custom includes
#include "../../../Socket/SocketProtocol.hh"
#include "UNAddressing.hh"
#include "../../../Socket/ClientSocketHandle.hh"
#include "../../../Socket/CommunicationPolicy.hh"

//#include "UNSocketProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    /// @{

    /** \brief Protocol facet providing Unix Domain Addressing related API

        This protocol facet introduces all the socket api protocol members which are related to Unix 
        Domain addressing.
     */
    class UNSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        virtual void close() const;   ///< Close socket
                              /**< This override will automatically \c shutdown() the
                                   socket whenever it is closed.
                                   \throws senf::SystemException */  
        virtual void terminate() const;       ///< Forcibly close socket
                                        /**< This override will automatically \c shutdown() the
                                           socket whenever it is called. Additionally it will
                                           disable SO_LINGER to ensure, that v_terminate will not
                                           block. Like the overriden method, this member will ignore
                                           failures and will never throw. It therefore safe to be
                                           called from a destructor. */        ///\name Abstract Interface Implementation
        ///@{

         unsigned available() const;
         bool eof() const;
         
    private:
        void check_and_unlink() const;  
      
        std::string path_;
    };

    ///@}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "UNSocketProtocol.cci"
//#include "UNSocketProtocol.ct"
//#include "UNSocketProtocol.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
