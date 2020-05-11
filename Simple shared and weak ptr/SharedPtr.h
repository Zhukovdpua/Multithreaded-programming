#pragma once
#include "Control_Block.h"

template<typename Resource>
class WeakPtr;

template <typename Resource>
class SharedPtr
{
	Control_Block* control_block;
	Resource* data;
	void common_create_code(Resource * res);
	friend class WeakPtr<Resource>;

public:
	SharedPtr();
	explicit SharedPtr(Resource* res);
	SharedPtr(const SharedPtr<Resource>& rhs);
	SharedPtr(const WeakPtr<Resource>& rhs);
	SharedPtr<Resource>& operator=(const SharedPtr<Resource>& rhs);
	~SharedPtr();

	void Reset();
	void Reset(Resource* res);
	void Swap(SharedPtr<Resource>& rhs);

	Resource* Get() const;
	Resource& operator*() const;
	Resource* operator->() const;
	long UseCount() const;
};






template<typename Resource>
void SharedPtr<Resource>::common_create_code(Resource * res) {
	data = res;
	control_block = new Control_Block;
	control_block->strongRef++;
}

template<typename Resource>
SharedPtr<Resource>::SharedPtr()
	:data(nullptr), control_block(nullptr)
{
}

template<typename Resource>
SharedPtr<Resource>::SharedPtr(Resource * res) {
	common_create_code(res);
}

template<typename Resource>
SharedPtr<Resource>::SharedPtr(const SharedPtr<Resource>& rhs)
	:data(rhs.data), control_block(rhs.control_block)
{
	if (data)
		control_block->strongRef++;
}

template<typename Resource>
SharedPtr<Resource>::SharedPtr(const WeakPtr<Resource>& rhs) {
	if (rhs.data && rhs.UseCount() > 0) {
		data = rhs.data;
		control_block = rhs.control_block;
		control_block->strongRef++;
	}
	else
		throw std::bad_weak_ptr();
}

template<typename Resource>
SharedPtr<Resource>& SharedPtr<Resource>::operator=(const SharedPtr<Resource>& rhs) {
	this->~SharedPtr();
	data = rhs.data;
	control_block = rhs.control_block;
	if (data)
		control_block->strongRef++;
	return *this;
}

template<typename Resource>
SharedPtr<Resource>::~SharedPtr() {
	if (data) {
		control_block->strongRef--;
		if (control_block->strongRef == 0) {
			delete data;
			if (control_block->weakRef == 0)
				delete control_block;
		}
	}
}

template<typename Resource>
void SharedPtr<Resource>::Reset() {
	this->~SharedPtr();
	data = nullptr;
	control_block = nullptr;
}

template<typename Resource>
void SharedPtr<Resource>::Reset(Resource * res) {
	this->~SharedPtr();
	common_create_code(res);
}

template<typename Resource>
void SharedPtr<Resource>::Swap(SharedPtr<Resource>& rhs) {
	std::swap(data, rhs.data);
	std::swap(control_block, rhs.control_block);
}

template<typename Resource>
Resource * SharedPtr<Resource>::Get() const {
	return data;
}

template<typename Resource>
Resource & SharedPtr<Resource>::operator*() const {
	return *data;
}

template<typename Resource>
Resource * SharedPtr<Resource>::operator->() const {
	return data;
}

template<typename Resource>
long SharedPtr<Resource>::UseCount() const {
	if (data)
		return control_block->strongRef;
	return 0;
}