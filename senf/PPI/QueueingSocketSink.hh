// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief QueueingSocketSink public header */

#ifndef HH_SENF_PPI_QueueingSocketSink_
#define HH_SENF_PPI_QueueingSocketSink_ 1

// Custom includes
#include <queue>
#include "SocketSink.hh"
#include <senf/Utils/Console/ScopedDirectory.hh>

//#include "QueueingSocketSink.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    class QueueingAlgorithm
        : private boost::noncopyable
    {
        console::ScopedDirectory<QueueingAlgorithm> dir_;

    public:
        typedef QueueingAlgorithm * ptr;

        virtual ~QueueingAlgorithm() {};

        console::DirectoryNode & consoleDir();
        Packet dequeue();
        bool enqueue(Packet const & packet);
        unsigned size();
        void clear();

    protected:
        QueueingAlgorithm();

        virtual Packet v_dequeue() = 0;
        virtual bool v_enqueue(Packet const & packet) = 0;
        virtual unsigned v_size() const = 0;
        virtual void v_clear() = 0;
    };


    namespace detail {
        struct QueueingAlgorithmRegistry_EntryBase
        {
            virtual QueueingAlgorithm::ptr create() const = 0;
        };

        template <class QAlgorithm>
        struct QueueingAlgorithmRegistry_Entry : QueueingAlgorithmRegistry_EntryBase
        {
            virtual QueueingAlgorithm::ptr create() const;
        };
    }

    class QueueingAlgorithmRegistry
        : public senf::singleton<QueueingAlgorithmRegistry>
    {
        typedef boost::ptr_map<std::string, detail::QueueingAlgorithmRegistry_EntryBase> QAlgoMap;
        QAlgoMap qAlgoMap_;

        QueueingAlgorithmRegistry() {};
    public:
        using senf::singleton<QueueingAlgorithmRegistry>::instance;
        friend class senf::singleton<QueueingAlgorithmRegistry>;

        struct Exception : public senf::Exception {
            Exception(std::string const & descr) : senf::Exception(descr) {}
        };

        template <class QAlgorithm>
        struct RegistrationProxy {
            RegistrationProxy(std::string const & key);
        };

        template <class QAlgorithm>
        void registerQAlgorithm(std::string key);

        QueueingAlgorithm::ptr createQAlgorithm(std::string const & key) const;
        void dump(std::ostream & os) const;
    };


#   define SENF_PPI_REGISTER_QALGORITHM( key, QAlgorithm )                          \
        namespace {                                                                 \
            senf::ppi::QueueingAlgorithmRegistry::RegistrationProxy<QAlgorithm>     \
                BOOST_PP_CAT(qAlgorithmRegistration_, __LINE__)( key);              \
        }


    class FIFOQueueingAlgorithm : public QueueingAlgorithm
    {
        std::queue<Packet> queue_;
        unsigned max_size_;

        FIFOQueueingAlgorithm();

        virtual Packet v_dequeue();
        virtual bool v_enqueue(Packet const & packet);
        virtual unsigned v_size() const;
        virtual void v_clear();

    public:
        static QueueingAlgorithm::ptr create();
    };


namespace module {

    /** \brief QueueingSocketSink

        \ingroup io_modules
     */
    template <class Writer=ConnectedDgramWriter>
    class PassiveQueueingSocketSink : public Module
    {
        SENF_PPI_MODULE(PassiveQueueingSocketSink);

    public:
        typedef typename Writer::Handle Handle; ///< Handle type requested by writer
        typedef typename Writer::PacketType PacketType;

        connector::PassiveInput<PacketType> input; ///< Input connector from which data is received
        console::ScopedDirectory<PassiveQueueingSocketSink<Writer> > dir;

        explicit PassiveQueueingSocketSink(Handle const & handle, QueueingAlgorithm::ptr qAlgorithm);

        Writer & writer();              ///< Access the Writer
        Handle & handle();              ///< Access handle
        void handle(Handle const & handle);
                                        ///< Set handle
                                        /**< Assigning an empty or in-valid() handle will disable
                                             the module until a new valid handle is assigned. */
        QueueingAlgorithm & qAlgorithm();
        void qAlgorithm(QueueingAlgorithm::ptr qAlgorithm);

    private:
        void write();
        void writable();
        void checkThrottle();
        void setQAlgorithm(std::string const & key);

        Handle handle_;
        Writer writer_;
        boost::scoped_ptr<QueueingAlgorithm> qAlgo_;
        IOEvent event_;
    };

}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "QueueingSocketSink.cci"
#include "QueueingSocketSink.ct"
#include "QueueingSocketSink.cti"
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
