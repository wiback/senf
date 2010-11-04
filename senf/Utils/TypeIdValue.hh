// $Id$
//
// Copyright (C) 2006
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
