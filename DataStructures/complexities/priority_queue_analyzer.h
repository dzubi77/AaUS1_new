#pragma once

#include <complexities/complexity_analyzer.h>
#include <libds/adt/priority_queue.h>
#include <random>

namespace ds::utils
{
    /**
     * @brief Common base for list analyzers.
     */
    template<class PriorityQueue>
    class PriorityQueueAnalyzer : public ComplexityAnalyzer<PriorityQueue>
    {
    protected:
        explicit PriorityQueueAnalyzer(const std::string& name);
         
    protected:
        void growToSize(PriorityQueue& structure, size_t size) override;

        int key_;
        int data_;

    private:
        std::default_random_engine rngData_;
        std::default_random_engine rngKey_;
    };

    /**
     * @brief Analyzes complexity of pushing to priority queue.
     */
    template<class PriorityQueue>
    class PriorityQueuePushAnalyzer : public PriorityQueueAnalyzer<PriorityQueue>
    {
    public:
        explicit PriorityQueuePushAnalyzer(const std::string& name);

    protected:
        void executeOperation(PriorityQueue& structure) override;
    };

    /**
     * @brief Analyzes complexity of popping from priority queue.
     */
    template<class PriorityQueue>
    class PriorityQueuePopAnalyzer : public PriorityQueueAnalyzer<PriorityQueue>
    {
    public:
        explicit PriorityQueuePopAnalyzer(const std::string& name);

    protected:
        void executeOperation(PriorityQueue& structure) override;
    };

    /**
     * @brief Container for all list analyzers.
     */
    class PriorityQueuesAnalyzer : public CompositeAnalyzer
    {
    public:
        PriorityQueuesAnalyzer();
    };

    //----------

    template<class PriorityQueue>
    PriorityQueueAnalyzer<PriorityQueue>::PriorityQueueAnalyzer(const std::string& name) :
        ComplexityAnalyzer<PriorityQueue>(name),
        key_(0), data_(0), rngData_(144), rngKey_(144)
    {
        this->registerBeforeOperation([this](PriorityQueue& priorityQueue)
            {
            });
    }

    template <class PriorityQueue>
    void PriorityQueueAnalyzer<PriorityQueue>::growToSize(PriorityQueue& structure, size_t size)
    {
    }

    //----------

    template <class PriorityQueue>
    PriorityQueuePushAnalyzer<PriorityQueue>::PriorityQueuePushAnalyzer(const std::string& name) :
        PriorityQueueAnalyzer<PriorityQueue>(name)
    {
    }

    template <class PriorityQueue>
    void PriorityQueuePushAnalyzer<PriorityQueue>::executeOperation(PriorityQueue& structure)
    {
    }

    //----------

    template <class PriorityQueue>
    PriorityQueuePopAnalyzer<PriorityQueue>::PriorityQueuePopAnalyzer(const std::string& name) :
        PriorityQueueAnalyzer<PriorityQueue>(name)
    {
    }

    template <class PriorityQueue>
    void PriorityQueuePopAnalyzer<PriorityQueue>::executeOperation(PriorityQueue& structure)
    {
    }

    //----------

    inline PriorityQueuesAnalyzer::PriorityQueuesAnalyzer() :
        CompositeAnalyzer("Priority queues")
    {
        this->addAnalyzer(std::make_unique<PriorityQueueAnalyzer<adt::TwoLists<int, int>>>("TwoLists"));
        this->addAnalyzer(std::make_unique<PriorityQueuePushAnalyzer<adt::TwoLists<int, int>>>("TwoLists-push"));
        this->addAnalyzer(std::make_unique<PriorityQueuePopAnalyzer<adt::TwoLists<int, int>>>("TwoLists-pop"));
    }
}
