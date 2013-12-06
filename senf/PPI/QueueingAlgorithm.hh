// $Id$
//
// Copyright (C) 2010
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief QueueingAlgorithm public header */

#ifndef HH_SENF_PPI_QueueingAlgorithm_
#define HH_SENF_PPI_QueueingAlgorithm_ 1

// Custom includes
#include <queue>
#include <senf/Packets/Packets.hh>
#include <senf/Utils/Console/ScopedDirectory.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

    class QueueingAlgorithm
        : private boost::noncopyable
    {
        console::ScopedDirectory<QueueingAlgorithm> dir_;

    public:
        typedef SENF_SMART_PTR<QueueingAlgorithm> ptr;

        virtual ~QueueingAlgorithm() {};

        console::DirectoryNode & consoleDir();
        Packet dequeue();
        bool enqueue(Packet const & packet);
        unsigned size() const;
        unsigned frontPacketSize() const;
        void clear();
        bool empty() const;

    protected:
        QueueingAlgorithm();

        virtual Packet v_dequeue() = 0;
        virtual bool v_enqueue(Packet const & packet) = 0;
        virtual unsigned v_size() const = 0;
        virtual unsigned v_frontPacketSize() const = 0;
        virtual void v_clear() = 0;
        virtual bool v_empty() const = 0;
    };


    namespace detail {
        struct QueueingAlgorithmRegistry_EntryBase
        {
            virtual ~QueueingAlgorithmRegistry_EntryBase() {}
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
        virtual unsigned v_frontPacketSize() const;
        virtual void v_clear();
        virtual bool v_empty() const;

    public:
        static QueueingAlgorithm::ptr create();
    };


    class NoneQueueingAlgorithm : public QueueingAlgorithm
    {
        virtual Packet v_dequeue();
        virtual bool v_enqueue(Packet const & packet);
        virtual unsigned v_size() const;
        virtual unsigned v_frontPacketSize() const;
        virtual void v_clear();
        virtual bool v_empty() const;

    public:
        static QueueingAlgorithm::ptr create();
    };


}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "QueueingAlgorithm.cci"
//#include "QueueingAlgorithm.ct"
#include "QueueingAlgorithm.cti"
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
