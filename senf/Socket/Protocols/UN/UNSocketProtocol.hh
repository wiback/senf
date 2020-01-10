//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
