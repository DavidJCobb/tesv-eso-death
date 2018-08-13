#pragma once
#include <vector>

namespace cobb {
   template <typename T>
   class unique_vector : private std::vector<T> {
      //
      // Conceived as an alternate to std::set, partially because it's probably 
      // faster to search when dealing with smaller datasets, and partiallly 
      // because std::set inexplicably segfaults just by iterating when compiled 
      // in VS2015.
      //
      private:
         typedef std::vector<T> base_vector;
      public:
         typedef typename base_vector::size_type       size_type;
         typedef typename base_vector::iterator        iterator;
         typedef typename base_vector::const_iterator  const_iterator;
         typedef typename T&       reference;
         typedef typename const T& const_reference;

         using base_vector::operator[];

         using base_vector::begin;
         using base_vector::clear;
         using base_vector::end;
         using base_vector::erase;
         //using base_vector::push_back;
         using base_vector::reserve;
         //using base_vector::resize;
         using base_vector::size;
         using base_vector::back;
         using base_vector::front;

         reference operator[](size_type n) = delete;
         void push_back(const value_type& val) = delete;
         void push_back(value_type&& val) = delete;
         void pop_back() = delete;
         reference at(size_type n) = delete;

         std::pair<iterator, bool> insert(const value_type& value) {
            {
               auto found = std::find(this->begin(), this->end(), value);
               if (found != this->end())
                  return std::make_pair(found, false);
            }
            base_vector::push_back(value);
            return std::make_pair(this->begin() + (this->size() - 1), false);
         };
         const_iterator find(const value_type& value) const {
            return std::find(this->begin(), this->end(), value);
         };
   };
   /*//
   template <typename T>
   class unique_vector {
      private:
         T*     _data;
         size_t _size;
         size_t _capacity;
      public:
         typedef typename size_t   size_type;
         typedef typename T        value_type;
         typedef typename T&       reference;
         typedef typename const T& const_reference;
         typedef typename T*       pointer;
         typedef typename const T* const_pointer;
         //
         unique_vector() : _size(0), _capacity(0), _data(nullptr) {};
         unique_vector(size_type n, const value_type& val = value_type()) : _size(n), _capacity(n) {
            this->_data = new value_type[n];
            for (size_type i = 0; i < n; i++)
               this->_data[i] = val;
         };
         //
         ~unique_vector() {
            if (this->_data)
               delete[] this->_data;
            this->_data = nullptr;
            this->_size = this->_capacity = 0;
         };
         //
         size_type size()     const { return this->_size; };
         size_type max_size() const noexcept { return std::numeric_limits<size_type>::max; };
         size_type capacity() const { return this->_capacity; };
         bool empty() const noexcept {
            if (this->_size == 0 || this->_data == nullptr)
               return true;
            return false;
         };
         const_reference operator[](size_type index) const {
            assert(index < this->_size);
            return this->_data[index];
         }
         const_reference at(size_type index) const {
            if (index >= this->_size)
               throw std::out_of_range("unique_vector::at");
            return this->_data[index];
         }
         void reserve(size_type n) {
            if (n <= this->_capacity)
               return;
            auto target = new value_type[n];
            auto old = this->_data;
            memcpy(target, old, sizeof(T) * n);
            this->_data = target;
            this->_capacity = n;
            delete old;
         };
         void resize(size_type n) {
            if (n <= this->_capacity) {
               this->_size = n;
               return;
            }
            auto target = new value_type[n];
            auto old    = this->_data;
            size_type minimum = (std::min)(n, this->_size);
            memcpy(target, old, sizeof(T) * minimum);
            this->_data = target;
            this->_size = n;
            this->_capacity = n;
            delete old;
         };
         void resize(size_type n, const value_type& val) {
            if (n > this->_capacity) {
               auto target = new value_type[n];
               auto old = this->_data;
               size_type minimum = (std::min)(n, this->_size);
               memcpy(target, old, sizeof(T) * minimum);
               this->_data = target;
               this->_capacity = n;
               delete old;
            }
            if (n > this->_size) {
               for (size_type i = this->_size; i < n; i++)
                  this->_data[i] = val;
            }
            this->_size = n;
         };
         //
         class iterator {
            public:
               typedef iterator self_type;
               typedef T value_type;
               typedef T& reference;
               typedef T* pointer;
               typedef std::forward_iterator_tag iterator_category;
               typedef int difference_type;
               iterator(pointer ptr) : ptr_(ptr) { }
               self_type operator++() { self_type i = *this; ptr_++; return i; }
               self_type operator++(int junk) { ptr_++; return *this; }
               reference operator*() { return *ptr_; }
               pointer operator->() { return ptr_; }
               bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
               bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
            private:
               pointer ptr_;
         };
         class const_iterator {
            public:
               typedef const_iterator self_type;
               typedef T  value_type;
               typedef T& reference;
               typedef T* pointer;
               typedef ptrdiff_t difference_type;
               typedef std::forward_iterator_tag iterator_category;
               const_iterator(pointer ptr) : ptr_(ptr) { }
               self_type operator++() { self_type i = *this; ptr_++; return i; }
               self_type operator++(int junk) { ptr_++; return *this; }
               const reference operator*() { return *ptr_; }
               const pointer operator->() { return ptr_; }
               bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
               bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
            private:
               pointer ptr_;
         };
         //
         const_iterator begin() const {
            return const_iterator(this->_data);
         }
         const_iterator end() const {
            return const_iterator(this->_data + this->_size);
         }
         //
         std::pair<iterator, bool> insert(const value_type& value) {
            {
               auto found = std::find(this->begin(), this->end(), value);
               if (found != this->end())
                  return std::pair<iterator, bool>(found, false);
            }
            if (this->_size <= this->_capacity)
               this->reserve(this->_size + 5);
            size_type pos = this->_size;
            this->_data[pos] = value;
            this->_size++;
            return this[pos];
         };
   };
   //*/
};