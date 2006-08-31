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

#ifndef HH_ServerSocketHandle_
#define HH_ServerSocketHandle_ 1

// Custom includes
#include <boost/static_assert.hpp>
#include <boost/call_traits.hpp>
#include "SocketHandle.hh"
#include "CommunicationPolicy.hh"
#include "AddressingPolicy.hh"

//#include "ServerSocketHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    template <class Policy> class ClientSocketHandle;

    /** \brief
      */
    template <class Policy>
    class ServerSocketHandle
        : public SocketHandle<Policy>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef typename Policy::AddressingPolicy::Address Address;
        typedef typename boost::call_traits<Address>::param_type AddressParam;
        typedef ClientSocketHandle<Policy> ClientSocketHandle;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // conversion constructors
        template <class OtherPolicy>
        ServerSocketHandle(ServerSocketHandle<OtherPolicy> other,
                           typename SocketHandle<Policy>::template IsCompatible<OtherPolicy>::type * = 0);

        template <class OtherPolicy>
        typename SocketHandle<Policy>::template IsCompatible<OtherPolicy>::type const & 
        operator=(ServerSocketHandle<OtherPolicy> other);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////////
        ///\name Server socket interface
        ///@{

        void         bind         (AddressParam addr);

        Address      local        ();
        void         local        (Address & addr);
        
        ClientSocketHandle 
                     accept       ();
        std::pair<ClientSocketHandle, Address>
                     acceptfrom   ();
        ClientSocketHandle
                     acceptfrom   (Address & addr);
        
        ///@}

        static ServerSocketHandle cast_static(FileHandle handle);

    protected:
        ServerSocketHandle(FileHandle other, bool isChecked);
        explicit ServerSocketHandle(std::auto_ptr<SocketProtocol> protocol);

    private:
        
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ServerSocketHandle.cci"
//#include "ServerSocketHandle.ct"
#include "ServerSocketHandle.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
