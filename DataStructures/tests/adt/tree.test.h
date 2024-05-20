#pragma once

#include <tests/_details/test.hpp>

namespace ds::tests
{
    class TreeTest : public CompositeTest
    {
    public:
        TreeTest() :
            CompositeTest("Tree")
        {
        }
    };
}