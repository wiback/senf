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
