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

#ifndef HH_SocketSink_
#define HH_SocketSink_ 1

// Custom includes
#include "../Packets/Packets.hh"
#include "../Socket/ClientSocketHandle.hh"
#include "../Socket/SocketPolicy.hh"
#include "../Socket/ReadWritePolicy.hh"
#include "../Socket/FramingPolicy.hh"
#include "../Socket/CommunicationPolicy.hh"
#include "Module.hh"
#include "Connectors.hh"

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
        
        void operator()(Handle handle, Packet packet);
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

    /** \brief Output module writing data to a FileHandle using the provided Writer.
        If using the default ConnectedDgramWriter the filehandle must be writable, connected and 
        able to handle complete datagrams.  
        
        This output module will write data to a FileHandle object using a given \a Writer. This
        output module is active. This requires the file handle to be able to signal its readiness to
        accept more data via the Scheduler.

        The default \a Writer is senf::ppi::ConnectedDgramWriter which will write out the complete packet to
        the file handle.

        A \a Writer must fulfill the following interface:
        \code
          class SomeWriter
          {
          public:
              typedef unspecified Handle;                          // type of handle requested

              SomeWriter();                                          // EITHER default constructible OR
              SomeWriter(SomeWriter const & other);                    // copy constructible

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

        connector::ActiveInput<> input; ///< Input connector from which data is received
        
        ActiveSocketSink(Handle handle); ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \pre Requires \a Writer to be default constructible
                                             \param[in] handle Handle to write data to */
        ActiveSocketSink(Handle handle, Writer const & writer); 
                                        ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \pre Requires \a Writer to be copy constructible
                                             \param[in] handle Handle to write data to 
                                             \param[in] writer Writer helper writing packet date to the
                                                 socket */

        Writer & writer();                  ///< Access the Writer
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

        connector::PassiveInput<> input; ///< Input connector from which data is received
        
        PassiveSocketSink(Handle handle); ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \pre Requires \a Writer to be default constructible
                                             \param[in] handle Handle to write data to */
        PassiveSocketSink(Handle handle, Writer const & writer);
                                        ///< Create new writer for the given handle
                                        /**< Data will be written to \a handle using \a Writer.
                                             \pre Requires \a Writer to be copy constructible
                                             \param[in] handle Handle to write data to */

        Writer & writer();                  ///< Access the Writer
        Handle & handle(); 					/**< Access the handle. This is intendet to be mainly used to reconnect the under
        										lying socket. */
       /* void reconnect(senf::SocketAddress newAddress);
        								///< Reconnect the handle to which the packets are written
       */
        void replaceHandle(Handle newHandle);
                                        /**< Replace the handle to which the packets are written
                                         * Normally you should access the handle and call connect with
                                         * the new address. This also works for other 
                                         * (active) ConnectedSocketSinks/Sources */
                                        
    private:
        void write();

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
