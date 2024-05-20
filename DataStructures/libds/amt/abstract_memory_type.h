#pragma once

#include <libds/constants.h>
#include <libds/mm/compact_memory_manager.h>
#include <functional>

#pragma warning( disable : 4250 )

namespace ds::amt {

	class unavailable_function_call :
		public std::bad_function_call
	{
	public:
		unavailable_function_call(std::string what) : what_(std::move(what)) {}
		unavailable_function_call(const unavailable_function_call& other) noexcept : what_(other.what_) {};
		unavailable_function_call& operator=(const unavailable_function_call& other) noexcept { what_ = other.what_; return *this; }
		const char* what() const noexcept override { return what_.c_str(); }

	private:
		std::string what_;
	};

	//----------

	template<typename DataType>
	struct MemoryBlock
	{
		using DataT = DataType;
		DataType data_;
	};

	//----------

	class AbstractMemoryType
	{
	public:
		virtual ~AbstractMemoryType() = default;
		virtual AbstractMemoryType& assign(const AbstractMemoryType& other) = 0;
		virtual void clear() = 0;
		virtual size_t size() const = 0;
		virtual bool isEmpty() const = 0;
		virtual bool equals(const AbstractMemoryType& other) = 0;
	};

	using AMT = AbstractMemoryType;

	//----------

	template<typename BlockType>
	class AbstractMemoryStructure :
		virtual public AMT
	{
	public:
		explicit AbstractMemoryStructure(mm::MemoryManager<BlockType>* memoryManager);
        ~AbstractMemoryStructure() override;

		size_t size() const override;
		bool isEmpty() const override;

    protected:
		mm::MemoryManager<BlockType>* memoryManager_;
	};

	template<typename BlockType>
	using AMS = AbstractMemoryStructure<BlockType>;

	//----------

	template<typename DataType>
	class ImplicitAbstractMemoryStructure :
		public AMS<MemoryBlock<DataType>>
	{
	public:
		using BlockType = MemoryBlock<DataType>;
		using MemoryManagerType = mm::CompactMemoryManager<BlockType>;

		ImplicitAbstractMemoryStructure();
		ImplicitAbstractMemoryStructure(size_t capacity, bool initBlocks);
        explicit ImplicitAbstractMemoryStructure(MemoryManagerType* memoryManager);
		ImplicitAbstractMemoryStructure(const ImplicitAbstractMemoryStructure<DataType>& other);

		AMT& assign(const AMT& other) override;
		void clear() override;
		bool equals(const AMT& other) override;

		size_t getCapacity();
		void changeCapacity(size_t newCapacity);

		static const int INIT_CAPACITY = 10;

	protected:
		MemoryManagerType* getMemoryManager() const;
	};

	template<typename DataType>
	using ImplicitAMS = ImplicitAbstractMemoryStructure<DataType>;

	//----------

	template<typename BlockType>
	class ExplicitAbstractMemoryStructure :
		public AMS<BlockType>
	{
	public:
		ExplicitAbstractMemoryStructure();
		explicit ExplicitAbstractMemoryStructure(mm::MemoryManager<BlockType>* memoryManager);
	};

	template<typename BlockType>
	using ExplicitAMS = ExplicitAbstractMemoryStructure<BlockType>;

	//----------

	template<typename BlockType>
	class Iterator
	{
	public:
		virtual ~Iterator() {}
		virtual void moveForward() = 0;
		virtual BlockType& accessData() const = 0;
		virtual bool hasNext() const = 0;
	};

	template<typename BlockType>
	class ForwardIterator :
	    public Iterator<BlockType>
	{
	public:
		virtual bool equals(const ForwardIterator<BlockType>& other) const = 0;
		virtual bool notEquals(const ForwardIterator<BlockType>& other) const = 0;
	};

	template<typename BlockType>
	class BidirectionalIterator :
	    public ForwardIterator<BlockType>
	{
	public:
		virtual void moveBackward() = 0;
		virtual bool hasPrevious() const = 0;
	};

	template<typename BlockType>
	class RandomAccessIterator :
	    public BidirectionalIterator<BlockType>
	{
	public:
		virtual bool isBefore(const RandomAccessIterator<BlockType>& other) const = 0;
		virtual bool isAfter(const RandomAccessIterator<BlockType>& other) const = 0;
		virtual void moveForwardN(size_t n) = 0;
	};

	template<typename BlockType>
	class CompactMemoryIterator :
	    public Iterator<BlockType>
	{
	public:
		CompactMemoryIterator(ImplicitAMS<BlockType>* structure, size_t current) :
		    structure_(structure),
		    current_(current)
		{
		}

		~CompactMemoryIterator() override
		{
			structure_ = nullptr;
			current_ = 0;
		}

		BlockType& accessData() const override
		{
			return structure_->getMemoryManager()->getBlockAt(current_);
		}

	private:
		ImplicitAMS<BlockType>* structure_;
		size_t current_;
	};

	template<typename BlockType>
	class NonCompactMemoryIterator :
	    public Iterator<BlockType>
	{
	public:
		NonCompactMemoryIterator(BlockType* current) :
			current_(current)
	    {
		}

		~NonCompactMemoryIterator() override
		{
			current_ = nullptr;
		}

		BlockType& accessData() const override
		{
			return *current_;
		}

	private:
		BlockType* current_;
	};

	//----------

	template<typename BlockType>
    AbstractMemoryStructure<BlockType>::AbstractMemoryStructure(mm::MemoryManager<BlockType>* memoryManager):
		memoryManager_(memoryManager)
	{
	}

	template<typename BlockType>
    AbstractMemoryStructure<BlockType>::~AbstractMemoryStructure()
	{
		delete memoryManager_;
		memoryManager_ = nullptr;
	}

	template<typename BlockType>
    size_t AbstractMemoryStructure<BlockType>::size() const
	{
		return memoryManager_->getAllocatedBlockCount();
	}

    template <typename BlockType>
    bool AbstractMemoryStructure<BlockType>::isEmpty() const
	{
	    return this->size() == 0;
	}

    template<typename DataType>
    ImplicitAbstractMemoryStructure<DataType>::ImplicitAbstractMemoryStructure():
		ImplicitAbstractMemoryStructure<DataType>(INIT_CAPACITY, false)
	{
	}

	template<typename DataType>
    ImplicitAbstractMemoryStructure<DataType>::ImplicitAbstractMemoryStructure(size_t capacity, bool initBlocks) :
		AMS<MemoryBlock<DataType>>(new MemoryManagerType(capacity))
	{
		if (initBlocks)
		{
			for (size_t i = 0; i < capacity; i++)
			{
				this->memoryManager_->allocateMemory();
			}
		}
	}

	template<typename DataType>
    ImplicitAbstractMemoryStructure<DataType>::ImplicitAbstractMemoryStructure(mm::CompactMemoryManager<BlockType>* memoryManager):
		AMS<MemoryBlock<DataType>>(memoryManager)
	{
	}

	template<typename DataType>
    ImplicitAbstractMemoryStructure<DataType>::ImplicitAbstractMemoryStructure(const ImplicitAbstractMemoryStructure<DataType>& other):
		AMS<MemoryBlock<DataType>>(new MemoryManagerType(*other.getMemoryManager()))
	{
	}

	template<typename DataType>
    AMT& ImplicitAbstractMemoryStructure<DataType>::assign(const AMT& other)
	{
		if (this != &other)
		{
			const ImplicitAMS<DataType>& otherImplicitStructure = dynamic_cast<const ImplicitAMS<DataType>&>(other);
			this->getMemoryManager()->assign(*otherImplicitStructure.getMemoryManager());
		}

		return *this;
	}

	template<typename DataType>
    void ImplicitAbstractMemoryStructure<DataType>::clear()
	{
		this->getMemoryManager()->clear();
	}

	template<typename DataType>
    bool ImplicitAbstractMemoryStructure<DataType>::equals(const AMT& other)
	{
		if (this != &other)
		{
			const ImplicitAMS<DataType>& otherImplicitStructure = dynamic_cast<const ImplicitAMS<DataType>&>(other);
			return this->getMemoryManager()->equals(*otherImplicitStructure.getMemoryManager());
		}
		else
		{
			return true;
		}
	}

	template<typename DataType>
    size_t ImplicitAbstractMemoryStructure<DataType>::getCapacity()
	{
		return this->getMemoryManager()->getCapacity();
	}

	template<typename DataType>
    void ImplicitAbstractMemoryStructure<DataType>::changeCapacity(size_t newCapacity)
	{
		this->getMemoryManager()->changeCapacity(newCapacity);
	}

	template<typename DataType>
    auto ImplicitAbstractMemoryStructure<DataType>::getMemoryManager() const -> MemoryManagerType*
	{
		return dynamic_cast<MemoryManagerType*>(AMS<BlockType>::memoryManager_);
	}

	template<typename BlockType>
    ExplicitAbstractMemoryStructure<BlockType>::ExplicitAbstractMemoryStructure():
		AMS<BlockType>(new mm::MemoryManager<BlockType>())
	{
	}

	template<typename BlockType>
    ExplicitAbstractMemoryStructure<BlockType>::ExplicitAbstractMemoryStructure(mm::MemoryManager<BlockType>* memoryManager):
		AMS<BlockType>(memoryManager)
	{
	}

}