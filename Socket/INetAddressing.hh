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

/** \file
    \brief INet[46]Address and INet[46]AddressingPolicy public header
 */

#ifndef HH_INetAddressing_
#define HH_INetAddressing_ 1

// Custom includes
#include <string>
#include <exception>
#include <netinet/in.h>
#include "SocketPolicy.hh"
#include "ClientSocketHandle.hh"
#include "CommunicationPolicy.hh"
#include "GenericAddressingPolicy.hh"

//#include "INetAddressing.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup addr_group
    /// @{

    /** \brief IPv4 socket address

	INet4Address wrapps the standard sockaddr_in datatype. It provides simple accessor methods
	to accss the host and port. It does \e not integrate \c gethostbyname or DNS lookup.

	\todo Implement real INet4Address datatype and rename this one to INet4SockAddress ...
	\todo Implement more complete interface
	\todo  gethostbyname support ?
     */
    class INet4Address
    {
    public:
        INet4Address();
        INet4Address(char const * address); ///< Set address and port
                                        /**< See INet4Address(std::string)
					     \throws InvalidINetAddressException
					     \fixme Why do I need this version? Shouldn't the
					     std::string version be enough ? */
        INet4Address(std::string address); ///< Set address and port
                                        /**< This constructor expects a string of the form
					     'xxx.xxx.xxx.xxx:pppp'. The constructor will use this
					     value to initialize the host and port members. This
					     constructor does \e only support numeric ip addresses
					     not hostnames 
					     \param[in] address Address and port 
					     \throws InvalidINetAddressException */
        INet4Address(std::string host, unsigned port); ///< Set address and port explicitly
                                        /**< \param[in] host ip address in dotted-quad notation
					     \param[in] port port number 
					     \throws InvalidINetAddressException */
	

        bool operator==(INet4Address const & other) const; ///< Check INet4Address for equality

        std::string str() const;        ///< Return "address:port" string
        std::string host() const;       ///< Return address in doted quad notation
        unsigned port() const;          ///< Return portnumber

        void clear();                   ///< Clear address/port to 0.0.0.0:0

	/// \name Generic Address Interface
	/// @{

        struct sockaddr * sockaddr_p();
        struct sockaddr const * sockaddr_p() const;
        unsigned sockaddr_len() const;

	/// @}

    private:
        void assignString(std::string addr);
        
        struct ::sockaddr_in addr_;
    };

    /** \brief Write address and port to os

	\related INet4Address 
     */
    std::ostream & operator<<(std::ostream & os, INet4Address const & addr);

    /** \brief IPv6 socket address

	\todo Implement
     */
    class INet6Address
    {
    };

    /** \brief Signal invalid INet address syntax

	\related INet4Address
	\relatesalso INet6Address
     */
    struct InvalidINetAddressException : public std::exception
    { char const * what() const throw() { return "invalid inet address"; } };

    /// @}

    /// \addtogroup policy_impl_group
    /// @{

    /** \brief Addressing policy supporting IPv4 addressing
	
	\par Address Type:
	    INet4Address
	
	This addressing policy implements addressing using Internet V4
	addresses.

	The various members are directly importet from
	GenericAddressingPolicy which see for a detailed
	documentation.
     */
    struct INet4AddressingPolicy 
        : public AddressingPolicyBase,
          private GenericAddressingPolicy<INet4Address>
    {
        typedef INet4Address Address;

        using GenericAddressingPolicy<INet4Address>::peer;
        using GenericAddressingPolicy<INet4Address>::local;
        using GenericAddressingPolicy<INet4Address>::connect;
        using GenericAddressingPolicy<INet4Address>::bind;
    };

    /** \brief Addressing policy supporting IPv6 addressing
	
	\par Address Type:
	    INet6Address
	
	This addressing policy implements addressing using Internet V6
	addresses.

	The various members are directly importet from
	GenericAddressingPolicy which see for a detailed
	documentation.

	\todo implement
     */
    struct INet6AddressingPolicy : public AddressingPolicyBase
    {
        typedef INet6Address Address;
    };

    /// @}

}

///////////////////////////////hh.e////////////////////////////////////////
#include "INetAddressing.cci"
//#include "INetAddressing.ct"
//#include "INetAddressing.cti"
//#include "INetAddressing.mpp"
#endif


// Local Variables:
// mode: c++
// c-file-style: "senf"
// fill-column: 100
// End:
