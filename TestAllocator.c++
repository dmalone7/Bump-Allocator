// ------------------------------------
// projects/allocator/TestAllocator1.c++
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------------

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator
#include <new>       // bad_alloc
#include <stdexcept> // invalid_argument

#include "gtest/gtest.h"

#include "Allocator.h"

// --------------
// TestAllocator1
// --------------

template <typename A>
struct TestAllocator1 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;
};

typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_1;

TYPED_TEST_CASE(TestAllocator1, my_types_1);

TYPED_TEST(TestAllocator1, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);
    }
}

TYPED_TEST(TestAllocator1, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type  x;
    const size_type       s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;
            }
        }
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;
        }
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);
    }
}

// --------------
// TestAllocator2
// --------------

TEST(TestAllocator2, const_index) {
    const Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
}

TEST(TestAllocator2, index) {
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
}

// New tests
TEST(TestAllocator2, allocate1) { // Allocator finds first fit
    Allocator<int, 100> x;

    int *p1 = x.allocate(10);
    ASSERT_EQ(p1, &x[4]);
    ASSERT_EQ(x[0], -40);
    ASSERT_EQ(x[44], -40);
    ASSERT_EQ(x[48], 44);
    ASSERT_EQ(x[96], 44);
}

TEST(TestAllocator2, allocate2) {
    Allocator<int, 100> x;
    
    int *p1 = x.allocate(10);
    ASSERT_EQ(p1, &x[4]);
    ASSERT_EQ(x[0], -40);
    ASSERT_EQ(x[44], -40);
    ASSERT_EQ(x[48], 44);
    ASSERT_EQ(x[96], 44);

    int *p2 = x.allocate(5);
    ASSERT_EQ(p2, &x[52]);
    ASSERT_EQ(x[48], -20);
    ASSERT_EQ(x[72], -20);
    ASSERT_EQ(x[76], 16);
    ASSERT_EQ(x[96], 16);
}

TEST(TestAllocator2, allocate3) {
    Allocator<int, 100> x;

    int *p1 = x.allocate(17);
    ASSERT_NE(p1, nullptr);

    int *p2 = x.allocate(1);
    ASSERT_NE(p2, nullptr);
}

TEST(TestAllocator4, allocate_no_fit) { // Allocator finds no fit
    Allocator<int, 100> x;

    int *p1 = x.allocate(20);
    ASSERT_NE(p1, nullptr);

    ASSERT_THROW({
        x.allocate(20);
    }, std::bad_alloc);
}

TEST(TestAllocator2, deallocate1) {
    Allocator<int, 100> x;

    int *p1 = x.allocate(10);
    
    x.deallocate(p1, 10 * sizeof(int));
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

TEST(TestAllocator2, deallocate2) { // coalesce before block
    Allocator<int, 100> x;
    
    int *p1 = x.allocate(10);
    
    int* p2 = x.allocate(5);
    
    x.deallocate(p1, 10 * sizeof(int));
    ASSERT_EQ(x[0], 40);
    ASSERT_EQ(x[44], 40);
    ASSERT_EQ(x[48], -20);
    ASSERT_EQ(x[72], -20);
    ASSERT_EQ(x[76], 16);
    ASSERT_EQ(x[96], 16);

    x.deallocate(p2, 5 * sizeof(int));
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

TEST(TestAllocator2, deallocate3) { // coalesce after block
    Allocator<int, 100> x;
    
    int *p1 = x.allocate(10);
    
    int* p2 = x.allocate(5);
    
    x.deallocate(p2, 5 * sizeof(int));
    ASSERT_EQ(x[0], -40);
    ASSERT_EQ(x[44], -40);
    ASSERT_EQ(x[48], 44);
    ASSERT_EQ(x[96], 44);

    x.deallocate(p1, 10 * sizeof(int));
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

TEST(TestAllocator2, deallocate4) {
    Allocator<int, 100> x;
    
    int *p1 = x.allocate(20);
    ASSERT_NE(p1, nullptr);
    x.deallocate(p1, 20 * sizeof(int));

    int *p2 = x.allocate(20); // see if deallocate freed the space
    ASSERT_NE(p2, nullptr);
    x.deallocate(p2, 20 * sizeof(int));
}

TEST(TestAllocator2, big) {
    Allocator<int, 1000000> x;
    ASSERT_EQ(x[0], 999992);
    ASSERT_EQ(x[999996], 999992);
}

TEST(TestAllocator2, extra_space) { // see if allocator gives extra space if there isn't enough left for another block
    Allocator<int, 100> x;

    x.allocate(21);
    ASSERT_EQ(x[0], -92);
    ASSERT_EQ(x[96], -92);
}

// ----------------------
// TestAllocatorEdgeCases
// ----------------------

TEST(TestAllocatorEdgeCases, too_big) {
    Allocator<int, 100> x;

    ASSERT_THROW(x.allocate(24), std::bad_alloc);
}

TEST(TestAllocatorEdgeCases, much_too_big) {
    Allocator<int, 100> x;

    ASSERT_THROW(x.allocate(100), std::bad_alloc);
}


TEST(TestAllocatorEdgeCases, allocator_bad_alloc) {
    typedef Allocator<int, 11> Allocator11; // because commas don't work in macro arguments
    ASSERT_THROW({
        Allocator11 x;
    }, std::bad_alloc);
}

TEST(TestAllocatorEdgeCases, allocator_min_size) {
    typedef Allocator<int, 20> Allocator20; // because commas don't work in macro arguments
    ASSERT_NO_THROW({
        Allocator20 x;
    });
}

// ---------
// TestValid
// ---------

TEST(TestValid, no_space) {
    Allocator<int, 100> x;
    x[0] = -92;
    x[96] = -92;
    //ASSERT_EQ(x.valid(), false);
}

// --------------
// TestAllocator3
// --------------

template <typename A>
struct TestAllocator3 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;
};

typedef testing::Types<
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_2;

TYPED_TEST_CASE(TestAllocator3, my_types_2);

TYPED_TEST(TestAllocator3, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);
    }
}

TYPED_TEST(TestAllocator3, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 10;
    const value_type     v = 2;
    const pointer        b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;
            }
        }
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);
            }
            x.deallocate(b, s);
            throw;
        }
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);
        }
        x.deallocate(b, s);
    }
}
