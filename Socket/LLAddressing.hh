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

#ifndef HH_LLAddressing_
#define HH_LLAddressing_ 1

// Custom includes
#include <boost/range/iterator_range.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>

#include <sys/socket.h>
#include <netpacket/packet.h>

#include "SocketPolicy.hh"
#include "FileHandle.hh"
#include "GenericAddressingPolicy.hh"

//#include "LLAddressing.mpp"
#include "LLAddressing.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    class LLSocketAddress
    {
    public:
        // Right now we use an arbitrary ForwardRange (see Boost.Range)
        // as the representation for a hardware address. The restrictions
        // for the range are:
        // a) the range must never be larger than 8 elements
        // b) the value_type must be convertible to unsigend char
        // and really we need only a single-pass range.
        //
        // Since a hardware address is so short (a maximum of 8
        // bytes), in the aftermath I think a simple container holding
        // a maximum of 8 unsigned chars (e.g. Boost.Array with
        // additional length parameter) will be much simpler and
        // probably even more efficient. This should have a conversion
        // constructor from an arbitrary ForwardRange to make it
        // compatible e.g. with the Packet library.
        //
        // However, since I have implemented it already as it is now,
        // I'll leave it as it is ...

        typedef boost::iterator_range<unsigned char const *> LLAddress;
        
        LLSocketAddress();
        // And this is for bind
        explicit LLSocketAddress(unsigned protocol, std::string interface="");
        explicit LLSocketAddress(std::string interface);
        // This is for sending packets ..
        // We must use enable_if here, so this constructor will not hide
        // above constructor if passed a plain int or short argument
        template <class ForwardRange>
        explicit LLSocketAddress(ForwardRange const & address, std::string interface="",
                                 typename boost::enable_if_c<! boost::is_integral<ForwardRange>::value >::type * = 0);

        void clear();

        unsigned protocol() const;
        std::string interface() const;
        unsigned arptype() const;
        unsigned pkttype() const;
        LLAddress address() const;

        // The mutating interface is purposely restricted to allow only
        // changing those members, which are sensible to be changed.

        template <class ForwardRange>
        void address(ForwardRange const & address);
        void interface(std::string interface);
        void protocol(unsigned protocol);

        struct sockaddr * sockaddr_p();
        struct sockaddr const * sockaddr_p() const;
        unsigned sockaddr_len() const;

    private:
        struct ::sockaddr_ll addr_;
    };

    detail::LLAddressFromStringRange llAddress(std::string address);
    // The enable_if condition here allows only for classes as range.
    // However, excluding zero-terminated strings (which we want to
    // pass to above) I cannot think of a non-class ForwardRange
    // except for academic cases
    template <class ForwardRange>
    std::string llAddress(ForwardRange const & address,
                          typename boost::enable_if< boost::is_class<ForwardRange> >::type * = 0);

    class LLAddressingPolicy
        : public AddressingPolicyBase,
          private GenericAddressingPolicy<LLSocketAddress>
    {
    public:
        typedef LLSocketAddress Address;

        using GenericAddressingPolicy<LLSocketAddress>::local;
        using GenericAddressingPolicy<LLSocketAddress>::bind;
    };

    struct InvalidLLSocketAddressException : public std::exception
    { char const * what() const throw() { return "invalid ll address"; } };
}}

///////////////////////////////hh.e////////////////////////////////////////
#include "LLAddressing.cci"
#include "LLAddressing.ct"
#include "LLAddressing.cti"
//#include "LLAddressing.mpp"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
