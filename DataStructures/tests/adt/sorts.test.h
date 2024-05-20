#pragma once

#include <algorithm>
#include <functional>
#include <libds/amt/implicit_sequence.h>
#include <libds/adt/sorts.h>
#include <random>
#include <string>
#include <tests/_details/test.hpp>

namespace ds::tests
{
    namespace details
    {
        auto constexpr makeReverseComparator = [](auto const& cmp)
        {
            return [cmp](auto const& l, auto const& r)
            {
                return cmp(r, l);
            };
        };

        class RandomStringKeyGenerator
        {
        public:
            RandomStringKeyGenerator(std::mt19937_64& seeder) :
                rngLength_(seeder()),
                rngChar_(seeder()),
                distLen_(std::size_t{2}, std::size_t{10}),
                distChar_(static_cast<int>('a'), static_cast<int>('z'))
            {
            }

            amt::ImplicitSequence<std::string*> operator()(std::size_t const n)
            {
                amt::ImplicitSequence<std::string*>keys(n, false);
                for (std::size_t i = 0; i < n; ++i)
                {
                    auto key = new std::string();
                    size_t const keyLength = distLen_(rngLength_);
                    key->reserve(keyLength);
                    for (std::size_t j = 0; j < keyLength; ++j)
                    {
                        *key += static_cast<char>(distChar_(rngChar_));
                    }
                    keys.insertLast().data_ = key;
                }
                return keys;
            }

        private:
            std::mt19937_64 rngLength_;
            std::mt19937_64 rngChar_;
            std::uniform_int_distribution<std::size_t> distLen_;
            std::uniform_int_distribution<int> distChar_;
        };

        struct StringKeyDisposer
        {
            void operator()(amt::ImplicitSequence<std::string*>& keys) const
            {
                for (std::string* key : keys)
                {
                    delete key;
                }
            }
        };

        class RandomIntKeyGenerator
        {
        public:
            RandomIntKeyGenerator(std::mt19937_64& seeder) :
                rngKey_(seeder())
            {
            }

            amt::ImplicitSequence<int> operator()(std::size_t const n)
            {
                auto keys = amt::ImplicitSequence<int>(n, false);
                for (auto i = 0; i < n; ++i)
                {
                    keys.insertLast().data_ = distKey_(rngKey_);
                }
                return keys;
            }

        private:
            std::mt19937_64 rngKey_;
            std::uniform_int_distribution<int> distKey_;
        };

        struct IntKeyDisposer
        {
            void operator()(amt::ImplicitSequence<int>&) const
            {
            }
        };

        struct StringLengthComparator
        {
            bool operator()(std::string* const lhs, std::string* const rhs) const
            {
                return lhs->size() < rhs->size();
            }
        };

        using ReverseStringLengthComparator = decltype(makeReverseComparator(StringLengthComparator()));

        struct StringPtrComparator
        {
            bool operator()(std::string* const lhs, std::string* const rhs) const
            {
                return *lhs < *rhs;
            }
        };

        using ReverseStringPtrComparator = decltype(makeReverseComparator(StringPtrComparator()));
    }

    /**
     * @brief Tests sorting algorithm
     * @tparam SortT Sort type
     * @tparam Comparator Comparator type
     * @tparam KeyGenerator Function object that generates sequence of n keys
     * @tparam KeyDisposer Function object that disposes keys
     */
    template<
        class SortT,
        class Comparator,
        class KeyGenerator,
        class KeyDisposer
    >
    class GenericSortTest : public LeafTest
    {
    public:
        GenericSortTest(
            const std::string& name,
            KeyGenerator keyGenerator,
            KeyDisposer keyDisposer,
            Comparator cmp,
            size_t elementCount
        ) :
            LeafTest(name),
            keyGenerator_(std::move(keyGenerator)),
            keyDisposer_(std::move(keyDisposer)),
            cmp_(std::move(cmp)),
            elementCount_(elementCount)
        {
        }

    protected:
        void test() override
        {
            auto keys = keyGenerator_(elementCount_);
            auto sorter = SortT();
            sorter.sort(keys, cmp_);
            this->assert_true(std::is_sorted(keys.begin(), keys.end(), cmp_), "Is sorted.");
            keyDisposer_(keys);
        }

    private:
        KeyGenerator keyGenerator_;
        KeyDisposer keyDisposer_;
        Comparator cmp_;
        size_t elementCount_;
    };

    /**
     * @brief Creates tests with int and string keys for @p SortT
     * @tparam SortT Sort type
     */
    template<template<class> class SortT>
    class MultiCmpSortTest : public CompositeTest
    {
    public:
        MultiCmpSortTest(std::string const& name, std::mt19937_64& seeder, std::initializer_list<int> elementCounts) :
            tests::CompositeTest(name)
        {
            using namespace details;
            for (auto const n : elementCounts)
            {
                this->add_test(std::make_unique<GenericSortTest<SortT<int>, std::less<>, RandomIntKeyGenerator, IntKeyDisposer>>(
                    "int-keys-asc-" + std::to_string(n),
                    RandomIntKeyGenerator(seeder),
                    IntKeyDisposer(),
                    std::less<>(),
                    n
                ));
                this->add_test(std::make_unique<GenericSortTest<SortT<int>, std::greater<>, RandomIntKeyGenerator, IntKeyDisposer>>(
                    "int-keys-desc-" + std::to_string(n),
                    RandomIntKeyGenerator(seeder),
                    IntKeyDisposer(),
                    std::greater<>(),
                    n
                ));
                this->add_test(std::make_unique<GenericSortTest<SortT<std::string*>, StringPtrComparator, RandomStringKeyGenerator, StringKeyDisposer>>(
                    "string-keys-asc-" + std::to_string(n),
                    RandomStringKeyGenerator(seeder),
                    StringKeyDisposer(),
                    StringPtrComparator(),
                    n
                ));
                this->add_test(std::make_unique<GenericSortTest<SortT<std::string*>, ReverseStringPtrComparator, RandomStringKeyGenerator, StringKeyDisposer>>(
                    "string-keys-desc-" + std::to_string(n),
                    RandomStringKeyGenerator(seeder),
                    StringKeyDisposer(),
                    makeReverseComparator(StringPtrComparator()),
                    n
                ));
                this->add_test(std::make_unique<GenericSortTest<SortT<std::string*>, StringLengthComparator, RandomStringKeyGenerator, StringKeyDisposer>>(
                    "string-keys-length-asc-" + std::to_string(n),
                    RandomStringKeyGenerator(seeder),
                    StringKeyDisposer(),
                    StringLengthComparator(),
                    n
                ));
                this->add_test(std::make_unique<GenericSortTest<SortT<std::string*>, ReverseStringLengthComparator, RandomStringKeyGenerator, StringKeyDisposer>>(
                    "string-keys-length-desc-" + std::to_string(n),
                    RandomStringKeyGenerator(seeder),
                    StringKeyDisposer(),
                    makeReverseComparator(StringLengthComparator()),
                    n
                ));
            }
        }
    };

    /**
     * @brief Special test case for radix sort
     */
    class RadixSortTest : public GenericSortTest<adt::RadixSort<int, int>, std::less<>, details::RandomIntKeyGenerator, details::IntKeyDisposer>
    {
    public:
        RadixSortTest(std::mt19937_64& seeder) :
            GenericSortTest(
                "RadixSort",
                details::RandomIntKeyGenerator(seeder),
                details::IntKeyDisposer(),
                std::less<>(),
                1'000
            )
        {
        }
    };

    /**
     * @brief All sort tests
     */
    class SortTest : public CompositeTest
    {
    public:
        SortTest() :
            CompositeTest("Sort")
        {
            auto seeder = std::mt19937_64(247);

            auto const smallNs = { 1,2,3,100 };
            auto const bigNs = { 1,2,3,10'000 };

            this->add_test(std::make_unique<MultiCmpSortTest<adt::SelectSort>>("SelectSort", seeder, smallNs));
            this->add_test(std::make_unique<MultiCmpSortTest<adt::BubbleSort>>("BubbleSort", seeder, smallNs));
            this->add_test(std::make_unique<MultiCmpSortTest<adt::InsertSort>>("InsertSort", seeder, smallNs));
            this->add_test(std::make_unique<MultiCmpSortTest<adt::QuickSort>>("QuickSort", seeder, bigNs));
            this->add_test(std::make_unique<MultiCmpSortTest<adt::HeapSort>>("HeapSort", seeder, bigNs));
            this->add_test(std::make_unique<MultiCmpSortTest<adt::ShellSort>>("ShellSort", seeder, bigNs));
            this->add_test(std::make_unique<MultiCmpSortTest<adt::MergeSort>>("MergeSort", seeder, bigNs));
            this->add_test(std::make_unique<RadixSortTest>(seeder));
        }
    };
}