#pragma once

#include <libds/amt/abstract_memory_type.h>
#include <libds/amt/sequence.h>

namespace ds::amt {

	template<typename DataType>
	class ImplicitSequence :
		public Sequence<MemoryBlock<DataType>>,
		public ImplicitAMS<DataType>
	{
	public:
		using BlockType = MemoryBlock<DataType>;
		using MemoryManagerType = typename ImplicitAMS<DataType>::MemoryManagerType;

		ImplicitSequence() = default;
		ImplicitSequence(size_t initSize, bool initBlocks);
		ImplicitSequence(const ImplicitSequence<DataType>& other);
		~ImplicitSequence() override = default;

		size_t calculateIndex(BlockType& block) override;

		BlockType* accessFirst() const override;
		BlockType* accessLast() const override;
		BlockType* access(size_t index) const override;
		BlockType* accessNext(const BlockType& block) const override;
		BlockType* accessPrevious(const BlockType& block) const override;

		BlockType& insertFirst() override;
		BlockType& insertLast() override;
		BlockType& insert(size_t index) override;
		BlockType& insertAfter(BlockType& block) override;
		BlockType& insertBefore(BlockType& block) override;

		void removeFirst() override;
		void removeLast() override;
		void remove(size_t index) override;
		void removeNext(const BlockType& block) override;
		void removePrevious(const BlockType& block) override;

		void reserveCapacity(size_t capacity);

		virtual size_t indexOfNext(size_t currentIndex) const;
		virtual size_t indexOfPrevious(size_t currentIndex) const;

	public:
		class ImplicitSequenceIterator
	    {
		public:
			ImplicitSequenceIterator(ImplicitSequence<DataType>* sequence, size_t index);
			ImplicitSequenceIterator(const ImplicitSequenceIterator& other);
			ImplicitSequenceIterator& operator++();
			ImplicitSequenceIterator operator++(int);
			bool operator==(const ImplicitSequenceIterator& other) const;
			bool operator!=(const ImplicitSequenceIterator& other) const;
			DataType& operator*();

		private:
			ImplicitSequence<DataType>* sequence_;
			size_t position_;
		};

		using IteratorType = ImplicitSequenceIterator;

		IteratorType begin();
		IteratorType end();
	};

	template<typename DataType>
	using IS = ImplicitSequence<DataType>;

	//----------

	template<typename DataType>
	class CyclicImplicitSequence : public IS<DataType>
	{
	public:
		CyclicImplicitSequence();
		CyclicImplicitSequence(size_t initSize, bool initBlocks);

		size_t indexOfNext(size_t currentIndex) const override;
		size_t indexOfPrevious(size_t currentIndex) const override;
	};

	template<typename DataType>
	using CIS = CyclicImplicitSequence<DataType>;

	//----------

	template<typename DataType>
    ImplicitSequence<DataType>::ImplicitSequence(size_t initSize, bool initBlocks):
		ImplicitAMS<DataType>(initSize, initBlocks)
	{
	}

	template<typename DataType>
    ImplicitSequence<DataType>::ImplicitSequence(const ImplicitSequence<DataType>& other):
		ImplicitAMS<DataType>::ImplicitAbstractMemoryStructure(other)
	{
	}

	template<typename DataType>
    size_t ImplicitSequence<DataType>::calculateIndex(BlockType& block)
	{
		// 03
		return this->getMemoryManager()->calculateIndex(block);
	}

	template<typename DataType>
    typename ImplicitSequence<DataType>::BlockType* ImplicitSequence<DataType>::accessFirst() const
    {
		// 03
		return this->size() > 0 ? &this->getMemoryManager()->getBlockAt(0) : nullptr;
	}

	template<typename DataType>
	typename ImplicitSequence<DataType>::BlockType* ImplicitSequence<DataType>::accessLast() const
    {
		// 03
		return this->size() > 0 ? &this->getMemoryManager()->getBlockAt(this->size() - 1) : nullptr;
	}

	template<typename DataType>
	typename ImplicitSequence<DataType>::BlockType* ImplicitSequence<DataType>::access(size_t index) const
    {
		// 03
		return index < this->size() ? &this->getMemoryManager()->getBlockAt(index) : nullptr;
	}

	template<typename DataType>
	typename ImplicitSequence<DataType>::BlockType* ImplicitSequence<DataType>::accessNext(const BlockType& block) const
    {
		// 03
		size_t index = this->indexOfNext(this->getMemoryManager()->calculateIndex(block));
		return index != INVALID_INDEX ? &this->getMemoryManager()->getBlockAt(index) : nullptr;

		// this->calculateIndex(block) - robi teoreticky to iste, skonci chybou
		// IS prebera Blokctype&, MM const BlockType& :)
    }

	template<typename DataType>
	typename ImplicitSequence<DataType>::BlockType* ImplicitSequence<DataType>::accessPrevious(const BlockType& block) const
    {
		// 03
		size_t index = this->indexOfPrevious(this->getMemoryManager()->calculateIndex(block));
		return index != INVALID_INDEX ? &this->getMemoryManager()->getBlockAt(index) : nullptr;
	}

	template<typename DataType>
	typename ImplicitSequence<DataType>::BlockType& ImplicitSequence<DataType>::insertFirst()
    {
		// 03
		return *this->getMemoryManager()->allocateMemoryAt(0);
	}

	template<typename DataType>
	typename ImplicitSequence<DataType>::BlockType& ImplicitSequence<DataType>::insertLast()
    {
		// 03
		return *this->getMemoryManager()->allocateMemory();
	}

	template<typename DataType>
	typename ImplicitSequence<DataType>::BlockType& ImplicitSequence<DataType>::insert(size_t index)
    {
		// 03
		return *this->getMemoryManager()->allocateMemoryAt(index);
	}

	template<typename DataType>
	typename ImplicitSequence<DataType>::BlockType& ImplicitSequence<DataType>::insertAfter(BlockType& block)
    {
		// 03
		return *this->getMemoryManager()->allocateMemoryAt(this->getMemoryManager()->calculateIndex(block) + 1);
	}

	template<typename DataType>
	typename ImplicitSequence<DataType>::BlockType& ImplicitSequence<DataType>::insertBefore(BlockType& block)
    {
		// 03
		return *this->getMemoryManager()->allocateMemoryAt(this->getMemoryManager()->calculateIndex(block));
	}

	template<typename DataType>
    void ImplicitSequence<DataType>::removeFirst()
	{
		// 03
		this->getMemoryManager()->releaseMemoryAt(0);
	}

	template<typename DataType>
    void ImplicitSequence<DataType>::removeLast()
	{
		// 03
		this->getMemoryManager()->releaseMemory();
	}

	template<typename DataType>
    void ImplicitSequence<DataType>::remove(size_t index)
	{
		// 03
		this->getMemoryManager()->releaseMemoryAt(index);
	}

	template<typename DataType>
    void ImplicitSequence<DataType>::removeNext(const BlockType& block)
	{
		// 03
		this->getMemoryManager()->releaseMemoryAt(this->indexOfNext(this->getMemoryManager()->calculateIndex(block)));
	}

	template<typename DataType>
    void ImplicitSequence<DataType>::removePrevious(const BlockType& block)
	{
		// 03
		this->getMemoryManager()->releaseMemoryAt(this->indexOfPrevious(this->getMemoryManager()->calculateIndex(block)));
	}

	template<typename DataType>
    void ImplicitSequence<DataType>::reserveCapacity(size_t capacity)
	{
		// 03
		this->getMemoryManager()->changeCapacity(capacity);
	}

	template<typename DataType>
    size_t ImplicitSequence<DataType>::indexOfNext(size_t currentIndex) const
	{
		// 03
		return currentIndex + 1 < this->size() ? currentIndex + 1 : INVALID_INDEX;
	}

	template<typename DataType>
    size_t ImplicitSequence<DataType>::indexOfPrevious(size_t currentIndex) const
	{
		// 03
		return currentIndex > 0 ? currentIndex - 1 : INVALID_INDEX;
	}

    template <typename DataType>
    ImplicitSequence<DataType>::ImplicitSequenceIterator::ImplicitSequenceIterator
        (ImplicitSequence<DataType>* sequence, size_t index) :
		    sequence_(sequence),
            position_(index)
    {
    }

    template <typename DataType>
    ImplicitSequence<DataType>::ImplicitSequenceIterator::ImplicitSequenceIterator
        (const ImplicitSequenceIterator& other) :
		    sequence_(other.sequence_), position_(other.position_)
    {
    }

    template <typename DataType>
    typename ImplicitSequence<DataType>::ImplicitSequenceIterator& ImplicitSequence<DataType>::ImplicitSequenceIterator::operator++() //posun na dalsi
    {
		// 03
		++position_;
		return *this;
    }

    template <typename DataType>
	typename ImplicitSequence<DataType>::ImplicitSequenceIterator ImplicitSequence<DataType>::ImplicitSequenceIterator::operator++(int) //postfixovy operator
    {
		ImplicitSequenceIterator tmp(*this);
	    this->operator++();
	    return tmp;
    }

    template <typename DataType>
    bool ImplicitSequence<DataType>::ImplicitSequenceIterator::operator==(const ImplicitSequenceIterator& other) const
    {
		// 03
		return sequence_ == other.sequence_ && position_ == other.position_;
    }

    template <typename DataType>
    bool ImplicitSequence<DataType>::ImplicitSequenceIterator::operator!=(const ImplicitSequenceIterator& other) const
    {
		return sequence_ != other.sequence_ || position_ != other.position_;
    }

    template <typename DataType>
    DataType& ImplicitSequence<DataType>::ImplicitSequenceIterator::operator*() //spristupni prvok
    {
		// 03
		return sequence_->access(position_)->data_;
    }

    template <typename DataType>
	typename ImplicitSequence<DataType>::ImplicitSequenceIterator ImplicitSequence<DataType>::begin()
    {
		return ImplicitSequenceIterator(this, 0);
    }

    template <typename DataType>
	typename ImplicitSequence<DataType>::ImplicitSequenceIterator ImplicitSequence<DataType>::end()
    {
		return ImplicitSequenceIterator(this, this->size());
    }

    template<typename DataType>
    CyclicImplicitSequence<DataType>::CyclicImplicitSequence():
		IS<DataType>()
	{
	}

	template<typename DataType>
    CyclicImplicitSequence<DataType>::CyclicImplicitSequence(size_t initCapacity, bool initBlocks):
		IS<DataType>(initCapacity, initBlocks)
	{
	}

	template<typename DataType>
    size_t CyclicImplicitSequence<DataType>::indexOfNext(size_t currentIndex) const
	{
		// 03
		return currentIndex < this->size() ? (currentIndex + 1) % this->size() : INVALID_INDEX;
	}

	template<typename DataType>
    size_t CyclicImplicitSequence<DataType>::indexOfPrevious(size_t currentIndex) const
	{
		// 03
		return this->size() > 0 ? (currentIndex > 0 ? currentIndex - 1 : this->size() -1) : INVALID_INDEX;
	}
}