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
    \brief Packet algorithms public header */

#ifndef HH_SENF_Packets_algorithms_
#define HH_SENF_Packets_algorithms_ 1

// Custom includes

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    template<class CollectionParser>
    class back_insert_iterator
        : public std::iterator<std::output_iterator_tag, void, void, void, void>
    {
    public:
        back_insert_iterator(CollectionParser const & parser)
            : container_(parser)
        {}

        template<class Value>
        back_insert_iterator & operator=(Value const & value)
        {
            container_.push_back(value);
            return *this;
        }

        back_insert_iterator & operator*()    { return *this; }
        back_insert_iterator & operator++()   { return *this; }
        back_insert_iterator & operator++(int){ return *this; }

    private:
        typename CollectionParser::container_type container_;
    };

    template<class CollectionParser>
    inline back_insert_iterator<CollectionParser> back_inserter(CollectionParser const & parser)
    {
        return back_insert_iterator<CollectionParser>(parser);
    }


    template <class ListPolicy, class OutputIterator>
    void copy(ListParser<ListPolicy> const & listParser, OutputIterator result)
    {
        typename ListParser<ListPolicy>::container_type container (listParser);
        std::copy( container.begin(), container.end(), result);
    }

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "algorithms.cci"
//#include "algorithms.ct"
//#include "algorithms.cti"
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
