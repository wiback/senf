// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief SocketSource public header */

#ifndef HH_SENF_PPI_SocketSource_
#define HH_SENF_PPI_SocketSource_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <senf/Socket/ClientSocketHandle.hh>
#include <senf/Socket/SocketPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/FramingPolicy.hh>
#include "Module.hh"
#include "Connectors.hh"
#include "IOEvent.hh"

//#include "SocketSource.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Reader for module::ActiveSocketSource

        This read helper will read a datagram from a datagram socket. This datagram will then be
        interpreted as a packet of type \a Packet as defined in the packet library. \a Packet
        defaults to DataPacket (type DataPacketType), which will place the data uninterpreted
        into a packet data structure.
     */
    template <class Packet=DataPacket, unsigned MaxSize=0u>
    class DgramReader
    {
    public:
        typedef Packet PacketType;
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
        the \a Reader. The default reader is senf::ppi::DgramReader <> which reads the data into a
        senf::DataPacket. To parse the data according to some other packet type, pass that packet
        type to senf::ppi::DgramReader:
        \code
        senf::ppi::module::ActiveSocketSource< senf::ppi::DgramReader<senf::EthernetPacket> > source;
        \endcode
        declares a \a reader module reading senf::EthernetPacket's

        A \a Reader must fulfill the following interface:
        \code
        class SomeReader
        {
        public:
            typedef unspecified_type Handle;                       // type of handle requested
            typedef unspecified_type PacketType                    // type of packet returned

            SomeReader();                                          // EITHER default constructible
            SomeReader(SomeReader const & other);                  // OR copy constructible

            PacketType operator()(Handle handle);                  // extraction function
        };
        \endcode
        Whenever the FileHandle object is ready for reading, the \a Reader's \c operator() is called
        to read a packet.

        \ingroup io_modules
     */
    template <class Reader=DgramReader<> >
    class ActiveSocketSource
        : public Module
    {
        SENF_PPI_MODULE(ActiveSocketSource);

    public:
        typedef typename Reader::Handle Handle; ///< Handle type requested by the reader

        connector::ActiveOutput<typename Reader::PacketType> output;
                                        ///< Output connector to which the data received is written

        ActiveSocketSource();           ///< Create non-connected reader
                                        /**< The reader will be disabled until a socket is set
                                             \pre Requires \a Reader to be default constructible */
        explicit ActiveSocketSource(Reader reader); ///< Create non-connected reader
                                        /**< The reader will be disabled until a socket is set
                                             \pre Requires \a Reader to be copy constructible */
        explicit ActiveSocketSource(Handle handle); ///< Create new reader for the given handle
                                        /**< Data will be read from \a handle and be parsed by \a
                                             Reader.
                                             \pre Requires \a Reader to be default constructible
                                             \param[in] handle Handle to read data from */
        ActiveSocketSource(Handle handle, Reader reader);
                                        ///< Create new reader for the given handle
                                        /**< Data will be read from \a handle and be parsed by \a
                                             Reader.
                                             \pre Requires \a Reader to be copy constructible
                                             \param[in] handle Handle to read data from */

        Reader & reader();              ///< Access Reader helper
        Handle handle();                ///< Access handle
        void handle(Handle handle);     ///< Set handle
                                        /**< Assigning an empty or in-valid() handle will disable
                                             the module until a new, valid handle is assigned. */

    private:
        void read();

        Handle handle_;
        IOEvent event_;
        Reader reader_;
    };

}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "SocketSource.cci"
#include "SocketSource.ct"
#include "SocketSource.cti"
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
