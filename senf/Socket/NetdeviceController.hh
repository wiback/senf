// $Id$
//
// Copyright (C) 2007
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
    \brief NetdeviceController public header
 */

#ifndef HH_SENF_Socket_NetdeviceController_
#define HH_SENF_Socket_NetdeviceController_ 1

// Custom includes
#include <string>
#include <boost/shared_ptr.hpp>

//#include "NetdeviceController.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

struct ifreq;

namespace senf {

    class MACAddress;


    /** \brief Netdevice Controller

        This controller provides an interface which can be used to configure network
        devices. Note, that some setting members are privileged operations.

        \see manual page netdevice(7) for more informations.
     */
    class NetdeviceController
    {
    public:
        explicit NetdeviceController(std::string const & interface_name);
                                        ///< Construct a new controller for the given interface name.
        explicit NetdeviceController(int interface_index);
                                        ///< Construct a new controller for the given interface index.
        int interfaceIndex() const;     ///< return the interface index
        MACAddress hardwareAddress() const;
                                        ///< return hardware address
        void hardwareAddress(MACAddress const & newAddress);
                                        ///< set hardware address
                                        /**< Changes the hardware address of the interface.
                                             Note, that setting the hardware address is a privileged
                                             operation. It is only allowed when the interface is not
                                             up. If the interface is up, this call will cause an
                                             SystemException to be thrown. */
        std::string interfaceName() const;
                                        ///< return interface name
        void interfaceName(std::string const & newName);
                                        ///< set interface name
                                        /**< Changes the name of the interface.
                                             Note, that setting the name is a privileged operation.
                                             It is only allowed when the interface is not up. If
                                             the interface is up, this call will cause an
                                             SystemException to be thrown. */

        int mtu() const;                ///< return the Maximum Transmission Unit
        void mtu(int new_mtu);          ///< set the Maximum Transmission Unit
                                        /**< Set the MTU (Maximum Transfer Unit) of the device.
                                             Note, that this is a privileged operation.
                                             Setting the MTU to too small values may cause kernel
                                             crashes. */

        int txqueuelen() const;         ///< return the Tx Queue Length
        void txqueuelen(int new_mtu);   ///< set the Tx Queue Length
                                        /**< Set the Tx Queue Length (in Packets) MTU of the device.
                                             Note, that this is a privileged operation. */


        bool promisc() const;           ///< return \c true if interface is in promiscuous mode
        void promisc(bool mode);        ///< enable/disable promiscuous mode of the interface
                                        /**< Note, that this is a privileged operation. */

        bool isUp() const;              ///< return \c true if interface is up
        void up();                      ///< ifconfig up interface
        void down();                    ///< ifconfig down interface

        struct SockFd {
            typedef boost::shared_ptr<SockFd> ptr;
            int fd;
            SockFd();
            ~SockFd();
        };

        static SockFd::ptr sockfd();

    private:
        void ifrName(ifreq & ifr) const;
        SockFd::ptr sockfd_;
        int ifindex_;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "NetdeviceController.cci"
//#include "NetdeviceController.ct"
//#include "NetdeviceController.cti"
//#include "NetdeviceController.mpp"
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
