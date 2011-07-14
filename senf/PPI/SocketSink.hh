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
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

        bool operator()(Handle & handle, Packet const & packet);
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

        bool operator()(Handle & handle, Packet const & packet); ///< Write \a packet to \a handle
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

        bool operator()(Handle & handle, Packet const & packet);
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

        bool operator()(Handle & handle, Packet const & packet);
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

              bool operator()(Handle handle, Packet packet);       // insertion function
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
        explicit ActiveSocketSink(Handle const & handle);
                                        ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \pre Requires \a Writer to be default constructible
                                             \param[in] handle Handle to write data to */
        ActiveSocketSink(Handle const & handle, Writer const & writer);
                                        ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \pre Requires \a Writer to be copy constructible
                                             \param[in] handle Handle to write data to
                                             \param[in] writer Writer helper writing packet date to
                                                 the socket */

        Writer & writer();              ///< Access the Writer
        Handle handle();                ///< Access handle
        void handle(Handle const & handle);
                                        ///< Set handle
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
        output module is passive. This implies, that <em>the output handle may not block</em>. This
        also implies, that data will probably get lost if written to fast for the underlying
        transport mechanism. Either this is desired (like for a UDP socket) or some additional
        bandwidth shaping needs to be used.

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

              bool operator()(Handle handle, Packet packet);       // insertion function
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
        explicit PassiveSocketSink(Handle const & handle);
                                        ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \pre Requires \a Writer to be default constructible
                                             \param[in] handle Handle to write data to */
        PassiveSocketSink(Handle const & handle, Writer const & writer);
                                        ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \pre Requires \a Writer to be copy constructible
                                             \param[in] handle Handle to write data to
                                             \param[in] writer Writer helper writing packet date to
                                                 the socket */

        Writer & writer();              ///< Access the Writer
        Handle & handle();              ///< Access handle
        void handle(Handle const & handle);
                                        ///< Set handle
                                        /**< Assigning an empty or in-valid() handle will disable
                                             the module until a new valid handle is assigned. */

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


//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
