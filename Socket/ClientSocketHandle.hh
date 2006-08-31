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
#include <boost/call_traits.hpp>
#include "SocketHandle.hh"

//#include "ClientSocketHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    template <class Policy> class ServerSocketHandle;

    /** \brief
      */
    template <class Policy>
    class ClientSocketHandle
        : public SocketHandle<Policy>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef typename Policy::AddressingPolicy::Address Address;
        typedef typename boost::call_traits<Address>::param_type AddressParam;
        typedef ServerSocketHandle<Policy> ServerSocketHandle;

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
                           typename SocketHandle<Policy>::template IsCompatible<OtherPolicy>::type * = 0);

        template <class OtherPolicy>
        typename SocketHandle<Policy>::template IsCompatible<OtherPolicy>::type const & 
        operator=(ClientSocketHandle<OtherPolicy> other);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////////
        ///\name reading and writing
        ///@{

        // read from socket (connected or unconnected)
        std::string  read         ();
        void         read         (std::string & buffer);
        unsigned     read         (char * buffer, unsigned size);

        // read from unconnected socket returning peer address
        std::pair<std::string, Address> 
                     readfrom     ();
        void         readfrom     (std::string & buffer, Address & from);
        unsigned     readfrom     (char * buffer, unsigned size, Address & from);

        // write to connected socket
        unsigned     write        (std::string const & data);
        unsigned     write        (char const * buffer, unsigned size);

        // write to unconnected socket
        unsigned     writeto      (AddressParam addr, std::string const & data);
        unsigned     writeto      (AddressParam addr, char const * buffer, unsigned size);

        ///@}

        ///////////////////////////////////////////////////////////////////////////
        ///\name Addressing
        ///@{

        void         connect      (AddressParam addr);
        void         bind         (AddressParam addr);

        Address      peer         ();
        void         peer         (Address & addr);
        Address      local        ();
        void         local        (Address & addr);

        ///@}

        ///////////////////////////////////////////////////////////////////////////
        ///\name Buffering
        ///@{
        
        unsigned     rcvbuf      ();
        void         rcvbuf      (unsigned size);        
        unsigned     sndbuf      ();
        void         sndbuf      (unsigned size);
        
        ///@}
        
        static ClientSocketHandle cast_static(FileHandle handle);
                 
    protected:
        ClientSocketHandle(FileHandle other, bool isChecked);
        explicit ClientSocketHandle(std::auto_ptr<SocketProtocol> protocol,
                                    int fd = -1);

    private:
        unsigned available();

        friend class satcom::lib::ServerSocketHandle<Policy>;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ClientSocketHandle.cci"
#include "ClientSocketHandle.ct"
#include "ClientSocketHandle.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
