/*****************************************************************************
 * 					written by : Orpaz Houta								 *
 * 					review :      						         *
 * 					Last update : 6.9.20									 *
 ****************************************************************************/

#include "bitarray.hpp"
#include "ca_test_util.hpp"

using namespace ilrd;
using namespace std;
using namespace ca_test_util;

const std::size_t SIZE = 5000;
void TestComparisonAndFlip();
void TestOperatorBrackets();
void TestBitwise();

int main()
{
    TestComparisonAndFlip();
    TestOperatorBrackets();
    TestBitwise();

    TestSummary();

    return 0;
}

void TestComparisonAndFlip()
{
    Bitarray<SIZE> b1;
    BoolTest((0 == b1.Count()));
    b1.SetAll();
    BoolTest((SIZE == b1.Count()));
    Bitarray<SIZE> b2(b1);
    BoolTest((b1 == b2));
    std::size_t maxRange = SIZE < 64 ? SIZE : 64;
    size_t toFlip = rand() % maxRange;
    b1[toFlip].Flip();
    BoolTest(b1 != b2);
    b1[toFlip].Flip();
    BoolTest((b1 == b2));
//    check the last word in array
    size_t toFlipLast = (rand() % (SIZE % 64)) + (SIZE / 64 * 64);
    b1[toFlipLast].Flip();
    BoolTest(b1 != b2);
    for (size_t i = 0; i < SIZE; ++i)
    {
        b2[i].Flip();
    }
    b1.ClearAll();
//    check change in the remain
    BoolTest(b1 == b2);

//    check operator=(const Bitarray &other)
    for (size_t i = 0; i < SIZE; ++i)
    {
        bool set = rand() % 2;
        b1.SetBit(i, set);
    }
    b1 = b2;
    BoolTest((b1 == b2));
}

void TestOperatorBrackets()
{
    const Bitarray<SIZE> consty;
    Bitarray<SIZE> nonconst(consty);
    Bitarray<SIZE> allset;
    allset.SetAll();
    for (size_t i = 0; i < SIZE; ++i)
    {
        BoolTest(!consty[i]);
    }
    nonconst.SetAll();
    BoolTest(nonconst[0]);
    for (size_t i = 0; i < SIZE; ++i)
    {
        nonconst[i] = consty[i];
    }
    BoolTest(consty == nonconst);
    nonconst.SetAll();
    for (size_t i = SIZE/2; i < SIZE; ++i)
    {
        nonconst.FlipBit(i);
    }
    for (size_t i = 0; i < SIZE; ++i)
    {
        nonconst[i] = 0;
    }
    BoolTest(consty == nonconst);
    for (size_t i = 0; i < SIZE; ++i)
    {
        nonconst[i] = allset[i];
    }
    for (size_t i = 0; i < SIZE; ++i)
    {
        BoolTest(nonconst[i]);
        BoolTest(nonconst.GetBit(i));
    }
    for (size_t i = 0; i < SIZE; ++i)
    {
        bool set = rand() % 2;
        nonconst.SetBit(i, set);
        BoolTest(set == nonconst.GetBit(i));
    }
}

void TestBitwise()
{
    Bitarray<SIZE> b1;
    Bitarray<SIZE> allset;
    allset.SetAll();
    Bitarray<SIZE> allclear;
    for (size_t i = 0; i < SIZE; ++i)
    {
        bool set = rand() % 2;
        b1.SetBit(i, set);
    }
    Bitarray<SIZE> b2(b1);
    BoolTest((b1.Count() == b2.Count()));
    b2 &= allset;
    BoolTest((b1 == b2));

    b2 |= allclear;
    BoolTest((b1 == b2));

    b1 ^= b2;
    BoolTest((b1.Count() == allclear.Count()));

    b2 &= allclear;
    BoolTest((b1 == b2));

    b2.Flip();
    BoolTest((b2 == allset));

    for (size_t i = 0; i < SIZE; ++i)
    {
        bool set = rand() % 2;
        b1.SetBit(i, set);
    }
    std::size_t ons = b1.Count();
    b1.Flip();
    BoolTest((b1.Count() == SIZE - ons));
}


