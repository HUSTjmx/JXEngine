#pragma once
#include <limits>
#include <iostream>

namespace JX_LIBRARY {
	template<class T>
	class MyAlloc {
	public:
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		template<class U>
		struct rebind {
			using other = MyAlloc<U>;
		};

		//return address of values
		pointer address(reference value) const {
			return &value;
		}

		const_pointer address(const_reference value)const {
			return &value;
		}

		MyAlloc()throw{}

		MyAlloc(const MyAlloc&) throw() {}

		template<class U>
		MyAlloc(const MyAlloc<U>&) throw() {}

		~MyAlloc() throw() {}

		//return maximum number of elements that can be allocated
		size_type max_size() const throw() {
			return std::numeric_limits<std::size_t>::max() / sizeof(T);
		}

		//allocated but don`t initialize num elements of type T
		pointer allocate(size_type num, const void* = 0) {
			std::cerr << "allocate" << num << "element(s)"
				<< "of size" << sizeof(T) << std::endl;

			pointer ret = (pointer)(::operator new(num * sizeof(T)));

			std::cerr << "allocated at: " << (void*)ret << std::endl;
			return ret;
		}

		//initialize elements of allocated storage p with value
		void construct(pointer p, const T& value) {
			new((void*)p)T(value);
		}

		//destroy elements of initialized storage p
		void destroy(pointer p) {
			p->~T();
		}

		//deallocate storage p of deleted elements
		void deallocate(pointer p, size_type num) {
			std::cerr << "deallocate" << num << "element(s)" << " of size " << sizeof(T) << " at : " << (void*)p << std::endl;
			::operator delete((void*)p);
		}
	};

	template<class T1, class T2>
	bool operator== (const MyAlloc<T1>&, const MyAlloc<T2>&) throw() {
		return true;
	}

	template<class T1, class T2>
	bool operator!=(const MyAlloc<T1>&, const MyAlloc<T2>&) throw() {
		return false;
	}
}