#pragma once

#include <tests/_details/test.hpp>
#include <libds/amt/implicit_hierarchy.h>
#include <libds/amt/explicit_hierarchy.h>
#include <memory>
#include <vector>

namespace ds::tests
{
    namespace details
    {
        template<class Hierarchy>
        struct HierarchyFixture
        {
            std::unique_ptr<Hierarchy> hierarchy_;
            std::vector<int> preOrder_;
            std::vector<int> postOrder_;
            std::vector<int> levelOrder_;
            std::vector<int> inOrder_;
        };

        /**
         *         0
         *    /         \
         *    1         2
         *  / | \       |
         *  3 4 5       6
         */
        inline auto const makeMWEH = []()-> HierarchyFixture<amt::MultiWayExplicitHierarchy<int>>
        {
            auto hierarchy = std::make_unique<amt::MultiWayExplicitHierarchy<int>>();
            auto& root = hierarchy->emplaceRoot();
            auto& one = hierarchy->emplaceSon(root, 0);
            auto& two = hierarchy->emplaceSon(root, 1);
            root.data_ = 0;
            one.data_ = 1;
            two.data_ = 2;
            hierarchy->emplaceSon(one, 0).data_ = 3;
            hierarchy->emplaceSon(one, 1).data_ = 4;
            hierarchy->emplaceSon(one, 2).data_ = 5;
            hierarchy->emplaceSon(two, 0).data_ = 6;
            return
            {
                std::move(hierarchy),
                {0, 1, 3, 4, 5, 2, 6},
                {3, 4, 5, 1, 6, 2, 0},
                {0, 1, 2, 3, 4, 5, 6},
                {}
            };
        };

        /**
         *         0
         *    /    |    \
         *    1    -    2
         *  / | \     / | \
         *  3 - 4     - 5 -
         */
        inline auto const makeKWEH = []() -> HierarchyFixture<amt::KWayExplicitHierarchy<int, 3>>
        {
            auto hierarchy = std::make_unique<amt::KWayExplicitHierarchy<int, 3>>();
            auto& root = hierarchy->emplaceRoot();
            auto& one = hierarchy->emplaceSon(root, 0);
            auto& two = hierarchy->emplaceSon(root, 2);
            root.data_ = 0;
            one.data_ = 1;
            two.data_ = 2;
            hierarchy->emplaceSon(one, 0).data_ = 3;
            hierarchy->emplaceSon(one, 2).data_ = 4;
            hierarchy->emplaceSon(two, 1).data_ = 5;
            return
            {
                std::move(hierarchy),
                {0, 1, 3, 4, 2, 5},
                {3, 4, 1, 5, 2, 0},
                {0, 1, 2, 3, 4, 5},
                {}
            };
        };

        /*         10
         *    /          \
         *    5          15
         *  /   \      /
         *  2   7     12
         */
        inline auto const makeBIH = []() -> HierarchyFixture<amt::BinaryImplicitHierarchy<int>>
        {
            auto hierarchy = std::make_unique<amt::BinaryImplicitHierarchy<int>>();
            hierarchy->insertLastLeaf().data_ = 10;
            hierarchy->insertLastLeaf().data_ = 5;
            hierarchy->insertLastLeaf().data_ = 15;
            hierarchy->insertLastLeaf().data_ = 2;
            hierarchy->insertLastLeaf().data_ = 7;
            hierarchy->insertLastLeaf().data_ = 12;
            return
            {
                std::move(hierarchy),
                {10, 5, 2, 7, 15, 12},
                {2, 7, 5, 12, 15, 10},
                {10, 5, 15, 2, 7, 12},
                {2, 5, 7, 10, 12, 15}
            };
        };

        /*         10
         *    /          \
         *    5          15
         *  /   \      /   \
         *  2   7      -   20
         */
        inline auto const makeBEH = []() -> HierarchyFixture<amt::BinaryExplicitHierarchy<int>>
        {
            auto hierarchy = std::make_unique<amt::BinaryExplicitHierarchy<int>>();
            auto& root = hierarchy->emplaceRoot();
            auto& five = hierarchy->insertLeftSon(root);
            auto& fifteen = hierarchy->insertRightSon(root);
            auto& two = hierarchy->insertLeftSon(five);
            auto& seven = hierarchy->insertRightSon(five);
            auto& twenty = hierarchy->insertRightSon(fifteen);
            root.data_ = 10;
            five.data_ = 5;
            fifteen.data_ = 15;
            two.data_ = 2;
            seven.data_ = 7;
            twenty.data_ = 20;
            return
            {
                std::move(hierarchy),
                {10, 5, 2, 7, 15, 20},
                {2, 7, 5, 20, 15, 10},
                {10, 5, 15, 2, 7, 20},
                {2, 5, 7, 10, 15, 20}
            };
        };
    }

    /**
     *  @brief Tests pre-order traversal of a hierarchy.
     */
    template<class MakeFixtureType>
    class HierarchyTestProcessPreOrder : public LeafTest
    {
    public:
        HierarchyTestProcessPreOrder(MakeFixtureType makeFixture, const std::string& name) :
            LeafTest(name),
            makeFixture_(std::move(makeFixture))
        {
        }

    protected:
        void test() override
        {
            const auto fixture = makeFixture_();
            const auto& hierarchy = fixture.hierarchy_;
            const auto& order = fixture.preOrder_;
            auto orderIt = begin(order);
            hierarchy->processPreOrder(hierarchy->accessRoot(), [&orderIt, this](auto* node)
                {
                    this->assert_equals(*orderIt, node->data_);
                    ++orderIt;
                });
        }

    private:
        MakeFixtureType makeFixture_;
    };

    /**
     *  @brief Tests post-order traversal of a hierarchy.
     */
    template<class MakeFixtureType>
    class HierarchyTestProcessPostOrder : public LeafTest
    {
    public:
        HierarchyTestProcessPostOrder(MakeFixtureType makeFixture, const std::string& name) :
            LeafTest(name),
            makeFixture_(std::move(makeFixture))
        {
        }

    protected:
        void test() override
        {
            const auto fixture = makeFixture_();
            const auto& hierarchy = fixture.hierarchy_;
            const auto& order = fixture.postOrder_;
            auto orderIt = begin(order);
            hierarchy->processPostOrder(hierarchy->accessRoot(), [&orderIt, this](auto* node)
                {
                    this->assert_equals(*orderIt, node->data_);
                    ++orderIt;
                });
        }

    private:
        MakeFixtureType makeFixture_;
    };

    /**
     *  @brief Tests level-order traversal of a hierarchy.
     */
    template<class MakeFixtureType>
    class HierarchyTestProcessLevelOrder : public LeafTest
    {
    public:
        HierarchyTestProcessLevelOrder(MakeFixtureType makeFixture, const std::string& name) :
            LeafTest(name),
            makeFixture_(std::move(makeFixture))
        {
        }

    protected:
        void test() override
        {
            const auto fixture = makeFixture_();
            const auto& hierarchy = fixture.hierarchy_;
            const auto& order = fixture.levelOrder_;
            auto orderIt = begin(order);
            hierarchy->processLevelOrder(hierarchy->accessRoot(), [&orderIt, this](auto* node)
                {
                    this->assert_equals(*orderIt, node->data_);
                    ++orderIt;
                });
        }

    private:
        MakeFixtureType makeFixture_;
    };

    /**
     *  @brief Tests pre-order iterator.
     */
    template<class MakeFixtureType>
    class HierarchyTestPreOrderIterator : public LeafTest
    {
    public:
        HierarchyTestPreOrderIterator(MakeFixtureType makeFixture, const std::string& name) :
            LeafTest(name),
            makeFixture_(std::move(makeFixture))
        {
        }

    protected:
        void test() override
        {
            auto fixture = makeFixture_();
            auto& hierarchy = fixture.hierarchy_;
            auto orderIt = begin(fixture.preOrder_);
            auto hierarchyIt = hierarchy->beginPre();
            const auto size = static_cast<int>(hierarchy->size());
            for (int i = 0; i < size; ++i)
            {
                this->assert_equals(*orderIt, *hierarchyIt);
                ++orderIt;
                ++hierarchyIt;
            }
            this->assert_equals(hierarchy->endPre(), hierarchyIt);
        }

    private:
        MakeFixtureType makeFixture_;
    };

    /**
     *  @brief Tests post-order iterator.
     */
    template<class MakeFixtureType>
    class HierarchyTestPostOrderIterator : public LeafTest
    {
    public:
        HierarchyTestPostOrderIterator(MakeFixtureType makeFixture, const std::string& name) :
            LeafTest(name),
            makeFixture_(std::move(makeFixture))
        {
        }

    protected:
        void test() override
        {
            auto fixture = makeFixture_();
            auto& hierarchy = fixture.hierarchy_;
            auto orderIt = begin(fixture.postOrder_);
            auto hierarchyIt = hierarchy->beginPost();
            const auto size = static_cast<int>(hierarchy->size());
            for (int i = 0; i < size; ++i)
            {
                this->assert_equals(*orderIt, *hierarchyIt);
                ++orderIt;
                ++hierarchyIt;
            }
            this->assert_equals(hierarchy->endPost(), hierarchyIt);
        }

    private:
        MakeFixtureType makeFixture_;
    };

    /**
     *  @brief Tests in-order traversal of a binary hierarchy.
     */
    template<class MakeFixtureType>
    class BinaryHierarchyTestProcessInOrder : public LeafTest
    {
    public:
        BinaryHierarchyTestProcessInOrder(MakeFixtureType makeFixture, const std::string& name) :
            LeafTest(name),
            makeFixture_(std::move(makeFixture))
        {
        }

    protected:
        void test() override
        {
            const auto fixture = makeFixture_();;
            const auto& hierarchy = fixture.hierarchy_;
            const auto& order = fixture.inOrder_;
            auto orderIt = begin(order);
            hierarchy->processInOrder(hierarchy->accessRoot(), [&orderIt, this](auto* node)
                {
                    this->assert_equals(*orderIt, node->data_);
                    ++orderIt;
                });
        }

    private:
        MakeFixtureType makeFixture_;
    };

    /**
     *  @brief Tests post-order iterator.
     */
    template<class MakeFixture>
    class BinaryHierarchyTestInOrderIterator : public LeafTest
    {
    public:
        BinaryHierarchyTestInOrderIterator(MakeFixture makeFixture, const std::string& name) :
            LeafTest(name),
            makeFixture_(makeFixture)
        {
        }

    protected:
        void test() override
        {
            const auto fixture = makeFixture_();
            const auto& hierarchy = fixture.hierarchy_;
            const auto& order = fixture.inOrder_;
            auto orderIt = begin(order);
            auto hierarchyIt = hierarchy->begin();
            const auto size = static_cast<int>(hierarchy->size());
            for (int i = 0; i < size; ++i)
            {
                this->assert_equals(*orderIt, *hierarchyIt);
                ++orderIt;
                ++hierarchyIt;
            }
            this->assert_equals(hierarchy->end(), hierarchyIt);
        }

    private:
        MakeFixture makeFixture_;
    };

    /**
     *  @brief Test for processing elements in various orders.
     */
    class HierarchyTest : public CompositeTest
    {
    public:
        HierarchyTest() :
            CompositeTest("Hierarchy")
        {
            using MakeKWEHType = decltype(details::makeKWEH);
            using MakeMWEHType = decltype(details::makeMWEH);
            using MakeBIHType = decltype(details::makeBIH);
            using MakeBEHType = decltype(details::makeBEH);

            this->add_test(std::make_unique<HierarchyTestProcessPreOrder<MakeKWEHType>>(details::makeKWEH, "process-pre-order-kweh"));
            this->add_test(std::make_unique<HierarchyTestProcessPreOrder<MakeMWEHType>>(details::makeMWEH, "process-pre-order-mweh"));
            this->add_test(std::make_unique<HierarchyTestProcessPreOrder<MakeBIHType>>(details::makeBIH, "process-pre-order-bih"));
            this->add_test(std::make_unique<HierarchyTestProcessPostOrder<MakeKWEHType>>(details::makeKWEH, "process-post-order-kweh"));
            this->add_test(std::make_unique<HierarchyTestProcessPostOrder<MakeMWEHType>>(details::makeMWEH, "process-post-order-mweh"));
            this->add_test(std::make_unique<HierarchyTestProcessPostOrder<MakeBIHType>>(details::makeBIH, "process-post-order-bih"));
            this->add_test(std::make_unique<HierarchyTestProcessLevelOrder<MakeKWEHType>>(details::makeKWEH, "process-level-order-kweh"));
            this->add_test(std::make_unique<HierarchyTestProcessLevelOrder<MakeMWEHType>>(details::makeMWEH, "process-level-order-mweh"));
            this->add_test(std::make_unique<HierarchyTestProcessLevelOrder<MakeBIHType>>(details::makeBIH, "process-level-order-bih"));
            this->add_test(std::make_unique<HierarchyTestPreOrderIterator<MakeKWEHType>>(details::makeKWEH, "pre-order-iterator-kweh"));
            this->add_test(std::make_unique<HierarchyTestPreOrderIterator<MakeMWEHType>>(details::makeMWEH, "pre-order-iterator-mweh"));
            this->add_test(std::make_unique<HierarchyTestPreOrderIterator<MakeBIHType>>(details::makeBIH, "pre-order-iterator-bih"));
            this->add_test(std::make_unique<HierarchyTestPostOrderIterator<MakeKWEHType>>(details::makeKWEH, "post-order-iterator-kweh"));
            this->add_test(std::make_unique<HierarchyTestPostOrderIterator<MakeMWEHType>>(details::makeMWEH, "post-order-iterator-mweh"));
            this->add_test(std::make_unique<HierarchyTestPostOrderIterator<MakeBIHType>>(details::makeBIH, "post-order-iterator-bih"));
            this->add_test(std::make_unique<BinaryHierarchyTestProcessInOrder<MakeBIHType>>(details::makeBIH, "process-in-order-bih"));
            this->add_test(std::make_unique<BinaryHierarchyTestProcessInOrder<MakeBEHType>>(details::makeBEH, "process-in-order-beh"));
            this->add_test(std::make_unique<BinaryHierarchyTestInOrderIterator<MakeBIHType>>(details::makeBIH, "in-order-iterator-bih"));
            this->add_test(std::make_unique<BinaryHierarchyTestInOrderIterator<MakeBEHType>>(details::makeBEH, "in-order-iterator-beh"));
        }
    };
}
