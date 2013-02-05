//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINERS_CONTAINERS_VECTOR_HPP
#define BOOST_CONTAINERS_CONTAINERS_VECTOR_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include "detail/config_begin.hpp"
#include INCLUDE_BOOST_CONTAINER_DETAIL_WORKAROUND_HPP
#include INCLUDE_BOOST_CONTAINER_CONTAINER_FWD_HPP

#include <cstddef>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <utility>
#include <boost/detail/no_exceptions_support.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>
#include <boost/type_traits/has_trivial_copy.hpp>
#include <boost/type_traits/has_trivial_assign.hpp>
#include <boost/type_traits/has_nothrow_copy.hpp>
#include <boost/type_traits/has_nothrow_assign.hpp>
#include INCLUDE_BOOST_CONTAINER_DETAIL_VERSION_TYPE_HPP
#include INCLUDE_BOOST_CONTAINER_DETAIL_ALLOCATION_TYPE_HPP
#include INCLUDE_BOOST_CONTAINER_DETAIL_UTILITIES_HPP
#include INCLUDE_BOOST_CONTAINER_DETAIL_ITERATORS_HPP
#include INCLUDE_BOOST_CONTAINER_DETAIL_ALGORITHMS_HPP
#include INCLUDE_BOOST_CONTAINER_DETAIL_DESTROYERS_HPP
#include INCLUDE_BOOST_CONTAINER_CONTAINER_FWD_HPP
#include INCLUDE_BOOST_CONTAINER_MOVE_HPP
#include <boost/pointer_to_other.hpp>
#include INCLUDE_BOOST_CONTAINER_DETAIL_MPL_HPP
#include INCLUDE_BOOST_CONTAINER_DETAIL_ADVANCED_INSERT_INT_HPP

namespace boost {
namespace container {

/// @cond

namespace containers_detail {

//! Const vector_iterator used to iterate through a vector. 
template <class Pointer>
class vector_const_iterator
   : public std::iterator<std::random_access_iterator_tag
                          ,const typename std::iterator_traits<Pointer>::value_type
                          ,typename std::iterator_traits<Pointer>::difference_type
                          ,typename boost::pointer_to_other
                              <Pointer
                              ,const typename std::iterator_traits<Pointer>::value_type
                              >::type
                          ,const typename std::iterator_traits<Pointer>::value_type &>
{
   public:
   typedef typename std::iterator_traits<Pointer>::value_type  value_type;
   typedef typename std::iterator_traits<Pointer>::difference_type   difference_type;
   typedef typename boost::pointer_to_other<Pointer, value_type>::type      pointer;
   typedef const value_type&                                               reference;

   /// @cond
   protected:
   Pointer m_ptr;

   public:
   Pointer get_ptr() const    {  return   m_ptr;  }
   explicit vector_const_iterator(Pointer ptr)  : m_ptr(ptr){}
   /// @endcond

   public:

   //Constructors
   vector_const_iterator() : m_ptr(0){}

   //Pointer like operators
   reference operator*()   const  
   {  return *m_ptr;  }

   const value_type * operator->()  const  
   {  return  containers_detail::get_pointer(m_ptr);  }

   reference operator[](difference_type off) const
   {  return m_ptr[off];   }

   //Increment / Decrement
   vector_const_iterator& operator++()       
   { ++m_ptr;  return *this; }

   vector_const_iterator operator++(int)      
   {  Pointer tmp = m_ptr; ++*this; return vector_const_iterator(tmp);  }

   vector_const_iterator& operator--()
   {  --m_ptr; return *this;  }

   vector_const_iterator operator--(int)
   {  Pointer tmp = m_ptr; --*this; return vector_const_iterator(tmp); }

   //Arithmetic
   vector_const_iterator& operator+=(difference_type off)
   {  m_ptr += off; return *this;   }

   vector_const_iterator operator+(difference_type off) const
   {  return vector_const_iterator(m_ptr+off);  }

   friend vector_const_iterator operator+(difference_type off, const vector_const_iterator& right)
   {  return vector_const_iterator(off + right.m_ptr); }

   vector_const_iterator& operator-=(difference_type off)
   {  m_ptr -= off; return *this;   }

   vector_const_iterator operator-(difference_type off) const
   {  return vector_const_iterator(m_ptr-off);  }

   difference_type operator-(const vector_const_iterator& right) const
   {  return m_ptr - right.m_ptr;   }

   //Comparison operators
   bool operator==   (const vector_const_iterator& r)  const
   {  return m_ptr == r.m_ptr;  }

   bool operator!=   (const vector_const_iterator& r)  const
   {  return m_ptr != r.m_ptr;  }

   bool operator<    (const vector_const_iterator& r)  const
   {  return m_ptr < r.m_ptr;  }

   bool operator<=   (const vector_const_iterator& r)  const
   {  return m_ptr <= r.m_ptr;  }

   bool operator>    (const vector_const_iterator& r)  const
   {  return m_ptr > r.m_ptr;  }

   bool operator>=   (const vector_const_iterator& r)  const
   {  return m_ptr >= r.m_ptr;  }
};

//! Iterator used to iterate through a vector
template <class Pointer>
class vector_iterator
   :  public vector_const_iterator<Pointer>
{
   public:
   explicit vector_iterator(Pointer ptr)
      : vector_const_iterator<Pointer>(ptr)
   {}

   public:
   typedef typename std::iterator_traits<Pointer>::value_type        value_type;
   typedef typename vector_const_iterator<Pointer>::difference_type  difference_type;
   typedef Pointer                                                   pointer;
   typedef value_type&                                               reference;

   //Constructors
   vector_iterator()
   {}

   //Pointer like operators
   reference operator*()  const  
   {  return *this->m_ptr;  }

   value_type* operator->() const  
   {  return  containers_detail::get_pointer(this->m_ptr);  }

   reference operator[](difference_type off) const 
   {  return this->m_ptr[off];   }

   //Increment / Decrement
   vector_iterator& operator++()  
   {  ++this->m_ptr; return *this;  }

   vector_iterator operator++(int)
   {  pointer tmp = this->m_ptr; ++*this; return vector_iterator(tmp);  }
   
   vector_iterator& operator--()
   {  --this->m_ptr; return *this;  }

   vector_iterator operator--(int)
   {  vector_iterator tmp = *this; --*this; return vector_iterator(tmp); }

   // Arithmetic
   vector_iterator& operator+=(difference_type off)
   {  this->m_ptr += off;  return *this;  }

   vector_iterator operator+(difference_type off) const
   {  return vector_iterator(this->m_ptr+off);  }

   friend vector_iterator operator+(difference_type off, const vector_iterator& right)
   {  return vector_iterator(off + right.m_ptr); }

   vector_iterator& operator-=(difference_type off)
   {  this->m_ptr -= off; return *this;   }

   vector_iterator operator-(difference_type off) const
   {  return vector_iterator(this->m_ptr-off);  }

   difference_type operator-(const vector_const_iterator<Pointer>& right) const
   {  return static_cast<const vector_const_iterator<Pointer>&>(*this) - right;   }
};

template <class T, class A>
struct vector_value_traits
{
   typedef T value_type;
   typedef A allocator_type;
   static const bool trivial_dctr = boost::has_trivial_destructor<value_type>::value;
   static const bool trivial_dctr_after_move = false;
      //::boost::has_trivial_destructor_after_move<value_type>::value || trivial_dctr;
   static const bool trivial_copy = has_trivial_copy<value_type>::value;
   static const bool nothrow_copy = has_nothrow_copy<value_type>::value;
   static const bool trivial_assign = has_trivial_assign<value_type>::value;
   static const bool nothrow_assign = has_nothrow_assign<value_type>::value;

   //This is the anti-exception array destructor
   //to deallocate values already constructed
   typedef typename containers_detail::if_c
      <trivial_dctr
      ,containers_detail::null_scoped_destructor_n<allocator_type>
      ,containers_detail::scoped_destructor_n<allocator_type>
      >::type   OldArrayDestructor;
   //This is the anti-exception array destructor
   //to destroy objects created with copy construction
   typedef typename containers_detail::if_c
      <nothrow_copy
      ,containers_detail::null_scoped_destructor_n<allocator_type>
      ,containers_detail::scoped_destructor_n<allocator_type>
      >::type   UCopiedArrayDestructor;
   //This is the anti-exception array deallocator
   typedef typename containers_detail::if_c
      <nothrow_copy
      ,containers_detail::null_scoped_array_deallocator<allocator_type>
      ,containers_detail::scoped_array_deallocator<allocator_type>
      >::type   UCopiedArrayDeallocator;
};

//!This struct deallocates and allocated memory
template <class A>
struct vector_alloc_holder 
{
   typedef typename A::pointer      pointer;
   typedef typename A::size_type    size_type;
   typedef typename A::value_type   value_type;
   typedef vector_value_traits<value_type, A> value_traits;

   //Constructor, does not throw
   vector_alloc_holder(const A &a)
      : members_(a)
   {}

   //Destructor
   ~vector_alloc_holder()
   {
      this->prot_destroy_all();
      this->prot_deallocate();
   }

   typedef containers_detail::integral_constant<unsigned, 1>      allocator_v1;
   typedef containers_detail::integral_constant<unsigned, 2>      allocator_v2;
   typedef containers_detail::integral_constant<unsigned,
      boost::container::containers_detail::version<A>::value> alloc_version;
   std::pair<pointer, bool>
      allocation_command(allocation_type command,
                         size_type limit_size, 
                         size_type preferred_size,
                         size_type &received_size, const pointer &reuse = 0)
   {
      return allocation_command(command, limit_size, preferred_size,
                               received_size, reuse, alloc_version());
   }

   std::pair<pointer, bool>
      allocation_command(allocation_type command,
                         size_type limit_size, 
                         size_type preferred_size,
                         size_type &received_size,
                         const pointer &reuse,
                         allocator_v1)
   {
      (void)limit_size;
      (void)reuse;
      if(!(command & allocate_new))
         return std::pair<pointer, bool>(pointer(0), false);
      received_size = preferred_size;
      return std::make_pair(this->alloc().allocate(received_size), false);
   }

   std::pair<pointer, bool>
      allocation_command(allocation_type command,
                         size_type limit_size, 
                         size_type preferred_size,
                         size_type &received_size,
                         const pointer &reuse,
                         allocator_v2)
   {
      return this->alloc().allocation_command
         (command, limit_size, preferred_size, received_size, reuse);
   }

   size_type next_capacity(size_type additional_objects) const
   {  return get_next_capacity(this->alloc().max_size(), this->members_.m_capacity, additional_objects);  }

   struct members_holder
      : public A
   {
      private:
      members_holder(const members_holder&);

      public:
      members_holder(const A &alloc)
         :  A(alloc), m_start(0), m_size(0), m_capacity(0)
      {}

      pointer     m_start;
      size_type   m_size;
      size_type   m_capacity;
   } members_;

   A &alloc()
   {  return members_;  }

   const A &alloc() const
   {  return members_;  }

   protected:
   void prot_deallocate()
   {
      if(!this->members_.m_capacity)   return;
      this->alloc().deallocate(this->members_.m_start, this->members_.m_capacity);
      this->members_.m_start     = 0;
      this->members_.m_size      = 0;
      this->members_.m_capacity  = 0;
   }

   void destroy(value_type* p)
   {
      if(!value_traits::trivial_dctr)
         containers_detail::get_pointer(p)->~value_type();
   }

   void destroy_n(value_type* p, size_type n)
   {
      if(!value_traits::trivial_dctr)
         for(; n--; ++p)   p->~value_type();
   }

   void prot_destroy_all()
   {
      this->destroy_n(containers_detail::get_pointer(this->members_.m_start), this->members_.m_size);
      this->members_.m_size = 0;
   }
};

}  //namespace containers_detail {
/// @endcond

//! \class vector
//! A vector is a sequence that supports random access to elements, constant 
//! time insertion and removal of elements at the end, and linear time insertion 
//! and removal of elements at the beginning or in the middle. The number of 
//! elements in a vector may vary dynamically; memory management is automatic.
//! boost::container::vector is similar to std::vector but it's compatible
//! with shared memory and memory mapped files.
template <class T, class A>
class vector : private containers_detail::vector_alloc_holder<A>
{
   /// @cond
   typedef vector<T, A>                   self_t;
   typedef containers_detail::vector_alloc_holder<A> base_t;
   typedef typename containers_detail::
      move_const_ref_type<T>::type insert_const_ref_type;
   /// @endcond
   public:
   //! The type of object, T, stored in the vector
   typedef T                                       value_type;
   //! Pointer to T
   typedef typename A::pointer                     pointer;
   //! Const pointer to T
   typedef typename A::const_pointer               const_pointer;
   //! Reference to T
   typedef typename A::reference                   reference;
   //! Const reference to T
   typedef typename A::const_reference             const_reference;
   //! An unsigned integral type
   typedef typename A::size_type                   size_type;
   //! A signed integral type
   typedef typename A::difference_type             difference_type;
   //! The allocator type
   typedef A                                       allocator_type;
   //! The random access iterator
   typedef containers_detail::vector_iterator<pointer>        iterator;
   //! The random access const_iterator
   typedef containers_detail::vector_const_iterator<pointer>  const_iterator;

   //! Iterator used to iterate backwards through a vector. 
   typedef std::reverse_iterator<iterator>   
      reverse_iterator;
   //! Const iterator used to iterate backwards through a vector. 
   typedef std::reverse_iterator<const_iterator>                 
      const_reverse_iterator;
   //! The stored allocator type
   typedef allocator_type                          stored_allocator_type;

   /// @cond
   private:
   BOOST_MOVE_MACRO_COPYABLE_AND_MOVABLE(vector)
   typedef containers_detail::advanced_insert_aux_int<T, T*>    advanced_insert_aux_int_t;
   typedef containers_detail::vector_value_traits<value_type, A> value_traits;

   typedef typename base_t::allocator_v1           allocator_v1;
   typedef typename base_t::allocator_v2           allocator_v2;
   typedef typename base_t::alloc_version          alloc_version;

   typedef constant_iterator<T, difference_type>   cvalue_iterator;
   typedef repeat_iterator<T, difference_type>     repeat_it;
   typedef BOOST_CONTAINER_MOVE_NAMESPACE::move_iterator<repeat_it>         repeat_move_it;
   /// @endcond

   public:

   //! <b>Effects</b>: Constructs a vector taking the allocator as parameter.
   //! 
   //! <b>Throws</b>: If allocator_type's copy constructor throws.
   //! 
   //! <b>Complexity</b>: Constant.
   explicit vector(const A& a = A())
      : base_t(a)
   {}

   //! <b>Effects</b>: Constructs a vector that will use a copy of allocator a
   //!   and inserts n default contructed values.
   //!
   //! <b>Throws</b>: If allocator_type's default constructor or copy constructor
   //!   throws or T's default or copy constructor throws.
   //! 
   //! <b>Complexity</b>: Linear to n.
   explicit vector(size_type n) 
      :  base_t(allocator_type())
   {  this->resize(n); }

   //! <b>Effects</b>: Constructs a vector that will use a copy of allocator a
   //!   and inserts n copies of value.
   //!
   //! <b>Throws</b>: If allocator_type's default constructor or copy constructor
   //!   throws or T's default or copy constructor throws.
   //! 
   //! <b>Complexity</b>: Linear to n.
   vector(size_type n, const T& value, const allocator_type& a = allocator_type()) 
      :  base_t(a)
   {  this->insert(this->cend(), n, value); }

   //! <b>Effects</b>: Copy constructs a vector.
   //!
   //! <b>Postcondition</b>: x == *this.
   //! 
   //! <b>Complexity</b>: Linear to the elements x contains.
   vector(const vector<T, A>& x) 
      :  base_t(static_cast<const base_t&>(x).alloc())
   {  *this = x;  }

   //! <b>Effects</b>: Move constructor. Moves mx's resources to *this.
   //!
   //! <b>Throws</b>: If allocator_type's copy constructor throws.
   //! 
   //! <b>Complexity</b>: Constant.
   vector(BOOST_MOVE_MACRO_RV_REF(vector) mx) 
      :  base_t(static_cast<base_t&>(mx).alloc())
   {  this->swap(mx);   }

   //! <b>Effects</b>: Constructs a vector that will use a copy of allocator a
   //!   and inserts a copy of the range [first, last) in the vector.
   //!
   //! <b>Throws</b>: If allocator_type's default constructor or copy constructor
   //!   throws or T's constructor taking an dereferenced InIt throws.
   //!
   //! <b>Complexity</b>: Linear to the range [first, last).
   template <class InIt>
   vector(InIt first, InIt last, const allocator_type& a = allocator_type())
      :  base_t(a)
   {  this->assign(first, last); }

   //! <b>Effects</b>: Destroys the vector. All stored values are destroyed
   //!   and used memory is deallocated.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Linear to the number of elements.
   ~vector() 
   {} //vector_alloc_holder clears the data

   //! <b>Effects</b>: Returns an iterator to the first element contained in the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   iterator begin()      
   { return iterator(this->members_.m_start); }

   //! <b>Effects</b>: Returns a const_iterator to the first element contained in the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_iterator begin() const
   { return const_iterator(this->members_.m_start); }

   //! <b>Effects</b>: Returns an iterator to the end of the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   iterator end()        
   { return iterator(this->members_.m_start + this->members_.m_size); }

   //! <b>Effects</b>: Returns a const_iterator to the end of the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_iterator end()   const
   { return this->cend(); }

   //! <b>Effects</b>: Returns a reverse_iterator pointing to the beginning 
   //! of the reversed vector. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   reverse_iterator rbegin()     
   { return reverse_iterator(this->end());      }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the beginning 
   //! of the reversed vector. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator rbegin()const
   { return this->crbegin(); }

   //! <b>Effects</b>: Returns a reverse_iterator pointing to the end
   //! of the reversed vector. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   reverse_iterator rend()       
   { return reverse_iterator(this->begin());       }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the end
   //! of the reversed vector. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator rend()  const
   { return this->crend(); }

   //! <b>Effects</b>: Returns a const_iterator to the first element contained in the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_iterator cbegin() const
   { return const_iterator(this->members_.m_start); }

   //! <b>Effects</b>: Returns a const_iterator to the end of the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_iterator cend()   const
   { return const_iterator(this->members_.m_start + this->members_.m_size); }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the beginning 
   //! of the reversed vector. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator crbegin()const
   { return const_reverse_iterator(this->end());}

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the end
   //! of the reversed vector. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator crend()  const
   { return const_reverse_iterator(this->begin()); }

   //! <b>Requires</b>: !empty()
   //!
   //! <b>Effects</b>: Returns a reference to the first element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   reference         front()       
   { return *this->members_.m_start; }

   //! <b>Requires</b>: !empty()
   //!
   //! <b>Effects</b>: Returns a const reference to the first element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reference   front() const 
   { return *this->members_.m_start; }

   //! <b>Requires</b>: !empty()
   //!
   //! <b>Effects</b>: Returns a reference to the first element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   reference         back()        
   { return this->members_.m_start[this->members_.m_size - 1]; }

   //! <b>Effects</b>: Returns a const reference to the first element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reference   back()  const 
   { return this->members_.m_start[this->members_.m_size - 1]; }

   //! <b>Returns</b>: A pointer such that [data(),data() + size()) is a valid range.
   //!   For a non-empty vector, data() == &front().
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   pointer data()        
   { return this->members_.m_start; }

   //! <b>Returns</b>: A pointer such that [data(),data() + size()) is a valid range.
   //!   For a non-empty vector, data() == &front().
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_pointer data()  const 
   { return this->members_.m_start; }

   //! <b>Effects</b>: Returns the number of the elements contained in the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   size_type size() const 
   { return this->members_.m_size; }

   //! <b>Effects</b>: Returns the largest possible size of the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   size_type max_size() const 
   { return this->alloc().max_size(); }

   //! <b>Effects</b>: Number of elements for which memory has been allocated.
   //!   capacity() is always greater than or equal to size().
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   size_type capacity() const 
   { return this->members_.m_capacity; }

   //! <b>Effects</b>: Returns true if the vector contains no elements.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   bool empty() const 
   { return !this->members_.m_size; }

   //! <b>Requires</b>: size() < n.
   //!
   //! <b>Effects</b>: Returns a reference to the nth element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   reference operator[](size_type n)         
   { return this->members_.m_start[n]; }

   //! <b>Requires</b>: size() < n.
   //!
   //! <b>Effects</b>: Returns a const reference to the nth element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reference operator[](size_type n) const   
   { return this->members_.m_start[n]; }

   //! <b>Requires</b>: size() < n.
   //!
   //! <b>Effects</b>: Returns a reference to the nth element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: std::range_error if n >= size()
   //! 
   //! <b>Complexity</b>: Constant.
   reference at(size_type n)
   { this->priv_check_range(n); return this->members_.m_start[n]; }

   //! <b>Requires</b>: size() < n.
   //!
   //! <b>Effects</b>: Returns a const reference to the nth element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: std::range_error if n >= size()
   //! 
   //! <b>Complexity</b>: Constant.
   const_reference at(size_type n) const
   { this->priv_check_range(n); return this->members_.m_start[n]; }

   //! <b>Effects</b>: Returns a copy of the internal allocator.
   //! 
   //! <b>Throws</b>: If allocator's copy constructor throws.
   //! 
   //! <b>Complexity</b>: Constant.
   allocator_type get_allocator() const 
   { return this->alloc();  }

   const stored_allocator_type &get_stored_allocator() const 
   {  return this->alloc(); }

   stored_allocator_type &get_stored_allocator()
   {  return this->alloc(); }

   //! <b>Effects</b>: If n is less than or equal to capacity(), this call has no
   //!   effect. Otherwise, it is a request for allocation of additional memory.
   //!   If the request is successful, then capacity() is greater than or equal to
   //!   n; otherwise, capacity() is unchanged. In either case, size() is unchanged.
   //! 
   //! <b>Throws</b>: If memory allocation allocation throws or T's copy constructor throws.
   void reserve(size_type new_cap)
   {
      if (this->capacity() < new_cap){
         //There is not enough memory, allocate a new
         //buffer or expand the old one.
         bool same_buffer_start;
         size_type real_cap = 0;
         std::pair<pointer, bool> ret =
            this->allocation_command
               (allocate_new | expand_fwd | expand_bwd,
                  new_cap, new_cap, real_cap, this->members_.m_start);

         //Check for forward expansion
         same_buffer_start = ret.second && this->members_.m_start == ret.first;
         if(same_buffer_start){
            #ifdef BOOST_CONTAINERS_VECTOR_ALLOC_STATS
            ++this->num_expand_fwd;
            #endif
            this->members_.m_capacity  = real_cap;
         }

         //If there is no forward expansion, move objects
         else{
            //We will reuse insert code, so create a dummy input iterator
            T *dummy_it(containers_detail::get_pointer(this->members_.m_start));
            containers_detail::advanced_insert_aux_proxy<T, BOOST_CONTAINER_MOVE_NAMESPACE::move_iterator<T*>, T*>
               proxy(::BOOST_CONTAINER_MOVE_NAMESPACE::make_move_iterator(dummy_it), ::BOOST_CONTAINER_MOVE_NAMESPACE::make_move_iterator(dummy_it));
            //Backwards (and possibly forward) expansion
            if(ret.second){
               #ifdef BOOST_CONTAINERS_VECTOR_ALLOC_STATS
               ++this->num_expand_bwd;
               #endif
               this->priv_range_insert_expand_backwards
                  ( containers_detail::get_pointer(ret.first)
                  , real_cap
                  , containers_detail::get_pointer(this->members_.m_start)
                  , 0
                  , proxy);
            }
            //New buffer
            else{
               #ifdef BOOST_CONTAINERS_VECTOR_ALLOC_STATS
               ++this->num_alloc;
               #endif
               this->priv_range_insert_new_allocation
                  ( containers_detail::get_pointer(ret.first)
                  , real_cap
                  , containers_detail::get_pointer(this->members_.m_start)
                  , 0
                  , proxy);
            }
         }
      }
   }

   //! <b>Effects</b>: Makes *this contain the same elements as x.
   //!
   //! <b>Postcondition</b>: this->size() == x.size(). *this contains a copy 
   //! of each of x's elements. 
   //!
   //! <b>Throws</b>: If memory allocation throws or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to the number of elements in x.
   vector& operator=(BOOST_MOVE_MACRO_COPY_ASSIGN_REF(vector) x)
   {
      if (&x != this){
         this->assign(x.members_.m_start, x.members_.m_start + x.members_.m_size);
      }
      return *this;
   }

   //! <b>Effects</b>: Move assignment. All mx's values are transferred to *this.
   //!
   //! <b>Postcondition</b>: x.empty(). *this contains a the elements x had
   //!   before the function.
   //!
   //! <b>Throws</b>: If allocator_type's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear.
   vector& operator=(BOOST_MOVE_MACRO_RV_REF(vector) x)
   {
      if (&x != this){
         this->swap(x);
         x.clear();
      }
      return *this;
   }

   //! <b>Effects</b>: Assigns the n copies of val to *this.
   //!
   //! <b>Throws</b>: If memory allocation throws or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to n.
   void assign(size_type n, const value_type& val)
   {  this->assign(cvalue_iterator(val, n), cvalue_iterator());   }

   //! <b>Effects</b>: Assigns the the range [first, last) to *this.
   //!
   //! <b>Throws</b>: If memory allocation throws or
   //!   T's constructor from dereferencing InpIt throws.
   //!
   //! <b>Complexity</b>: Linear to n.
   template <class InIt>
   void assign(InIt first, InIt last) 
   {
      //Dispatch depending on integer/iterator
      const bool aux_boolean = containers_detail::is_convertible<InIt, std::size_t>::value;
      typedef containers_detail::bool_<aux_boolean> Result;
      this->priv_assign_dispatch(first, last, Result());
   }

   //! <b>Effects</b>: Inserts a copy of x at the end of the vector.
   //!
   //! <b>Throws</b>: If memory allocation throws or
   //!   T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Amortized constant time.
   void push_back(insert_const_ref_type x) 
   {  return priv_push_back(x);  }

   #if defined(BOOST_NO_RVALUE_REFERENCES) && !defined(BOOST_MOVE_DOXYGEN_INVOKED)
   void push_back(T &x) { push_back(const_cast<const T &>(x)); }

   template<class U>
   void push_back(const U &u, typename containers_detail::enable_if_c
                  <containers_detail::is_same<T, U>::value &&
                   !::BOOST_CONTAINER_MOVE_NAMESPACE::is_movable<U>::value
                  >::type* =0)
   { return priv_push_back(u); }
   #endif

   //! <b>Effects</b>: Constructs a new element in the end of the vector
   //!   and moves the resources of mx to this new element.
   //!
   //! <b>Throws</b>: If memory allocation throws.
   //!
   //! <b>Complexity</b>: Amortized constant time.
   void push_back(BOOST_MOVE_MACRO_RV_REF(T) x) 
   {
      if (this->members_.m_size < this->members_.m_capacity){
         //There is more memory, just construct a new object at the end
         new((void*)containers_detail::get_pointer(this->members_.m_start + this->members_.m_size))value_type(BOOST_CONTAINER_MOVE_NAMESPACE::move(x));
         ++this->members_.m_size;
      }
      else{
         this->insert(this->cend(), BOOST_CONTAINER_MOVE_NAMESPACE::move(x));
      }
   }

   #if defined(BOOST_CONTAINERS_PERFECT_FORWARDING) || defined(BOOST_CONTAINER_DOXYGEN_INVOKED)

   //! <b>Effects</b>: Inserts an object of type T constructed with
   //!   std::forward<Args>(args)... in the end of the vector.
   //!
   //! <b>Throws</b>: If memory allocation throws or the in-place constructor throws.
   //!
   //! <b>Complexity</b>: Amortized constant time.
   template<class ...Args>
   void emplace_back(Args &&...args)
   {
      T* back_pos = containers_detail::get_pointer(this->members_.m_start) + this->members_.m_size;
      if (this->members_.m_size < this->members_.m_capacity){
         //There is more memory, just construct a new object at the end
         new((void*)(back_pos))value_type(BOOST_CONTAINER_MOVE_NAMESPACE::forward<Args>(args)...);
         ++this->members_.m_size;
      }
      else{
         typedef containers_detail::advanced_insert_aux_emplace<T, T*, Args...> type;
         type &&proxy = type(BOOST_CONTAINER_MOVE_NAMESPACE::forward<Args>(args)...);
         priv_range_insert(back_pos, 1, proxy);
      }
   }

   //! <b>Requires</b>: position must be a valid iterator of *this.
   //!
   //! <b>Effects</b>: Inserts an object of type T constructed with
   //!   std::forward<Args>(args)... before position
   //!
   //! <b>Throws</b>: If memory allocation throws or the in-place constructor throws.
   //!
   //! <b>Complexity</b>: If position is end(), amortized constant time
   //!   Linear time otherwise.
   template<class ...Args>
   iterator emplace(const_iterator position, Args && ...args)
   {
      //Just call more general insert(pos, size, value) and return iterator
      size_type pos_n = position - cbegin();
      typedef containers_detail::advanced_insert_aux_emplace<T, T*, Args...> type;
      type &&proxy = type(BOOST_CONTAINER_MOVE_NAMESPACE::forward<Args>(args)...);
      priv_range_insert(position.get_ptr(), 1, proxy);
      return iterator(this->members_.m_start + pos_n);
   }

   #else

   void emplace_back()
   {
      T* back_pos = containers_detail::get_pointer(this->members_.m_start) + this->members_.m_size;
      if (this->members_.m_size < this->members_.m_capacity){
         //There is more memory, just construct a new object at the end
         new((void*)(back_pos))value_type();
         ++this->members_.m_size;
      }
      else{
         containers_detail::advanced_insert_aux_emplace<value_type, T*> proxy;
         priv_range_insert(back_pos, 1, proxy);
      }
   }

   iterator emplace(const_iterator position)
   {
      size_type pos_n = position - cbegin();
      containers_detail::advanced_insert_aux_emplace<value_type, T*> proxy;
      priv_range_insert(containers_detail::get_pointer(position.get_ptr()), 1, proxy);
      return iterator(this->members_.m_start + pos_n);
   }

   #define BOOST_PP_LOCAL_MACRO(n)                                                              \
   template<BOOST_PP_ENUM_PARAMS(n, class P)>                                                   \
   void emplace_back(BOOST_PP_ENUM(n, BOOST_CONTAINERS_PP_PARAM_LIST, _))                     \
   {                                                                                            \
      T* back_pos = containers_detail::get_pointer(this->members_.m_start) + this->members_.m_size;        \
      if (this->members_.m_size < this->members_.m_capacity){                                   \
         new((void*)(back_pos))value_type                                                       \
            (BOOST_PP_ENUM(n, BOOST_CONTAINERS_PP_PARAM_FORWARD, _));                         \
         ++this->members_.m_size;                                                               \
      }                                                                                         \
      else{                                                                                     \
         containers_detail::BOOST_PP_CAT(BOOST_PP_CAT(advanced_insert_aux_emplace, n), arg)                \
            <value_type, T*, BOOST_PP_ENUM_PARAMS(n, P)>                                        \
               proxy(BOOST_PP_ENUM(n, BOOST_CONTAINERS_PP_PARAM_FORWARD, _));                 \
         priv_range_insert(back_pos, 1, proxy);                                                 \
      }                                                                                         \
   }                                                                                            \
                                                                                                \
   template<BOOST_PP_ENUM_PARAMS(n, class P)>                                                   \
   iterator emplace(const_iterator pos, BOOST_PP_ENUM(n, BOOST_CONTAINERS_PP_PARAM_LIST, _))  \
   {                                                                                            \
      size_type pos_n = pos - cbegin();                                                         \
      containers_detail::BOOST_PP_CAT(BOOST_PP_CAT(advanced_insert_aux_emplace, n), arg)                   \
         <value_type, T*, BOOST_PP_ENUM_PARAMS(n, P)>                                           \
            proxy(BOOST_PP_ENUM(n, BOOST_CONTAINERS_PP_PARAM_FORWARD, _));                    \
      priv_range_insert(containers_detail::get_pointer(pos.get_ptr()), 1, proxy);                          \
      return iterator(this->members_.m_start + pos_n);                                          \
   }                                                                                            \
   //!
   #define BOOST_PP_LOCAL_LIMITS (1, BOOST_CONTAINERS_MAX_CONSTRUCTOR_PARAMETERS)
   #include BOOST_PP_LOCAL_ITERATE()

   #endif   //#ifdef BOOST_CONTAINERS_PERFECT_FORWARDING
  
   //! <b>Effects</b>: Swaps the contents of *this and x.
   //!   If this->allocator_type() != x.allocator_type()
   //!   allocators are also swapped.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   void swap(vector& x)
   {
      allocator_type &this_al = this->alloc(), &other_al = x.alloc();
      //Just swap internals
      containers_detail::do_swap(this->members_.m_start, x.members_.m_start);
      containers_detail::do_swap(this->members_.m_size, x.members_.m_size);
      containers_detail::do_swap(this->members_.m_capacity, x.members_.m_capacity);

      if (this_al != other_al){
         containers_detail::do_swap(this_al, other_al);
      }
   }

   //! <b>Requires</b>: position must be a valid iterator of *this.
   //!
   //! <b>Effects</b>: Insert a copy of x before position.
   //!
   //! <b>Throws</b>: If memory allocation throws or x's copy constructor throws.
   //!
   //! <b>Complexity</b>: If position is end(), amortized constant time
   //!   Linear time otherwise.
   iterator insert(const_iterator position, insert_const_ref_type x) 
   {  return this->priv_insert(position, x); }

   #if defined(BOOST_NO_RVALUE_REFERENCES) && !defined(BOOST_MOVE_DOXYGEN_INVOKED)
   iterator insert(const_iterator position, T &x) { return this->insert(position, const_cast<const T &>(x)); }

   template<class U>
   iterator insert(const_iterator position, const U &u, typename containers_detail::enable_if_c<containers_detail::is_same<T, U>::value && !::BOOST_CONTAINER_MOVE_NAMESPACE::is_movable<U>::value >::type* =0)
   {  return this->priv_insert(position, u); }
   #endif

   //! <b>Requires</b>: position must be a valid iterator of *this.
   //!
   //! <b>Effects</b>: Insert a new element before position with mx's resources.
   //!
   //! <b>Throws</b>: If memory allocation throws.
   //!
   //! <b>Complexity</b>: If position is end(), amortized constant time
   //!   Linear time otherwise.
   iterator insert(const_iterator position, BOOST_MOVE_MACRO_RV_REF(T) x) 
   {
      //Just call more general insert(pos, size, value) and return iterator
      size_type pos_n = position - cbegin();
      this->insert(position
                  ,repeat_move_it(repeat_it(x, 1))
                  ,repeat_move_it(repeat_it()));
      return iterator(this->members_.m_start + pos_n);
   }

   //! <b>Requires</b>: pos must be a valid iterator of *this.
   //!
   //! <b>Effects</b>: Insert a copy of the [first, last) range before pos.
   //!
   //! <b>Throws</b>: If memory allocation throws, T's constructor from a
   //!   dereferenced InpIt throws or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to std::distance [first, last).
   template <class InIt>
   void insert(const_iterator pos, InIt first, InIt last)
   {
      //Dispatch depending on integer/iterator
      const bool aux_boolean = containers_detail::is_convertible<InIt, std::size_t>::value;
      typedef containers_detail::bool_<aux_boolean> Result;
      this->priv_insert_dispatch(pos, first, last, Result());
   }

   //! <b>Requires</b>: pos must be a valid iterator of *this.
   //!
   //! <b>Effects</b>: Insert n copies of x before pos.
   //!
   //! <b>Throws</b>: If memory allocation throws or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to n.
   void insert(const_iterator p, size_type n, const T& x)
   {  this->insert(p, cvalue_iterator(x, n), cvalue_iterator());  }

   //! <b>Effects</b>: Removes the last element from the vector.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant time.
   void pop_back() 
   {
      //Destroy last element
      --this->members_.m_size;
      this->destroy(containers_detail::get_pointer(this->members_.m_start) + this->members_.m_size);
   }

   //! <b>Effects</b>: Erases the element at position pos.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Linear to the elements between pos and the 
   //!   last element. Constant if pos is the first or the last element.
   iterator erase(const_iterator position) 
   {
      T *pos = containers_detail::get_pointer(position.get_ptr());
      T *beg = containers_detail::get_pointer(this->members_.m_start);
      BOOST_CONTAINER_MOVE_NAMESPACE::move(pos + 1, beg + this->members_.m_size, pos);
      --this->members_.m_size;
      //Destroy last element
      base_t::destroy(containers_detail::get_pointer(this->members_.m_start) + this->members_.m_size);
      return iterator(position.get_ptr());
   }

   //! <b>Effects</b>: Erases the elements pointed by [first, last).
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Linear to the distance between first and last.
   iterator erase(const_iterator first, const_iterator last) 
   {
      if (first != last){   // worth doing, copy down over hole
         T* end_pos = containers_detail::get_pointer(this->members_.m_start) + this->members_.m_size;
         T* ptr = containers_detail::get_pointer(BOOST_CONTAINER_MOVE_NAMESPACE::move
            (containers_detail::get_pointer(last.get_ptr())
            ,end_pos
            ,containers_detail::get_pointer(first.get_ptr())
            ));
         size_type destroyed = (end_pos - ptr);
         this->destroy_n(ptr, destroyed);
         this->members_.m_size -= destroyed;
      }
      return iterator(first.get_ptr());
   }

   //! <b>Effects</b>: Inserts or erases elements at the end such that
   //!   the size becomes n. New elements are copy constructed from x.
   //!
   //! <b>Throws</b>: If memory allocation throws, or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to the difference between size() and new_size.
   void resize(size_type new_size, const T& x) 
   {
      pointer finish = this->members_.m_start + this->members_.m_size;
      if (new_size < size()){
         //Destroy last elements
         this->erase(const_iterator(this->members_.m_start + new_size), this->end());
      }
      else{
         //Insert new elements at the end
         this->insert(const_iterator(finish), new_size - this->size(), x);
      }
   }

   //! <b>Effects</b>: Inserts or erases elements at the end such that
   //!   the size becomes n. New elements are default constructed.
   //!
   //! <b>Throws</b>: If memory allocation throws, or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to the difference between size() and new_size.
   void resize(size_type new_size) 
   {
      if (new_size < this->size()){
         //Destroy last elements
         this->erase(const_iterator(this->members_.m_start + new_size), this->end());
      }
      else{
         size_type n = new_size - this->size();
         this->reserve(new_size);
         containers_detail::default_construct_aux_proxy<T, T*, size_type> proxy(n);
         priv_range_insert(this->cend().get_ptr(), n, proxy);
      }
   }

   //! <b>Effects</b>: Erases all the elements of the vector.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the vector.
   void clear() 
   {  this->prot_destroy_all();  }

   //! <b>Effects</b>: Tries to deallocate the excess of memory created
   //!   with previous allocations. The size of the vector is unchanged
   //!
   //! <b>Throws</b>: If memory allocation throws, or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to size().
   void shrink_to_fit()
   {  priv_shrink_to_fit(alloc_version());   }

   /// @cond

   private:
   iterator priv_insert(const_iterator position, const T &x) 
   {
      //Just call more general insert(pos, size, value) and return iterator
      size_type pos_n = position - cbegin();
      this->insert(position, (size_type)1, x);
      return iterator(this->members_.m_start + pos_n);
   }

   void priv_push_back(const T &x) 
   {
      if (this->members_.m_size < this->members_.m_capacity){
         //There is more memory, just construct a new object at the end
         new((void*)(containers_detail::get_pointer(this->members_.m_start) + this->members_.m_size))value_type(x);
         ++this->members_.m_size;
      }
      else{
         this->insert(this->cend(), x);
      }
   }

   void priv_shrink_to_fit(allocator_v1)
   {
      if(this->members_.m_capacity){
         if(!size()){
            this->prot_deallocate();
         }
         else{
            //This would not work with stateful allocators
            vector<T, A>(*this).swap(*this);
         }
      }
   }

   void priv_shrink_to_fit(allocator_v2)
   {
      if(this->members_.m_capacity){
         if(!size()){
            this->prot_deallocate();
         }
         else{
            size_type received_size;
            if(this->alloc().allocation_command
               ( shrink_in_place | nothrow_allocation
               , this->capacity(), this->size()
               , received_size,   this->members_.m_start).first){
               this->members_.m_capacity = received_size;
               #ifdef BOOST_CONTAINERS_VECTOR_ALLOC_STATS
               ++this->num_shrink;
               #endif
            }
         }
      }
   }

   template <class FwdIt>
   void priv_range_insert(pointer pos, FwdIt first, FwdIt last, std::forward_iterator_tag)
   {
      if(first != last){        
         const size_type n = std::distance(first, last);
         containers_detail::advanced_insert_aux_proxy<T, FwdIt, T*> proxy(first, last);
         priv_range_insert(pos, n, proxy);
      }
   }

   void priv_range_insert(pointer pos, const size_type n, advanced_insert_aux_int_t &interf)
   {
      //Check if we have enough memory or try to expand current memory
      size_type remaining = this->members_.m_capacity - this->members_.m_size;
      bool same_buffer_start;
      std::pair<pointer, bool> ret;
      size_type real_cap = this->members_.m_capacity;

      //Check if we already have room
      if (n <= remaining){
         same_buffer_start = true;
      }
      else{
         //There is not enough memory, allocate a new
         //buffer or expand the old one.
         size_type new_cap = this->next_capacity(n);
         ret = this->allocation_command
               (allocate_new | expand_fwd | expand_bwd,
                  this->members_.m_size + n, new_cap, real_cap, this->members_.m_start);

         //Check for forward expansion
         same_buffer_start = ret.second && this->members_.m_start == ret.first;
         if(same_buffer_start){
            this->members_.m_capacity  = real_cap;
         }
      }
      
      //If we had room or we have expanded forward
      if (same_buffer_start){
         #ifdef BOOST_CONTAINERS_VECTOR_ALLOC_STATS
         ++this->num_expand_fwd;
         #endif
         this->priv_range_insert_expand_forward
            (containers_detail::get_pointer(pos), n, interf);
      }
      //Backwards (and possibly forward) expansion
      else if(ret.second){
         #ifdef BOOST_CONTAINERS_VECTOR_ALLOC_STATS
         ++this->num_expand_bwd;
         #endif
         this->priv_range_insert_expand_backwards
            ( containers_detail::get_pointer(ret.first)
            , real_cap
            , containers_detail::get_pointer(pos)
            , n
            , interf);
      }
      //New buffer
      else{
         #ifdef BOOST_CONTAINERS_VECTOR_ALLOC_STATS
         ++this->num_alloc;
         #endif
         this->priv_range_insert_new_allocation
            ( containers_detail::get_pointer(ret.first)
            , real_cap
            , containers_detail::get_pointer(pos)
            , n
            , interf);
      }
   }

   void priv_range_insert_expand_forward(T* pos, size_type n, advanced_insert_aux_int_t &interf)
   {
      //n can't be 0, because there is nothing to do in that case
      if(!n) return;
      //There is enough memory
      T* old_finish = containers_detail::get_pointer(this->members_.m_start) + this->members_.m_size;
      const size_type elems_after = old_finish - pos;

      if (elems_after > n){
         //New elements can be just copied.
         //Move to uninitialized memory last objects
         ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_move(old_finish - n, old_finish, old_finish);
         this->members_.m_size += n;
         //Copy previous to last objects to the initialized end
         BOOST_CONTAINER_MOVE_NAMESPACE::move_backward(pos, old_finish - n, old_finish);
         //Insert new objects in the pos
         interf.copy_all_to(pos);
      }
      else {
         //The new elements don't fit in the [pos, end()) range. Copy
         //to the beginning of the unallocated zone the last new elements.
         interf.uninitialized_copy_some_and_update(old_finish, elems_after, false);
         this->members_.m_size += n - elems_after;
         //Copy old [pos, end()) elements to the uninitialized memory
         ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_move
            ( pos, old_finish, containers_detail::get_pointer(this->members_.m_start) + this->members_.m_size);
         this->members_.m_size += elems_after;
         //Copy first new elements in pos
         interf.copy_all_to(pos);
      }
   }

   void priv_range_insert_new_allocation
      (T* new_start, size_type new_cap, T* pos, size_type n, advanced_insert_aux_int_t &interf)
   {
      //n can be zero, if we want to reallocate!
      T *new_finish = new_start;
      T *old_finish;
      //Anti-exception rollbacks
      typename value_traits::UCopiedArrayDeallocator scoped_alloc(new_start, this->alloc(), new_cap);
      typename value_traits::UCopiedArrayDestructor constructed_values_destroyer(new_start, 0u);

      //Initialize with [begin(), pos) old buffer 
      //the start of the new buffer
      T *old_buffer = containers_detail::get_pointer(this->members_.m_start);
      if(old_buffer){
         new_finish = ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_move
            (containers_detail::get_pointer(this->members_.m_start), pos, old_finish = new_finish);
         constructed_values_destroyer.increment_size(new_finish - old_finish);
      }
      //Initialize new objects, starting from previous point
      interf.uninitialized_copy_all_to(old_finish = new_finish);
      new_finish += n;
      constructed_values_destroyer.increment_size(new_finish - old_finish);
      //Initialize from the rest of the old buffer, 
      //starting from previous point
      if(old_buffer){
         new_finish = ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_move
            (pos, old_buffer + this->members_.m_size, new_finish);
         //Destroy and deallocate old elements
         //If there is allocated memory, destroy and deallocate
         if(!value_traits::trivial_dctr_after_move)
            this->destroy_n(old_buffer, this->members_.m_size); 
         this->alloc().deallocate(this->members_.m_start, this->members_.m_capacity);
      }
      this->members_.m_start     = new_start;
      this->members_.m_size      = new_finish - new_start;
      this->members_.m_capacity  = new_cap;
      //All construction successful, disable rollbacks
      constructed_values_destroyer.release();
      scoped_alloc.release();
   }

   void priv_range_insert_expand_backwards
         (T* new_start, size_type new_capacity,
          T* pos, const size_type n, advanced_insert_aux_int_t &interf)
   {
      //n can be zero to just expand capacity
      //Backup old data
      T* old_start  = containers_detail::get_pointer(this->members_.m_start);
      T* old_finish = old_start + this->members_.m_size;
      size_type old_size = this->members_.m_size;

      //We can have 8 possibilities:
      const size_type elemsbefore   = (size_type)(pos - old_start);
      const size_type s_before      = (size_type)(old_start - new_start);

      //Update the vector buffer information to a safe state
      this->members_.m_start      = new_start;
      this->members_.m_capacity   = new_capacity;
      this->members_.m_size = 0;

      //If anything goes wrong, this object will destroy
      //all the old objects to fulfill previous vector state
      typename value_traits::OldArrayDestructor old_values_destroyer(old_start, old_size);
      //Check if s_before is big enough to hold the beginning of old data + new data
      if(difference_type(s_before) >= difference_type(elemsbefore + n)){
         //Copy first old values before pos, after that the new objects
         ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_move(old_start, pos, new_start);
         this->members_.m_size = elemsbefore;
         interf.uninitialized_copy_all_to(new_start + elemsbefore);
         this->members_.m_size += n;
         //Check if s_before is so big that even copying the old data + new data
         //there is a gap between the new data and the old data
         if(s_before >= (old_size + n)){
            //Old situation:
            // _________________________________________________________
            //|            raw_mem                | old_begin | old_end |
            //| __________________________________|___________|_________|
            //
            //New situation:
            // _________________________________________________________
            //| old_begin |    new   | old_end |         raw_mem        |
            //|___________|__________|_________|________________________|
            //
            //Now initialize the rest of memory with the last old values
            ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_move
               (pos, old_finish, new_start + elemsbefore + n);
            //All new elements correctly constructed, avoid new element destruction
            this->members_.m_size = old_size + n;
            //Old values destroyed automatically with "old_values_destroyer"
            //when "old_values_destroyer" goes out of scope unless the have trivial
            //destructor after move.
            if(value_traits::trivial_dctr_after_move)
               old_values_destroyer.release();
         }
         //s_before is so big that divides old_end
         else{
            //Old situation:
            // __________________________________________________
            //|            raw_mem         | old_begin | old_end |
            //| ___________________________|___________|_________|
            //
            //New situation:
            // __________________________________________________
            //| old_begin |   new    | old_end |  raw_mem        |
            //|___________|__________|_________|_________________|
            //
            //Now initialize the rest of memory with the last old values
            //All new elements correctly constructed, avoid new element destruction
            size_type raw_gap = s_before - (elemsbefore + n);
            //Now initialize the rest of s_before memory with the
            //first of elements after new values
            ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_move(pos, pos + raw_gap, new_start + elemsbefore + n);
            //Update size since we have a contiguous buffer
            this->members_.m_size = old_size + s_before;
            //All new elements correctly constructed, avoid old element destruction
            old_values_destroyer.release();
            //Now copy remaining last objects in the old buffer begin
            T *to_destroy = BOOST_CONTAINER_MOVE_NAMESPACE::move(pos + raw_gap, old_finish, old_start);
            //Now destroy redundant elements except if they were moved and
            //they have trivial destructor after move
            size_type n_destroy =  old_finish - to_destroy;
            if(!value_traits::trivial_dctr_after_move)
               this->destroy_n(to_destroy, n_destroy);
            this->members_.m_size -= n_destroy;
         }
      }
      else{
         //Check if we have to do the insertion in two phases
         //since maybe s_before is not big enough and
         //the buffer was expanded both sides
         //
         //Old situation:
         // _________________________________________________
         //| raw_mem | old_begin + old_end |  raw_mem        |
         //|_________|_____________________|_________________|
         //
         //New situation with do_after:
         // _________________________________________________
         //|     old_begin + new + old_end     |  raw_mem    |
         //|___________________________________|_____________|
         //
         //New without do_after:
         // _________________________________________________
         //| old_begin + new + old_end  |  raw_mem           |
         //|____________________________|____________________|
         //
         bool do_after    = n > s_before;

         //Now we can have two situations: the raw_mem of the
         //beginning divides the old_begin, or the new elements:
         if (s_before <= elemsbefore) {
            //The raw memory divides the old_begin group:
            //
            //If we need two phase construction (do_after)
            //new group is divided in new = new_beg + new_end groups
            //In this phase only new_beg will be inserted
            //
            //Old situation:
            // _________________________________________________
            //| raw_mem | old_begin | old_end |  raw_mem        |
            //|_________|___________|_________|_________________|
            //
            //New situation with do_after(1):
            //This is not definitive situation, the second phase
            //will include
            // _________________________________________________
            //| old_begin | new_beg | old_end |  raw_mem        |
            //|___________|_________|_________|_________________|
            //
            //New situation without do_after:
            // _________________________________________________
            //| old_begin | new | old_end |  raw_mem            |
            //|___________|_____|_________|_____________________|
            //
            //Copy the first part of old_begin to raw_mem
            T *start_n = old_start + difference_type(s_before); 
            ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_move(old_start, start_n, new_start);
            //The buffer is all constructed until old_end,
            //release destroyer and update size
            old_values_destroyer.release();
            this->members_.m_size = old_size + s_before;
            //Now copy the second part of old_begin overwriting himself
            T* next = BOOST_CONTAINER_MOVE_NAMESPACE::move(start_n, pos, old_start);
            if(do_after){
               //Now copy the new_beg elements
               interf.copy_some_and_update(next, s_before, true);
            }
            else{
               //Now copy the all the new elements
               interf.copy_all_to(next);
               T* move_start = next + n;
               //Now displace old_end elements
               T* move_end   = BOOST_CONTAINER_MOVE_NAMESPACE::move(pos, old_finish, move_start);
               //Destroy remaining moved elements from old_end except if
               //they have trivial destructor after being moved
               difference_type n_destroy = s_before - n;
               if(!value_traits::trivial_dctr_after_move)
                  this->destroy_n(move_end, n_destroy);
               this->members_.m_size -= n_destroy;
            }
         }
         else {
            //If we have to expand both sides,
            //we will play if the first new values so
            //calculate the upper bound of new values

            //The raw memory divides the new elements
            //
            //If we need two phase construction (do_after)
            //new group is divided in new = new_beg + new_end groups
            //In this phase only new_beg will be inserted
            //
            //Old situation:
            // _______________________________________________________
            //|   raw_mem     | old_begin | old_end |  raw_mem        |
            //|_______________|___________|_________|_________________|
            //
            //New situation with do_after():
            // ____________________________________________________
            //| old_begin |    new_beg    | old_end |  raw_mem     |
            //|___________|_______________|_________|______________|
            //
            //New situation without do_after:
            // ______________________________________________________
            //| old_begin | new | old_end |  raw_mem                 |
            //|___________|_____|_________|__________________________|
            //
            //First copy whole old_begin and part of new to raw_mem
            ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_move(old_start, pos, new_start);
            this->members_.m_size = elemsbefore;

            const size_type mid_n = difference_type(s_before) - elemsbefore;
            interf.uninitialized_copy_some_and_update(new_start + elemsbefore, mid_n, true);
            this->members_.m_size = old_size + s_before;
            //The buffer is all constructed until old_end,
            //release destroyer and update size
            old_values_destroyer.release();

            if(do_after){
               //Copy new_beg part
               interf.copy_some_and_update(old_start, s_before - mid_n, true);
            }
            else{
               //Copy all new elements
               interf.copy_all_to(old_start);
               T* move_start = old_start + (n-mid_n);
               //Displace old_end
               T* move_end = BOOST_CONTAINER_MOVE_NAMESPACE::move(pos, old_finish, move_start);
               //Destroy remaining moved elements from old_end except if they
               //have trivial destructor after being moved
               difference_type n_destroy = s_before - n;
               if(!value_traits::trivial_dctr_after_move)
                  this->destroy_n(move_end, n_destroy);
               this->members_.m_size -= n_destroy;
            }
         }

         //This is only executed if two phase construction is needed
         //This can be executed without exception handling since we
         //have to just copy and append in raw memory and
         //old_values_destroyer has been released in phase 1.
         if(do_after){
            //The raw memory divides the new elements
            //
            //Old situation:
            // ______________________________________________________
            //|   raw_mem    | old_begin |  old_end   |  raw_mem     |
            //|______________|___________|____________|______________|
            //
            //New situation with do_after(1):
            // _______________________________________________________
            //| old_begin   +   new_beg  | new_end |old_end | raw_mem |
            //|__________________________|_________|________|_________|
            //
            //New situation with do_after(2):
            // ______________________________________________________
            //| old_begin      +       new            | old_end |raw |
            //|_______________________________________|_________|____|
            //
            const size_type n_after  = n - s_before;
            const difference_type elemsafter = old_size - elemsbefore;

            //We can have two situations:
            if (elemsafter > difference_type(n_after)){
               //The raw_mem from end will divide displaced old_end
               //
               //Old situation:
               // ______________________________________________________
               //|   raw_mem    | old_begin |  old_end   |  raw_mem     |
               //|______________|___________|____________|______________|
               //
               //New situation with do_after(1):
               // _______________________________________________________
               //| old_begin   +   new_beg  | new_end |old_end | raw_mem |
               //|__________________________|_________|________|_________|
               //
               //First copy the part of old_end raw_mem
               T* finish_n = old_finish - difference_type(n_after);
               ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_move(finish_n, old_finish, old_finish);
               this->members_.m_size += n_after;
               //Displace the rest of old_end to the new position
               BOOST_CONTAINER_MOVE_NAMESPACE::move_backward(pos, finish_n, old_finish);
               //Now overwrite with new_end
               //The new_end part is [first + (n - n_after), last)
               interf.copy_all_to(pos);
            }
            else {
               //The raw_mem from end will divide new_end part
               //
               //Old situation:
               // _____________________________________________________________
               //|   raw_mem    | old_begin |  old_end   |  raw_mem            |
               //|______________|___________|____________|_____________________|
               //
               //New situation with do_after(2):
               // _____________________________________________________________
               //| old_begin   +   new_beg  |     new_end   |old_end | raw_mem |
               //|__________________________|_______________|________|_________|
               //
               size_type mid_last_dist = n_after - elemsafter;
               //First initialize data in raw memory
               //The new_end part is [first + (n - n_after), last)
               interf.uninitialized_copy_some_and_update(old_finish, elemsafter, false);
               this->members_.m_size += mid_last_dist;
               ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_move(pos, old_finish, old_finish + mid_last_dist);
               this->members_.m_size += n_after - mid_last_dist;
               //Now copy the part of new_end over constructed elements
               interf.copy_all_to(pos);
            }
         }
      }
   }

   template <class InIt>
   void priv_range_insert(const_iterator pos, InIt first, InIt last, std::input_iterator_tag)
   {
      for(;first != last; ++first){
         this->insert(pos, BOOST_CONTAINER_MOVE_NAMESPACE::move(value_type(*first)));
      }
   }

   template <class InIt>
   void priv_assign_aux(InIt first, InIt last, std::input_iterator_tag)
   {
      //Overwrite all elements we can from [first, last)
      iterator cur = begin();
      for ( ; first != last && cur != end(); ++cur, ++first){
         *cur = *first;
      }

      if (first == last){
         //There are no more elements in the sequence, erase remaining
         this->erase(cur, cend());
      }
      else{
         //There are more elements in the range, insert the remaining ones
         this->insert(this->cend(), first, last);
      }
   }

   template <class FwdIt>
   void priv_assign_aux(FwdIt first, FwdIt last, std::forward_iterator_tag)
   {
      size_type n = std::distance(first, last);
      if(!n){
         this->prot_destroy_all();
         return;
      }
      //Check if we have enough memory or try to expand current memory
      size_type remaining = this->members_.m_capacity - this->members_.m_size;
      bool same_buffer_start;
      std::pair<pointer, bool> ret;
      size_type real_cap = this->members_.m_capacity;

      if (n <= remaining){
         same_buffer_start = true;
      }
      else{
         //There is not enough memory, allocate a new buffer
         size_type new_cap = this->next_capacity(n);
         ret = this->allocation_command
               (allocate_new | expand_fwd | expand_bwd,
                  this->size() + n, new_cap, real_cap, this->members_.m_start);
         same_buffer_start = ret.second && this->members_.m_start == ret.first;
         if(same_buffer_start){
            this->members_.m_capacity  = real_cap;
         }
      }
      
      if(same_buffer_start){
         T *start = containers_detail::get_pointer(this->members_.m_start);
         if (this->size() >= n){
            //There is memory, but there are more old elements than new ones
            //Overwrite old elements with new ones
            std::copy(first, last, start);
            //Destroy remaining old elements
            this->destroy_n(start + n, this->members_.m_size - n);
            this->members_.m_size = n;
         }
         else{
            //There is memory, but there are less old elements than new ones
            //First overwrite some old elements with new ones
            FwdIt mid = first;
            std::advance(mid, this->size());
            // iG T *end = std::copy(first, mid, start);
            T *end = std::copy(first, mid, start);
            //Initialize the remaining new elements in the uninitialized memory
            // iG std::uninitialized_copy(mid, last, end);
            ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_copy_or_move(mid, last, end);
            this->members_.m_size = n;
         }
      }
      else if(!ret.second){
         typename value_traits::UCopiedArrayDeallocator scoped_alloc(ret.first, this->alloc(), real_cap);
         // iG std::uninitialized_copy(first, last, containers_detail::get_pointer(ret.first));
         ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_copy_or_move(first, last, containers_detail::get_pointer(ret.first));
         scoped_alloc.release();
         //Destroy and deallocate old buffer
         if(this->members_.m_start != 0){
            this->destroy_n(containers_detail::get_pointer(this->members_.m_start), this->members_.m_size); 
            this->alloc().deallocate(this->members_.m_start, this->members_.m_capacity);
         }
         this->members_.m_start     = ret.first;
         this->members_.m_size      = n;
         this->members_.m_capacity  = real_cap;
      }
      else{
         //Backwards expansion
         //If anything goes wrong, this object will destroy old objects
         T *old_start         = containers_detail::get_pointer(this->members_.m_start);
         size_type old_size   = this->members_.m_size;
         typename value_traits::OldArrayDestructor old_values_destroyer(old_start, old_size);
         //If something goes wrong size will be 0
         //but holding the whole buffer
         this->members_.m_size  = 0;
         this->members_.m_start = ret.first;
         this->members_.m_capacity = real_cap;
         
         //Backup old buffer data
         size_type old_offset    = old_start - containers_detail::get_pointer(ret.first);
         size_type first_count   = containers_detail::min_value(n, old_offset);

         FwdIt mid = first;
         std::advance(mid, first_count);
         // iG std::uninitialized_copy(first, mid, containers_detail::get_pointer(ret.first));
         ::BOOST_CONTAINER_MOVE_NAMESPACE::uninitialized_copy_or_move(first, mid, containers_detail::get_pointer(ret.first));

         if(old_offset > n){
            //All old elements will be destroyed by "old_values_destroyer" 
            this->members_.m_size = n;
         }
         else{
            //We have constructed objects from the new begin until
            //the old end so release the rollback destruction
            old_values_destroyer.release();
            this->members_.m_start  = ret.first;
            this->members_.m_size   = first_count + old_size;
            //Now overwrite the old values
            size_type second_count = containers_detail::min_value(old_size, n - first_count);
            FwdIt mid2 = mid;
            std::advance(mid2, second_count);
            // iG std::copy(mid, mid2, old_start);
            std::copy(mid, mid2, old_start);
            
            //Check if we still have to append elements in the
            //uninitialized end
            if(second_count == old_size){
               // iG std::copy(mid2, last, old_start + old_size);
               std::copy(mid2, last, old_start + old_size);
            }
            else{
               //We have to destroy some old values
               this->destroy_n
                  (old_start + second_count, old_size - second_count);
               this->members_.m_size = n;
            }
            this->members_.m_size = n;                        
         }
      }
   }

   template <class Integer>
   void priv_assign_dispatch(Integer n, Integer val, containers_detail::true_)
   { this->assign((size_type) n, (value_type)val); }

   template <class InIt>
   void priv_assign_dispatch(InIt first, InIt last, containers_detail::false_)
   { 
      //Dispatch depending on integer/iterator
      typedef typename std::iterator_traits<InIt>::iterator_category ItCat;
      this->priv_assign_aux(first, last, ItCat()); 
   }

   template <class Integer>
   void priv_insert_dispatch(const_iterator pos, Integer n, Integer val, containers_detail::true_) 
   {  this->insert(pos, (size_type)n, (T)val);  }

   template <class InIt>
   void priv_insert_dispatch(const_iterator pos, InIt first, 
                             InIt last,      containers_detail::false_)
   {
      //Dispatch depending on integer/iterator
      typedef typename std::iterator_traits<InIt>::iterator_category ItCat;
      this->priv_range_insert(pos.get_ptr(), first, last, ItCat());
   }

   void priv_check_range(size_type n) const 
   {
      //If n is out of range, throw an out_of_range exception
      if (n >= size())
         throw std::out_of_range("vector::at");
   }

   #ifdef BOOST_CONTAINERS_VECTOR_ALLOC_STATS
   public:
   unsigned int num_expand_fwd;
   unsigned int num_expand_bwd;
   unsigned int num_shrink;
   unsigned int num_alloc;
   void reset_alloc_stats()
   {  num_expand_fwd = num_expand_bwd = num_alloc = 0, num_shrink = 0;   }
   #endif
   /// @endcond
};

template <class T, class A>
inline bool 
operator==(const vector<T, A>& x, const vector<T, A>& y)
{
   //Check first size and each element if needed
   return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <class T, class A>
inline bool 
operator!=(const vector<T, A>& x, const vector<T, A>& y)
{
   //Check first size and each element if needed
  return x.size() != y.size() || !std::equal(x.begin(), x.end(), y.begin());
}

template <class T, class A>
inline bool 
operator<(const vector<T, A>& x, const vector<T, A>& y)
{
   return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <class T, class A>
inline void swap(vector<T, A>& x, vector<T, A>& y)
{  x.swap(y);  }

}}

/// @cond

namespace boost {

/*

//!has_trivial_destructor_after_move<> == true_type
//!specialization for optimizations
template <class T, class A>
struct has_trivial_destructor_after_move<boost::container::vector<T, A> >
{
   static const bool value = has_trivial_destructor<A>::value;
};

*/

}

/// @endcond

#include INCLUDE_BOOST_CONTAINER_DETAIL_CONFIG_END_HPP

#endif //   #ifndef  BOOST_CONTAINERS_CONTAINERS_VECTOR_HPP

