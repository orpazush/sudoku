/*****************************************************************************
 * 					written by : Orpaz Houta								 *
 * 					Last update : 6.9.20									 *
 ****************************************************************************/
#ifndef ILRD_RD8788_BITARRAY
#define ILRD_RD8788_BITARRAY

#if __cplusplus<201103L
#define noexcept throw()
#endif

#include <string>                   // string
#include <climits>                  // CHAR_BIT
#include <algorithm>                // copy
#include <functional>               // bit_or
#include <numeric>                  // accumulate
#include <stdexcept>                // out_of_range

namespace ilrd
{
namespace bitarray_details
{
extern const unsigned char s_byteCountMap[256];
const size_t BYTES_IN_WORD = sizeof(size_t);
const size_t BITS_IN_WORD = (BYTES_IN_WORD * CHAR_BIT);
const size_t MASK_BIT = 1;
const size_t ALL_SET = ~0;

size_t FlipWord(size_t word);

class CountBits
{
public:
    size_t operator()(size_t sum, unsigned char byte);
};

class BitToString
{
public:
    void operator()(size_t wordToConvert);
    std::string GetStr() const;
private:
    std::string m_string;
};
} // namespace bitarray_details

using namespace bitarray_details;

template<size_t SIZE>
class Bitarray
{
    class Bit;

public:
    explicit Bitarray() noexcept;
    ~Bitarray() noexcept;
    Bitarray(const Bitarray &other) noexcept;
    Bitarray &operator=(const Bitarray &other) noexcept;

    bool operator[](size_t index) const noexcept;
    Bit operator[](size_t index) noexcept;

    bool operator==(const Bitarray &other) const noexcept;
    bool operator!=(const Bitarray &other) const noexcept;
    Bitarray &operator|=(const Bitarray &other) noexcept;
    Bitarray &operator&=(const Bitarray &other) noexcept;
    Bitarray &operator^=(const Bitarray &other) noexcept;
    // advanced
    Bitarray &operator>>=(size_t n_shifts) noexcept;
    Bitarray &operator<<=(size_t n_shifts) noexcept;

    size_t Count() const noexcept;
    std::string ToString() const;
    Bitarray &SetAll() noexcept;
    Bitarray &ClearAll() noexcept;
    Bitarray &Flip() noexcept;

    Bitarray &FlipBit(size_t index);
    bool GetBit(size_t index) const;
    void SetBit(size_t index, bool state);
    Bit At(size_t index);

private:

    static const size_t BITARRAY_SIZE = (SIZE / BITS_IN_WORD)
                                        + !(!(SIZE & (BITS_IN_WORD - 1)));
    // instead of using %
    static const size_t MASK_REMAIN = ~(ALL_SET << (SIZE & (BITS_IN_WORD - 1)));

    size_t m_container[BITARRAY_SIZE];

    inline const size_t *begin() const noexcept;
    inline const size_t *end() const noexcept;
    inline size_t *begin() noexcept;
    inline size_t *end() noexcept;
    inline size_t WordIdx(size_t index) const noexcept;
    inline size_t BitIdx(size_t index) const noexcept;
    inline void SetBitNoThrow(size_t index, bool state) noexcept;
    inline bool GetBitNoThrow(size_t index) const noexcept;

    class Bit
    {
    public:
        Bit(size_t index, Bitarray &bitarray) noexcept;

        Bit &operator=(const Bit &other) noexcept;
        Bit &operator=(bool state) noexcept;

        void Flip() noexcept;
        operator bool() const noexcept;
    private:
        Bitarray &m_bitarray;
        const size_t m_index;
    };
};

///////////////////////////////// Ctors & Dtors //////////////////////////////
template<size_t SIZE>
Bitarray<SIZE>::Bitarray() noexcept
{
    ClearAll();
}

template<size_t SIZE>
Bitarray<SIZE>::~Bitarray() noexcept
{
//    empty
}

template<size_t SIZE>
Bitarray<SIZE>::Bitarray(const Bitarray &other) noexcept
{
    std::copy(other.begin(), other.end(), begin());
}

template<size_t SIZE>
Bitarray<SIZE> &Bitarray<SIZE>::operator=(const Bitarray &other) noexcept
{
    std::copy(other.begin(), other.end(), begin());
    return (*this);
}

//-------------------------- Bitarray Operators ------------------------------
template<size_t SIZE>
bool Bitarray<SIZE>::operator==(const Bitarray &other) const noexcept
{
    size_t endThis = m_container[BITARRAY_SIZE - 1];
    size_t endOther = other.m_container[BITARRAY_SIZE - 1];

    return (std::equal(begin(),(end() - 1), other.begin())
            && !((endOther ^ endThis) & MASK_REMAIN));
}

template<size_t SIZE>
bool Bitarray<SIZE>::operator!=(const Bitarray &other) const noexcept
{
    return !(*this == other);
}

template<size_t SIZE>
Bitarray<SIZE> &Bitarray<SIZE>::operator|=(const Bitarray &other) noexcept
{
    std::transform(begin(),end(), other.begin(),begin(),std::bit_or<size_t>());

    return (*this);
}

template<size_t SIZE>
Bitarray<SIZE> &Bitarray<SIZE>::operator&=(const Bitarray &other) noexcept
{
    std::transform(begin(),end(), other.begin(),begin(),std::bit_and<size_t>());

    return (*this);
}

template<size_t SIZE>
Bitarray<SIZE> &Bitarray<SIZE>::operator^=(const Bitarray &other) noexcept
{
    std::transform(begin(), end(), other.begin(), begin(), std::bit_xor<size_t>());

    return (*this);
}

template<size_t SIZE>
bool Bitarray<SIZE>::operator[](size_t index) const noexcept
{
    return GetBitNoThrow(index);
}

template<size_t SIZE>
typename Bitarray<SIZE>::Bit Bitarray<SIZE>::operator[](size_t index) noexcept
{
    return Bit(index, *this);
}

//---------------------------- Bitarray Methods -------------------------------
template<size_t SIZE>
size_t Bitarray<SIZE>::Count() const noexcept
{
    size_t *lastWord = const_cast<size_t*>(m_container);
    lastWord[BITARRAY_SIZE-1] &= MASK_REMAIN;

    const unsigned char *char_arr = reinterpret_cast<const unsigned char*>(m_container);
    size_t counter = std::accumulate (char_arr, char_arr + (BITARRAY_SIZE * BYTES_IN_WORD) , 0, CountBits());

    return counter;
}

template<size_t SIZE>
std::string Bitarray<SIZE>::ToString() const
{
    BitToString newString;
    newString = std::for_each(begin(), end(), newString);

    return newString.GetStr().substr(0, SIZE);
}

template<size_t SIZE>
Bitarray<SIZE> &Bitarray<SIZE>::Flip() noexcept
{
    std::transform(begin(), end(), begin(), FlipWord);

    return (*this);
}

template<size_t SIZE>
Bitarray<SIZE> &Bitarray<SIZE>::SetAll() noexcept
{
    std::fill(begin(), end(), ALL_SET);
    return (*this);
}

template<size_t SIZE>
Bitarray<SIZE> &Bitarray<SIZE>::ClearAll() noexcept
{
    std::fill(begin(), end(), 0);
    return (*this);
}

template<size_t SIZE>
bool Bitarray<SIZE>::GetBit(size_t index) const
{
    if (SIZE < index)
    {
        throw std::out_of_range("index is out of range");
    }

    return GetBitNoThrow(index);
}

template<size_t SIZE>
void Bitarray<SIZE>::SetBit(size_t index, bool state)
{
    if (SIZE < index)
    {
        throw std::out_of_range("index is out of range");
    }
    SetBitNoThrow(index, state);
}

template<size_t SIZE>
Bitarray<SIZE> &Bitarray<SIZE>::FlipBit(size_t index)
{
    if (SIZE < index)
    {
        throw std::out_of_range("index is out of range");
    }
    Bit toFlip(index, *this);
    toFlip.Flip();

    return (*this);
}

template<size_t SIZE>
typename Bitarray<SIZE>::Bit Bitarray<SIZE>::At(size_t index)
{
    if (SIZE < index)
    {
        throw std::out_of_range("index is out of range");
    }

    return Bit(index, *this);
}

//---------------------------- Bitarray Inlines -------------------------------
template<size_t SIZE>
size_t *Bitarray<SIZE>::begin() noexcept
{
    return m_container;
}

template<size_t SIZE>
size_t *Bitarray<SIZE>::end() noexcept
{
    return m_container + BITARRAY_SIZE;
}

template<size_t SIZE>
const size_t *Bitarray<SIZE>::begin() const noexcept
{
    return m_container;
}

template<size_t SIZE>
const size_t *Bitarray<SIZE>::end() const noexcept
{
    return m_container + BITARRAY_SIZE;
}
template<size_t SIZE>
size_t Bitarray<SIZE>::WordIdx(size_t index) const noexcept
{
    return (index / BITS_IN_WORD);
}

template<size_t SIZE>
size_t Bitarray<SIZE>::BitIdx(size_t index) const noexcept
{
    return (index & (BITS_IN_WORD - 1));
}

template<size_t SIZE>
void Bitarray<SIZE>::SetBitNoThrow(size_t index, bool state) noexcept
{
    size_t mask = m_container[WordIdx(index)] >> BitIdx(index);
    mask = state ^ (mask & MASK_BIT);

    mask <<= BitIdx(index);

    m_container[WordIdx(index)] ^= mask;
}

template<size_t SIZE>
bool Bitarray<SIZE>::GetBitNoThrow(size_t index) const noexcept
{
    return (MASK_BIT & (m_container[WordIdx(index)] >> BitIdx(index)));
}

//------------------------------- Bit Methods ---------------------------------
template<size_t SIZE>
Bitarray<SIZE>::Bit::Bit(size_t index, Bitarray &bitarray) noexcept :
        m_bitarray(bitarray), m_index(index)
{
//    empty
}

template<size_t SIZE>
Bitarray<SIZE>::Bit::operator bool() const noexcept
{
    return (m_bitarray.GetBitNoThrow(m_index));
}

template<size_t SIZE>
typename Bitarray<SIZE>::Bit &Bitarray<SIZE>::Bit::operator=
        (const Bitarray::Bit &other) noexcept
{
    bool state = other.m_bitarray.GetBitNoThrow(other.m_index);
    m_bitarray.SetBitNoThrow(m_index, state);

    return (*this);
}

template<size_t SIZE>
typename Bitarray<SIZE>::Bit &Bitarray<SIZE>::Bit::operator=(bool state) noexcept
{
    m_bitarray.SetBitNoThrow(m_index, state);

    return (*this);
}

template<size_t SIZE>
void Bitarray<SIZE>::Bit::Flip() noexcept
{
    m_bitarray.m_container[m_bitarray.WordIdx(m_index)] ^=
            (MASK_BIT << m_bitarray.BitIdx(m_index));
}
} //ilrd

#endif // ILRD_RD8788_BITARRAY
