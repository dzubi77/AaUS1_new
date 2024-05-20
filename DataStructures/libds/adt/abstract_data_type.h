#pragma once

#include <libds/amt/abstract_memory_type.h>
#include <stdexcept>
#include <string>

namespace ds::adt {

    class structure_error :
        public std::runtime_error
    {
    public:
        structure_error(const std::string& what) : std::runtime_error(what) {}
        structure_error(const structure_error& other) noexcept : std::runtime_error(other) {}
    };

    //----------

    class AbstractDataType
    {
    public:
        virtual ~AbstractDataType() = default;
        virtual AbstractDataType& assign(const AbstractDataType& other) = 0;
        virtual void clear() = 0;
        virtual size_t size() const = 0;
        virtual bool isEmpty() const = 0;
        virtual bool equals(const AbstractDataType& other) = 0;
    };

    using ADT = AbstractDataType;

    //----------

    template<typename T>
    class AbstractDataStructure :
        virtual public ADT
    {
    public:
        explicit AbstractDataStructure(amt::AMT* memoryStructure);
        AbstractDataStructure(const AbstractDataStructure& other) = delete;
        AbstractDataStructure(amt::AMT* memoryStructure, const AbstractDataStructure& other);
        ~AbstractDataStructure() override;

        ADT& assign(const ADT& other) override;
        void clear() override;
        size_t size() const override;
        bool isEmpty() const override;
        bool equals(const ADT& other) override;

    protected:
        amt::AMT* memoryStructure_;
    };

    template<typename T>
    using ADS = AbstractDataStructure<T>;

    //----------

    template<typename T>
    class AbstractDataMultistructure :
        virtual public ADT
    {
    };

    template<typename T>
    using AUMS = AbstractDataMultistructure<T>;

    //----------

    template<typename T>
    AbstractDataStructure<T>::AbstractDataStructure(amt::AMT* memoryStructure) :
        memoryStructure_(memoryStructure)
    {
    }

    template<typename T>
    AbstractDataStructure<T>::AbstractDataStructure(amt::AMT* memoryStructure, const AbstractDataStructure& other):
        AbstractDataStructure(memoryStructure)
    {
        AbstractDataStructure<T>::assign(other);
    }

    template<typename T>
    AbstractDataStructure<T>::~AbstractDataStructure()
    {
        delete memoryStructure_;
    }

    template<typename T>
    ADT& AbstractDataStructure<T>::assign(const ADT& other)
    {
        if (this != &other)
        {
            const ADS<T>& otherADS = dynamic_cast<const ADS<T>&>(other);
            memoryStructure_->assign(*otherADS.memoryStructure_);
        }

        return *this;
    }

    template<typename T>
    void AbstractDataStructure<T>::clear()
    {
        memoryStructure_->clear();
    }

    template<typename T>
    size_t AbstractDataStructure<T>::size() const
    {
        return memoryStructure_->size();
    }

    template<typename T>
    bool AbstractDataStructure<T>::isEmpty() const
    {
        return this->size() == 0;
    }

    template<typename T>
    bool AbstractDataStructure<T>::equals(const ADT& other)
    {
        if (this != &other)
        {
            const ADS<T>& otherADS = dynamic_cast<const ADS<T>&>(other);
            return memoryStructure_->equals(*otherADS.memoryStructure_);
        }
        else
        {
            return true;
        }
    }
}