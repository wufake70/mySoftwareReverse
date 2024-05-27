#pragma once

#pragma once

// reference https://github.com/andrivet/ADVobfuscator

#if defined(_MSC_VER)
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE __attribute__((always_inline))
#endif

#include <iomanip>
#include <iostream>


// std::index_sequence will be available with C++14 (C++1y). For the moment, implement a (very) simplified and partial version. You can find more complete versions on the Internet
// MakeIndex<N>::type generates Indexes<0, 1, 2, 3, ..., N>

namespace andrivet {
    namespace ADVobfuscator {

        template<int... I>
        struct Indexes { using type = Indexes<I..., sizeof...(I)>; };

        template<int N>
        struct Make_Indexes { using type = typename Make_Indexes<N - 1>::type::type; };

        template<>
        struct Make_Indexes<0> { using type = Indexes<>; };

    }
}


// Very simple compile-time random numbers generator.

// For a more complete and sophisticated example, see:
// http://www.researchgate.net/profile/Zalan_Szgyi/publication/259005783_Random_number_generator_for_C_template_metaprograms/file/e0b49529b48272c5a6.pdf

#include <random>

namespace andrivet {
    namespace ADVobfuscator {

        namespace
        {
            // I use current (compile time) as a seed

            constexpr char time[] = __TIME__; // __TIME__ has the following format: hh:mm:ss in 24-hour time

            // Convert time string (hh:mm:ss) into a number
            constexpr int DigitToInt(char c) { return c - '0'; }
            const int seed = DigitToInt(time[7]) +
                DigitToInt(time[6]) * 10 +
                DigitToInt(time[4]) * 60 +
                DigitToInt(time[3]) * 600 +
                DigitToInt(time[1]) * 3600 +
                DigitToInt(time[0]) * 36000;
        }

        // 1988, Stephen Park and Keith Miller
        // "Random Number Generators: Good Ones Are Hard To Find", considered as "minimal standard"
        // Park-Miller 31 bit pseudo-random number generator, implemented with G. Carta's optimisation:
        // with 32-bit math and without division

        template<int N>
        struct MetaRandomGenerator
        {
        private:
            static constexpr unsigned a = 16807;        // 7^5
            static constexpr unsigned m = 2147483647;   // 2^31 - 1

            static constexpr unsigned s = MetaRandomGenerator<N - 1>::value;
            static constexpr unsigned lo = a * (s & 0xFFFF);                // Multiply lower 16 bits by 16807
            static constexpr unsigned hi = a * (s >> 16);                   // Multiply higher 16 bits by 16807
            static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16);     // Combine lower 15 bits of hi with lo's upper bits
            static constexpr unsigned hi2 = hi >> 15;                       // Discard lower 15 bits of hi
            static constexpr unsigned lo3 = lo2 + hi;

        public:
            static constexpr unsigned max = m;
            static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
        };

        template<>
        struct MetaRandomGenerator<0>
        {
            static constexpr unsigned value = seed;
        };

        // Note: A bias is introduced by the modulo operation.
        // However, I do belive it is neglictable in this case (M is far lower than 2^31 - 1)

        template<int N, int M>
        struct MetaRandom
        {
            static const int value = MetaRandomGenerator<N + 1>::value % M;
        };
    }
}

namespace andrivet {
    namespace ADVobfuscator {

        struct HexChar
        {
            unsigned char c_;
            unsigned width_;
            HexChar(unsigned char c, unsigned width) : c_{ c }, width_{ width } {}
        };

        inline std::ostream& operator<<(std::ostream& o, const HexChar& c)
        {
            return (o << std::setw(c.width_) << std::setfill('0') << std::hex << (int)c.c_ << std::dec);
        }

        inline HexChar hex(char c, int w = 2)
        {
            return HexChar(c, w);
        }

    }
}

namespace andrivet {
    namespace ADVobfuscator {

        // Represents an obfuscated string, parametrized with an alrorithm number N, a list of indexes Indexes and a key Key

        template<int N, char Key, typename Indexes>
        struct MetaString;

        // Partial specialization with a list of indexes I, a key K and algorithm N = 0
        // Each character is encrypted (XOR) with the same key

        template<char K, int... I>
        struct MetaString<0, K, Indexes<I...>>
        {
            // Constructor. Evaluated at compile time.
            constexpr ALWAYS_INLINE MetaString(const char* str)
                : key_{ K }, buffer_{ encrypt(str[I], K)... } { }

            // Runtime decryption. Most of the time, inlined
            inline const char* decrypt()
            {
                for (size_t i = 0; i < sizeof...(I); ++i)
                    buffer_[i] = decrypt(buffer_[i]);
                buffer_[sizeof...(I)] = 0;
                //LOG("--- Implementation #" << 0 << " with key 0x" << hex(key_));
                return const_cast<const char*>(buffer_);
            }

        private:
            // Encrypt / decrypt a character of the original string with the key
            constexpr char key() const { return key_; }
            constexpr char ALWAYS_INLINE encrypt(char c, int k) const { return c ^ k; }
            constexpr char decrypt(char c) const { return encrypt(c, key()); }

            volatile int key_; // key. "volatile" is important to avoid uncontrolled over-optimization by the compiler
            volatile char buffer_[sizeof...(I) + 1]; // Buffer to store the encrypted string + terminating null byte
        };

        // Partial specialization with a list of indexes I, a key K and algorithm N = 1
        // Each character is encrypted (XOR) with an incremented key.

        template<char K, int... I>
        struct MetaString<1, K, Indexes<I...>>
        {
            // Constructor. Evaluated at compile time.
            constexpr ALWAYS_INLINE MetaString(const char* str)
                : key_(K), buffer_{ encrypt(str[I], I)... } { }

            // Runtime decryption. Most of the time, inlined
            inline const char* decrypt()
            {
                for (size_t i = 0; i < sizeof...(I); ++i)
                    buffer_[i] = decrypt(buffer_[i], i);
                buffer_[sizeof...(I)] = 0;
                //LOG("--- Implementation #" << 1 << " with key 0x" << hex(key_));
                return const_cast<const char*>(buffer_);
            }

        private:
            // Encrypt / decrypt a character of the original string with the key
            constexpr char key(size_t position) const { return static_cast<char>(key_ + position); }
            constexpr char ALWAYS_INLINE encrypt(char c, size_t position) const { return c ^ key(position); }
            constexpr char decrypt(char c, size_t position) const { return encrypt(c, position); }

            volatile int key_; // key. "volatile" is important to avoid uncontrolled over-optimization by the compiler
            volatile char buffer_[sizeof...(I) + 1]; // Buffer to store the encrypted string + terminating null byte
        };

        // Partial specialization with a list of indexes I, a key K and algorithm N = 2
        // Shift the value of each character and does not store the key. It is only used at compile-time.

        template<char K, int... I>
        struct MetaString<2, K, Indexes<I...>>
        {
            // Constructor. Evaluated at compile time. Key is *not* stored
            constexpr ALWAYS_INLINE MetaString(const char* str)
                : buffer_{ encrypt(str[I])..., 0 } { }

            // Runtime decryption. Most of the time, inlined
            inline const char* decrypt()
            {
                for (size_t i = 0; i < sizeof...(I); ++i)
                    buffer_[i] = decrypt(buffer_[i]);
                //LOG("--- Implementation #" << 2 << " with key 0x" << hex(K));
                return const_cast<const char*>(buffer_);
            }

        private:
            // Encrypt / decrypt a character of the original string with the key
            // Be sure that the encryption key is never 0.
            constexpr char key(char key) const { return 1 + (key % 13); }
            constexpr char ALWAYS_INLINE encrypt(char c) const { return c + key(K); }
            constexpr char decrypt(char c) const { return c - key(K); }

            // Buffer to store the encrypted string + terminating null byte. Key is not stored
            volatile char buffer_[sizeof...(I) + 1];
        };

        // Helper to generate a key
        template<int N>
        struct MetaRandomChar
        {
            // Use 0x7F as maximum value since most of the time, char is signed (we have however 1 bit less of randomness)
            static const char value = static_cast<char>(1 + MetaRandom<N, 0x7F - 1>::value);
        };
    }
}

// Prefix notation
//#define DEF_OBFUSCATED(str) andrivet::ADVobfuscator::MetaString<andrivet::ADVobfuscator::MetaRandom<__COUNTER__, 3>::value, andrivet::ADVobfuscator::MetaRandomChar<__COUNTER__>::value, andrivet::ADVobfuscator::Make_Indexes<sizeof(str) - 1>::type>(str)
//#define OBFUSCATED(str) (DEF_OBFUSCATED(str).decrypt())

#define DEF(str) andrivet::ADVobfuscator::MetaString<andrivet::ADVobfuscator::MetaRandom<__COUNTER__, 3>::value, andrivet::ADVobfuscator::MetaRandomChar<__COUNTER__>::value, andrivet::ADVobfuscator::Make_Indexes<sizeof(str) - 1>::type>(str)
#define O(str) (DEF(str).decrypt())
#define A(str) ((char*)DEF(str).decrypt())