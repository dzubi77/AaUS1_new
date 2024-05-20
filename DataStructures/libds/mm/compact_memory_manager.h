#pragma once

#include <libds/mm/memory_manager.h>
#include <libds/mm/memory_omanip.h>
#include <libds/constants.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <utility>


namespace ds::mm {

    template<typename BlockType>
    class CompactMemoryManager : public MemoryManager<BlockType> {
    public:
        CompactMemoryManager();
        CompactMemoryManager(size_t size);
        CompactMemoryManager(const CompactMemoryManager<BlockType>& other);
        ~CompactMemoryManager() override;

        BlockType* allocateMemory() override;
        BlockType* allocateMemoryAt(size_t index);
        void releaseMemory(BlockType* pointer) override;
        void releaseMemoryAt(size_t index);
        void releaseMemory();

        size_t getCapacity() const;

        CompactMemoryManager<BlockType>& assign(const CompactMemoryManager<BlockType>& other);
        void changeCapacity(size_t newCapacity);
        void shrinkMemory();
        void clear();
        bool equals(const CompactMemoryManager<BlockType>& other) const;
        void* calculateAddress(const BlockType& data);
        size_t calculateIndex(const BlockType& data);
        BlockType& getBlockAt(size_t index);
        void swap(size_t index1, size_t index2);

        void print(std::ostream& os);

    private:
        size_t getAllocatedBlocksSize() const;
        size_t getAllocatedCapacitySize() const;

    private:
        BlockType* base_;
        BlockType* end_;
        BlockType* limit_;

        static const size_t INIT_SIZE = 4;
    };

    template<typename BlockType>
    CompactMemoryManager<BlockType>::CompactMemoryManager() :
        CompactMemoryManager(INIT_SIZE)
    {
    }

    template<typename BlockType>
    CompactMemoryManager<BlockType>::CompactMemoryManager(size_t size) :
        base_(static_cast<BlockType*>(std::calloc(size, sizeof(BlockType)))),
        end_(base_),
        limit_(base_ + size)
    {
    }

    template<typename BlockType>
    CompactMemoryManager<BlockType>::CompactMemoryManager(const CompactMemoryManager<BlockType>& other) :
        CompactMemoryManager(other.getAllocatedBlockCount())
    {
        // 02
        this->assign(other);
    }

    template<typename BlockType>
    CompactMemoryManager<BlockType>::~CompactMemoryManager()
    {
        // 02
        this->clear();
        std::free(this->base_); //vraciame pamat OS
        this->base_ = nullptr;
        this->end_ = nullptr;
        this->limit_ = nullptr;
    }

    template<typename BlockType>
    BlockType* CompactMemoryManager<BlockType>::allocateMemory()
    {
        // 02
        // return this->allocateMemoryAt(this->end_ - this->base_);
        return this->allocateMemoryAt(this->getAllocatedBlockCount());
    }

    template<typename BlockType>
    BlockType* CompactMemoryManager<BlockType>::allocateMemoryAt(size_t index)
    {
        // 02
        if (this->end_ == this->limit_)
        {
            // radsej nasobok nez konstanta; nedegradovanie zlozitosti allocateMemory()
            this->changeCapacity(2 * this->getAllocatedBlockCount());
        }
        if (this->base_ + index != this->end_)
        {
            std::memmove(this->base_ + index + 1, this->base_ + index, (this->end_ - this->base_ - index) * sizeof(BlockType));
        }
        ++this->allocatedBlockCount_;
        ++this->end_;
        return placement_new(this->base_ + index); // nealokuje miesto, iba vyvolava konstruktor
    }

    template<typename BlockType>
    void CompactMemoryManager<BlockType>::releaseMemory(BlockType* pointer)
    {
        // 02
        BlockType* p = pointer;
        while (p != this->end_)
        {
            destroy(p); //delete nemozno volat, lebo delete vracia pamat OS
            ++p;
        }
        this->end_ = pointer;
        this->allocatedBlockCount_ = this->end_ - this->base_;
    }

    template<typename BlockType>
    void CompactMemoryManager<BlockType>::releaseMemoryAt(size_t index)
    {
        // 02
        destroy(&this->getBlockAt(index));
        std::memmove(this->base_ + index, this->base_ + index + 1, (this->end_ - this->base_ - index - 1) * sizeof(BlockType));
        --this->end_;
        --this->allocatedBlockCount_;
    }

    template<typename BlockType>
    void CompactMemoryManager<BlockType>::releaseMemory()
    {
        // 02
        this->releaseMemory(this->end_ - 1);
    }

    template<typename BlockType>
    size_t CompactMemoryManager<BlockType>::getCapacity() const
    {
        // 02
        return this->limit_ - this->base_;
    }

    template<typename BlockType>
    CompactMemoryManager<BlockType>& CompactMemoryManager<BlockType>::assign
    (const CompactMemoryManager<BlockType>& other)
    {
        // 02
        if (this != &other)
        {
            this->clear();
            this->allocatedBlockCount_ = other.getAllocatedBlockCount();
            void* newBase = std::realloc(this->base_, other.getAllocatedCapacitySize());
            if (newBase == nullptr)
            {
                throw std::bad_alloc();
            }
            this->base_ = static_cast<BlockType*>(newBase);
            this->end_ = this->base_ + this->getAllocatedBlockCount();
            this->limit_ = this->base_ + (other.limit_ - other.base_);
            for (size_t i = 0; i < this->getAllocatedBlockCount(); i++)
            {
                placement_copy(this->base_ + i, *(other.base_ + i));
            }
        }
        return *this;

        //if (this != &other)
        //{
         //   this->clear();
         //   if (this->getCapacity() < other.getAllocatedBlockCount())
	    //    {
        //        //std::memcpy(this->base_, other.base_, other.getAllocatedBlockCount() * sizeof(BlockType));
        //        //memcpy - problem pri objektovych typoch v pamati 
		
	    //    	for (BlockType* otherCur = other.base_; otherCur != other.end_; ++otherCur)
        //        {
        //            //v prvej iteracii this->end_ = this->base_;, preto mozno inkrementovat end
        //            placement_copy(this->end_, *otherCur);
        //            ++this->end_;
        //        }
        //        this->allocatedBlockCount_ = other.allocatedBlockCount_;
	    //    }
        //}
        //return *this;
    }

    template<typename BlockType>
    void CompactMemoryManager<BlockType>::shrinkMemory()
    {
        size_t newCapacity = static_cast<size_t>(end_ - base_);

        if (newCapacity < CompactMemoryManager<BlockType>::INIT_SIZE)
        {
            newCapacity = CompactMemoryManager<BlockType>::INIT_SIZE;
        }

        this->changeCapacity(newCapacity);
    }

    template<typename BlockType>
    void CompactMemoryManager<BlockType>::changeCapacity(size_t newCapacity)
    {
        // 02
        if (newCapacity == this->getCapacity())
        {
            return;
        }
        if (newCapacity < this->getAllocatedBlockCount())
        {
            this->releaseMemory(this->base_ + newCapacity);
        }
        void* newBase = std::realloc(this->base_, newCapacity * sizeof(BlockType));
        if (newBase == nullptr)
        {
            throw std::bad_alloc();
        }
        this->base_ = static_cast<BlockType*>(newBase);
        this->end_ = this->base_ + this->getAllocatedBlockCount();
        this->limit_ = this->base_ + newCapacity;
    }

    template<typename BlockType>
    void CompactMemoryManager<BlockType>::clear()
    {
        // 02
        this->releaseMemory(this->base_);
    }

    template<typename BlockType>
    bool CompactMemoryManager<BlockType>::equals(const CompactMemoryManager<BlockType>& other) const
    {
        // 02
        return this == &other || (this->getAllocatedBlockCount() == other.getAllocatedBlockCount()
            && std::memcmp(this->base_, other.base_, this->getAllocatedBlocksSize()) == 0);
    }

    template<typename BlockType>
    void* CompactMemoryManager<BlockType>::calculateAddress(const BlockType& data)
    {
        // 02 - stacilo by aj return &data, ale nevieme ci je blok spravovany CMM, preto tak "zlozito", plus precvicenie iterovania cez pole :D
        BlockType* p = this->base_;
        while (p != this->end_ && p != &data)
        {
            ++p;
        }
        return p == this->end_ ? nullptr : p;
    }

    template<typename BlockType>
    size_t CompactMemoryManager<BlockType>::calculateIndex(const BlockType& data)
    {
        // 02
        return &data < this->end_ && &data >= this->base_ ? &data - this->base_ : INVALID_INDEX;
    }

    template<typename BlockType>
    BlockType& CompactMemoryManager<BlockType>::getBlockAt(size_t index)
    {
        // 02
        return *(this->base_ + index);
    }

    template<typename BlockType>
    void CompactMemoryManager<BlockType>::swap(size_t index1, size_t index2)
    {
        std::swap(this->getBlockAt(index1), this->getBlockAt(index2));
    }

    template<typename BlockType>
    size_t CompactMemoryManager<BlockType>::getAllocatedBlocksSize() const
    {
        // 02
        return (this->end_ - this->base_) * sizeof(BlockType);
    }

    template<typename BlockType>
    size_t CompactMemoryManager<BlockType>::getAllocatedCapacitySize() const
    {
        // 02
        return (this->limit_ - this->base_) * sizeof(BlockType);
    }

    template<typename BlockType>
    void CompactMemoryManager<BlockType>::print(std::ostream& os)
    {
        os << "first = " << base_ << std::endl;
        os << "last = " << end_ << std::endl;
        os << "limit = " << limit_ << std::endl;
        os << "block size = " << sizeof(BlockType) << "B" << std::endl;

        BlockType* ptr = base_;
        while (ptr != limit_)
        {
            std::cout << ptr;
            os << PtrPrintBin<BlockType>(ptr);

            if (ptr == base_) {
                os << "<- first";
            }
            else if (ptr == end_) {
                os << "<- last";
            }
            os << std::endl;
            ++ptr;
        }

        os << limit_ << "|<- limit" << std::endl;
    }
}