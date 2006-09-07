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

    template <class Protocol> class ProtocolServerSocketHandle;

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

#       define BOOST_PP_ITERATION_PARAMS_1 (4, (1, 9, "Socket/ProtocolClientSocketHandle.mpp", 1))
#       include BOOST_PP_ITERATE()
        
        ///@}
        ///////////////////////////////////////////////////////////////////////////

        Protocol const & protocol();

        static ProtocolClientSocketHandle cast_static(FileHandle handle);
        static ProtocolClientSocketHandle cast_dynamic(FileHandle handle);

        void state(SocketStateMap & map, unsigned lod=0);
        std::string dumpState(unsigned lod=0);

    protected:
        ProtocolClientSocketHandle(FileHandle other, bool isChecked);

    private:
        friend class ProtocolServerSocketHandle<Protocol>;
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
