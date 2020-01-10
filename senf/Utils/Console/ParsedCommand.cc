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
    \brief ParsedCommand non-inline non-template implementation */

#include "ParsedCommand.hh"
#include "ParsedCommand.ih"

// Custom includes

#include "ParsedCommand.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::detail::ArgumentInfoBase

prefix_ senf::console::detail::ArgumentInfoBase::~ArgumentInfoBase()
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::ParsedCommandOverloadBase

prefix_ unsigned senf::console::ParsedCommandOverloadBase::v_numArguments()
    const
{
    return parameters_.size();
}

prefix_ void senf::console::ParsedCommandOverloadBase::v_argumentDoc(unsigned index,
                                                                     ArgumentDoc & doc)
    const
{
    BOOST_ASSERT( index < parameters_.size() );
    detail::ArgumentInfoBase & arg (*parameters_[index]);
    doc.name = arg.name.empty()
        ? (boost::format("arg%d%d") % overloadIndex() % (index+1)).str()
        : arg.name;
    doc.type = arg.type;
    if (arg.hasDefault) {
        doc.defaultValue = arg.defaultDoc.empty()
            ? arg.defaultValueStr()
            : arg.defaultDoc;
        if (doc.defaultValue.empty())
            doc.defaultValue = "(empty)";
    }
    doc.doc = arg.doc;
    doc.singleToken = arg.singleToken;
}

prefix_ std::string senf::console::ParsedCommandOverloadBase::v_doc()
    const
{
    return doc_;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
#include "ParsedCommand.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
