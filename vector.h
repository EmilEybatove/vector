#include <cstdint>
#include <algorithm>
#include <cstdarg>
#include <stdarg.h>
#include <iostream>
#include <cstdint>
#include <new>
#include <cassert>

template <typename T>
class Vector {
public:
    Vector();
    constexpr Vector(size_t n, const T& value);
    explicit Vector(size_t n);

    template<class InputIt>
    constexpr Vector(InputIt first, InputIt last);
    constexpr Vector(const Vector<T>& other);
    constexpr Vector(Vector<T>&& other);
    constexpr Vector(std::initializer_list<T> other);

    constexpr ~Vector();

    constexpr Vector& operator=(const Vector& other);
    Vector& operator=(Vector&& other);
    constexpr Vector& operator=(std::initializer_list<T> ilist);
    
    constexpr void assign(size_t count, const T& value);
    template<class InputIt>
    constexpr void assign(InputIt first, InputIt last);
    constexpr void assign(std::initializer_list<T> ilist);

    constexpr T& at(size_t pos);
    constexpr const T& at(size_t pos) const;

    constexpr T& operator[] (size_t index);
    constexpr const T& operator[] (size_t index) const;

    constexpr T& front();
    constexpr const T& front() const;

    constexpr T& back();
    constexpr const T& back() const;

    constexpr T* data() noexcept;
    constexpr const T* data() const noexcept;

    constexpr bool empty() const noexcept;
    constexpr size_t size() const noexcept;
    constexpr size_t max_size() const noexcept;
    constexpr void reserve(size_t new_cap);
    constexpr size_t capacity() const noexcept;
    constexpr void shrink_to_fit();

    constexpr void clear() noexcept;

    constexpr void push_back(const T& element);
    constexpr void push_back(T&& value);

    template<class... Args>
    void emplace_back(Args&&... args);

    template<class... Args>
    constexpr T& emplace_back(Args&&... args);

    constexpr void pop_back();
    constexpr void resize(size_t count);
    constexpr void resize(size_t count, const T& value);

    constexpr void swap(Vector& other) noexcept;

private:
    size_t capacity_;
    size_t size_;
    T* data_;

    void relocate();
};