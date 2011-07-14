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
//   David Wagner <dw6@berlios.de>
/** \file
    \brief UNSocketProtocol public header */

#ifndef HH_SENF_Socket_Protocols_UN_UNSocketProtocol_
#define HH_SENF_Socket_Protocols_UN_UNSocketProtocol_ 1

// Custom includes
#include <senf/Socket/SocketProtocol.hh>

//#include "UNSocketProtocol.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    //\{

    /** \brief Protocol facet providing Unix Domain Addressing related API

        This protocol facet introduces all the socket api protocol members which are related to Unix
        Domain addressing.
     */
    class UNSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        virtual void close();           ///< Close socket
                                        /**< This override will automatically \c shutdown() the
                                             socket whenever it is closed.
                                             \throws senf::SystemException */ //
        virtual void terminate() const;       ///< Forcibly close socket
                                        /**< This override will automatically \c shutdown() the
                                           socket whenever it is called. Additionally it will
                                           disable SO_LINGER to ensure, that v_terminate will not
                                           block. Like the overridden method, this member will
                                           ignore failures and will never throw. It therefore
                                           safe to be called from a destructor. */

        ///\name Abstract Interface Implementation
        //\{

        unsigned available() const;
        bool eof() const;

        //\}

    private:
        void check_and_unlink() const;

        std::string path_;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "UNSocketProtocol.cci"
//#include "UNSocketProtocol.ct"
//#include "UNSocketProtocol.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
