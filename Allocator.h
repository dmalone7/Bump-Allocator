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
    int smallest_block;
    // -----
    // valid
    // -----

    /**
     * O(1) in space
     * O(n) in time
     * Checks if the sentinels in the heap are valid by checking
     * if they are the same.
     * @param none
     * @return true if valid, false if not
     * https://code.google.com/p/googletest/wiki/AdvancedGuide#Private_Class_Members
     */
    FRIEND_TEST(TestValid, valid_1);
    FRIEND_TEST(TestValid, valid_2);
    FRIEND_TEST(TestValid, valid_3);
    bool valid () const {
      int i, s1, s2;
      i = 0;
      s1 = s2 = 0;

      while (s1 == s2) {
        if (i < N) 
          break;
        s1 = *((int *)&a[i]);
        i = abs(s1) + sizeof(int);
        s2 = *((int *)&a[i]);
        i += sizeof(int);
      }

      if (s1 == s2)
        return true;

      return false;
    }

    // -----
    // abs
    // -----

    /**
     * O(1) in space
     * O(1) in time
     * @param any int to make absolute
     * @return int absolute value
     */
    FRIEND_TEST(TestAbsolute, abs_1);
    FRIEND_TEST(TestAbsolute, abs_2);
    FRIEND_TEST(TestAbsolute, abs_3);
    int abs(int num) const {
      int mask = (num >> 31);
      int pos = (num ^ mask) - mask;

      assert(pos >= 0);
      return pos;
    }

  public:
    // ------------
    // constructors
    // ------------

    /**
     * O(1) in space
     * O(1) in time
     * Default constructor that creates and initializes Allocator 
     * object for use.
     * throw a bad_alloc exception, if N is less than sizeof(T) + (2 * sizeof(int))
     * @param none
     * @return none
     */
    Allocator () {
      smallest_block = sizeof(T) + (2 * sizeof(int));
      if (N < smallest_block)
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

    /** 
     * O(1) in space
     * O(n) in time
     * after allocation there must be enough space left for a valid block
     * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
     * choose the first block that fits
     * throw a bad_alloc exception, if n is invalid
     * @param size_type n to allocate n * sizeof(T) space in heap
     * @return pointer to the allocated space in heap
     */
    pointer allocate (size_type n) {
      assert(valid());

      if (n < 1)
        throw std::bad_alloc();

      pointer p = nullptr;
      int sentinel = 0;
      int i = 0;

      while (i < N) {
        sentinel = *((int*)&a[i]);

        if (sentinel > 0 && sentinel >= ((n * sizeof(T)))) {
          p = (pointer)&a[i + 4];
          break;
        }
        i += abs(sentinel) + (2 * sizeof(int));
      }

      if (i >= N)
        throw std::bad_alloc();

      int old_sentinel = sentinel;
      int *p1 = ((int*)&a[i]);

      sentinel = (n * sizeof(T));
      *((int*)&a[i]) = 0-sentinel;

      i += sentinel + sizeof(int);
      *((int*)&a[i]) = 0-sentinel;

      i += sizeof(int);

      if (i >= N - sizeof(T) - 2 * sizeof(int)) {
        i = (char*)p1 - &a[0];
        i += old_sentinel + sizeof(int);
        *p1 = 0-old_sentinel;
        *((int*)&a[i]) = 0-old_sentinel;
        return p;
      }

      sentinel = old_sentinel - (sentinel + 2 * sizeof(int));
      *((int *)&a[i]) = sentinel;
      i += sentinel + sizeof(int);
      *((int *)&a[i]) = sentinel;

      assert(valid());
      return p;
    }

    // ---------
    // construct
    // ---------

    /**
     * O(1) in space
     * O(1) in time
     * @param pointer p, const_reference v
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
     * @param pointer to space to deallocate in heap, size_type t
     * @return none
     */
    void deallocate (pointer p, size_type t) {
      assert(valid());

      int i = (char*)(p) - &a[0];
      int sentinel = *((int *)&a[i - 4]);

      int *p1 = (int *)(&a[i - 4]);
      *p1 = abs(sentinel);

      int *p2 = (int *)(&a[i - sentinel]);
      *p2 = abs(sentinel);
            
      int pToEnd = &a[N - 1] - (char*)(p);

      int seninel_new = sentinel;

      if(i - 4 >= smallest_block) {
        int seninel_left = *((int *)&a[i - (2 * sizeof(int))]);

        if(seninel_left > 0) {
          seninel_new = seninel_left + abs(sentinel) + 2 * sizeof(int); 
          int *p3 = (int *)(&a[i - (3 * sizeof(int)) - seninel_left]);

          *p3 = seninel_new;
          *p2 = seninel_new;
        }
      }

      if(pToEnd - 4 - (sentinel * -1) >= smallest_block) {
        int seninel_right = *((int *)&a[i - sentinel + 4]);

        if(seninel_right > 0) {
          seninel_new = seninel_right + abs(sentinel) + 2 * sizeof(int);
          int *p4 = (int *)(&a[i + abs(sentinel) + seninel_right + (2 * sizeof(int))]);

          *p4 = seninel_new;
          *p1 = seninel_new;
        }
      }
      assert(valid());
    }

    // -------
    // destroy
    // -------

    /**
     * O(1) in space
     * O(1) in time
     * @param pointer p
     * @return none
     */
    void destroy (pointer p) {
      p->~T();               // this is correct
      assert(valid());
    }

    /**
     * O(1) in space
     * O(1) in time
     * @param int for array indexing
     * @return read-only reference to heap
     */
    const int& operator [] (int i) const {
      return *reinterpret_cast<const int*>(&a[i]);
    }

     /**
     * O(1) in space
     * O(1) in time
     * @param int for array indexing
     * @return reference to heap
     */

    int& operator [] (int i) {
      return *reinterpret_cast<int*>(&a[i]);
    }
};

#endif // Allocator_h
