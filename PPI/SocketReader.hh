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
    \brief SocketReader public header */

#ifndef HH_SocketReader_
#define HH_SocketReader_ 1

// Custom includes
#include "Packets/Packets.hh"
#include "Socket/ClientSocketHandle.hh"
#include "Socket/SocketPolicy.hh"
#include "Socket/ReadWritePolicy.hh"
#include "Socket/FramingPolicy.hh"
#include "Module.hh"
#include "Connectors.hh"
#include "IOEvent.hh"

//#include "SocketReader.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Read helper for module::ActiveSocketReader

        This read helper will read a datagram from a datagram socket. This datagram will then be
        interpreted as a packet of type \a Packet as defined in the packet library. \a Packet
        defaults to \ref DataPacket, which will place the data uninterpreted into a packet data
        structure.
     */
    template <class Packet=DataPacket>
    class PacketReader
    {
    public:
        typedef senf::ClientSocketHandle<
            senf::MakeSocketPolicy< senf::ReadablePolicy,
                                    senf::DatagramFramingPolicy >::policy > Handle;
                                        ///< Handle type supported by this reader

        Packet operator()(Handle handle);
                                        ///< Read packet from \a handle
                                        /**< Read a datagram from \a handle and interpret is as
                                             packet of type \c Packet.
                                             \param[in] handle Handle to read data from
                                             \returns Pointer to new packet instance or 0, if no
                                                 packet could be read */
    };

}}

namespace senf {
namespace ppi {
namespace module {

    /** \brief Input module reading data from an arbitrary FileHandle

        This input module will read data from a FileHandle object and parse the data according to
        the \a Reader.
        
        The \a Reader must fulfill the following interface:
        \code
          class SomeReader
          {
          public:
              typedef unspecified_type Handle;        // type of handle requested
              SomeReader();                           // default constructible
              Packet::ptr operator()(Handle handle);  // extraction function
          };
        \endcode
     */
    template <class Reader=PacketReader<> >
    class ActiveSocketReader : public Module
    {
    public:
        typedef typename Reader::Handle Handle; ///< Handle type requested by the reader

        connector::ActiveOutput output; ///< Output connector to which the data received is written
        
        ActiveSocketReader(Handle handle); ///< Create new reader for the given handle
                                        /**< Data will be read from \a handle and be parsed by \a
                                             Reader.
                                             \param[in] handle Handle to read data from */

    private:
        void read();
        
        Handle handle_;
        IOEvent event_;
        Reader reader_;
    };

}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "SocketReader.cci"
#include "SocketReader.ct"
//#include "SocketReader.cti"
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
