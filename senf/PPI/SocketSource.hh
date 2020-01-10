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


    struct ReadTimestamp
    {
        ClockService::clock_type value;
    };

    std::ostream & operator<<(std::ostream & os, ReadTimestamp const & annotation);

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

        ActiveSocketSource(unsigned max_burst=1);
        explicit ActiveSocketSource(Reader reader, unsigned max_burst=1);
        explicit ActiveSocketSource(Handle const & handle, unsigned max_burst=1);
        ActiveSocketSource(Handle const & handle, Reader reader, unsigned max_burst=1);

        Reader & reader();              ///< Access Reader helper
        Handle handle();                ///< Access handle
        void handle(Handle const & handle);
                                        ///< Set handle
                                        /**< Assigning an empty or in-valid() handle will disable
                                             the module until a new, valid handle is assigned. */

        unsigned maxBurst() const;
        void maxBurst(unsigned max_burst);

        void flush();

        struct RxStats {
            RxStats () {
                clear();
            }
            void clear() {
                memset(this, 0, sizeof(*this));
            }
            void dump(std::ostream & os) const {
                os << "received "  << received << ", "
                   << "noop "       << noop << ".";
            }

            unsigned received;
            unsigned noop;
        };

        RxStats & rxStats() {
            return rxStats_;
        }
        
    private:
        Handle handle_;
        IOEvent event_;
        Reader reader_;
        unsigned maxBurst_;
        unsigned burst_;
        bool flush_pending_;
        RxStats rxStats_;
        void read();
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "SocketSource.cci"
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
