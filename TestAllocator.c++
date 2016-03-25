// ------------------------------------
// projects/allocator/TestAllocator1.c++
// Copyright (C) 2016
// David Malone
// ------------------------------------

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator

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
    typedef typename A::pointer    pointer;};

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
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
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
    ASSERT_EQ(x[96], 92);}

TEST(TestAllocator2, index1) {
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);}

TEST(TestAllocator2, index2) {
    const Allocator<int, 52> x;
    ASSERT_EQ(x[48], 44);
    ASSERT_EQ(x[0], 44);
}

// --------------
// TestAllocate
// --------------

TEST(TestAllocate, allocate_1) {
    Allocator<int, 100> x;
    x.allocate(4);
    ASSERT_EQ(x[0], -16);
    ASSERT_EQ(x[20], -16);
    ASSERT_EQ(x[24], 68);
    ASSERT_EQ(x[96], 68);
}

TEST(TestAllocate, allocate_2) {
    Allocator<double, 100> x;
    x.allocate(5);
    x.allocate(3);
    ASSERT_EQ(x[0], -40);
    ASSERT_EQ(x[44], -40);
    ASSERT_EQ(x[48], -24);
    ASSERT_EQ(x[76], -24);
    ASSERT_EQ(x[80], 12);
    ASSERT_EQ(x[96], 12);
}

TEST(TestAllocate, allocate_3) {
    Allocator<int, 100> x;

    int *p1 = x.allocate(17);
    ASSERT_NE(p1, nullptr);

    int *p2 = x.allocate(1);
    ASSERT_NE(p2, nullptr);
}

TEST(TestAllocate, allocate_4) {
    try {
        Allocator<int, 101> x;
        x.allocate(23);
        x.allocate(1);
    } catch (std::bad_alloc) {
        ASSERT_TRUE(true);
    }
}

// ---------------
// TestDeallocator
// ---------------

TEST(TestDeallocator, deallocate_1) {
    Allocator<int, 100> x;
    int *p1 = x.allocate(10);
    x.deallocate(p1, 10 * sizeof(int));
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

TEST(TestDeallocator, deallocate_2) {
    Allocator<double, 100> x;
    double* p = x.allocate(5);
    ASSERT_EQ(x[0], -40);
    ASSERT_EQ(x[44], -40);
    ASSERT_EQ(x[48], 44);
    ASSERT_EQ(x[96], 44);
    x.deallocate(p, (size_t)5);
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

TEST(TestDeallocator, deallocate_3) {
    Allocator<int, 100> x;
    int *p = x.allocate(23);
    x.deallocate(p, 0);
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
    try {
        x.deallocate(p, 0);
    } catch (std::invalid_argument) {
        ASSERT_TRUE(true);
    }
}

// --------------
// TestAbs
// --------------

TEST(TestAbsolute, abs_1) {
    Allocator<int, 100> x;
    ASSERT_EQ(x.abs(-1000), 1000);
}

TEST(TestAbsolute, abs_2) {
    Allocator<int, 100> x;
    ASSERT_EQ(x.abs(1000), 1000);
}

TEST(TestAbsolute, abs_3) {
    Allocator<int, 100> x;
    ASSERT_EQ(x.abs(0), 0);
}

// --------------
// TestValid
// --------------

TEST(TestValid, valid_1) {
    Allocator<int, 52> y;
    y[0] = -44;
    y[48] = -44;
    ASSERT_TRUE(y.valid());
}

TEST(TestValid, valid_2) {
   Allocator<int, 100> x;
   x[0] = -40;  
   x[44] = -40;
   x[48] = 24;
   x[76] = 24;
   x[80] = -12;
   x[96] = -12;
   ASSERT_TRUE(x.valid());
}

TEST(TestValid, valid_3) {
    Allocator<int, 100> x;
    int *p = x.allocate(20);
    x.deallocate(p, 0);
    ASSERT_TRUE(x.valid());
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
    typedef typename A::pointer    pointer;};

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
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);
        }
        x.deallocate(b, s);
    }
}
