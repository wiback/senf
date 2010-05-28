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

#ifndef BOOST_INTRUSIVE_IHASHSET_HOOK_HPP
#define BOOST_INTRUSIVE_IHASHSET_HOOK_HPP

#include "detail/config_begin.hpp"
#include "detail/utilities.hpp"
#include "detail/pointer_type.hpp"
#include "detail/pointer_to_other.hpp"
#include "islist_hook.hpp"
#include "linking_policy.hpp"
#include <boost/get_pointer.hpp>
#include <stdexcept>

namespace boost {
namespace intrusive {

//! Derive a class from iunordered_set_base_hook in order to store objects in 
//! in an iunordered_set/iunordered_multi_set. iunordered_set_base_hook holds the data necessary to maintain 
//! the unordered_set/unordered_multi_set and provides an appropriate value_traits class for iunordered_set/iunordered_multi_set.
//! 
//! The first integer template argument defines a tag to identify the node. 
//! The same tag value can be used in different classes, but if a class is 
//! derived from more than one iunordered_set_base_hook, then each iunordered_set_base_hook needs its 
//! unique tag.
//!
//! The second boolean template parameter will activate the safe-mode checks
//! if it's configured as "true".
//!
//! The third argument is the pointer type that will be used internally in the hook
//! and the iunordered_set/iunordered_multi_set configured from this hook.
template<typename Tag, bool SafeMode = true, class VoidPointer = void*>
class iunordered_set_base_hook
{
   typedef islist_base_hook<Tag, SafeMode, VoidPointer> IsListHook;
   IsListHook m_islisthook;
   typedef IsListHook                                 implementation_defined;

   public:
   enum { linking_policy = SafeMode? safe_mode_link : normal_link};
   typedef typename implementation_defined::node_traits  node_traits;
   typedef typename node_traits::node                    node;
   typedef typename boost::pointer_to_other
      <VoidPointer, node>::type                          node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const node>::type                    const_node_ptr;
   typedef iunordered_set_base_hook
      <Tag, SafeMode, VoidPointer>                       this_type;
   typedef typename boost::pointer_to_other
      <VoidPointer, this_type>::type                     this_type_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const this_type>::type               const_this_type_ptr;

   //! <b>Effects</b>: If SafeMode is true initializes the node
   //!   to an unlinked state.
   //! 
   //! <b>Throws</b>: Nothing.
   iunordered_set_base_hook()
      :  m_islisthook()
   {}

   //! <b>Effects</b>: If SafeMode is true initializes the node
   //!   to an unlinked state. The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Rationale</b>: Providing a copy-constructor
   //!   makes classes using iunordered_set_base_hook STL-compliant without forcing the 
   //!   user to do some additional work. "swap" can be used to emulate
   //!   move-semantics.
   iunordered_set_base_hook(const iunordered_set_base_hook &other)
      :  m_islisthook(other.m_islisthook)
   {}

   //! <b>Effects</b>: If SafeMode is true, an assertion is raised
   //!   if the node is still linked. After that, the node is initialized
   //!   to an unlinked state. The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Rationale</b>: Providing an assignment operator 
   //!   makes classes using iunordered_set_base_hook STL-compliant without forcing the 
   //!   user to do some additional work. "swap" can be used to emulate
   //!   move-semantics.
   iunordered_set_base_hook& operator=(const iunordered_set_base_hook &other)
   {  m_islisthook = other.m_islisthook;  return *this;  }

   //! <b>Effects</b>: If SafeMode is set to false, the destructor does
   //!   nothing (ie. no code is generated). If SafeMode is true and the
   //!   object is stored in an iunordered_set/iunordered_multiset an assertion is raised.
   //! 
   //! <b>Throws</b>: Nothing.
   ~iunordered_set_base_hook() 
   {}

   //! <b>Precondition</b>: The hook must be in safe-mode.
   //!
   //! <b>Returns</b>: true, if the node belongs to a container, false
   //!   otherwise. This function can be used to test whether iunordered_set/iunordered_multiset::current 
   //!   will return a valid iterator. 
   //!
   //! <b>Complexity</b>: Constant
   bool linked() const 
   {  return m_islisthook.linked(); }

   //! The value_traits class is used as the first template argument for iunordered_set/iunordered_multiset. 
   //! The template argument T defines the class type stored in iunordered_set/iunordered_multiset. Objects 
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
      return this_type_ptr((this_type*)get_pointer(IsListHook::to_hook_ptr(p)));
   }

   //! <b>Effects</b>: Converts a const pointer to a node stored in a container into
   //!   a const pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing.
   static const_this_type_ptr to_hook_ptr(const_node_ptr p)
   {
      using boost::get_pointer;
      return const_this_type_ptr((const this_type*)get_pointer(IsListHook::to_hook_ptr(p)));
   }

   //! <b>Effects</b>: Returns a pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing.
   node_ptr to_node_ptr()
   {  return m_islisthook.to_node_ptr();  }

   //! <b>Effects</b>: Returns a const pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing.
   const_node_ptr to_node_ptr() const
   {  return m_islisthook.to_node_ptr();  }
};


//! Derive a class from iunordered_set_auto_base_hook in order to store objects in an 
//! iunordered_set/iunordered_multi_set. iunordered_set_auto_base_hook holds the data necessary to maintain the 
//! unordered_set and provides an appropriate value_traits class for iunordered_set/iunordered_multi_set.
//!
//! The difference between iunordered_set_auto_base_hook and iunordered_set_base_hook is that
//! iunordered_set_auto_base_hook removes itself automatically from the container
//! in the assignment operator and the destructor. It also provides a new
//! "unlink" method so that the user can unlink its class without using
//! the container.
//!
//! iunordered_set_auto_base_hook can only be used with non constant-time iunordered_set/iunordered_multi_sets.
//! 
//! The first integer template argument defines a tag to identify the node. 
//! The same tag value can be used in different classes, but if a class is 
//! derived from more than one iunordered_set_auto_base_hook, then each iunordered_set_auto_base_hook needs its 
//! unique tag.
//!
//! The second argument is the pointer type that will be used internally in the hook
//! and the iunordered_set/unordered_multi_set configured from this hook.
template<typename Tag, class VoidPointer = void*>
class iunordered_set_auto_base_hook
{
   typedef islist_auto_base_hook<Tag, VoidPointer> IsListHook;
   IsListHook m_islisthook;
   typedef IsListHook implementation_defined;

   public:
   enum { linking_policy = auto_unlink };
   typedef typename implementation_defined::node_traits  node_traits;
   typedef typename node_traits::node                    node;
   typedef typename boost::pointer_to_other
      <VoidPointer, node>::type                          node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const node>::type                    const_node_ptr;
   typedef iunordered_set_auto_base_hook
      <Tag,VoidPointer>                                  this_type;
   typedef typename boost::pointer_to_other
      <VoidPointer, this_type>::type                     this_type_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const this_type>::type               const_this_type_ptr;

   public:

   //! <b>Effects</b>: Initializes the node
   //!   to an unlinked state.
   //! 
   //! <b>Throws</b>: Nothing. 
   iunordered_set_auto_base_hook()
      :  m_islisthook()
   {}

   //! <b>Effects</b>: Initializes the node
   //!   to an unlinked state. The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing. 
   //! 
   //! <b>Rationale</b>: Providing a copy-constructor
   //!   makes classes using iunordered_set_auto_base_hook STL-compliant without forcing the 
   //!   user to do some additional work.
   iunordered_set_auto_base_hook(const iunordered_set_auto_base_hook &other)
      :  m_islisthook(other.m_islisthook)
   {}

   //! <b>Effects</b>: Removes the node if it's inserted in a container.
   //!   The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing. 
   //! 
   //! <b>Rationale</b>: Providing an assignment operator 
   //!   makes classes using iunordered_set_auto_base_hook STL-compliant without forcing the 
   //!   user to do some additional work.
   iunordered_set_auto_base_hook& operator=(const iunordered_set_auto_base_hook &other)
   {  m_islisthook = other.m_islisthook;  return *this;  }

   //! <b>Effects</b>: Removes the node if it's inserted in a container.
   //! 
   //! <b>Throws</b>: Nothing. 
   ~iunordered_set_auto_base_hook() 
   {}

   //! <b>Returns</b>: true, if the node belongs to a container, false
   //!   otherwise. This function can be used to test whether iunordered_set/iunordered_multiset::current 
   //!   will return a valid iterator. 
   //!
   //! <b>Complexity</b>: Constant 
   bool linked() const 
   {  return m_islisthook.linked();  }

   //! <b>Effects</b>: Removes the node if it's inserted in a container.
   //! 
   //! <b>Throws</b>: Nothing. 
   void unlink()
   {  return m_islisthook.unlink();  }

   //! The value_traits class is used as the first template argument for iunordered_set/iunordered_multiset. 
   //! The template argument T defines the class type stored in iunordered_set/iunordered_multiset. Objects 
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
      return this_type_ptr((this_type*)get_pointer(IsListHook::to_hook_ptr(p)));
   }

   //! <b>Effects</b>: Converts a const pointer to a node stored in a container into
   //!   a const pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing. 
   static const_this_type_ptr to_hook_ptr(const_node_ptr p)
   {
      using boost::get_pointer;
      return const_this_type_ptr((const this_type*)get_pointer(IsListHook::to_hook_ptr(p)));
   }

   //! <b>Effects</b>: Returns a pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing.
   node_ptr to_node_ptr()
   {  return m_islisthook.to_node_ptr();  }

   //! <b>Effects</b>: Returns a const pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing.
   const_node_ptr to_node_ptr() const
   {  return m_islisthook.to_node_ptr();  }
};


//! Put a public data member iunordered_set_member_hook in order to store objects of this class in
//! an iunordered_set/iunordered_multi_set. iunordered_set_member_hook holds the data necessary for maintaining the
//! unordered_set/unordered_multi_set and provides an appropriate value_traits class for iunordered_set/iunordered_multi_set.
//! 
//! The template argument T defines the class type stored in iunordered_set/iunordered_multi_set. Objects of 
//! type T and of types derived from T can be stored. T doesn't need to be 
//! copy-constructible or assignable.
//! 
//! The second boolean template argument SafeMode controls initializes
//! the node to a safe state in the constructor and asserts if the node is destroyed
//! or it's assigned but it's still inserted in a iunordered_set/iunordered_multi_set.
//!
//! The third argument is the pointer type that will be used internally in the hook
//! and the iunordered_set/iunordered_multi_set configured from this hook.
template<class T, bool SafeMode = true, class VoidPointer = void*>
class iunordered_set_member_hook
{
   typedef islist_member_hook<T, SafeMode, VoidPointer> IsListHook;
   IsListHook m_islisthook;
   typedef IsListHook                                 implementation_defined;

   public:
   enum { linking_policy = SafeMode? safe_mode_link : normal_link};
   typedef typename implementation_defined::node_traits  node_traits;
   typedef typename node_traits::node                    node;
   typedef typename boost::pointer_to_other
      <VoidPointer, node>::type                          node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const node>::type                    const_node_ptr;
   typedef iunordered_set_member_hook
      <T, SafeMode, VoidPointer>                         this_type;
   typedef typename boost::pointer_to_other
      <VoidPointer, this_type>::type                     this_type_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const this_type>::type               const_this_type_ptr;

   public:
   //! <b>Effects</b>: If SafeMode is true initializes the node
   //!   to an unlinked state.
   //! 
   //! <b>Throws</b>: Nothing. 
   iunordered_set_member_hook()
      :  m_islisthook()
   {}

   //! <b>Effects</b>: If SafeMode is true initializes the node
   //!   to an unlinked state. The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing. 
   //! 
   //! <b>Rationale</b>: Providing a copy-constructor
   //!   makes classes using iunordered_set_member_hook STL-compliant without forcing the 
   //!   user to do some additional work.
   iunordered_set_member_hook(const iunordered_set_member_hook &other)
      :  m_islisthook(other.m_islisthook)
   {}

   //! <b>Effects</b>: If SafeMode is true, an assertion is raised
   //!   if the node is still linked. After that, the node is initialized
   //!   to an unlinked state. The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing. 
   //! 
   //! <b>Rationale</b>: Providing an assignment operator 
   //!   makes classes using iunordered_set_member_hook STL-compliant without forcing the 
   //!   user to do some additional work.
   iunordered_set_member_hook& operator=(const iunordered_set_member_hook &other) 
   {  m_islisthook = other.m_islisthook;  return *this;  }

   //! <b>Effects</b>: If SafeMode is set to false, the destructor does
   //!   nothing (ie. no code is generated). If SafeMode is true and the
   //!   object is stored in an iunordered_set/iunordered_multiset an assertion is raised.
   //! 
   //! <b>Throws</b>: Nothing. 
   ~iunordered_set_member_hook() 
   {}

   //! <b>Returns</b>: true, if the node belongs to a container, false
   //!   otherwise. This function can be used to test whether iunordered_set/iunordered_multiset::current 
   //!   will return a valid iterator. 
   //!
   //! <b>Complexity</b>: Constant 
   bool linked() const 
   {  return m_islisthook.linked();  }

   //! The value_traits class is used as the first template argument for iunordered_set/iunordered_multiset. 
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
   {  m_islisthook.unlink();   }

   //! <b>Effects</b>: Converts a pointer to a node into
   //!   a pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing. 
   static this_type_ptr to_hook_ptr(node_ptr p)
   {
      using boost::get_pointer;
      return this_type_ptr((this_type*)get_pointer(IsListHook::to_hook_ptr(p)));
   }

   //! <b>Effects</b>: Converts a const pointer to a node stored in a container into
   //!   a const pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing. 
   static const_this_type_ptr to_hook_ptr(const_node_ptr p)
   {
      using boost::get_pointer;
      return const_this_type_ptr((const this_type*)get_pointer(IsListHook::to_hook_ptr(p)));
   }

   //! <b>Effects</b>: Returns a pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing. 
   node_ptr to_node_ptr()
   {  return m_islisthook.to_node_ptr();  }

   //! <b>Effects</b>: Returns a const pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing. 
   const_node_ptr to_node_ptr() const
   {  return m_islisthook.to_node_ptr();  }
};

//! Put a public data member iunordered_set_auto_member_hook in order to store objects of this class in
//! an iunordered_set/iunordered_multiset. iunordered_set_auto_member_hook holds the data necessary for maintaining the list and 
//! provides an appropriate value_traits class for iunordered_set/iunordered_multiset.
//!
//! The difference between iunordered_set_auto_member_hook and iunordered_set_member_hook is that
//! iunordered_set_auto_member_hook removes itself automatically from the container
//! in the assignment operator and the destructor. It also provides a new
//! "unlink" method so that the user can unlink its class without using
//! the container.
//!
//! iunordered_set_auto_member_hook can only be used with non constant-time iunordered_sets/iunordered_multisets.
//! 
//! The first template argument T defines the class type stored in iunordered_set/iunordered_multiset. Objects of
//! type T and of types derived from T can be stored. T doesn't need to be 
//! copy-constructible or assignable.
//!
//! The second argument is the pointer type that will be used internally in the hook
//! and the iunordered_set/iunordered_multiset configured from this hook.
template<class T, class VoidPointer = void*>
class iunordered_set_auto_member_hook
{
   typedef islist_auto_member_hook<T, VoidPointer> IsListHook;
   IsListHook m_islisthook;
   typedef IsListHook implementation_defined;

   public:
   enum { linking_policy = auto_unlink };
   typedef typename implementation_defined::node_traits  node_traits;
   typedef typename node_traits::node                    node;
   typedef iunordered_set_auto_member_hook
      <T, VoidPointer>                                   this_type;
   typedef typename boost::pointer_to_other
      <VoidPointer, node>::type                          node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const node>::type                    const_node_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, this_type>::type                     this_type_ptr;
   typedef typename boost::pointer_to_other
      <VoidPointer, const this_type>::type               const_this_type_ptr;

   public:

   //! <b>Effects</b>: Initializes the node
   //!   to an unlinked state.
   //! 
   //! <b>Throws</b>: Nothing. 
   iunordered_set_auto_member_hook()
      :  m_islisthook()
   {}

   //! <b>Effects</b>: Initializes the node
   //!   to an unlinked state. The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing. 
   //! 
   //! <b>Rationale</b>: Providing a copy-constructor
   //!   makes classes using iunordered_set_auto_base_hook STL-compliant without forcing the 
   //!   user to do some additional work.
   iunordered_set_auto_member_hook(const iunordered_set_auto_member_hook &other)
      :  m_islisthook(other.m_islisthook)
   {}

   //! <b>Effects</b>: Removes the node if it's inserted in a container.
   //!   The argument is ignored.
   //! 
   //! <b>Throws</b>: Nothing. 
   //! 
   //! <b>Rationale</b>: Providing an assignment operator 
   //!   makes classes using iunordered_set_auto_base_hook STL-compliant without forcing the 
   //!   user to do some additional work.
   iunordered_set_auto_member_hook& operator=(const iunordered_set_auto_member_hook &other)
   {  m_islisthook = other.m_islisthook;  return *this;  }

   //! <b>Effects</b>: Removes the node if it's inserted in a container.
   //! 
   //! <b>Throws</b>: Nothing. 
   ~iunordered_set_auto_member_hook() 
   {}

   //! <b>Returns</b>: true, if the node belongs to a container, false
   //!   otherwise. This function can be used to test whether iunordered_set/iunordered_multiset::current 
   //!   will return a valid iterator. 
   //!
   //! <b>Complexity</b>: Constant 
   bool linked() const 
   {  return m_islisthook.linked();  }

   //! <b>Effects</b>: Removes the node if it's inserted in a container.
   //! 
   //! <b>Throws</b>: Nothing. 
   void unlink()
   {  return m_islisthook.unlink();  }

   //! The value_traits class is used as the first template argument for iunordered_set/iunordered_multiset. 
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
      return this_type_ptr((this_type*)get_pointer(IsListHook::to_hook_ptr(p)));
   }

   //! <b>Effects</b>: Converts a const pointer to a node stored in a container into
   //!   a const pointer to the hook that holds that node.
   //! 
   //! <b>Throws</b>: Nothing. 
   static const_this_type_ptr to_hook_ptr(const_node_ptr p)
   {
      using boost::get_pointer;
      return const_this_type_ptr((const this_type*)get_pointer(IsListHook::to_hook_ptr(p)));
   }

   //! <b>Effects</b>: Returns a pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing.
   node_ptr to_node_ptr()
   {  return m_islisthook.to_node_ptr();  }

   //! <b>Effects</b>: Returns a const pointer to the node that this hook holds.
   //! 
   //! <b>Throws</b>: Nothing.
   const_node_ptr to_node_ptr() const
   {  return m_islisthook.to_node_ptr();  }
};

} //namespace intrusive 
} //namespace boost 

#include "detail/config_end.hpp"

#endif //BOOST_INTRUSIVE_IHASHSET_HOOK_HPP
