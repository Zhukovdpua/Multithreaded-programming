#pragma once
#include "Control_Block.h"

template<typename Resource>
class SharedPtr;

template<typename Resource>
class WeakPtr
{
	Control_Block* control_block;
	Resource* data;
	void common_create_code();
	void common_assignment_code(Resource* data, Control_Block* control_block);
	friend class SharedPtr<Resource>;

public:
	WeakPtr();
	WeakPtr(const WeakPtr<Resource>& rhs);
	WeakPtr(const SharedPtr<Resource>& rhs);
	WeakPtr<Resource>& operator=(const WeakPtr<Resource>& rhs);
	WeakPtr<Resource>& operator=(const SharedPtr<Resource>& rhs);
	~WeakPtr();

	void Reset();
	void Swap(WeakPtr<Resource>& rhs);
	long UseCount() const;
	bool Expired() const;
	SharedPtr<Resource> Lock() const;
};




template<typename Resource>
void WeakPtr<Resource>::common_create_code() {
	if (data && UseCount() > 0)
		control_block->weakRef++;
	else {
		data = nullptr;
		control_block = nullptr;
	}
}

template<typename Resource>
void WeakPtr<Resource>::common_assignment_code(Resource * _data, Control_Block * _control_block) {
	this->~WeakPtr();
	data = _data;
	control_block = _control_block;
	common_create_code();
}


template<typename Resource>
WeakPtr<Resource>::WeakPtr()
	:control_block(nullptr), data(nullptr)
{
}


template<typename Resource>
WeakPtr<Resource>::WeakPtr(const WeakPtr<Resource>& rhs)
	:data(rhs.data), control_block(rhs.control_block)
{
	common_create_code();
}

template<typename Resource>
WeakPtr<Resource>::WeakPtr(const SharedPtr<Resource>& rhs)
	:data(rhs.data), control_block(rhs.control_block)
{
	if (data)
		control_block->weakRef++;
}

template<typename Resource>
WeakPtr<Resource>& WeakPtr<Resource>::operator=(const WeakPtr<Resource>& rhs) {
	common_assignment_code(rhs.data, rhs.control_block);
	return *this;
}

template<typename Resource>
WeakPtr<Resource>& WeakPtr<Resource>::operator=(const SharedPtr<Resource>& rhs) {
	common_assignment_code(rhs.data, rhs.control_block);
	return *this;
}

template<typename Resource>
WeakPtr<Resource>::~WeakPtr() {
	if (data) {
		control_block->weakRef--;
		if (control_block->weakRef == 0 && control_block->strongRef == 0)
			delete control_block;
	}
}

template<typename Resource>
void WeakPtr<Resource>::Reset() {
	this->~WeakPtr();
	data = nullptr;
	control_block = nullptr;
}

template<typename Resource>
void WeakPtr<Resource>::Swap(WeakPtr<Resource>& rhs) {
	std::swap(data, rhs.data);
	std::swap(control_block, rhs.control_block);
}

template<typename Resource>
long WeakPtr<Resource>::UseCount() const {
	return control_block->strongRef;
}

template<typename Resource>
bool WeakPtr<Resource>::Expired() const {
	return control_block->strongRef == 0 ? true : false;
}

template<typename Resource>
SharedPtr<Resource> WeakPtr<Resource>::Lock() const {
	try {
		return SharedPtr<Resource>(*this);
	}
	catch (typename std::bad_weak_ptr&) {
		return SharedPtr<Resource>();
	}
}
