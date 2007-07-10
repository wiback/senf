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
    \brief ProtocolClientSocketHandle public header
 */

#ifndef HH_ProtocolClientSocketHandle_
#define HH_ProtocolClientSocketHandle_ 1

// Custom includes
#include "ClientSocketHandle.hh"

#include "ProtocolClientSocketHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup handle_group
    /// @{

    template <class Protocol> class ProtocolServerSocketHandle;

    /** \brief Protocol specific socket handle (client interface)

        The ProtocolClientSocketHandle is the client interface leaf class of the handle
        hierarchy. This is the class to instantiate to open a new socket. This is also the \e only
        class, which can be used to open a client socket.

        The \a Protocol template argument defines the protocol of the socket. This protocol provides
        the protocol interface of the socket as well as the complete socket policy of this protocol.

        The ProtocolClientSocketHandle adds the protocol interface as an additional interface to the
        socket handle. This interface is only accessible via the protocol class. All socket
        functionality not available through the policy interface (see ClientSocketHandle) is
        accessible via the protocol() member.

        \see \ref protocol_group
      */
    template <class SocketProtocol>
    class ProtocolClientSocketHandle
        : public ClientSocketHandle<typename SocketProtocol::Policy>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef SocketProtocol Protocol; ///< The sockets protocol
        enum UninitializedType { Uninitialized }; ///< Flag to call 'uninitialized' constructor

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        /** \brief Create new client socket

            This constructor is one of the possible constructors. The exact Signature of the
            constructor (or constructors) is defined by the \c init_client() member (or members) of
            the \a Protocol class. ProtocolClientSocketHandle defines a number of constructors
            taking up to 9 arguments which just forward to a corresponding \a Protocol\c
            ::init_client() member. See the documentation of the respective Protocol class for a
            detailed documentation of that protocols constructors.
         */
        ProtocolClientSocketHandle();

#       define BOOST_PP_ITERATION_PARAMS_1 (4, (1, 9, "Socket/ProtocolClientSocketHandle.mpp", 1))
#       include BOOST_PP_ITERATE()

        /** \brief Create uninitialized socket variable

            This special constructor is called when passing
            ProtocolClientSocketHandle::Uninitialized as only argument to the constructor. This will
            create an in-\ref valid() socket handle which can however be assigned later with another
            socket instance.

            \implementation The socket handle will have no \c body allocated.
         */
        ProtocolClientSocketHandle(UninitializedType);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        Protocol const & protocol();    ///< Access the protocol interface
                                        /**< The returned protocol class reference gives access to
                                           the complete protocol interface as defined by that
                                           class. See the respective protocol class documentation.
                                           \returns \a Protocol class reference */

        static ProtocolClientSocketHandle cast_static(FileHandle handle);
        static ProtocolClientSocketHandle cast_dynamic(FileHandle handle);

        void state(SocketStateMap & map, unsigned lod=0);
        std::string dumpState(unsigned lod=0);

    protected:
        ProtocolClientSocketHandle(FileHandle other, bool isChecked);

    private:
        friend class ProtocolServerSocketHandle<Protocol>;
    };

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ProtocolClientSocketHandle.cci"
//#include "ProtocolClientSocketHandle.ct"
#include "ProtocolClientSocketHandle.cti"
#include "ProtocolClientSocketHandle.mpp"
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
