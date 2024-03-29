//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief RestrictedInt public header */

#ifndef HH_SENF_Utils_RestrictedInt_
#define HH_SENF_Utils_RestrictedInt_ 1

// Custom includes
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
                 senf::comparable_safe_bool<RestrictedInt<Base,Tag> > > > > >
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

    private:
        Base value_;
    };


    template <class Base, class Tag, typename OtherType>
    RestrictedInt<Base, Tag> operator*(OtherType a, RestrictedInt<Base,Tag> b)
    {
        return RestrictedInt<Base, Tag>( a * b.value());
    }

    template <class Base, class Tag, typename OtherType>
    RestrictedInt<Base, Tag> operator*(RestrictedInt<Base,Tag> a, OtherType b)
    {
        return RestrictedInt<Base, Tag>( a.value() * b);
    }

    template <class Base, class Tag, typename OtherType>
    RestrictedInt<Base, Tag> operator/(OtherType a, RestrictedInt<Base,Tag> b)
    {
        return RestrictedInt<Base, Tag>( a / b.value());
    }

    template <class Base, class Tag, typename OtherType>
    RestrictedInt<Base, Tag> operator/(RestrictedInt<Base,Tag> a, OtherType b)
    {
        return RestrictedInt<Base, Tag>( a.value() / b);
    }

    template <class Base, class Tag>
    std::ostream & operator<<(std::ostream & os, RestrictedInt<Base, Tag> value)
    {
        os << value.value();
        return os;
    }

    template <class Base, class Tag>
    std::istream & operator>>(std::istream & is, RestrictedInt<Base, Tag> & value)
    {
        Base v; is >> v; value = RestrictedInt<Base,Tag>(v);
        return is;
    }

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
