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
    \brief SocketSink public header */

#ifndef HH_SENF_PPI_SocketSink_
#define HH_SENF_PPI_SocketSink_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <senf/Socket/ClientSocketHandle.hh>
#include <senf/Socket/SocketPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include "Module.hh"
#include "Connectors.hh"
#include <senf/Socket/Protocols/INet/INetAddressing.hh>
#include "IOEvent.hh"

//#include "SocketSink.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Writer for module::ActiveSocketSink / module::PassiveSocketSink

        This writer will write the packets completely as datagrams to the given socket which must be connected.
     */
    class ConnectedDgramWriter
    {
    public:
        typedef senf::ClientSocketHandle<
            senf::MakeSocketPolicy< senf::WriteablePolicy,
                                    senf::DatagramFramingPolicy,
                                    senf::ConnectedCommunicationPolicy>::policy > Handle;
                                        ///< Handle type supported by this writer
        typedef Packet PacketType;

        void operator()(Handle handle, Packet const & packet);
                                        ///< Write \a packet to \a handle
                                        /**< Write the complete \a packet as a datagram to \a
                                             handle.
                                             \param[in] handle Handle to write data to
                                             \param[in] packet Packet to write */
    };

    /** \brief Writer sending data with ClientSocketHandle::writeto()

        This writer will send out data using ClientSocketHandle::writeto(). The target address can
        be specified in the writer constructor and can be adjusted at any time.

        If no target address is set, incoming data will be <em>silently dropped</em>.
     */
    template <class HandleType>
    class TargetDgramWriter
    {
    public:
        typedef HandleType Handle;
        typedef Packet PacketType;

        TargetDgramWriter();            ///< Create TargetDgramWriter with unset target address
        TargetDgramWriter(typename Handle::Address const & target);
                                        ///< Create TargetDgramWriter sending to \a target

        typename Handle::Address target() const; ///< Get current target address
        void target(typename Handle::Address const & target); ///< Set target address

        void operator()(Handle handle, Packet const & packet); ///< Write \a packet to \a handle
                                        /**< Write the complete \a packet as a datagram to \a
                                             handle.
                                             \param[in] handle Handle to write data to
                                             \param[in] packet Packet to write */

    private:
        typename Handle::Address target_;
    };

    class IPv4SourceForcingDgramWriter : ConnectedDgramWriter
    {
    public:
        IPv4SourceForcingDgramWriter();
        IPv4SourceForcingDgramWriter(senf::INet4Address sourceAddr, senf::INet4SocketAddress destAddr);
        typedef senf::ClientSocketHandle<
            senf::MakeSocketPolicy< senf::WriteablePolicy,
                                    senf::DatagramFramingPolicy>::policy > Handle;
                                        ///< Handle type supported by this writer
        typedef Packet PacketType;

        void source(senf::INet4Address & source);
        senf::INet4Address source();
        void destination(senf::INet4SocketAddress & dest);
        senf::INet4SocketAddress destination();

        void operator()(Handle handle, Packet const & packet);
                                        ///< Write \a packet to \a handle
                                        /**< Write the complete \a packet as a datagram to \a
                                             handle.
                                             \param[in] handle Handle to write data to
                                             \param[in] packet Packet to write */
    private:
        int sendtoandfrom(int sock, const void *data, size_t dataLen, const in_addr *dst, int port, const in_addr *src);
        senf::INet4Address source_;
        senf::INet4Address destination_;
        unsigned int protocolId_;
    };

    class IPv6SourceForcingDgramWriter : ConnectedDgramWriter
    {
    public:
        IPv6SourceForcingDgramWriter();
        IPv6SourceForcingDgramWriter(senf::INet6Address sourceAddr, senf::INet6SocketAddress destAddr);
        typedef senf::ClientSocketHandle<
            senf::MakeSocketPolicy< senf::WriteablePolicy,
                                    senf::DatagramFramingPolicy>::policy > Handle;
                                        ///< Handle type supported by this writer
        typedef Packet PacketType;

        void source(senf::INet6Address & source);
        senf::INet6Address source();
        void destination(senf::INet6SocketAddress & dest);
        senf::INet6SocketAddress destination();

        void operator()(Handle handle, Packet const & packet);
                                        ///< Write \a packet to \a handle
                                        /**< Write the complete \a packet as a datagram to \a
                                             handle.
                                             \param[in] handle Handle to write data to
                                             \param[in] packet Packet to write */
    private:
        int sendtoandfrom(int sock, const void *data, size_t dataLen, const in6_addr *dst, int port, const in6_addr *src);
        senf::INet6Address source_;
        senf::INet6Address destination_;
        unsigned int protocolId_;
};


}}

namespace senf {
namespace ppi {
namespace module {

    /** \brief Output %module writing data to a FileHandle using the provided Writer.
        If using the default ConnectedDgramWriter the filehandle must be writable, connected and
        able to handle complete datagrams.

        This output %module will write data to a FileHandle object using a given \a Writer. This
        output %module is active. This requires the file handle to be able to signal its readiness to
        accept more data via the Scheduler.

        The default \a Writer is senf::ppi::ConnectedDgramWriter which will write out the complete packet to
        the file handle.

        A \a Writer must fulfill the following interface:
        \code
          class SomeWriter
          {
          public:
              typedef unspecified Handle;                          // type of handle requested
              typedef unspecified_type PacketType                  // type of packet read

              SomeWriter();                                        // EITHER default constructible OR
              SomeWriter(SomeWriter const & other);                // copy constructible

              void operator()(Handle handle, Packet packet);       // insertion function
          };
        \endcode
        Whenever a packet is received for sending, the \a Writer's \c operator() is called.

        \ingroup io_modules
     */
    template <class Writer=ConnectedDgramWriter>
    class ActiveSocketSink : public Module
    {
        SENF_PPI_MODULE(ActiveSocketSink);

    public:
        typedef typename Writer::Handle Handle; ///< Handle type requested by writer

        connector::ActiveInput<typename Writer::PacketType> input; ///< Input connector from which data is received

        ActiveSocketSink();             ///< Create non-connected writer
                                        /**< The writer will be disabled until a socket is set
                                             \pre Requires \a Writer to be default constructible */
        explicit ActiveSocketSink(Writer const & writer);
                                        ///< Create non-connected writer
                                        /**< The writer will be disabled until a socket is set
                                             \pre Requires \a Writer to be copy constructible
                                             \param[in] writer Writer helper writing packet date to
                                                 the socket */
        explicit ActiveSocketSink(Handle handle); ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \pre Requires \a Writer to be default constructible
                                             \param[in] handle Handle to write data to */
        ActiveSocketSink(Handle handle, Writer const & writer);
                                        ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \pre Requires \a Writer to be copy constructible
                                             \param[in] handle Handle to write data to
                                             \param[in] writer Writer helper writing packet date to
                                                 the socket */

        Writer & writer();              ///< Access the Writer
        Handle handle();                ///< Access handle
        void handle(Handle handle);     ///< Set handle
                                        /**< Assigning an empty or in-valid() handle will disable
                                             the module until a new. valid handle is assigned. */

    private:
        void write();

        Handle handle_;
        IOEvent event_;
        Writer writer_;
    };

    /** \brief Output module writing data to a FileHandle using the provided \a Writer.
        If using the default ConnectedDgramWriter the filehandle must be writable, connected and
        able to handle complete datagrams.

        This output module will write data to a FileHandle object using a given \a Writer. This
        output module is passive. This implies, that the output handle may not block. This also
        implies, that data will probably get lost if written to fast for the underlying transport
        mechanism. Either this is desired (like for a UDP socket) or some additional bandwidth
        shaping needs to be used.

        The default \a Writer is senf::ppi::ConnectedDgramWriter which will write out the complete packet to
        the file handle.

        The \a Writer must fulfill the following interface:
        \code
          class SomeWriter
          {
          public:
              typedef unspecified Handle;                          // type of handle requested
              typedef unspecified_type PacketType                  // type of packet read

              SomeWriter();                                          // EITHER default constructible
              SomeWriter(SomeWriter const & other);                    // OR copy constructible

              void operator()(Handle handle, Packet packet);       // insertion function
          };
        \endcode
        Whenever a packet is received for sending, the \a Writer's \c operator() is called.

        \ingroup io_modules
     */
    template <class Writer=ConnectedDgramWriter>
    class PassiveSocketSink : public Module
    {
        SENF_PPI_MODULE(PassiveSocketSink);

    public:
        typedef typename Writer::Handle Handle; ///< Handle type requested by writer

        connector::PassiveInput<typename Writer::PacketType> input; ///< Input connector from which data is received

        PassiveSocketSink();            ///< Create non-connected writer
                                        /**< The writer will be disabled until a socket is set
                                             \pre Requires \a Writer to be default constructible */
        explicit PassiveSocketSink(Writer const & writer);
                                        ///< Create non-connected writer
                                        /**< The writer will be disabled until a socket is set
                                             \pre Requires \a Writer to be copy constructible
                                             \param[in] writer Writer helper writing packet date to
                                                 the socket */
        explicit PassiveSocketSink(Handle handle); ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \pre Requires \a Writer to be default constructible
                                             \param[in] handle Handle to write data to */
        PassiveSocketSink(Handle handle, Writer const & writer);
                                        ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \pre Requires \a Writer to be copy constructible
                                             \param[in] handle Handle to write data to
                                             \param[in] writer Writer helper writing packet date to
                                                 the socket */

        Writer & writer();              ///< Access the Writer
        Handle & handle();              ///< Access handle
        void handle(Handle handle);     ///< Set handle
                                        /**< Assigning an empty or in-valid() handle will disable
                                             the module until a new. valid handle is assigned. */

#ifndef DOXYGEN
        void replaceHandle(Handle newHandle);
#endif

    private:
        void write();
        void checkThrottle();

        Handle handle_;
        Writer writer_;
    };

}}}


///////////////////////////////hh.e////////////////////////////////////////
#include "SocketSink.cci"
#include "SocketSink.ct"
#include "SocketSink.cti"
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
