// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <g0dil@berlios.de>
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
    \brief SocketWriter public header */

#ifndef HH_SocketWriter_
#define HH_SocketWriter_ 1

// Custom includes
#include "Packets/Packet.hh"
#include "Module.hh"
#include "Connector.hh"

//#include "SocketWriter.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Write helper for module::ActiveSocketWriter / module::PassiveSocketWriter
        
        This write helper will write the packets completely as datagrmas to the given socket.
     */
    class PacketWriter
    {
    public:
        typedef senf::ClientSocketHandle<
            senf::MakeSocketPolicy< senf::WriteablePolicy,
                                    senf::DatagramFramingPolicy > > Handle;
                                        ///< Handle type supported by this writer
        
        void operator()(Handle handle, Packet::ptr packet);
                                        ///< Write \a packet to \a handle
                                        /**< Write the complete \a packet as a datagram to \a
                                             handle.
                                             \param[in] handle Handle to write data to
                                             \param[in] packet Packet to write */
    };

}}

namespace senf {
namespace ppi {
namespace module {

    /** \brief Output module writing data to arbitrary FileHandle
        
        This output module will write data to a FileHandle object using a given \a Writer. This
        output module is active. This requires the file handle to be able to signal its readiness to
        accept more data via the Scheduler.

        The \a Writer must fulfill the following interface:
        \code
          class SomeWriter
          {
          public:
              typedef unspecified Handle;                          // type of handle requested
              SomeWriter();                                        // default constructible
              void operator()(Handle handle, Packet::ptr packet);  // insertion function
          };
        \endcode
     */
    template <class Writer=PacketWriter>
    class ActiveSocketWriter : public Module
    {
    public:
        typedef typename Writer:Handle Handle; ///< Handle type requested by writer

        connector::ActiveInput input; ///< Input connector from which data is received
        
        ActiveSocketWriter(Handle handle); ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \param[in] handle Handle to write data to */
    };

    /** \brief Output module writing data to arbitrary FileHandle
        
        This output module will write data to a FileHandle object using a given \a Writer. This
        output module is passive. This implies, that the output handle may not block. This also
        implies, that data will probably get lost if written to fast for the underlying transport
        mechanism. Either this is desired (like for a UDP socket) or some additional bandwidth
        shaping needs to be used.

        The \a Writer must fulfill the following interface:
        \code
          class SomeWriter
          {
          public:
              typedef unspecified Handle;                          // type of handle requested
              SomeWriter();                                        // default constructible
              void operator()(Handle handle, Packet::ptr packet);  // insertion function
          };
        \endcode
     */
    template <class Writer=PacketWriter>
    class PassiveSocketWriter : public Module
    {
    public:
        typedef typename Writer:Handle Handle; ///< Handle type requested by writer

        connector::PassiveInput input; ///< Input connector from which data is received
        
        ActiveSocketWriter(Handle handle); ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \param[in] handle Handle to write data to */
    };

}}}


///////////////////////////////hh.e////////////////////////////////////////
//#include "SocketWriter.cci"
//#include "SocketWriter.ct"
//#include "SocketWriter.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
