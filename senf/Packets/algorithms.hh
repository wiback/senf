// $Id$
//
// Copyright (C) 2012
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
//   Thorsten Horstmann <tho@berlios.de>

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
