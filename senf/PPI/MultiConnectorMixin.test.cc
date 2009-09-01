// $Id$
//
// Copyright (C) 2009 
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
    \brief MultiConnectorMixin.test unit tests */

//#include "MultiConnectorMixin.test.hh"
//#include "MultiConnectorMixin.test.ih"

// Custom includes
#include "PPI.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace debug = module::debug;

namespace {
    // We only test the user-collection case, all other cases are already handled by
    // existing modules

    // Primitive duplicator
    class MyModule
        : public module::Module,
          public module::MultiConnectorMixin<MyModule, connector::ActiveOutput<>, void, void>
    {
        SENF_PPI_MODULE(MyModule);
    public:
        connector::PassiveInput<> input;

        MyModule()
        {
            noroute(input); 
            input.onRequest(&MyModule::request);
        }

    private:
        void connectorSetup(std::auto_ptr<ConnectorType> c)
        {
            route(input, *c);
            connectors_.push_back(boost::shared_ptr<ConnectorType>(c));
        }

        void request()
        {
            senf::Packet p (input());
            for (Connectors::iterator i (connectors_.begin()), i_end (connectors_.end());
                    i != i_end; ++i)
                (**i)(p);
        }

        typedef std::vector< boost::shared_ptr<MyModule::ConnectorType> > Connectors;
        Connectors connectors_;
                
        friend class module::MultiConnectorMixin<MyModule, connector::ActiveOutput<>, void, void>;
    };
        
    struct IntAnnotation {
        int value;
        bool operator<(IntAnnotation const & other) const { return value < other.value; }
        IntAnnotation() {}
        IntAnnotation(int v) : value(v) {}
    };

    std::ostream & operator<<(std::ostream & os, IntAnnotation const & value)
    { os << value.value; return os; }
}

BOOST_AUTO_UNIT_TEST(multiConnectorMixin_userContainer)
{
    debug::ActiveSource source;
    MyModule module;
    debug::PassiveSink sink1;
    debug::PassiveSink sink2;

    ppi::connect(source, module);
    ppi::connect(module, sink1);
    ppi::connect(module, sink2);
    ppi::init();

    senf::Packet p (senf::DataPacket::create());

    source.submit(p);
    BOOST_CHECK_EQUAL( sink1.size(), 1u );
    BOOST_CHECK_EQUAL( sink2.size(), 1u );
    BOOST_CHECK( sink1.pop_front() == p );
    BOOST_CHECK( sink2.pop_front() == p );
}

BOOST_AUTO_UNIT_TEST(multiConnectorMixin_multipleModules)
{
    debug::ActiveSource source;
    debug::PassiveSink sink;
    module::PassiveJoin join1;
    module::PassiveJoin join2;
    module::AnnotationRouter<IntAnnotation> router;
    MyModule module;
    
    ppi::connect(source, join1);
    ppi::connect(join1, router);
    ppi::connect(router, join2, 1);
    ppi::connect(join2, module);
    ppi::connect(module, sink);
    
    senf::Packet p (senf::DataPacket::create());
    p.annotation<IntAnnotation>().value = 1;

    source.submit(p);
    BOOST_CHECK_EQUAL( sink.size(), 1u );
    BOOST_CHECK( sink.pop_front() == p );
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
