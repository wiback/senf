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
    \brief NetdeviceController public header
 */

#ifndef HH_SENF_Socket_NetdeviceController_
#define HH_SENF_Socket_NetdeviceController_ 1

// Custom includes
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>


//#include "NetdeviceController.mpp"
///////////////////////////////hh.p////////////////////////////////////////

struct ifreq;

namespace senf {

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
        void hardwareAddress(const MACAddress &newAddress);
                                        ///< set hardware address
                                        /**< Changes the hardware address of the interface.
                                             Note, that setting the hardware address is a privileged
                                             operation. It is only allowed when the interface is not
                                             up. If the interface is up, this call will cause an
                                             SystemException to be thrown. */
        std::string interfaceName() const;
                                        ///< return interface name
        void interfaceName(const std::string &newName);
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
        void doIoctl(ifreq& ifr, int request) const;
        void ifrName(ifreq& ifr) const;
        SockFd::ptr sockfd_;
        int ifindex_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
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
