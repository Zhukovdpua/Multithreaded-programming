#pragma once
#include <memory>

namespace MyLib
{
	template <
		typename T,
		template < typename Y> class Allocator = std::allocator
	>
	class Vector
	{
	public:
		typedef Allocator<T> AllocatorType;
		typedef Vector<T, Allocator> VectorType;
	public:
		explicit Vector(const AllocatorType& allocator = AllocatorType());
		Vector(
			const std::initializer_list<T>& values,
			const AllocatorType& allocator = AllocatorType()
		);
		~Vector();
		Vector(const Vector& rhs);
		Vector(Vector&& rhs) noexcept;
		Vector& operator= (const Vector& rhs);
		Vector& operator= (Vector&& rhs);
		T& operator [](std::size_t i) noexcept;
		const T& operator [](std::size_t i) const noexcept;
		T* data() noexcept;
		const T* data() const noexcept;
		void push_back(const T& value);
		void push_back(T&& value);
		void pop_back();
		void resize(std::size_t size);
		void reserve(std::size_t capacity);
		std::size_t size() const noexcept;
		std::size_t capacity() const noexcept;
		bool empty() const noexcept;

	private:
		T* begin;
		AllocatorType m_allocator;
		int m_size;
		int m_capacity;
		
		T* allocate_and_copy(int count, T* copy_source);
		
		template<typename Type>
		void pass_to_construct(Type&& arg);
	};




	template<typename T, template < typename Y> class Allocator>
	Vector<T, Allocator>::Vector(const AllocatorType & allocator)
		: begin(nullptr), m_size(0), m_capacity(0)
	{
	}

	template<typename T, template < typename Y> class Allocator>
	Vector<T, Allocator>::Vector(const std::initializer_list<T>& values, const AllocatorType & allocator)
		:begin(nullptr), m_size(0), m_capacity(0), m_allocator(allocator)
	{
		m_capacity = values.size();
		begin = m_allocator.allocate(m_capacity);
		T* begin_tmp = begin;
		for (auto& i : values)
			m_allocator.construct(begin++, i);
		m_size = m_capacity;
		begin = begin_tmp;
	}

	template<typename T, template < typename Y> class Allocator>
	Vector<T, Allocator>::~Vector() {
		for (int i = 0; i < m_size; i++)
			m_allocator.destroy(begin + i);
		m_allocator.deallocate(begin, m_capacity);
	}

	template<typename T, template < typename Y> class Allocator>
	Vector<T, Allocator>::Vector(const Vector & rhs)
		:m_allocator(rhs.m_allocator), m_size(rhs.m_size), m_capacity(rhs.m_capacity)
	{
		begin = allocate_and_copy(m_capacity, rhs.begin);
	}

	template<typename T, template < typename Y> class Allocator>
	Vector<T, Allocator>::Vector(Vector&& rhs) noexcept
		:m_allocator(rhs.m_allocator), m_size(rhs.m_size), m_capacity(rhs.m_capacity), begin(rhs.begin)
	{
		rhs.begin = nullptr;
		rhs.m_size = 0;
		rhs.m_capacity = 0;
	}

	template<typename T, template < typename Y> class Allocator>
	Vector<T, Allocator>& Vector<T, Allocator>::operator=(const Vector& rhs) {
		if (this == &rhs)
			return *this;
		int tmp_size = m_size;
		m_size = rhs.m_size;
		T* tmp_begin = allocate_and_copy(rhs.m_capacity, rhs.begin);
		m_size = tmp_size;
		this->~Vector();
		begin = tmp_begin;
		m_size = rhs.m_size;
		m_capacity = rhs.m_capacity;
	}

	template<typename T, template < typename Y> class Allocator>
	Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector&& rhs) {
		if (this == &rhs)
			return *this;
		this->~Vector();
		begin = rhs.begin;
		m_allocator = rhs.m_allocator;
		m_size = rhs.m_size;
		m_capacity = rhs.m_capacity;

		rhs.begin = nullptr;
		rhs.m_size = 0;
		rhs.m_capacity = 0;
	}

	template<typename T, template < typename Y> class Allocator>
	T& Vector<T, Allocator>::operator[](std::size_t i) noexcept {
		return begin[i];
	}

	template<typename T, template < typename Y> class Allocator>
	const T& Vector<T, Allocator>::operator[](std::size_t i) const noexcept {
		return begin[i];
	}

	template<typename T, template < typename Y> class Allocator>
	T* Vector<T, Allocator>::data() noexcept {
		return begin;
	}

	template<typename T, template < typename Y> class Allocator>
	const T* Vector<T, Allocator>::data() const noexcept {
		return begin;
	}

	template<typename T, template < typename Y> class Allocator>
	void Vector<T, Allocator>::push_back(const T & value) {
		pass_to_construct(value);

	}

	template<typename T, template < typename Y> class Allocator>
	void Vector<T, Allocator>::push_back(T&& value) {
		pass_to_construct(std::move(value));
	}

	template<typename T, template < typename Y> class Allocator>
	void Vector<T, Allocator>::pop_back() {
		m_allocator.destroy(begin + m_size - 1);
		m_size--;
	}

	template<typename T, template < typename Y> class Allocator>
	void Vector<T, Allocator>::resize(std::size_t size) {
		if (size > m_size) {
			if (size > m_capacity) {
				T* tmp_begin = allocate_and_copy(size, begin);
				for (int i = m_size; i < size; i++)
					m_allocator.construct(tmp_begin + i);
				this->~Vector();
				begin = tmp_begin;
				m_capacity = m_size = size;
			}
			else {
				for (int i = m_size; i < size; i++)
					m_allocator.construct(begin + i);
				m_size = size;
			}
		}
		else {
			for (int i = size; i < m_size; i++)
				m_allocator.destroy(begin + i);
			m_size = size;
		}
	}

	template<typename T, template < typename Y> class Allocator>
	void Vector<T, Allocator>::reserve(std::size_t capacity) {
		if (capacity > m_capacity) {
			T* tmp_begin = allocate_and_copy(capacity, begin);
			this->~Vector();
			begin = tmp_begin;
			m_capacity = capacity;
		}
	}

	template<typename T, template < typename Y> class Allocator>
	std::size_t Vector<T, Allocator>::size() const noexcept {
		return m_size;
	}

	template<typename T, template < typename Y> class Allocator>
	std::size_t Vector<T, Allocator>::capacity() const noexcept {
		return m_capacity;
	}

	template<typename T, template < typename Y> class Allocator>
	bool Vector<T, Allocator>::empty() const noexcept {
		return !size();
	}

	template<typename T, template < typename Y> class Allocator>
	T* Vector<T, Allocator>::allocate_and_copy(int count, T* copy_source) {
		T* tmp_begin = m_allocator.allocate(count);
		for (int i = 0; i < m_size; i++)
			m_allocator.construct(tmp_begin + i, copy_source[i]);
		return tmp_begin;
	}

	template<typename T, template < typename Y> class Allocator>
	template<typename Type>
	void Vector<T, Allocator>::pass_to_construct(Type&& arg) {
		if (m_size < m_capacity)
			m_allocator.construct(begin + m_size++, std::forward<Type>(arg));
		else {
			T* tmp_begin = allocate_and_copy(m_capacity * 2, begin);
			m_allocator.construct(tmp_begin + m_size, std::forward<Type>(arg));
			this->~Vector();
			begin = tmp_begin;
			m_capacity *= 2;
			m_size++;
		}
	}
}