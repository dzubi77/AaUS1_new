#pragma once

#include <complexities/complexity_analyzer.h>
#include <list>
#include <random>
#include <vector>

namespace ds::utils
{
    /**
     * @brief Common base for list analyzers.
     */
    template<class List>
    class ListAnalyzer : public ComplexityAnalyzer<List>
    {
    protected:
        explicit ListAnalyzer(const std::string& name);

    protected:
        void growToSize(List& structure, size_t size) override;        

        size_t getRandomIndex() const;
        int getRandomData() const;

    private:
        std::default_random_engine rngData_;
        std::default_random_engine rngIndex_;
        size_t index_;
        int data_;
    };

    /**
     * @brief Analyzes complexity of an insertion at the beginning.
     */
    template<class List>
    class ListInsertAnalyzer : public ListAnalyzer<List>
    {
    public:
        explicit ListInsertAnalyzer(const std::string& name);

    protected:
        void executeOperation(List& structure) override;
    };

    /**
     * @brief Analyzes complexity of an erasure at the beginning.
     */
    template<class List>
    class ListRemoveAnalyzer : public ListAnalyzer<List>
    {
    public:
        explicit ListRemoveAnalyzer(const std::string& name);

    protected:
        void executeOperation(List& structure) override;
    };

    /**
     * @brief Container for all list analyzers.
     */
    class ListsAnalyzer : public CompositeAnalyzer
    {
    public:
        ListsAnalyzer();
    };

    //----------

    template<class List>
    ListAnalyzer<List>::ListAnalyzer(const std::string& name) :
        ComplexityAnalyzer<List>(name),
        rngData_(144),
        rngIndex_(144),
        index_(0),
        data_(0)
    {
        // 01
        this->registerBeforeOperation([this](List& list)
            {
                std::uniform_int_distribution dist(100, 10000);
                data_ = dist(rngData_);
            });
    }

    template <class List>
    void ListAnalyzer<List>::growToSize(List& structure, size_t size)
    {
        // 01
        size_t count = size - structure.size();
        for (size_t i = 0; i < count; ++i)
        {
            structure.push_back(this->rngData_()); // IntelliSense nejde :(
        }
    }

    template<class List>
    size_t ListAnalyzer<List>::getRandomIndex() const
    {
        return index_;
    }

    template<class List>
    int ListAnalyzer<List>::getRandomData() const
    {
        return data_;
    }

    //----------

    template <class List>
    ListInsertAnalyzer<List>::ListInsertAnalyzer(const std::string& name) :
        ListAnalyzer<List>(name)
    {
    }

    template <class List>
    void ListInsertAnalyzer<List>::executeOperation(List& structure)
    {
        // 01
        structure.insert(structure.begin(), this->getRandomData());
    }

    //----------

    template <class List>
    ListRemoveAnalyzer<List>::ListRemoveAnalyzer(const std::string& name) :
        ListAnalyzer<List>(name)
    {
    }

    template <class List>
    void ListRemoveAnalyzer<List>::executeOperation(List& structure)
    {
        // 01
        structure.erase(structure.begin());
    }

    //----------

    inline ListsAnalyzer::ListsAnalyzer() :
        CompositeAnalyzer("Lists")
    {
        this->addAnalyzer(std::make_unique<ListInsertAnalyzer<std::vector<int>>>("vector-insert"));
        this->addAnalyzer(std::make_unique<ListInsertAnalyzer<std::list<int>>>("list-insert"));
        this->addAnalyzer(std::make_unique<ListRemoveAnalyzer<std::vector<int>>>("vector-remove"));
        this->addAnalyzer(std::make_unique<ListRemoveAnalyzer<std::list<int>>>("list-remove"));
    }
}
