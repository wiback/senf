// $Id:PacketSocketHandle.hh 218 2007-03-20 14:39:32Z tho $
//
// Copyright (C) 2008
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief PacketSocketProtocol and PacketSocketHandle public header
 */

#ifndef HH_SENF_Socket_Protocols_Raw_TunTapSocketHandle_
#define HH_SENF_Socket_Protocols_Raw_TunTapSocketHandle_ 1

// Custom includes
#include <senf/Socket/SocketPolicy.hh>
#include <senf/Socket/SocketProtocol.hh>
#include <senf/Socket/ProtocolClientSocketHandle.hh>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/Protocols/BSDSocketProtocol.hh>

//#include "TunTapSocketHandle.mpp"
//#include "TunTapSocketHandle.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    //\{

    typedef MakeSocketPolicy<
        NoAddressingPolicy,
        DatagramFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy Tap_Policy;        ///< Policy for TapSocketProtocol

    /** \brief SocketProcol for the tap pseudo-device.

        \par Socket Handle typedefs:
            \ref TapSocketHandle

        \par Policy Interface:
            ClientSocketHandle::read() ...

        The TapSocketProtocol provides access to the Linux tap device.

        The tap device is a virtual ethernet network device. The tap driver was designed as low
        level kernel support for ethernet tunneling. Userland application can write Ethernet
        frames to the socket and kernel will receive this frames from the tap interface.
        In the same time every frame that kernel writes to the tap interface can be read by
        userland application from the socket.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle via the
        Socket Handle typedef above.

        \see <a href="http://www.kernel.org/pub/linux/kernel/people/marcelo/linux-2.4/Documentation/networking/tuntap.txt">Kernel documentation for the TUN/TAP device driver.</a>
     */
    class TapSocketProtocol
        : public ConcreteSocketProtocol<Tap_Policy,TapSocketProtocol>,
          public BSDSocketProtocol
    {
    public:
        ///\name Constructors
        //\{
        void init_client() const;       ///< Open tap socket and create new tap interface
                                        /**< Opens the tun/tap socket and create a new tap interface
                                             Use \ref ifaceName() to get the actual name of the newly
                                             created interface.

                                             The new interface is down and has to be set up separately.
                                             After closing the socket, the tap interface and all
                                             corresponding routes will be deleted automatically. */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        void init_client(std::string const & interface_name, bool NO_PI=true) const;
                                        ///< Open tap socket and create new tap interface
                                        /**< Opens the tun/tap socket and create a new tap interface
                                             with the given name. Note that the created interface can
                                             have a different name as specified. Use \ref ifaceName()
                                             to get the actual name.

                                             The new interface is down and has to be set up separately.
                                             After closing the socket, the tap interface and all
                                             corresponding routes will be deleted automatically.
                                             \param[in] interface_name name of the new tap interface.
                                             \param[in] NO_PI if set to \c false each packet has a
                                                 additional 4 bytes header (flags, proto) */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        //\}

        ///\name Abstract Interface Implementation
        //\{

        unsigned available() const;
        bool eof() const;               ///< returns always false.
        unsigned int ifaceIndex() const;///< returns the index of the correspondent tap interface
        std::string ifaceName() const;  ///< returns the actual name of the correspondent tap interface

    private:
        mutable unsigned int ifaceIndex_;
        //\}
    };

    typedef ProtocolClientSocketHandle<TapSocketProtocol> TapSocketHandle;
                                        ///< SocketHandle of TapSocketProtocol
                                        /**< \related TapSocketProtocol */

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
