//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief NetdeviceController public header
 */

#ifndef HH_SENF_Socket_NetdeviceController_
#define HH_SENF_Socket_NetdeviceController_ 1

// Custom includes
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>


//#include "NetdeviceController.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

struct ifreq;

namespace senf {

    struct MACAddress;


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
        void reset(std::string const & interface_name);
                                        ///< reset the ifindex_ from the given iface_name (i.e. after USB Ethernet unplug/plug
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

        void addVLAN(std::uint16_t vlanId);
                                        ///< add a VLAN interface 
                                        /**< Note, that this is a privileged operation. */
        void delVLAN(std::uint16_t vlanId);
                                        ///< delete a VLAN interface 
                                        /**< Note, that this is a privileged operation. */

        void timestamping(int txType, int rxFilter);
                                        ///< configures hw timestamping for RX and/or TX 
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

        static std::string macToName(MACAddress const & mac);
        static MACAddress readMACAddressFromFile(boost::filesystem::path const & path);

    protected:
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
