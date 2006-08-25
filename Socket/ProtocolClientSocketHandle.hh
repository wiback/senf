// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#ifndef HH_ProtocolClientSocketHandle_
#define HH_ProtocolClientSocketHandle_ 1

// Custom includes
#include "ClientSocketHandle.hh"

#include "ProtocolClientSocketHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    /** \brief
      */
    template <class SocketProtocol>
    class ProtocolClientSocketHandle
        : public ClientSocketHandle<typename SocketProtocol::Policy>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef SocketProtocol Protocol;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        ProtocolClientSocketHandle();
        template <class A1>
        ProtocolClientSocketHandle(A1 const & a1);

#       define BOOST_PP_ITERATION_PARAMS_1 (4, (2, 9, "Socket/ProtocolClientSocketHandle.mpp", 1))
#       include BOOST_PP_ITERATE()
        
        ///@}
        ///////////////////////////////////////////////////////////////////////////

        Protocol const & protocol();

    protected:

    private:

    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ProtocolClientSocketHandle.cci"
//#include "ProtocolClientSocketHandle.ct"
#include "ProtocolClientSocketHandle.cti"
#include "ProtocolClientSocketHandle.mpp"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
