/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Olaf Krzikalla 2004-2006.
// (C) Copyright Ion Gaztañaga  2006-2007
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_LIST_NODE_HPP
#define BOOST_INTRUSIVE_LIST_NODE_HPP

#include "config_begin.hpp"
#include <iterator>
#include <boost/assert.hpp>
#include "pointer_type.hpp"
#include "pointer_to_other.hpp"
#include "../list_algorithms.hpp"
#include <boost/get_pointer.hpp>
#include <cstddef>

namespace boost {
namespace intrusive {
namespace detail {

// list_node_traits can be used with list_algorithms and supplies
// a list_node holding the pointers needed for a double-linked list
// it is used by ilist_derived_node and ilist_member_node
template<class VoidPointer>
struct list_node_traits
{
   struct node;
   typedef typename boost::pointer_to_other
      <VoidPointer, node>::type          node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const node>::type    const_node_ptr;

   struct node
   {
      node_ptr prev_, next_;
   };

   static node_ptr get_previous(const_node_ptr n)
   {  return n->prev_;  }

   static void set_previous(node_ptr n, node_ptr prev)
   {  n->prev_ = prev;  }

   static node_ptr get_next(const_node_ptr n)
   {  return n->next_;  }

   static void set_next(node_ptr n, node_ptr next)
   {  n->next_ = next;  }
};


// list_iterator provides some basic functions for a 
// node oriented bidirectional iterator:
template<class T, class Self, class NodeTraits>
class list_iterator
   :  public std::iterator<std::bidirectional_iterator_tag, T>
{
   protected:
   typedef typename NodeTraits::node            node;
   typedef typename NodeTraits::node_ptr        node_ptr;
 
   list_iterator()
      : node_ (0)
   {}

   explicit list_iterator(node_ptr node)
      : node_ (node)
   {}

   node_ptr list_node() const
   { return node_; }

   Self &operator=(const node_ptr &node)
   {  node_ = node;  return static_cast<Self&>(*this);  }

   public:
   Self& operator++() 
   { 
      node_ = NodeTraits::get_next(node_); 
      return static_cast<Self&> (*this); 
   }
   
   Self operator++(int)
   {
      Self result (node_);
      node_ = NodeTraits::get_next(node_);
      return result;
   }

   Self& operator--() 
   { 
      node_ = NodeTraits::get_previous(node_); 
      return static_cast<Self&> (*this); 
   }
   
   Self operator--(int)
   {
      Self result (node_);
      node_ = NodeTraits::get_previous(node_);
      return result;
   }

   bool operator== (const Self& i) const
   { return node_ == i.list_node(); }

   bool operator!= (const Self& i) const
   { return !operator== (i); }

   private:
   node_ptr node_;
};

} //namespace detail 
} //namespace intrusive 
} //namespace boost 

#include "config_end.hpp"

#endif //BOOST_INTRUSIVE_LIST_NODE_HPP
