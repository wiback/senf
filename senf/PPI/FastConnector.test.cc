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
    \brief FastConnector.test unit tests */

#include "FastConnector.hh"

// Custom includes
#include <deque>
#include "Module.hh"
#include "Setup.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace ppi = senf::ppi;
namespace module = senf::ppi::module;
namespace connector = senf::ppi::connector;

namespace {

    template <class PacketType=senf::Packet>
    class FastActiveSource : public module::Module
    {
        SENF_PPI_MODULE(FastActiveSource);

    public:
        connector::FastActiveOutput<PacketType> output;

        FastActiveSource()
            {
                noroute(output);
            }

        void submit(PacketType const & packet)
            {
                output(packet);
            }
    };

    template <class PacketType=senf::Packet>
    class FastPassiveSink : public module::Module
    {
        SENF_PPI_MODULE(FastPassiveSink);
        typedef std::deque<PacketType> Queue;

    public:
        typedef typename Queue::iterator iterator;
        typedef typename Queue::size_type size_type;

        connector::FastPassiveInput<PacketType> input;

        FastPassiveSink()
            {
                noroute(input);
                input.onRequest(&FastPassiveSink::request);
            }

        bool empty() { return packets_.empty(); }
        size_type size() { return packets_.size(); }
        iterator begin() { return packets_.begin(); }
        iterator end() { return packets_.end(); }

        PacketType front() { if (empty()) return PacketType(); else return packets_.front(); }
        PacketType pop_front()
            {
                PacketType p (front());
                if (p) packets_.pop_front();
                return p;
            }

    private:
        void request()
            {
                packets_.push_back(input());
            }

        Queue packets_;
    };
}

SENF_AUTO_TEST_CASE(fastConnector)
{
    FastActiveSource<> source;
    FastPassiveSink<> target;

    ppi::connect(source, target);
    ppi::init();

    BOOST_CHECK_EQUAL( & source.output.module(), & source );
    BOOST_CHECK_EQUAL( & target.input.module(), & target );
    BOOST_CHECK_EQUAL( & source.output.peer(), & target.input );
    BOOST_CHECK_EQUAL( & target.input.peer(), & source.output );

    senf::PacketData::byte data[] = { 0x13u, 0x24u, 0x35u };
    senf::Packet p (senf::DataPacket::create(data));

    source.submit(p);

    BOOST_CHECK_EQUAL( target.size(), 1u );
    BOOST_CHECK( target.front() == p );
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
