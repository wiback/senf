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

#ifndef HH_SocketPolicy_
#define HH_SocketPolicy_ 1

// Custom includes
#include <boost/type_traits.hpp>

//#include "SocketPolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    class AddressingPolicyBase
    {};
    typedef AddressingPolicyBase UnspecifiedAddressingPolicy;

    class FramingPolicyBase
    {};
    typedef FramingPolicyBase UnspecifiedFramingPolicy;
    
    class CommunicationPolicyBase
    {};
    typedef CommunicationPolicyBase UnspecifiedCommunicationPolicy;
    
    class ReadPolicyBase
    {};
    typedef ReadPolicyBase UnspecifiedReadPolicy;
    
    class WritePolicyBase
    {};
    typedef WritePolicyBase UnspecifiedWritePolicy;
    
    class BufferingPolicyBase
    {};
    typedef BufferingPolicyBase UnspecifiedBufferingPolicy;
    
    class SocketPolicyBase
    {
        typedef AddressingPolicyBase AddressingPolicy;
        typedef FramingPolicyBase FramingPolicy;
        typedef CommunicationPolicyBase CommunicationPolicy;
        typedef ReadPolicyBase ReadPolicy;
        typedef WritePolicyBase WritePolicy;
        typedef BufferingPolicyBase BufferingPolicy;
    };

    template <class AddressingPolicy_=AddressingPolicyBase,
              class FramingPolicy_=FramingPolicyBase,
              class CommunicationPolicy_=CommunicationPolicyBase,
              class ReadPolicy_=ReadPolicyBase,
              class WritePolicy_=WritePolicyBase,
              class BufferingPolicy_=BufferingPolicyBase>
    class SocketPolicy
        : public SocketPolicyBase
    {
    public:
        typedef AddressingPolicy_ AddressingPolicy;
        typedef FramingPolicy_ FramingPolicy;
        typedef CommunicationPolicy_ CommunicationPolicy;
        typedef ReadPolicy_ ReadPolicy;
        typedef WritePolicy_ WritePolicy;
        typedef BufferingPolicy_ BufferingPolicy;

        AddressingPolicy addressingPolicy;
        FramingPolicy framingPolicy;
        CommunicationPolicy communicationPolicy;
        ReadPolicy readPolicy;
        WritePolicy writePolicy;
        BufferingPolicy bufferingPolicy;
    };

    struct nil {};

    template <class T1, class T2=nil, class T3=nil, class T4=nil, class T5=nil, class T6=nil>
    class MakeSocketPolicy;
    // the rest is implementation detail

    template <class BasePolicy, class DerivedPolicy>
    struct SocketPolicyIsBaseOf;
    // the rest is implementation detail ...

    template <class Policy, class Trait>
    struct IfAddressingPolicyIs;
    // the rest is implementation detail ...

    template <class Policy, class Trait>
    struct IfFramingPolicyIs;
    // the rest is implementation detail ...

    template <class Policy, class Trait>
    struct IfCommunicationPolicyIs;
    // the rest is implementation detail ...

    template <class Policy, class Trait>
    struct IfReadPolicyIs;
    // the rest is implementation detail ...

    template <class Policy, class Trait>
    struct IfWritePolicyIs;
    // the rest is implementation detail ...

    template <class Policy, class Trait>
    struct IfBufferingPolicyIs;
    // the rest is implementation detail ...
}}

//////////////////////////////hh.e////////////////////////////////////////
#include "SocketPolicy.ih"
//#include "SocketPolicy.cci"
//#include "SocketPolicy.ct"
//#include "SocketPolicy.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
