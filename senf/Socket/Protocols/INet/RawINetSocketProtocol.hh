// $Id: RawINetSocketProtocol.hh 597 2008-01-15 09:16:20Z g0dil $
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
//   David Wagner <dw6@berlios.de>

#ifndef HH_SENF_Socket_Protocols_INet_RawINetSocketProtocol_
#define HH_SENF_Socket_Protocols_INet_RawINetSocketProtocol_ 1

// Custom includes
#include "INetSocketProtocol.hh"

namespace senf {

    /// \addtogroup protocol_facets_group
    //\{

    /** \brief Protocol facet to support RAW operations upon IPv4/6

        This protocol facet provides all those protocol functions,
        which are available on a PF_INET[6]/RAW socket.

     */
    class RawINetSocketProtocol
        : public virtual INetSocketProtocol
    {
    public:
        ///\name Abstract Interface Implementation
        //\{

        unsigned available() const;
        bool eof() const;

        //\}
    };

    //\}
}

#endif /*HH_SENF_Socket_Protocols_INet_RawINetSocketProtocol_*/
