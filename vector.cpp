#include "vector.h"

template <typename T>
Vector<T>::Vector() : size_(0), capacity_(0), data_(nullptr) {};

template <typename T>
constexpr Vector<T>::Vector(size_t n, const T& value) {
    static_assert(std::is_copy_constructible<T>::value);
    static_assert(n <= max_size());

    try {
        capacity_ = n;
        data_ = reinterpret_cast<T*>(operator new(capacity_ * sizeof(T), std::align_val_t(alignof(T))));
        for (size_ = 0; size_ < n; ++size_) {
            new(&data_[size_])T{value};
        }
        ++size_;
    } catch(const std::exception& e) {
        ~this;
        std::cerr << e.what() << '\n';
    }
}

template <typename T>
Vector<T>::Vector(size_t n) {
    static_assert(std::is_default_constructible<T>::value);
    static_assert(n <= max_size());

    try {
        capacity_ = n;
        data_ = reinterpret_cast<T*>(operator new(capacity_ * sizeof(T), std::align_val_t(alignof(T))));
        for (size_ = 0; size_ < n; ++size_) {
            new(&data_[size_])T{};
        }
        ++size_;
    } catch(const std::exception& e) {
        ~this;
        std::cerr << e.what() << '\n';
    }
}

template <typename T>
template <class InputIt>
constexpr Vector<T>::Vector(InputIt first, InputIt last) {
    static_assert(std::is_copy_constructible<T>::value);
    static_assert(std::is_copy_constructible<InputIt>::value);

    try {
        capacity_ = 0;
        InputIt firstForCountingSize = first;

        while (firstForCountingSize != last) {
            ++firstForCountingSize;
            ++capacity_;
        }

        static_assert(capacity_ <= max_size());

        data_ = reinterpret_cast<T*>(operator new(capacity_ * sizeof(T), std::align_val_t(alignof(T))));

        for (size_ = 0; size_ < capacity_; ++size_) {
            new(&data_[size_])T{*first};
            ++first;
        }
        ++size_;
    } catch(const std::exception& e) {
        ~this;
        std::cerr << e.what() << '\n';
    }
}


template <typename T>
constexpr Vector<T>::Vector(const Vector<T>& other) {
    static_assert(std::is_copy_constructible<T>::value);
    static_assert(other.size() <= max_size());

    try {
        capacity_ = other.capacity_;
        data_ = reinterpret_cast<T*>(operator new(capacity_ * sizeof(T), std::align_val_t(alignof(T))));
        
        for (size_ = 0; size_ < other.size_; ++size_) {
            new(&data_[size_])T{other[size_]};
        }
        ++size_;
    } catch(const std::exception& e) {
        ~this;
        std::cerr << e.what() << '\n';
    }
}

template <typename T>
constexpr Vector<T>::Vector(Vector<T>&& other) {
    static_assert(other.size() <= max_size());

    size_ = other.size_;
    capacity_ = other.capacity_;
    std::swap(data_, other.data_);
}

template <typename T>
constexpr Vector<T>::Vector(std::initializer_list<T> other) {
    static_assert(std::is_move_constructible<T>::value || std::is_copy_constructible<T>::value);
    static_assert(other.size() <= max_size());

    try {
        capacity_ = other.size();
        data_ = reinterpret_cast<T*>(operator new(capacity_ * sizeof(T), std::align_val_t(alignof(T))));
        size_ = 0;

        for (auto elem : other) {
            if (std::is_move_constructible<T>::value) {
                new(&data_[size_])T{std::move(elem)};
            } else {
                new(&data_[size_])T{elem};
            }
            ++size_;
        }
    } catch(const std::exception& e) {
        ~this;
        std::cerr << e.what() << '\n';
    }
};

template <typename T>
constexpr Vector<T>::~Vector() {
    for (size_t i = 0; i < size_; ++i) {
        data_[i].~T();
    }
    delete[] data_;
}

template <typename T>
constexpr Vector<T>& Vector<T>::operator=(const Vector<T>& other) {
    static_assert(std::is_copy_constructible<T>::value);
    static_assert(other.size() <= max_size());

    size_t i = 0;
    T* tmpData = reinterpret_cast<T*>(operator new(other.capacity_ * sizeof(T), std::align_val_t(alignof(T))));

    try {
        for (; i < other.size_; ++i) {
            new(&tmpData[i])T{other.data_[i]};
        }
    } catch(const std::exception& e) {
        for (size_t j = 0; j < i; ++j) {
            ~tmpData[j];
        }
        delete [] tmpData;
        std::cerr << e.what() << '\n';
    }

    clear();
    capacity_ = other.capacity();
    data_ = tmpData;
    size_ = other.size();

    return this;
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector&& other) {
    static_assert(other.size() <= max_size());

    size_ = other.size_;
    capacity_ = other.capacity_;
    std::swap(data_, other.data_);
    return this;
}

template <typename T>
constexpr Vector<T>& Vector<T>::operator=(std::initializer_list<T> ilist) {
    static_assert(std::is_move_constructible<T>::value || std::is_copy_constructible<T>::value);
    static_assert(ilist.size() <= max_size());

    size_t i = 0;
    T* tmpData = reinterpret_cast<T*>(operator new(ilist.size() * sizeof(T), std::align_val_t(alignof(T))));

    try {
        for (auto elem : ilist) {
            if (std::is_move_constructible<T>::value) {
                new(&tmpData[i])T{std::move(elem)};            
            } else {
                new(&tmpData[i])T{elem};
            }
            ++i;
        }
    } catch(const std::exception& e) {
        for (size_t j = 0; j < i; ++j) {
            ~tmpData[j];
        }
        delete [] tmpData;
        std::cerr << e.what() << '\n';
    }

    clear();
    capacity_ = ilist.size();
    data_ = tmpData;
    size_ = ilist.size();

    return this;
}

template <typename T>
constexpr void Vector<T>::assign(size_t count, const T& value) {
    static_assert(std::is_copy_constructible<T>::value);
    static_assert(count <= max_size());

    
    size_t i = 0;
    T* tmpData = reinterpret_cast<T*>(operator new(count * sizeof(T), std::align_val_t(alignof(T))));

    try {
        for (; i < count; ++i) {
            new(&tmpData[i])T{value};
        }
    } catch(const std::exception& e) {
        for (size_t j = 0; j < i; ++j) {
            ~tmpData[j];
        }
        delete [] tmpData;
        std::cerr << e.what() << '\n';
    }

    clear();
    capacity_ = count;
    data_ = tmpData;
    size_ = count;
}

template <typename T>
template<class InputIt>
constexpr void Vector<T>::assign(InputIt first, InputIt last) {
    static_assert(std::is_copy_constructible<T>::value);
    static_assert(std::is_copy_constructible<InputIt>::value);

    InputIt firstForCountingSize = first;
    size_t tmpSize = 0;

    while (firstForCountingSize != last) {
        ++firstForCountingSize;
        ++tmpSize;
    }

    static_assert(tmpSize <= max_size());

    
    size_t i = 0;
    T* tmpData = reinterpret_cast<T*>(operator new(tmpSize * sizeof(T), std::align_val_t(alignof(T))));

    try {
        for (; i < tmpSize; ++i) {
            new(&tmpData[i])T{*first};
            ++first;
        }
    } catch(const std::exception& e) {
        for (size_t j = 0; j < i; ++j) {
            ~tmpData[j];
        }
        delete [] tmpData;
        std::cerr << e.what() << '\n';
    }

    clear();
    capacity_ = tmpSize;
    data_ = tmpData;
    size_ = tmpSize;
}

template <typename T>
constexpr void Vector<T>::assign(std::initializer_list<T> ilist) {
    static_assert(std::is_move_constructible<T>::value || std::is_copy_constructible<T>::value);
    static_assert(ilist.size() <= max_size());


    size_t i = 0;
    T* tmpData = reinterpret_cast<T*>(operator new(ilist.size() * sizeof(T), std::align_val_t(alignof(T))));

    try {
        for (auto elem : ilist) {
            if (std::is_move_constructible<T>::value) {
                new(&tmpData[i])T{std::move(elem)};
            } else {
                new(&tmpData[i])T{elem};
            }
            ++i;
        }
    } catch(const std::exception& e) {
        for (size_t j = 0; j < i; ++j) {
            ~tmpData[j];
        }
        delete [] tmpData;
        std::cerr << e.what() << '\n';
    }    

    data_ = tmpData;
    capacity_ = ilist.size();
    size_ = ilist.size();
}

template <typename T>
constexpr T& Vector<T>::at(size_t pos) {
    static_assert(pos >= size_);
    return data_[pos];
}

template <typename T>
constexpr const T& Vector<T>::at(size_t pos) const {
    static_assert(pos >= size_);
    return data_[pos];
}

template <typename T>
constexpr T& Vector<T>::operator[] (size_t index) {
    return data_[index]; // не баг, а фича
}

template <typename T>
constexpr const T& Vector<T>::operator[] (size_t index) const {
    return data_[index]; // не баг, а фича
}

template <typename T>
constexpr T& Vector<T>::front() {
    // Это не баг а фича. в cpp reference разрешили делать ub
    return data_[0];
}

template <typename T>
constexpr const T& Vector<T>::front() const {
    return data_[0];
}

template <typename T>
constexpr T& Vector<T>::back() {
    return data_[size_ - 1];
}

template <typename T>
constexpr const T& Vector<T>::back() const {
    return data_[size_ - 1];    
}

template <typename T>
constexpr T* Vector<T>::data() noexcept {
    if (size_ == 0) {
        return nullptr;
    }
    return data_;
}

template <typename T>
constexpr const T* Vector<T>::data() const noexcept {
    if (size_ == 0) {
        return nullptr;
    }
    return data_;
}

template <typename T>
constexpr bool Vector<T>::empty() const noexcept {
    return size_ == 0;
}

template <typename T>
constexpr size_t Vector<T>::size() const noexcept {
    return size_;
}

template <typename T>
constexpr size_t Vector<T>::max_size() const noexcept {
    size_t max_for_type = __gnu_cxx::__numeric_traits<ptrdiff_t>::__max / sizeof(T);
    size_t max_alloc = __gnu_cxx::__alloc_traits<std::allocator<T>>::max_size(std::allocator<T>()); 
    return std::min<size_t>(max_for_type, max_alloc);
}

template <typename T>
constexpr void Vector<T>::reserve(size_t new_cap) {
    static_assert(std::is_copy_constructible<T>::value || std::is_nothrow_move_assignable<T>::value);
    static_assert(new_cap <= max_size());

    T* tmpData = reinterpret_cast<T*>(operator new(new_cap * sizeof(T), std::align_val_t(alignof(T))));
    size_t i = 0;
    
    try {
        for (; i < size_; ++i) {
            if (std::is_copy_constructible<T>::value) {
                new(&tmpData[i])T{data_[i]};
            } else {
                new(&tmpData[i])T{std::move(data_[i])};
            }
        }
    } catch(const std::exception& e) {
        for (size_t j = 0; j < i; ++j) {
            ~tmpData[j];
        }
        delete [] tmpData;
        std::cerr << e.what() << '\n';
    }

    capacity_ = new_cap;
    data_ = tmpData;
}


template <typename T>
constexpr size_t Vector<T>::capacity() const noexcept {
    return capacity_;
}

template <typename T>
constexpr void Vector<T>::shrink_to_fit() {
    static_assert(std::is_copy_constructible<T>::value || std::is_nothrow_move_constructible<T>::value);

    T* tmpData = reinterpret_cast<T*>(operator new(size_ * sizeof(T), std::align_val_t(alignof(T))));
    size_t i = 0;
    
    try {
        for (; i < size_; ++i) {
            if (std::is_copy_constructible<T>::value) {
                new(&tmpData[i])T{data_[i]};
            } else {
                new(&tmpData[i])T{std::move(data_[i])};
            }
        }
    } catch(const std::exception& e) {
        for (size_t j = 0; j < i; ++j) {
            ~tmpData[j];
        }
        delete [] tmpData;
        std::cerr << e.what() << '\n';
    }

    capacity_ = size_;
    data_ = tmpData;
}

template <typename T>
constexpr void Vector<T>::clear() noexcept {
    for (size_t i = 0; i < size_; ++i) {
        data_[i].~T();
    }
    size_ = 0;
    capacity_ = 0;
    data_ = nullptr;
}

template <typename T>
constexpr void Vector<T>::push_back(const T& element) {
    static_assert(size_ + 1 <= max_size());
    static_assert(std::is_copy_constructible<T>::value);

    if (capacity_ <= size_ + 1) {
        relocate();
    }

    try {
        new(&data_[size_])T{element};
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    ++size_;
}

template <typename T>
constexpr void Vector<T>::push_back(T&& value) {
    static_assert(size_ + 1 <= max_size());
    static_assert(std::is_copy_constructible<T>::value || std::is_nothrow_move_assignable<T>::value);

    if (capacity_ <= size_ + 1) {
        relocate();
    }

    try {
        if (std::is_copy_constructible<T>::value) {
            new(&data_[size_])T{value};
        } else {
            new(&data_[size_])T{std::move(value)};
        }
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    ++size_;
}

template<typename T>
template<class... Args>
void Vector<T>::emplace_back(Args&&... args) {
    static_assert(std::is_copy_constructible<T>::value || std::is_nothrow_move_assignable<T>::value);

    T* value;

    try {
        value = new T(args...); // TODO мб это так не работает
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    

    if (capacity_ <= size_ + 1) {
        relocate();
    }

    try {
        if (std::is_copy_constructible<T>::value) {
            new(&data_[size_])T{value};
        } else {
            new(&data_[size_])T{std::move(value)};
        }
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    ++size_;
}

template<typename T>
template<class... Args>
constexpr T& Vector<T>::emplace_back(Args&&... args) {
    static_assert(std::is_copy_constructible<T>::value || std::is_nothrow_move_assignable<T>::value);

    T* value;

    try {
        value = new T(args...); // TODO мб это так не работает
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    

    if (capacity_ <= size_ + 1) {
        relocate();
    }

    try {
        if (std::is_copy_constructible<T>::value) {
            new(&data_[size_])T{value};
        } else {
            new(&data_[size_])T{std::move(value)};
        }
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    ++size_;

    return back();
}


template <typename T>
void Vector<T>::relocate() {

    size_t tmpCapacity = capacity_;
    
    if (tmpCapacity == 0) {
        tmpCapacity = 1;
    }

    tmpCapacity = std::min<size_t>(tmpCapacity * 2, max_size());

    reserve(tmpCapacity);
}


template <typename T>
constexpr void Vector<T>::pop_back() {
    // это тоже не ub. на cpp reference можно
    data_[size_ - 1].~T();
    --size_;

    if (size_ < capacity_ / 4) {
        shrink_to_fit();
    }
}

template <typename T>
constexpr void Vector<T>::resize(size_t count) {
    static_assert(count <= max_size());

    if (count < size_) {
        for (size_t i = count; i < size_; ++i) {
            data_[i].~T();
        }
        size_ = count;
    } else if (count > size_) {
        static_assert(std::is_default_constructible<T>::value);

        if (count > capacity_) {
            static_assert(std::is_copy_constructible<T>::value || std::is_nothrow_move_assignable<T>::value);
            T* tmpData = reinterpret_cast<T*>(operator new(count * sizeof(T), std::align_val_t(alignof(T))));
            size_t i = 0;
            
            try {
                for (; i < size_; ++i) {
                    if (std::is_copy_constructible<T>::value) {
                        new(&tmpData[i])T{data_[i]};
                    } else {
                        new(&tmpData[i])T{std::move(data_[i])};
                    }
                }
                for (; i < count; ++i) {
                    new(&tmpData[i])T{};
                }
            } catch(const std::exception& e) {
                for (size_t j = 0; j < i; ++j) {
                    ~tmpData[j];
                }
                delete [] tmpData;
                std::cerr << e.what() << '\n';
            }

            data_ = tmpData;
        } else {
            size_t i = size_;
            try {
                for (; i < count; ++i) {
                    new(&data_[i])T{};
                }
            }
            catch(const std::exception& e) {
                for (size_t j = size_; j < count; ++j) {
                    ~data_[j];
                }
                std::cerr << e.what() << '\n';
            }
        }

        capacity_ = count;
        size_ = count;
    }
}

template <typename T>
constexpr void Vector<T>::resize(size_t count, const T& value) {
    static_assert(count <= max_size());

    if (count < size_) {
        for (size_t i = count; i < size_; ++i) {
            data_[i].~T();
        }
        size_ = count;
    } else if (count > size_) {
        static_assert(std::is_copy_constructible<T>::value);

        if (count > capacity_) {
            T* tmpData = reinterpret_cast<T*>(operator new(count * sizeof(T), std::align_val_t(alignof(T))));
            size_t i = 0;
            
            try {
                for (; i < size_; ++i) {
                    if (std::is_copy_constructible<T>::value) {
                        new(&tmpData[i])T{data_[i]};
                    } else {
                        new(&tmpData[i])T{std::move(data_[i])};
                    }
                }
                for (; i < count; ++i) {
                    new(&tmpData[i])T{value};
                }
            } catch(const std::exception& e) {
                for (size_t j = 0; j < i; ++j) {
                    ~tmpData[j];
                }
                delete [] tmpData;
                std::cerr << e.what() << '\n';
            }

            data_ = tmpData;
        } else {
            size_t i = size_;
            try {
                for (; i < count; ++i) {
                    new(&data_[i])T{value};
                }
            }
            catch(const std::exception& e) {
                for (size_t j = size_; j < count; ++j) {
                    ~data_[j];
                }
                std::cerr << e.what() << '\n';
            }
        }

        capacity_ = count;
        size_ = count;
    }
}

template <typename T>
constexpr void Vector<T>::swap(Vector<T>& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(capacity_, other.capacity_);
    std::swap(size_, other.size_);
}