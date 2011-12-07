// $Id$
//
// Copyright (C) 2008
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
    \brief OverloadedCommand non-inline non-template implementation */

#include "OverloadedCommand.hh"
//#include "OverloadedCommand.ih"

// Custom includes

//#include "OverloadedCommand.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::OverloadedCommandNode

prefix_ senf::console::OverloadedCommandNode &
senf::console::OverloadedCommandNode::insertOverload(DirectoryNode & dir,
                                                     std::string const & name,
                                                     CommandOverload::ptr overload)
{
    OverloadedCommandNode & node (
        dir.hasChild(name)
        ? dynamic_cast<OverloadedCommandNode &>(dir(name))
        : dir.add(name, OverloadedCommandNode::create()));
    node.add(overload);
    return node;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// So soll die doku aussehen:
//
// Usage:
//      1- foo arg1:int arg2:double
//      2- foo arg3:string
//      3- foo
//
// With:
//      arg1 -   arg1-doc
//      arg2 -   arg2-doc
//          default: 1.23
//      arg3 -   arg3-doc
//
// Generic documentation foo blalsdljfl laj flkajslkjs fdlkj oiwlksdj ;llkaj
// sdflkja sldkfjslkdfj sdlkfj lskjdf lskjdf lksj dflkj lsdkfj lskdjf lskjkd
// Generic documentation foo blalsdljfl laj flkajslkjs fdlkj oiwlksdj ;llkaj
// sdflkja sldkfjslkdfj sdlkfj lskjdf lskjdf lksj dflkj lsdkfj lskdjf lskjkd
// Generic documentation foo blalsdljfl laj flkajslkjs fdlkj oiwlksdj ;llkaj
// sdflkja sldkfjslkdfj sdlkfj lskjdf lskjdf lksj dflkj lsdkfj lskdjf lskjkd
//
// Variant 1:
// Variant 1 doc la;ksjf lkj sdlkfj lkjekj sdflkj ekljsdlkfj wlej
// slkj dkj sldkfj lwekljsdf skldjf lskjdf l jsd
//
// Variant 2:
// Variant 2 doc lskdfj lwkej lksjdflksjf
//
// Variatn 3:
// Variant 3 doc slkjflw ekj lskdfj lskdjf laksdj flksj elkj aldskjf lwkejlksdj
// ldkfaj wlekj slkdfj lskdjf lwkejlkasdjf

prefix_ void senf::console::OverloadedCommandNode::v_help(std::ostream & os)
    const
{
    typedef std::vector<ArgumentDoc> ArgumentDocs;
    ArgumentDocs argumentDocs;
    bool haveDocumentedArg (false);

    os << "Usage:\n";
    {
        Overloads::const_iterator i (overloads_.begin());
        Overloads::const_iterator const i_end (overloads_.end());
        unsigned index (1);
        for (; i != i_end; ++i, ++index) {
            os << "    ";
            if (overloads_.size() > 1) os << index << "- ";
            os << name();
            for (unsigned j (0); j < (*i)->numArguments(); ++j) {
                ArgumentDoc arg;
                (*i)->argumentDoc(j, arg);

                os << ' ';
                if (! arg.defaultValue.empty())
                    os << '[';
                if (! arg.name.empty()) os << arg.name;
                if (! arg.type.empty()) os << ':' << arg.type;
                if (arg.name.empty() && arg.type.empty()) os << "...";
                if (! arg.defaultValue.empty())
                    os << ']';

                if (! arg.name.empty() || ! arg.defaultValue.empty()) {
                    ArgumentDocs::iterator k (argumentDocs.begin());
                    ArgumentDocs::iterator const k_end (argumentDocs.end());
                    for (; k != k_end; ++k)
                        if (k->name == arg.name && k->defaultValue == arg.defaultValue) {
                            if (! arg.doc.empty() && k->doc.empty()) {
                                k->doc = arg.doc;
                                haveDocumentedArg = true;
                            }
                            break;
                        }
                    if (k == k_end) {
                        argumentDocs.push_back(arg);
                        if (! arg.doc.empty())
                            haveDocumentedArg = true;
                    }
                }
            }
            os << '\n';
        }
    }

    if (haveDocumentedArg) {
        os << "\n" "With:\n";
        ArgumentDocs::const_iterator i (argumentDocs.begin());
        ArgumentDocs::const_iterator const i_end (argumentDocs.end());
        for (; i != i_end; ++i) {
            if (! i->doc.empty() || ! i->defaultValue.empty()) {
                os << "    "
                   << i->name
                   << std::string(i->name.length()<8 ? 8-i->name.length() : 0, ' ')
                   << "  "
                   << i->doc
                   << '\n';
                if (! i->defaultValue.empty())
                    os << "        default: " << i->defaultValue << '\n';
            }
        }
    }

    if (! doc_.empty())
        os << "\n" << doc_ << "\n";

    {
        Overloads::const_iterator i (overloads_.begin());
        Overloads::const_iterator const i_end (overloads_.end());
        unsigned index (1);
        for (; i != i_end; ++i, ++index) {
            std::string overloadDoc ((*i)->doc());
            if (! overloadDoc.empty()) {
                os << "\n";
                if (overloads_.size() > 1)
                    os << "Variant " << index << ":\n";
                os << overloadDoc << "\n";
            }
        }
    }
}

prefix_ std::string senf::console::OverloadedCommandNode::v_shorthelp()
    const
{
    if (!shortdoc_.empty())
        return shortdoc_;
    if (!doc_.empty())
        return doc_.substr(0,doc_.find('\n'));
    Overloads::const_iterator i (overloads_.begin());
    Overloads::const_iterator const i_end (overloads_.end());
    for (; i != i_end; ++i) {
        std::string overloadDoc ((*i)->doc());
        if (! overloadDoc.empty())
            return overloadDoc.substr(0,overloadDoc.find('\n'));
    }
    return "";
}

prefix_ void senf::console::OverloadedCommandNode::v_execute(boost::any & rv,
                                                             std::ostream & os,
                                                             ParseCommandInfo const & command)
    const
{
    Overloads::const_iterator i (overloads_.begin());
    Overloads::const_iterator const i_end (overloads_.end());
    SyntaxErrorException err;
    for (; i != i_end; ++i) {
        try {
            (**i)(rv, os, command);
            return;
        }
        catch (SyntaxErrorException & ex) {
            err = ex;
        };
    }
    throw err;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::SimpleCommandOverload

prefix_ unsigned senf::console::SimpleCommandOverload::v_numArguments()
    const
{
    return 1;
}

prefix_ void senf::console::SimpleCommandOverload::v_argumentDoc(unsigned index, ArgumentDoc & doc)
    const
{}

prefix_ std::string senf::console::SimpleCommandOverload::v_doc()
    const
{
    return doc_;
}

prefix_ void senf::console::SimpleCommandOverload::v_execute(boost::any & rv,
                                                             std::ostream & os,
                                                             ParseCommandInfo const & command)
    const
{
    fn_(os, command);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "OverloadedCommand.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
