// $Id$
//
// Copyright (C) 2013
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
    \brief FastConnector.test unit tests */

//#include "FastConnector.test.hh"
//#include "FastConnector.test.ih"

// Custom includes
#include "FastConnector.hh"

#include <deque>
#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>
#include "Module.hh"
#include "Setup.hh"

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

SENF_AUTO_UNIT_TEST(fastConnector)
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
