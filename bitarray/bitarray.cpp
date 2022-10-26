/*****************************************************************************
 * 					written by : Orpaz Houta								 *
 * 					Last update : 6.9.20									 *
 ****************************************************************************/
#include <cstddef>

#include "bitarray.hpp"

namespace  ilrd
{

namespace bitarray_details
{
#define B2(n) n, n + 1, n + 1, n + 2
#define B4(n) B2(n) , B2(n + 1), B2(n + 1), B2(n + 2)
#define B6(n) B4(n) , B4(n + 1), B4(n + 1), B4(n + 2)

const unsigned char s_byteCountMap[256] = { B6(0), B6(1), B6(1), B6(2)};

size_t FlipWord(size_t word)
{
    return ~word;
}

void BitToString::operator()(size_t wordToConvert)
{
    for (size_t i = 0; i < BITS_IN_WORD; ++i)
    {
        size_t bit = (wordToConvert >> i) & MASK_BIT;
        m_string += static_cast<char>(bit + '0');
    }
}

std::string BitToString::GetStr() const
{
    return m_string;
}

size_t CountBits::operator()(size_t sum, unsigned char byte)
{
	return (sum + s_byteCountMap[byte]);
}
}// namespace bitarray_details
}//namespace  ilrd

