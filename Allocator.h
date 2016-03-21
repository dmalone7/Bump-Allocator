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
            int size = N;
            int sentinel1, sentinel2;
            int i = 0;
            while (size > 0) {
                sentinel1 = *reinterpret_cast<const int*>(&a[i]);
                i = sentinel1/sizeof(T) + sizeof(int)/sizeof(T);
                sentinel2 = *reinterpret_cast<const int*>(&a[i]); 
                if(sentinel1 != sentinel2) 
                    return false;
                size -= (sentinel1 + 2 * sizeof(int));
            }
            return true;
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
            return *reinterpret_cast<int*>(&a[i]);}

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
            int ndx, sentinel1, sentinel2;
            if (N < sizeof(T) + (2 * sizeof(int)))
                throw std::bad_alloc();

            ndx = 0;
            sentinel1 = N - (2 * sizeof(int));
            *reinterpret_cast<int*>(&a[ndx]) = sentinel1;

            ndx = N / sizeof(T) - 1;
            sentinel2 = sentinel1;
            *reinterpret_cast<int*>(&a[ndx]) = sentinel2;

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
         */
        pointer allocate (size_type n) {
            int size = N;
            int sentinel1 = 0;
            int sentinel2 = 0;
            int i = 0;

            // error checking 
            if (n < 1)
                throw std::bad_alloc();

            // while not at end
            while (size > 0) {
                // check first sentinel
                // if negative, go to second sentinel
                sentinel1 = *reinterpret_cast<int*>(&a[i]);

                // if (sizeof(T) + (2*sizeof(int)) < sentinel1)
                //     return nullptr;

                if (sentinel1 > 0 && sentinel1 > (sizeof(T) * n + (2 * sizeof(int)))) {
                    return reinterpret_cast<T*>(&a[i]);
                }
                    // repeat until positive sentinel found
                // check first sentinel
                // if less than n, go to second sentinel
                    // repeat until sentinel is greater than n
                // create two new sentinels
                // store old sentinel in temp
                // new sentinel1 = n
                // jump forward sentinel1 + sizeof(int)
                // new sentinel2 = new sentinel1
                // jump forward 1;
                // create new sentinel
                // new sentinel1 = temp - n - n - 2*sizeof(int)
                // jump forward sentinel1 + sizeof(int)
                // sentinel2 = sentinel1
            }
            assert(valid());
            // return nullptr;
        }             // replace!

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         */
        void construct (pointer p, const_reference v) {
            new (p) T(v);                               // this is correct and exempt
            assert(valid());}                           // from the prohibition of new

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
            assert(valid());}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         */
        void destroy (pointer p) {
            p->~T();               // this is correct
            assert(valid());}

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        const int& operator [] (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};

#endif // Allocator_h
