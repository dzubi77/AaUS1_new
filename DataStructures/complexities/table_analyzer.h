#pragma once

#include <complexities/complexity_analyzer.h>
#include <libds/adt/table.h>
#include <random>

namespace ds::utils
{
    /**
     * @brief Common base for list analyzers.
     */
    template<class Table>
    class TableAnalyzer : public ComplexityAnalyzer<Table>
    {
    protected:
        explicit TableAnalyzer(const std::string& name);

    protected:
        void growToSize(Table& structure, size_t size) override;

        int key_;
        int data_;

    private:
        std::default_random_engine rngData_;
        std::default_random_engine rngKey_;
    };

    /**
     * @brief Analyzes complexity of an insertion at the beginning.
     */
    template<class Table>
    class TableInsertAnalyzer : public TableAnalyzer<Table>
    {
    public:
        explicit TableInsertAnalyzer(const std::string& name);

    protected:
        void executeOperation(Table& structure) override;
    };

    /**
     * @brief Container for all list analyzers.
     */
    class TablesAnalyzer : public CompositeAnalyzer
    {
    public:
        TablesAnalyzer();
    };

    //----------

    template<class Table>
    TableAnalyzer<Table>::TableAnalyzer(const std::string& name) :
        ComplexityAnalyzer<Table>(name),
        key_(0), data_(0), rngData_(144), rngKey_(144)
    {
        this->registerBeforeOperation([this](Table& table)
            {
                std::uniform_int_distribution dist(1, 10000000);
                key_ = dist(rngKey_);
                data_ = rngData_();
                while (table.contains(key_))
                {
                    key_ = dist(rngKey_);
                }
            });
    }

    template <class Table>
    void TableAnalyzer<Table>::growToSize(Table& structure, size_t size)
    {
        const size_t neededCount = size - structure.size();

        for (size_t i = 0; i < neededCount; ++i)
        {
            std::uniform_int_distribution dist(1, 10000000);
            int key = dist(rngKey_);
            int data = rngData_();
            while (structure.contains(key))
            {
                key = dist(rngKey_);
            }
            structure.insert(key, data);
        }
    }

    //----------

    template <class Table>
    TableInsertAnalyzer<Table>::TableInsertAnalyzer(const std::string& name) :
        TableAnalyzer<Table>(name)
    {
    }

    template <class Table>
    void TableInsertAnalyzer<Table>::executeOperation(Table& structure)
    {
        structure.insert(this->key_, this->data_);
    }

    //----------

    inline TablesAnalyzer::TablesAnalyzer() :
        CompositeAnalyzer("Tables")
    {
        this->addAnalyzer(std::make_unique<TableInsertAnalyzer<adt::SortedSequenceTable<int, int>>>("SortedSTab-insert"));
        this->addAnalyzer(std::make_unique<TableInsertAnalyzer<adt::BinarySearchTree<int, int>>>("BST-insert"));
    }
}
