// $Id:PacketSocketHandle.hh 218 2007-03-20 14:39:32Z tho $
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS) 
// Competence Center NETwork research (NET), St. Augustin, GERMANY 
//     Stefan Bund <g0dil@berlios.de>
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
    \brief PacketProtocol and PacketSocketHandle public header

    \todo Implement global promisc() helper based on ioctl() interface.
 */

#ifndef HH_TunTapSocketHandle_
#define HH_TunTapSocketHandle_ 1

// Custom includes
#include "../../../Socket/SocketPolicy.hh"
#include "../../../Socket/SocketProtocol.hh"
#include "../../../Socket/ProtocolClientSocketHandle.hh"
#include "../../../Socket/FramingPolicy.hh"
#include "../../../Socket/CommunicationPolicy.hh"
#include "../../../Socket/ReadWritePolicy.hh"
#include "../../../Socket/Protocols/BSDSocketProtocol.hh"
#include "LLAddressing.hh"

//#include "TunTapSocketHandle.mpp"
//#include "TunTapSocketHandle.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    /// @{

    typedef MakeSocketPolicy<
        NoAddressingPolicy,
        DatagramFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy Tap_Policy;        ///< Policy for TAP

    /** \brief TAP

        \todo document me

        \par Socket Handle typedefs:


        \par Policy Interface:


        \par Address Type:


        This class is utilized as the protocol class of the ProtocolClientSocketHandle via the
        Socket Handle typedefs above.
     */
    class TapProtocol
        : public ConcreteSocketProtocol<Tap_Policy,TapProtocol>,
          public BSDSocketProtocol
    {
    public:
        ///\name Constructors
        ///@{
        void init_client() const;
                                        ///< Create TAP socket
                                        /**< \todo document me */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        void init_client(std::string const & interface_name, bool const NO_PI=true) const;
                                        ///< Create TAP socket
                                        /**< \todo document me
                                             \param[in] address remote address to connect to */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        
        ///@}

        ///\name Abstract Interface Implementation
        ///@{

        unsigned available() const;
        bool eof() const;

        ///@}
    };

    typedef ProtocolClientSocketHandle<TapProtocol> TapSocketHandle;
                                        ///< SocketHandle of TapProtocol
                                        /**< \related TapPrototol */

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "TunTapSocketHandle.cci"
//#include "TunTapSocketHandle.ct"
//#include "TunTapSocketHandle.cti"
//#include "TunTapSocketHandle.mpp"
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
