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

#ifndef HH_ClientSocketHandle_
#define HH_ClientSocketHandle_ 1

// Custom includes
#include "SocketHandle.hh"

//#include "ClientSocketHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    /** \brief
      */
    template <class SocketPolicy>
    class ClientSocketHandle
        : public SocketHandle<SocketPolicy>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // conversion constructors
        template <class OtherPolicy>
        ClientSocketHandle(ClientSocketHandle<OtherPolicy> other,
                           typename SocketHandle<SocketPolicy>::template IsCompatible<OtherPolicy>::type * = 0);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        template <class OtherPolicy>
        typename SocketHandle<SocketPolicy>::template IsCompatible<OtherPolicy>::type const & 
        operator=(ClientSocketHandle<OtherPolicy> other);

    protected:
        explicit ClientSocketHandle(std::auto_ptr<SocketProtocol> protocol);

    private:

    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ClientSocketHandle.cci"
//#include "ClientSocketHandle.ct"
#include "ClientSocketHandle.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
