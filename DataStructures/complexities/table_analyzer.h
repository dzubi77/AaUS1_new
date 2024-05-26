#pragma once

#include <complexities/complexity_analyzer.h>
#include <libds/adt/table.h>

#include <random>

#include "list_analyzer.h"

namespace ds::utils
{
    /**
     * @brief Common base for list analyzers.
     */
    template<class Table >
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
     * @brief Analyzes complexity of an insertion.
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
     * @brief Analyzes complexity of an finding.
     */
    template<class Table>
    class TableTryFindAnalyzer : public TableAnalyzer<Table>
    {
    public:
        explicit TableTryFindAnalyzer(const std::string& name);

    protected:
        void executeOperation(Table& structure) override;
    };

    /**
     * @brief Container for all table analyzers.
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
        rngData_(144),
        rngKey_(144),
        key_(0),
        data_(0)
    {
        // 01
        this->registerBeforeOperation([this](Table& table)
            {
                std::uniform_int_distribution dist(1, 1000000);
                this->key_ = dist(this->rngKey_);
				while (table.contains(this->key_))
				{
                    this->key_ = dist(this->rngKey_);
				}
                this->data_ = this->rngData_();
            });
    }

    template <class Table>
    void TableAnalyzer<Table>::growToSize(Table& structure, size_t size)
    {
        // 01
        size_t count = size - structure.size();
        for (size_t i = 0; i < count; ++i)
        {
            std::uniform_int_distribution dist(1, 1000000);
            int key = dist(this->rngKey_);
            int data = this->rngData_();
            while (structure.contains(key))
            {
                key = dist(this->rngKey_);
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

    template <class Table>
    TableTryFindAnalyzer<Table>::TableTryFindAnalyzer(const std::string& name) :
        TableAnalyzer<Table>(name)
    {
    }

    template <class Table>
    void TableTryFindAnalyzer<Table>::executeOperation(Table& structure)
    {
    }

    //----------

    inline TablesAnalyzer::TablesAnalyzer() :
        CompositeAnalyzer("Tables")
    {
        this->addAnalyzer(std::make_unique<TableInsertAnalyzer<adt::HashTable<int, int>>>("HashTable-insert"));
        this->addAnalyzer(std::make_unique<TableTryFindAnalyzer<adt::HashTable<int, int>>>("HashTable-tryFind"));
    }
}
