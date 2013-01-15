// $Id$
//
// Copyright (C) 2009
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
    \brief MultiConnectorMixin.test unit tests */

//#include "MultiConnectorMixin.test.hh"
//#include "MultiConnectorMixin.test.ih"

// Custom includes
#include "MultiConnectorMixin.hh"
#include "DebugModules.hh"
#include "Joins.hh"
#include "AnnotationRouter.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace debug = module::debug;

namespace {

    // Primitive join
    class SequenceContainerModule
        : public module::Module,
          public module::MultiConnectorMixin<SequenceContainerModule, connector::PassiveInput<> >
    {
        SENF_PPI_MODULE(SequenceContainerModule);
        typedef module::MultiConnectorMixin<SequenceContainerModule, connector::PassiveInput<> > base;

    public:
        connector::ActiveOutput<> output;

        using base::connectors;

        SequenceContainerModule() : count (0u)
        {
            noroute(output);
        }

        unsigned count;

    private:
        void connectorSetup(connector::PassiveInput<> & c)
        {
            route(c, output);
            c.onRequest(boost::bind(&SequenceContainerModule::request, this, boost::ref(c)));
            ++ count;
        }

        void connectorDestroy(connector::PassiveInput<> const & c)
        {
            -- count;
        }

        void request(connector::PassiveInput<> & c)
        {
            output(c());
        }

        friend class module::MultiConnectorMixin<SequenceContainerModule, connector::PassiveInput<> >;
    };

    // Primitive duplicator
    class UserContainerModule
        : public module::Module,
          public module::MultiConnectorMixin<UserContainerModule, connector::ActiveOutput<>, void, void>
    {
        SENF_PPI_MODULE(UserContainerModule);
        typedef std::vector< boost::shared_ptr<UserContainerModule::ConnectorType> > Connectors;
    public:
        connector::PassiveInput<> input;

        UserContainerModule()
        {
            noroute(input);
            input.onRequest(&UserContainerModule::request);
        }

        Connectors const & connectors() const
        { return connectors_; }

    private:
        void connectorSetup(std::auto_ptr<ConnectorType> c)
        {
            route(input, *c);
            connectors_.push_back(boost::shared_ptr<ConnectorType>(c));
        }

        void connectorDestroy(ConnectorType const * c)
        {
            Connectors::iterator i (
                std::find_if(connectors_.begin(), connectors_.end(),
                             boost::bind(&Connectors::value_type::get,_1) == c));
            if (i != connectors_.end())
                connectors_.erase(i);
        }

        void request()
        {
            senf::Packet p (input());
            for (Connectors::iterator i (connectors_.begin()), i_end (connectors_.end());
                    i != i_end; ++i)
                (**i)(p);
        }

        Connectors connectors_;

        friend class module::MultiConnectorMixin<UserContainerModule, connector::ActiveOutput<>, void, void>;
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

SENF_AUTO_UNIT_TEST(multiConnectorMixin_sequenceContainer)
{
    debug::ActiveSource source1;
    debug::ActiveSource source2;
    SequenceContainerModule module;
    debug::PassiveSink sink;

    ppi::connect(source1, module);
    ppi::connect(source2, module);
    ppi::connect(module, sink);
    ppi::init();

    senf::Packet p (senf::DataPacket::create());

    source1.submit(p);
    BOOST_CHECK_EQUAL( sink.size(), 1u );
    BOOST_CHECK( sink.pop_front() == p );
    source2.submit(p);
    BOOST_CHECK_EQUAL( sink.size(), 1u );
    BOOST_CHECK( sink.pop_front() == p );

    BOOST_CHECK_EQUAL( module.connectors().size(), 2u );
    BOOST_CHECK_EQUAL( module.count, 2u );
    source1.output.disconnect();
    BOOST_CHECK_EQUAL( module.connectors().size(), 1u );
    BOOST_CHECK_EQUAL( module.count, 1u );
}

SENF_AUTO_UNIT_TEST(multiConnectorMixin_userContainer)
{
    debug::ActiveSource source;
    UserContainerModule module;
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

    BOOST_CHECK_EQUAL( module.connectors().size(), 2u );
    sink1.input.disconnect();
    BOOST_CHECK_EQUAL( module.connectors().size(), 1u );
}

SENF_AUTO_UNIT_TEST(multiConnectorMixin_multipleModules)
{
    debug::ActiveSource source;
    debug::PassiveSink sink;
    module::PassiveJoin<> join1;
    module::PassiveJoin<> join2;
    module::AnnotationRouter<IntAnnotation> router;
    UserContainerModule module;

    ppi::connect(source, join1);
    ppi::connect(join1, router);
    ppi::connect(router, 1, join2);
    ppi::connect(join2, module);
    ppi::connect(module, sink);

    senf::Packet p (senf::DataPacket::create());
    p.annotation<IntAnnotation>().value = 1;

    source.submit(p);
    BOOST_CHECK_EQUAL( sink.size(), 1u );
    BOOST_CHECK( sink.pop_front() == p );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
