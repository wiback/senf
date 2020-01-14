//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
        console::ScopedDirectory<> dir_;

    public:
        typedef std::unique_ptr<QueueingAlgorithm> ptr;

        virtual ~QueueingAlgorithm() {};

        console::DirectoryNode & consoleDir();
        void pop();
        Packet const & front() const;
        bool enqueue(Packet const & packet, bool force = false);
        unsigned size() const;
        unsigned peek(unsigned maxSize = 0x7fffffff) const;
        void clear();
        bool empty() const;

    protected:
        QueueingAlgorithm();

        virtual Packet const & v_front() const = 0;
        virtual void v_pop() = 0;
        virtual bool v_enqueue(Packet const & packet, bool force) = 0;
        virtual unsigned v_size() const = 0;
        virtual unsigned v_peek(unsigned maxSize) const = 0;
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

        virtual Packet const & v_front() const;
        virtual unsigned v_peek(unsigned maxSize) const;
        virtual void v_pop();
        virtual bool v_enqueue(Packet const & packet, bool force);
        virtual unsigned v_size() const;
        virtual void v_clear();
        virtual bool v_empty() const;

    public:
        static QueueingAlgorithm::ptr create();
    };


    class NoneQueueingAlgorithm : public QueueingAlgorithm
    {
        virtual Packet const & v_front() const;
        virtual unsigned v_peek(unsigned maxSize) const;
        virtual void v_pop();
        virtual bool v_enqueue(Packet const & packet, bool force);
        virtual unsigned v_size() const;
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
