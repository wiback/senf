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

#ifndef BOOST_INTRUSIVE_ISLIST_HOOK_HPP
#define BOOST_INTRUSIVE_ISLIST_HOOK_HPP

#include <boost/intrusive/detail/config_begin.hpp>
#include <boost/intrusive/detail/utilities.hpp>
#include <boost/intrusive/detail/pointer_type.hpp>
#include <boost/intrusive/detail/pointer_to_other.hpp>
#include <boost/intrusive/detail/slist_node.hpp>
#include <boost/intrusive/slist_algorithms.hpp>
#include <boost/intrusive/linking_policy.hpp>
#include <boost/get_pointer.hpp>
#include <stdexcept>

namespace boost {
namespace intrusive {

//! Derive a class from islist_base_hook in order to store objects in 
//! in an islist. islist_base_hook holds the data necessary to maintain the 
//! list and provides an appropriate value_traits class for islist.
//! 
//! The first integer template argument defines a tag to identify the node. 
//! The same tag value can be used in different classes, but if a class is 
//! derived from more than one islist_base_hook, then each islist_base_hook needs its 
//! unique tag.
//!
//! The second boolean template parameter will activate the safe-mode checks
//! if it's configured as "true".
//!
//! The third argument is the pointer type that will be used internally in the hook
//! and the islist configured from this hook.
template<typename Tag, bool SafeMode = true, class VoidPointer = void*>
class islist_base_hook
   :  private detail::slist_node_traits<VoidPointer>::node
{
   public:
   typedef detail::slist_node_traits<VoidPointer>     node_traits;
   enum { linking_policy = SafeMode? safe_mode_link : normal_link};

   private:
   typedef slist_algorithms<node_traits> node_algorithms;

   public:
   typedef typename node_traits::node                 node;
   typedef islist_base_hook
      <Tag, SafeMode, VoidPointer>                    this_type;
   typedef typename boost::pointer_to_other
      <VoidPointer, node>::type                       node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const node>::type                 const_node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, this_type>::type                  this_type_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const this_type>::type            const_this_type_ptr;

   private:
   node_ptr this_as_node()
   {  return node_ptr(static_cast<node *const>(this)); }

   const_node_ptr this_as_node() const
   {  return const_node_ptr(static_cast<const node *const>(this)); }

   public:

   //! <b>Effects</b>: If SafeMode is true initializes the node
   //!   to an unlinked state.
   //! 
   //! <b>Throws</b>: Nothing.
   islist_base_hook()
      : node_traits::node()
   {
      if(SafeMode){
         node_algorithms::init(this_as_node());
      }
   }

   //! <b>Effects</b>: If SafeMode is true initializes the node
   //!   to an unlinked state. The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Rationale</b>: Providing a copy-constructor
   //!   makes classes using islist_base_hook STL-compliant without forcing the 
   //!   user to do some additional work. "swap" can be used to emulate
   //!   move-semantics.
   islist_base_hook(const islist_base_hook& )
      :  node_traits::node()
   {
      if(SafeMode){
         node_algorithms::init(this_as_node());
      }
   }

   //! <b>Effects</b>: If SafeMode is true, an assertion is raised
   //!   if the node is still linked. After that, the node is initialized
   //!   to an unlinked state. The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Rationale</b>: Providing an assignment operator 
   //!   makes classes using islist_base_hook STL-compliant without forcing the 
   //!   user to do some additional work. "swap" can be used to emulate
   //!   move-semantics.
   islist_base_hook& operator=(const islist_base_hook& )
   { 
      if(SafeMode){
         BOOST_ASSERT(!this->linked());
      }
      return *this; 
   }

   //! <b>Effects</b>: If SafeMode is set to false, the destructor does
   //!   nothing (ie. no code is generated). If SafeMode is true and the
   //!   object is stored in an islist an assertion is raised.
   //! 
   //! <b>Throws</b>: Nothing.
   ~islist_base_hook() 
   { 
      if(SafeMode){
         BOOST_ASSERT(!this->linked());
      }
   }

   //! <b>Effects</b>: Swapping two nodes swaps the position of the elements 
   //!   related to those nodes in one or two containers. That is, if the node 
   //!   this is part of the element e1, the node x is part of the element e2 
   //!   and both elements are included in the containers s1 and s2, then after 
   //!   the swap-operation e1 is in s2 at the position of e2 and e2 is in s1 
   //!   at the position of e1. If one element is not in a container, then 
   //!   after the swap-operation the other element is not in a container. 
   //!   Iterators to e1 and e2 related to those nodes are invalidated. 
   //!
   //! <b>Complexity</b>: Linear
   //!
   //! <b>Throws</b>: Nothing. 
   void swap_nodes(islist_base_hook& other) 
   { node_algorithms::swap_nodes(this_as_node(), other.this_as_node()); }

   //! <b>Precondition</b>: The hook must be in safe-mode.
   //!
   //! <b>Returns</b>: true, if the node belongs to a container, false
   //!   otherwise. This function can be used to test whether islist::current 
   //!   will return a valid iterator. 
   //!
   //! <b>Complexity</b>: Constant
   bool linked() const 
   {
      //linked() can be only used in safe-mode
      BOOST_ASSERT(SafeMode);
      return !node_algorithms::unique(this_as_node());
   }

   //! The value_traits class is used as the first template argument for islist. 
   //! The template argument T defines the class type stored in islist. Objects 
   //! of type T and of types derived from T can be stored. T doesn't need to be 
   //! copy-constructible or assignable.
   template<class T>
   struct value_traits
      : detail::derivation_value_traits<T, this_type, Tag>
   {};

   //! <b>Effects</b>: Converts a pointer to a node into
   //!   a pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing.
   static this_type_ptr to_hook_ptr(node_ptr p)
   {
      using boost::get_pointer;
      return this_type_ptr(static_cast<islist_base_hook*> (get_pointer(p))); 
   }

   //! <b>Effects</b>: Converts a const pointer to a node stored in a container into
   //!   a const pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing.
   static const_this_type_ptr to_hook_ptr(const_node_ptr p)
   {
      using boost::get_pointer;
      return this_type_ptr(static_cast<const islist_base_hook*> (get_pointer(p))); 
   }

   //! <b>Effects</b>: Returns a pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing.
   node_ptr to_node_ptr()
   { return this_as_node(); }

   //! <b>Effects</b>: Returns a const pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing.
   const_node_ptr to_node_ptr() const
   { return this_as_node(); }
};


//! Derive a class from islist_auto_base_hook in order to store objects in 
//! in an islist. islist_auto_base_hook holds the data necessary to maintain the 
//! list and provides an appropriate value_traits class for islist.
//!
//! The difference between islist_auto_base_hook and islist_base_hook is that
//! islist_auto_base_hook removes itself automatically from the container
//! in the assignment operator and the destructor. It also provides a new
//! "unlink" method so that the user can unlink its class without using
//! the container.
//!
//! islist_auto_base_hook can only be used with non constant-time islists.
//! 
//! The first integer template argument defines a tag to identify the node. 
//! The same tag value can be used in different classes, but if a class is 
//! derived from more than one islist_auto_base_hook, then each islist_auto_base_hook needs its 
//! unique tag.
//!
//! The second argument is the pointer type that will be used internally in the hook
//! and the islist configured from this hook.
template<typename Tag, class VoidPointer = void*>
class islist_auto_base_hook
   :  private detail::slist_node_traits<VoidPointer>::node
{
   public:
   typedef detail::slist_node_traits<VoidPointer>     node_traits;
   enum { linking_policy = auto_unlink  };

   private:
   typedef slist_algorithms<node_traits> node_algorithms;

   public:
   typedef islist_auto_base_hook
      <Tag, VoidPointer>                              this_type;
   typedef typename node_traits::node                 node;
   typedef typename boost::pointer_to_other
      <VoidPointer, node>::type                       node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const node>::type                 const_node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, this_type>::type                  this_type_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const this_type>::type            const_this_type_ptr;

   private:
   node_ptr this_as_node()
   {  return node_ptr(static_cast<node *const>(this)); }

   const_node_ptr this_as_node() const
   {  return const_node_ptr(static_cast<const node *const>(this)); }

   public:

   //! <b>Effects</b>: Initializes the node
   //!   to an unlinked state.
   //! 
   //! <b>Throws</b>: Nothing. 
   islist_auto_base_hook()
      : node_traits::node()
   {  node_algorithms::init(this_as_node());   }

   //! <b>Effects</b>: Initializes the node
   //!   to an unlinked state. The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing. 
   //! 
   //! <b>Rationale</b>: Providing a copy-constructor
   //!   makes classes using islist_auto_base_hook STL-compliant without forcing the 
   //!   user to do some additional work.
   islist_auto_base_hook(const islist_auto_base_hook& )
      :  node_traits::node()
   {  node_algorithms::init(this_as_node());   }

   //! <b>Effects</b>: Removes the node if it's inserted in a container.
   //!   The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing. 
   //! 
   //! <b>Rationale</b>: Providing an assignment operator 
   //!   makes classes using islist_auto_base_hook STL-compliant without forcing the 
   //!   user to do some additional work.
   islist_auto_base_hook& operator=(const islist_auto_base_hook& )
   {  this->unlink(); return *this;  }

   //! <b>Effects</b>: Removes the node if it's inserted in a container.
   //! 
   //! <b>Throws</b>: Nothing. 
   ~islist_auto_base_hook() 
   {  this->unlink();  }

   //! <b>Effects</b>: Swapping two nodes swaps the position of the elements 
   //!   related to those nodes in one or two containers. That is, if the node 
   //!   this is part of the element e1, the node x is part of the element e2 
   //!   and both elements are included in the containers s1 and s2, then after 
   //!   the swap-operation e1 is in s2 at the position of e2 and e2 is in s1 
   //!   at the position of e1. If one element is not in a container, then 
   //!   after the swap-operation the other element is not in a container. 
   //!   Iterators to e1 and e2 related to those nodes are invalidated. 
   //!
   //! <b>Complexity</b>: Linear 
   //!
   //! <b>Throws</b>: Nothing.
   void swap_nodes(islist_auto_base_hook& other) 
   { node_algorithms::swap_nodes(this_as_node(), other.this_as_node()); }

   //! <b>Returns</b>: true, if the node belongs to a container, false
   //!   otherwise. This function can be used to test whether islist::current 
   //!   will return a valid iterator. 
   //!
   //! <b>Complexity</b>: Constant 
   bool linked() const 
   {  return !node_algorithms::unique(this_as_node());  }

   //! <b>Effects</b>: Removes the node if it's inserted in a container.
   //! 
   //! <b>Throws</b>: Nothing. 
   void unlink()
   {
      node_algorithms::unlink(this_as_node());
      node_algorithms::init(this_as_node());
   }

   //! The value_traits class is used as the first template argument for islist. 
   //! The template argument T defines the class type stored in islist. Objects 
   //! of type T and of types derived from T can be stored. T doesn't need to be 
   //! copy-constructible or assignable.
   template<class T>
   struct value_traits
      : detail::derivation_value_traits<T, this_type, Tag>
   {};

   //! <b>Effects</b>: Converts a pointer to a node into
   //!   a pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing. 
   static this_type_ptr to_hook_ptr(node_ptr p)
   {
      using boost::get_pointer;
      return this_type_ptr(static_cast<islist_auto_base_hook*> (get_pointer(p))); 
   }

   //! <b>Effects</b>: Converts a const pointer to a node stored in a container into
   //!   a const pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing. 
   static const_this_type_ptr to_hook_ptr(const_node_ptr p)
   {
      using boost::get_pointer;
      return this_type_ptr(static_cast<const islist_auto_base_hook*> (get_pointer(p))); 
   }

   //! <b>Effects</b>: Returns a pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing.
   node_ptr to_node_ptr()
   { return this_as_node(); }

   //! <b>Effects</b>: Returns a const pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing.
   const_node_ptr to_node_ptr() const
   { return this_as_node(); }
};


//! Put a public data member islist_member_hook in order to store objects of this class in
//! an islist. islist_member_hook holds the data necessary for maintaining the list and 
//! provides an appropriate value_traits class for islist.
//! 
//! The template argument T defines the class type stored in islist. Objects of type 
//! T and of types derived from T can be stored. T doesn't need to be 
//! copy-constructible or assignable.
//! 
//! The second boolean template parameter will activate the safe-mode checks
//! if it's configured as "true".
//!
//! The third argument is the pointer type that will be used internally in the hook
//! and the islist configured from this hook.
template<class T, bool SafeMode = true, class VoidPointer = void*>
class islist_member_hook 
   :  private detail::slist_node_traits<VoidPointer>::node
{
   public:
   typedef detail::slist_node_traits<VoidPointer>     node_traits;
   enum { linking_policy = SafeMode? safe_mode_link : normal_link};

   private:
   typedef slist_algorithms<node_traits>              node_algorithms;

   public:
   typedef typename node_traits::node                 node;
   typedef islist_member_hook<T, SafeMode, VoidPointer> this_type;
   typedef typename boost::pointer_to_other
      <VoidPointer, node>::type                       node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const node>::type                 const_node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, this_type >::type                 this_type_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const this_type >::type           const_this_type_ptr;

   private:
   node_ptr this_as_node()
   {  return node_ptr(static_cast<node *const>(this)); }

   const_node_ptr this_as_node() const
   {  return const_node_ptr(static_cast<const node *const>(this)); }

   public:
   //! <b>Effects</b>: If SafeMode is true initializes the node
   //!   to an unlinked state.
   //! 
   //! <b>Throws</b>: Nothing. 
   islist_member_hook()
      :  node_traits::node()
   {
      if(SafeMode){
         node_algorithms::init(this_as_node());
      }
   }

   //! <b>Effects</b>: If SafeMode is true initializes the node
   //!   to an unlinked state. The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing. 
   //! 
   //! <b>Rationale</b>: Providing a copy-constructor
   //!   makes classes using islist_member_hook STL-compliant without forcing the 
   //!   user to do some additional work. "swap" can be used to emulate
   //!   move-semantics.
   islist_member_hook(const islist_member_hook& )
      :  node_traits::node()
   {
      if(SafeMode){
         node_algorithms::init(this_as_node());
      }
   }

   //! <b>Effects</b>: If SafeMode is true, an assertion is raised
   //!   if the node is still linked. After that, the node is initialized
   //!   to an unlinked state. The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing. 
   //! 
   //! <b>Rationale</b>: Providing an assignment operator 
   //!   makes classes using islist_member_hook STL-compliant without forcing the 
   //!   user to do some additional work. "swap" can be used to emulate
   //!   move-semantics.
   islist_member_hook& operator=(const islist_member_hook& ) 
   { 
      if(SafeMode){
         BOOST_ASSERT(!this->linked());
      }
      return *this; 
   }

   //! <b>Effects</b>: If SafeMode is set to false, the destructor does
   //!   nothing (ie. no code is generated). If SafeMode is true and the
   //!   object is stored in an islist an assertion is raised.
   //! 
   //! <b>Throws</b>: Nothing. 
   ~islist_member_hook() 
   { 
      if(SafeMode){
         BOOST_ASSERT(!this->linked());
      }
   }

   //! <b>Effects</b>: Swapping two nodes swaps the position of the elements 
   //!   related to those nodes in one or two containers. That is, if the node 
   //!   this is part of the element e1, the node x is part of the element e2 
   //!   and both elements are included in the containers s1 and s2, then after 
   //!   the swap-operation e1 is in s2 at the position of e2 and e2 is in s1 
   //!   at the position of e1. If one element is not in a container, then 
   //!   after the swap-operation the other element is not in a container. 
   //!   Iterators to e1 and e2 related to those nodes are invalidated. 
   //!
   //! <b>Complexity</b>: Linear 
   //!
   //! <b>Throws</b>: Nothing.
   void swap_nodes(islist_member_hook& other) 
   { node_algorithms::swap_nodes(this_as_node(), other.this_as_node()); }

   //! <b>Precondition</b>: The hook must be in safe-mode.
   //!
   //! <b>Returns</b>: true, if the node belongs to a container, false
   //!   otherwise. This function can be used to test whether islist::current 
   //!   will return a valid iterator. 
   //!
   //! <b>Complexity</b>: Constant
   bool linked() const 
   {
      //We must be in safe-mode to know if we are really linked
      //Otherwise, this would lead to an unknown state
      BOOST_ASSERT(SafeMode);
      return !node_algorithms::unique(this_as_node()); 
   }

   //! The value_traits class is used as the first template argument for islist. 
   //! The template argument is a pointer to member pointing to the node in 
   //! the class. Objects of type T and of types derived from T can be stored. 
   //! T doesn't need to be copy-constructible or assignable.
   template<this_type T::* M>
   struct value_traits
      : detail::member_value_traits<T, this_type, M>
   {};

   //! <b>Effects</b>: Converts a pointer to a node into
   //!   a pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing. 
   static this_type_ptr to_hook_ptr(node_ptr p)
   {
      using boost::get_pointer;
      return this_type_ptr(static_cast<this_type*> (get_pointer(p))); 
   }

   //! <b>Effects</b>: Converts a const pointer to a node stored in a container into
   //!   a const pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing. 
   static const_this_type_ptr to_hook_ptr(const_node_ptr p)
   {
      using boost::get_pointer;
      return this_type_ptr(static_cast<this_type*> (get_pointer(p))); 
   }

   //! <b>Effects</b>: Returns a pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing. 
   node_ptr to_node_ptr()
   { return this_as_node(); }

   //! <b>Effects</b>: Returns a const pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing. 
   const_node_ptr to_node_ptr() const
   { return this_as_node(); }
};


//! Put a public data member islist_auto_member_hook in order to store objects of this class in
//! an islist. islist_auto_member_hook holds the data necessary for maintaining the list and 
//! provides an appropriate value_traits class for islist.
//!
//! The difference between islist_auto_member_hook and islist_member_hook is that
//! islist_auto_member_hook removes itself automatically from the container
//! in the assignment operator and the destructor. It also provides a new
//! "unlink" method so that the user can unlink its class without using
//! the container.
//!
//! islist_auto_member_hook can only be used with non constant-time islists.
//! 
//! The first template argument T defines the class type stored in islist. Objects of
//! type T and of types derived from T can be stored. T doesn't need to be 
//! copy-constructible or assignable.
//!
//! The second argument is the pointer type that will be used internally in the hook
//! and the islist configured from this hook.
template<class T, class VoidPointer = void*>
class islist_auto_member_hook 
   :  private detail::slist_node_traits<VoidPointer>::node
{
   public:
   typedef detail::slist_node_traits<VoidPointer>     node_traits;
   enum { linking_policy = auto_unlink  };

   private:
   typedef slist_algorithms<node_traits>              node_algorithms;

   public:
   typedef typename node_traits::node                 node;
   typedef islist_auto_member_hook<T, VoidPointer>    this_type;
   typedef typename boost::pointer_to_other
      <VoidPointer, node>::type                       node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const node>::type                 const_node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, this_type >::type                 this_type_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const this_type >::type           const_this_type_ptr;

   private:
   node_ptr this_as_node()
   {  return node_ptr(static_cast<node *const>(this)); }

   const_node_ptr this_as_node() const
   {  return const_node_ptr(static_cast<const node *const>(this)); }

   public:
   //! <b>Effects</b>: Initializes the node
   //!   to an unlinked state.
   //! 
   //! <b>Throws</b>: Nothing. 
   islist_auto_member_hook()
      :  node_traits::node()
   {  node_algorithms::init(this_as_node());   }

   //! <b>Effects</b>: Initializes the node
   //!   to an unlinked state. The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing. 
   //! 
   //! <b>Rationale</b>: Providing a copy-constructor
   //!   makes classes using islist_auto_base_hook STL-compliant without forcing the 
   //!   user to do some additional work.
   islist_auto_member_hook(const islist_auto_member_hook& )
      :  node_traits::node()
   {  node_algorithms::init(this_as_node());   }

   //! <b>Effects</b>: Removes the node if it's inserted in a container.
   //!   The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing. 
   //! 
   //! <b>Rationale</b>: Providing an assignment operator 
   //!   makes classes using islist_auto_base_hook STL-compliant without forcing the 
   //!   user to do some additional work.
   islist_auto_member_hook& operator=(const islist_auto_member_hook& ) 
   {  this->unlink(); return *this;  }

   //! <b>Effects</b>: If SafeMode is set to false, the destructor does
   //!   nothing (ie. no code is generated). If SafeMode is true and the
   //!   object is stored in an islist an assertion is raised.
   //! 
   //! <b>Throws</b>: Nothing. 
   ~islist_auto_member_hook() 
   {  this->unlink();  }

   //! <b>Effects</b>: Swapping two nodes swaps the position of the elements 
   //!   related to those nodes in one or two containers. That is, if the node 
   //!   this is part of the element e1, the node x is part of the element e2 
   //!   and both elements are included in the containers s1 and s2, then after 
   //!   the swap-operation e1 is in s2 at the position of e2 and e2 is in s1 
   //!   at the position of e1. If one element is not in a container, then 
   //!   after the swap-operation the other element is not in a container. 
   //!   Iterators to e1 and e2 related to those nodes are invalidated. 
   //!
   //! <b>Complexity</b>: Constant 
   //!
   //! <b>Throws</b>: Nothing.
   void swap_nodes(islist_auto_member_hook& other) 
   { node_algorithms::swap_nodes(this_as_node(), other.this_as_node()); }

   //! <b>Returns</b>: true, if the node belongs to a container, false
   //!   otherwise. This function can be used to test whether islist::current 
   //!   will return a valid iterator. 
   //!
   //! <b>Complexity</b>: Constant 
   bool linked() const 
   {  return !node_algorithms::unique(this_as_node());  }

   //! The value_traits class is used as the first template argument for islist. 
   //! The template argument is a pointer to member pointing to the node in 
   //! the class. Objects of type T and of types derived from T can be stored. 
   //! T doesn't need to be copy-constructible or assignable.
   template<this_type T::* M>
   struct value_traits
      : detail::member_value_traits<T, this_type, M>
   {};

   //! <b>Effects</b>: Removes the node if it's inserted in a container.
   //! 
   //! <b>Throws</b>: Nothing. 
   void unlink()
   {
      node_algorithms::unlink(this_as_node());
      node_algorithms::init(this_as_node());
   }

   //! <b>Effects</b>: Converts a pointer to a node into
   //!   a pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing. 
   static this_type_ptr to_hook_ptr(node_ptr p)
   {
      using boost::get_pointer;
      return this_type_ptr(static_cast<this_type*> (get_pointer(p))); 
   }

   //! <b>Effects</b>: Converts a const pointer to a node stored in a container into
   //!   a const pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing. 
   static const_this_type_ptr to_hook_ptr(const_node_ptr p)
   {
      using boost::get_pointer;
      return const_this_type_ptr(static_cast<const this_type*> (get_pointer(p))); 
   }

   //! <b>Effects</b>: Returns a pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing. 
   node_ptr to_node_ptr()
   { return this_as_node(); }

   //! <b>Effects</b>: Returns a const pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing. 
   const_node_ptr to_node_ptr() const
   { return this_as_node(); }
};

} //namespace intrusive 
} //namespace boost 

#include<boost/intrusive/detail/config_end.hpp>

#endif //BOOST_INTRUSIVE_ISLIST_HOOK_HPP
