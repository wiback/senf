// $Id$
//
// Copyright (C) 2008 
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
    \brief ParseParameter public header */

#ifndef HH_ParseParameter_
#define HH_ParseParameter_ 1

// Custom includes
#include <iostream>
#include <boost/intrusive_ptr.hpp>
#include "../Utils/intrusive_refcount.hh"
#include "Parse.hh"
#include "Node.hh"

//#include "ParseParameter.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {
namespace detail {
    
    struct ParameterInfoBase
        : public intrusive_refcount
    {
        typedef boost::intrusive_ptr<ParameterInfoBase> ptr;
        
        std::string type;
        std::string name;
        bool hasDefault;
        
        ParameterInfoBase(std::string const & type);
    };
    
    template <class ParameterType>
    struct ParameterInfo 
        : public ParameterInfoBase
    {
        typedef boost::intrusive_ptr<ParameterInfo> ptr;

        static ptr create();
        ParameterInfo();

        ParameterType defaultValue;
    };
    
    template <class Type>
    struct ReturnValueTraits
    {
        typedef Type type;

        template <class Fn>
        static void callAndWrite(Fn const & fn, std::ostream & os);
    };

    template <>
    struct ReturnValueTraits<void>
    {
        typedef void type;

        template <class Fn>
        static void callAndWrite(Fn const & fn, std::ostream & os);
    };

    template <class Type>
    struct ParameterTraits
    {
        typedef Type type;
        static void parse(ParseCommandInfo::TokensRange const & tokens, Type & out);
        static std::string typeDescription();
    };
    
}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ParseParameter.cci"
//#include "ParseParameter.ct"
#include "ParseParameter.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
