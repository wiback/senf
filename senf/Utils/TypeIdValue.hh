// $Id$
//
// Copyright (C) 2006
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
    \brief TypeIdValue public header */

#ifndef HH_SENF_Utils_TypeIdValue_
#define HH_SENF_Utils_TypeIdValue_ 1

// Custom includes
#include <typeinfo>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/operators.hpp>

//#include "TypeIdValue.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {


    /** \brief Wrapper to use types as key's in a map
      */
    class TypeIdValue : public boost::totally_ordered<TypeIdValue>
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        // my default constructor
        // my copy constructor
        // my copy assignment
        // default destructor
        // no conversion constructors

        TypeIdValue();
        TypeIdValue(std::type_info const & v);

        //\}
        //-////////////////////////////////////////////////////////////////////////

        bool operator==(TypeIdValue const & other) const;
        bool operator<(TypeIdValue const & other) const;

        std::string name() const;
        std::string prettyName() const;

        std::type_info const & id() const;

    protected:

    private:
        std::type_info const * p_;
    };

    /**
        \related TypeIdValue
     */
    TypeIdValue const typeIdValue();

    /**
        \related TypeIdValue
     */
    template <class Type>
    TypeIdValue const typeIdValue();

    /**
        \related TypeIdValue
     */
    template <class Type>
    TypeIdValue const typeIdValue(Type const & ob);

    /**
        \related TypeIdValue
     */
    std::ostream & operator<<(std::ostream & os, TypeIdValue const & v);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "TypeIdValue.cci"
//#include "TypeIdValue.ct"
#include "TypeIdValue.cti"
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
