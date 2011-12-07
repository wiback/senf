// $Id$
//
// Copyright (C) 2010
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
//   Philipp Batroff <philipp.batroff@fokus.fraunhofer.de>

/** \file
    \brief RestrictedInt public header */

#ifndef HH_SENF_senf_Utils_RestrictedInt_
#define HH_SENF_senf_Utils_RestrictedInt_ 1

// Custom includes
#include <iostream>
#include <boost/operators.hpp>
#include <senf/Utils/safe_bool.hh>

//#include "RestrictedInt.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    // Really ... the correct spelling is 'euclidean' as in the current boost
    // Versions but older versions only have this spelling so we can' t use
    // the correct class name ...

    template <class Base, class Tag>
    class RestrictedInt
        : public boost::ordered_euclidian_ring_operators< RestrictedInt<Base,Tag>,
                 boost::unit_steppable< RestrictedInt<Base,Tag>,
                 boost::shiftable< RestrictedInt<Base,Tag>,
                 boost::bitwise1< RestrictedInt<Base,Tag>,
                 senf::comparable_safe_bool< RestrictedInt<Base,Tag> > > > > >
    {
    public:
        typedef Base base_type;
        typedef Tag tag_type;

        explicit RestrictedInt(Base value) : value_ (value) {}
        RestrictedInt() : value_ () {}

#       define BinaryOp(op)                                             \
            RestrictedInt & operator op ## = (RestrictedInt other)      \
                { value_ op ## = other.value_; return *this; }
#       define IncDecOp(op)                                             \
            RestrictedInt & operator op ()                              \
                { op value_; return *this; }
#       define PrefixOp(op)                                             \
            RestrictedInt const operator op () const                    \
                { return RestrictedInt(op value_); }
#       define CompareOp(op)                                            \
            bool operator op (RestrictedInt other) const                \
                { return value_ op other.value_; }

        BinaryOp(+)
        BinaryOp(-)
        BinaryOp(*)
        BinaryOp(/)
        BinaryOp(%)
        BinaryOp(>>)
        BinaryOp(<<)
        BinaryOp(&)
        BinaryOp(|)
        BinaryOp(^)

        IncDecOp(++)
        IncDecOp(--)

        PrefixOp(~)
        PrefixOp(-)

        CompareOp(<)
        CompareOp(==)

#       undef CompareOp
#       undef PrefixOp
#       undef PostfixOp
#       undef BinaryOp

        Base value() const
            { return value_; }

        bool boolean_test() const
            { return value_; }

        RestrictedInt<Base, Tag> operator*(int i) const
            { return RestrictedInt<Base, Tag>(value_ * i); }

        RestrictedInt<Base, Tag> operator/(int i) const
            { return RestrictedInt<Base, Tag>(value_ / i); }

        bool operator>(int i) const
            { return value_ > i; }

        bool operator>=(int i) const
            { return value_ >= i; }

        bool operator<(int i) const
            { return value_ < i; }

        bool operator<=(int i) const
            { return value_ <= i; }

        bool operator==(int i) const
            { return value_ == i; }

        bool operator!=(int i) const
            { return value_ != i; }

    private:
        Base value_;
    };

    template <class Base, class Tag>
    std::ostream & operator<<(std::ostream & os, RestrictedInt<Base, Tag> value)
        { os << value.value(); return os; }

    template <class Base, class Tag>
    std::istream & operator>>(std::istream & is, RestrictedInt<Base, Tag> & value)
        { Base v; is >> v; value = RestrictedInt<Base,Tag>(v); return is; }

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "RestrictedInt.cci"
//#include "RestrictedInt.ct"
//#include "RestrictedInt.cti"
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
