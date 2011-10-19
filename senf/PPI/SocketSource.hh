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
//   Stefan Bund <g0dil@berlios.de>

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
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

        Packet operator()(Handle & handle);
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
        explicit ActiveSocketSource(Handle const & handle);
                                        ///< Create new reader for the given handle
                                        /**< Data will be read from \a handle and be parsed by \a
                                             Reader.
                                             \pre Requires \a Reader to be default constructible
                                             \param[in] handle Handle to read data from */
        ActiveSocketSource(Handle const & handle, Reader reader);
                                        ///< Create new reader for the given handle
                                        /**< Data will be read from \a handle and be parsed by \a
                                             Reader.
                                             \pre Requires \a Reader to be copy constructible
                                             \param[in] handle Handle to read data from */

        Reader & reader();              ///< Access Reader helper
        Handle handle();                ///< Access handle
        void handle(Handle const & handle);
                                        ///< Set handle
                                        /**< Assigning an empty or in-valid() handle will disable
                                             the module until a new, valid handle is assigned. */

    private:
        Handle handle_;
        IOEvent event_;
        Reader reader_;

        void read();
    };


    template <class Reader=DgramReader<> >
    class ActiveBurstSocketSource
        : public Module
    {
        SENF_PPI_MODULE(ActiveBurstSocketSource);

    public:
        typedef typename Reader::Handle Handle; ///< Handle type requested by the reader

        connector::ActiveOutput<typename Reader::PacketType> output;
                                        ///< Output connector to which the data received is written

        ActiveBurstSocketSource(unsigned max_burst=0);
        explicit ActiveBurstSocketSource(Reader reader, unsigned max_burst=0);
        explicit ActiveBurstSocketSource(Handle const & handle, unsigned max_burst=0);
        ActiveBurstSocketSource(Handle const & handle, Reader reader, unsigned max_burst=0);

        Reader & reader();              ///< Access Reader helper
        Handle handle();                ///< Access handle
        void handle(Handle const & handle);
                                        ///< Set handle
                                        /**< Assigning an empty or in-valid() handle will disable
                                             the module until a new, valid handle is assigned. */

        unsigned maxBurst() const;
        void maxBurst(unsigned max_burst);

        void flush();

    private:
        Handle handle_;
        IOEvent event_;
        Reader reader_;
        unsigned maxBurst_;

        void read();
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
