// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2016
// David Malone
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <cstddef>   // ptrdiff_t, size_t
#include <new>       // bad_alloc, new
#include <stdexcept> // invalid_argument

// ---------
// Allocator
// ---------

template <typename T, std::size_t N>
class Allocator {
  public:
    // --------
    // typedefs
    // --------

    typedef T                 value_type;

    typedef std::size_t       size_type;
    typedef std::ptrdiff_t    difference_type;

    typedef       value_type*       pointer;
    typedef const value_type* const_pointer;

    typedef       value_type&       reference;
    typedef const value_type& const_reference;

  public:
    // -----------
    // operator ==
    // -----------

    friend bool operator == (const Allocator&, const Allocator&) {
      return true;}                                              // this is correct

    // -----------
    // operator !=
    // -----------

    friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
      return !(lhs == rhs);}

  private:
    // ----
    // data
    // ----

    char a[N];

    // -----
    // valid
    // -----

    /**
     * O(1) in space
     * O(n) in time
     * <your documentation>
     */
    bool valid () const {
      // uint b = 0, e;
      // while (b < N) {
      //   int v = *((int *)&a[b]);

      //   e = b + sizeof(int) + (v < 0 ? -v : v);
      //   if (e >= N) return false;

      //   if (v != *((int *)&a[e])) return false;

      //   b = e + sizeof(int);
      // }
      // return true;
      int i, s1, s2;
      i = 0;
      s1 = 0;
      s2 = 1;

      while (s1 != s2) {
        s1 = *((int *)&a[i]);
        i = abs(s1) + sizeof(int);
        s2 = *((int *)&a[i]);
      }

      if (s1 == s2)
        return true;

      return false;
    }

    /**
     * O(1) in space
     * O(1) in time
     * <your documentation>
     * https://code.google.com/p/googletest/wiki/AdvancedGuide#Private_Class_Members
     */
    FRIEND_TEST(TestAllocator2, index);
    FRIEND_TEST(TestAllocator2, double_index);
    int& operator [] (int i) {
      return *reinterpret_cast<int*>(&a[i]);
    }

    int abs(int num) const {
      int mask = (num >> 31);
      return (num ^ mask) - mask;
    }

  public:
    // ------------
    // constructors
    // ------------

    /**
     * O(1) in space
     * O(1) in time
     * throw a bad_alloc exception, if N is less than sizeof(T) + (2 * sizeof(int))
     */
    Allocator () {
      // if (N < sizeof(int) * 2) {
      //   throw std::bad_alloc();
      // }
      // *((int *)&a[0]) = N - sizeof(int) * 2;
      // *((int *)&a[N - sizeof(int)]) = *((int *)&a[0]);
      // assert(valid());

      if (N < sizeof(T) + (2 * sizeof(int)))
        throw std::bad_alloc();

      int i = 0;
      int sentinel = N - (2 * sizeof(int));
      *((int*)&a[i]) = sentinel;

      i = N - sizeof(int);
      *((int*)&a[i]) = sentinel;

      assert(valid());
    }

    // Default copy, destructor, and copy assignment
    // Allocator  (const Allocator&);
    // ~Allocator ();
    // Allocator& operator = (const Allocator&);

    // --------
    // allocate
    // --------

    /**make 
     * O(1) in space
     * O(n) in time
     * after allocation there must be enough space left for a valid block
     * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
     * choose the first block that fits
     * throw a bad_alloc exception, if n is invalid
     */
    pointer allocate (size_type n) {
      if (n < 1)
        throw std::bad_alloc();

      pointer p = nullptr;
      int sentinel = 0;
      int i = 0;

      while (i < N) {
        sentinel = *((int*)&a[i]);

        if (sentinel > 0 && sentinel > ((n * sizeof(T)) + (2 * sizeof(int)))) {
          p = (pointer)&a[i];
          break;
        }
        i = abs(sentinel) + (2 * sizeof(int));
      }

      int old_sentinel = sentinel;

      sentinel = (n * sizeof(T));
      *((int*)&a[i]) = 0-sentinel;

      i += sentinel + sizeof(int);
      *((int *)&a[i]) = 0-sentinel;

      i += sizeof(int);
      sentinel = old_sentinel - (sentinel + 2 * sizeof(int));
      *((int *)&a[i]) = sentinel;

      i = sentinel + sizeof(int);
      *((int *)&a[i]) = sentinel;

      assert(valid());
      return p;

      // assert(valid());
      // int size = n * sizeof(T);
      // uint b = 0, e;
      // while (b < N) {
      //   int v = view(b);
      //   e = b + sizeof(int) + (v < 0 ? -v : v);
      //   if (v >= size) {
      //     if (v - size < min_block()) {
      //       size = v;
      //     }
      //     view(b) = -(size);
      //     view(b + size + sizeof(int)) = view(b);

      //     if ((b + size + sizeof(int)) != e) {
      //       int diff = v - size - 2 * sizeof(int);
      //       view(b + size + sizeof(int) * 2) = diff;
      //       view(e) = diff;
      //     }
      //     return reinterpret_cast<pointer>(&a[b + sizeof(int)]);
      //   }
      //   b = e + sizeof(int);
      // }
      // throw std::bad_alloc();  
    }

    // ---------
    // construct
    // ---------

    /**
     * O(1) in space
     * O(1) in time
     */
    void construct (pointer p, const_reference v) {
      new (p) T(v);                               // this is correct and exempt
      assert(valid());
    }                           // from the prohibition of new

    // ----------
    // deallocate
    // ----------

    /**
     * O(1) in space
     * O(1) in time
     * after deallocation adjacent free blocks must be coalesced
     * throw an invalid_argument exception, if p is invalid
     * <your documentation>
     */
    void deallocate (pointer p, size_type) {
      // <your code>
      // size_type i = *p;
      assert(valid());
    }

    // -------
    // destroy
    // -------

    /**
     * O(1) in space
     * O(1) in time
     */
    void destroy (pointer p) {
      p->~T();               // this is correct
      assert(valid());
    }

    /**
     * O(1) in space
     * O(1) in time
     * <your documentation>
     */
    const int& operator [] (int i) const {
      return *reinterpret_cast<const int*>(&a[i]);
    }
};

#endif // Allocator_h
