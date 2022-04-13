/*
 * TrailObjects.h
 * - Implements variable length array in class.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_TRAILOBJECTS_H
#define BUILDSCRIPT_UTILS_TRAILOBJECTS_H

#include <array>
#include <vector>
#include <memory>
#include <type_traits>
#include <iterator>

#include <BuildScript/Utils/PointerArith.h>

namespace BuildScript {
    /**
     * @brief Immutable iterator for @c TrailObject.
     * @tparam T the type of element to iterate.
     */
    template <typename T>
    class TrailIterator {
        template <typename, typename...> friend class TrailObjects;

    public:
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;

    private:
        pointer m_base;
        size_type m_length;

    public:
        TrailIterator(pointer base, size_type length)
            : m_base(base), m_length(length) {}

        class Cursor {
        private:
            pointer m_ptr;

        public:
            using size_type = TrailIterator::size_type;
            using difference_type = TrailIterator::difference_type;
            using value_type = TrailIterator::value_type;
            using pointer = TrailIterator::pointer;
            using reference = TrailIterator::reference;
            using iterator_category = std::random_access_iterator_tag;

            explicit Cursor(pointer pointer) : m_ptr(pointer) {}

            bool operator ==(Cursor other) const { return m_ptr == other.m_ptr; }
            bool operator !=(Cursor other) const { return !(*this == other); /*NOLINT*/ }

            Cursor& operator +=(difference_type n) {
                m_ptr += n;
                return *this;
            }
            Cursor operator +(difference_type n) {
                auto temp = *this;
                temp += n;
                return temp;
            }
            friend Cursor operator +(difference_type, Cursor);
            Cursor& operator ++() { return (*this += 1); }
            Cursor operator ++(int) { return *this + 1; }

            Cursor& operator -=(difference_type n) {
                m_ptr -= n;
                return *this;
            }
            Cursor operator -(difference_type n) {
                auto temp = *this;
                temp -= n;
                return temp;
            }
            Cursor& operator --() { return (*this -= 1); }
            Cursor operator --(int) { return (*this - 1); }
            friend Cursor operator -(difference_type, Cursor);

            difference_type operator -(Cursor other) const { return m_ptr - other.m_ptr; }
            reference operator [](size_type index) const { return *(m_ptr + index); }
            reference operator *() const { return *m_ptr; }
            reference operator ->() const { return *m_ptr; }

            bool operator <(Cursor other) const { return m_ptr < other.m_ptr; }
            bool operator >(Cursor other) const { return other < *this; }
            bool operator <=(Cursor other) const { return !(*this > other); /*NOLINT*/}
            bool operator >=(Cursor other) const { return !(*this < other); /*NOLINT*/}
        };

        Cursor begin() const { return Cursor{ m_base }; }
        Cursor end() const { return Cursor{ m_base + m_length }; }

        reference operator [](size_type index) const {
            assert((index < m_length) && "out of bound");

            return *(m_base + index);
        }

        size_t size() const { return m_length; }
    }; // end class TrailIterator<T>

    template <typename T>
    typename TrailIterator<T>::Cursor operator +(typename TrailIterator<T>::Cursor::difference_type n,
                                                 typename TrailIterator<T>::Cursor cursor) {
        return cursor += n;
    }

    template <typename T>
    typename TrailIterator<T>::Cursor operator -(typename TrailIterator<T>::Cursor::difference_type n,
                                                 typename TrailIterator<T>::Cursor cursor) {
        return cursor -= n;
    }

    /**
     * @brief Mutable iterator for @c TrailObject.
     * @tparam T the type of element to iterate.
     */
    template <typename T>
    class MutableTrailIterator {
        template <typename, typename...> friend class TrailObjects;

    public:
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;

    private:
        pointer m_base;
        size_type m_length;

    public:
        MutableTrailIterator(pointer base, size_type length)
            : m_base(base), m_length(length) {}

        class Cursor {
        private:
            pointer m_ptr;

        public:
            using size_type = MutableTrailIterator::size_type;
            using difference_type = MutableTrailIterator::difference_type;
            using value_type = MutableTrailIterator::value_type;
            using pointer = MutableTrailIterator::pointer;
            using reference = MutableTrailIterator::reference;

            explicit Cursor(pointer pointer) : m_ptr(pointer) {}

            bool operator ==(Cursor other) const { return false; }
            bool operator !=(Cursor other) const { return !(*this == other); /*NOLINT*/ }

            Cursor& operator +=(difference_type n) {
                m_ptr += n;
                return *this;
            }
            Cursor operator +(difference_type n) {
                auto temp = *this;
                temp += n;
                return temp;
            }
            friend Cursor operator +(difference_type, Cursor);
            Cursor& operator ++() { return (*this += 1); }
            Cursor operator ++(int) { return *this + 1; }

            Cursor& operator -=(difference_type n) {
                m_ptr -= n;
                return *this;
            }
            Cursor operator -(difference_type n) {
                auto temp = *this;
                temp -= n;
                return temp;
            }
            Cursor& operator --() { return (*this -= 1); }
            Cursor operator --(int) { return (*this - 1); }
            friend Cursor operator -(difference_type, Cursor);

            difference_type operator -(Cursor other) const { return m_ptr - other.m_ptr; }
            reference operator [](difference_type index) { return *(m_ptr + index); }
            reference operator *() { return *m_ptr; }
            reference operator ->() { return *m_ptr; }

            bool operator <(Cursor other) const { return m_ptr < other.m_ptr; }
            bool operator >(Cursor other) const { return other < *this; }
            bool operator <=(Cursor other) const { return !(*this > other); /*NOLINT*/}
            bool operator >=(Cursor other) const { return !(*this < other); /*NOLINT*/}
        };

        Cursor begin() { return Cursor{ m_base }; }
        Cursor end() { return Cursor{ m_base + m_length }; }

        reference operator [](size_type index) {
            assert((index < m_length) && "out of bound");

            return *(m_base + index);
        }

        size_type size() const { return m_length; }
    }; // end class MutableTrailIterator<T>

    template <typename T>
    typename MutableTrailIterator<T>::Cursor operator +(typename MutableTrailIterator<T>::Cursor::difference_type n,
                                                        typename MutableTrailIterator<T>::Cursor cursor) {
        return cursor += n;
    }

    template <typename T>
    typename MutableTrailIterator<T>::Cursor operator -(typename MutableTrailIterator<T>::Cursor::difference_type n,
                                                        typename MutableTrailIterator<T>::Cursor cursor) {
        return cursor -= n;
    }

    /**
     * @brief Implements variable length array in class.
     *
     * This class implements variable length array with one and more types.
     * Referenced from TrailingObjects in LLVM.
     *
     * @section Requirements
     * - To calculate correct address of requested trailing type, implementing
     * `size_t GetTrailCount(OverloadToken<T>) const` of every trail types is required to every classes
     * inherits TrailObjects.
     * - To prevent overriding other class' contents and/or inheriting class contains variable length array that cause
     * unpredicted behavior, every classes inherits TrailObjects requires to be `final` and inherit `TrailObjects` in
     * last order of inheritance.
     * - Difference between return value at `GetTrailCount` and parameter at `GetTrailSize` may cause memory overrun
     * and/or heap corruption.
     *
     * @section Usage
     * @code
     * class SomeClass final : TrailObjects<SomeClass, double, int> {
     *   size_t m_doubleCount;
     *   size_t m_intCount;
     *
     *   // Must be defined for all trail types.
     *   size_t GetTrailCount(OverloadToken<double>) const { return m_doubleCount; }
     *   size_t GetTrailCount(OverloadToken<int>) const { return m_intCount; }
     *
     * public:
     *   SomeClass(size_t doubleCount, size_t intCount)
     *     : m_doubleCount(doubleCount), m_intCount(intCount) {}
     *
     *   // To iterate all elements, use GetTrailObjects().
     *   TrailIterator<double> GetDoubles() { return GetTrailObjects<double>(); }
     *
     *   // Use GetTrailObjects(begin, end) to iterate elements in specific range.
     *   TrailIterator<int> GetIntFrom2To4() { return GetTrailObjects<int>(2, 4); }
     *
     *   // Direct access to trail arrays, use GetTrailObjectBase().
     *   // Use without care may cause undefined behaviors.
     *   double* GetDoubleArray() { return GetTrailObjectBase<double>(); }
     *
     *   // To get an element of specific index, use At().
     *   int GetIntAt(size_t index) const { return At<int>(index); }
     *
     *   static SomeClass* Create(std::vector<double> doubles, std::vector<int> ints) {
     *     auto trailSize = GetTrailSizes(doubles.size(), ints.size());
     *     void* memory = std::malloc(sizeof(SomeClass) + trailSize);
     *     auto* self = new (memory) SomeClass(doubles.size(), ints.size());
     *
     *     // Copy elements of vector to trail. Also supports std::array and buffer with length.
     *     self->SetTrailObjects(doubles);
     *     self->SetTrailObjects(ints);
     *
     *     return self;
     *   }
     * };
     * @endcode
     *
     * @tparam BaseType Base class that inherits @c TrailObjects.
     * @tparam TrailTypes Types to be appended to end of base class.
     */
    template <typename BaseType, typename... TrailTypes>
    class TrailObjects {
    protected:
        /**
         * @brief Expand parameter packs with T.
         * @tparam T the type that expand to.
         */
        template <typename, typename T>
        using expand_pack_t = T;

        /**
         * @brief Phony type for overloading.
         * @tparam T Type to overload.
         */
        template <typename T>
        struct OverloadToken {};

    private:
        template <typename Type>
        static constexpr size_t CalculateSize(size_t count) {
            auto size = sizeof(Type) * count;

            if ((size % alignof(Type)) != 0) {
                size += (alignof(Type) - (size % alignof(Type)));
            }

            return size;
        }

        template <typename Type, typename NextType, typename... NextTypes>
        static constexpr size_t
        CalculateSize(size_t count, size_t nextCount, expand_pack_t<NextTypes, size_t>... counts) {
            auto size = sizeof(Type) * count;

            if ((size % alignof(NextType)) != 0) {
                size += (alignof(NextType) - (size % alignof(NextType)));
            }

            return size + CalculateSize<NextType, NextTypes...>(nextCount, counts...);
        }

        // Base of Helper struct.
        template <typename...>
        struct Helper {};

        // Root type of Helper. Referenced recursively.
        using RootHelper = Helper<BaseType, TrailTypes...>;

        // Partial specialization when TrailTypes is single type.
        template <typename LastType>
        struct Helper<LastType> {
            size_t GetCount(const BaseType*, OverloadToken<BaseType>) {
                return 1;
            }

            ptrdiff_t GetOffset(const BaseType*, OverloadToken<BaseType>) {
                return 0;
            }
        }; // end struct Helper<LastType>

        template <typename PrevType, typename Type, typename... NextTypes>
        struct Helper<PrevType, Type, NextTypes...> : Helper<Type, NextTypes...> {
            using ParentHelper = Helper<Type, NextTypes...>;
            using ParentHelper::GetOffset;
            using ParentHelper::GetCount;

            size_t GetCount(const BaseType* base, OverloadToken<Type>) {
                return base->GetTrailCount(OverloadToken<Type>());
            }

            ptrdiff_t GetOffset(const BaseType* base, OverloadToken<Type>) {
                auto* self = static_cast<RootHelper*>(this);
                auto offset = self->GetOffset(base, OverloadToken<PrevType>())
                              + self->GetCount(base, OverloadToken<PrevType>()) * sizeof(PrevType);

                if ((offset % alignof(Type)) != 0) {
                    offset += (alignof(Type) - (offset % alignof(Type)));
                }

                return offset;
            }
        }; // end struct Helper<PrevType, Type, NextTypes...>

        template <typename T>
        size_t GetCount() const {
            return static_cast<const BaseType*>(this)->GetTrailCount(OverloadToken<T>());
        }

        template <typename T>
        ptrdiff_t GetOffset() const {
            const auto* base = static_cast<const BaseType*>(this);
            return RootHelper().GetOffset(base, OverloadToken<T>());
        }

    public:
        TrailObjects() { static_assert(std::is_final_v<BaseType>, "BaseType is not final."); }
        TrailObjects(const TrailObjects&) = delete;
        TrailObjects(TrailObjects&&) = delete;
        TrailObjects& operator =(const TrailObjects&) = delete;
        TrailObjects& operator =(TrailObjects&&) = delete;

        /**
         * @brief Get base address of writable array of desired type.
         * @tparam T the desired type.
         * @return a pointer to base of trailing type @c T.
         * @note Use without caution may cause undefined behavior.
         */
        template <typename T>
        T* GetTrailObjectBase() { return reinterpret_cast<T*>(add(static_cast<BaseType*>(this), GetOffset<T>())); }

        /**
         * @brief Get base address of readonly array of desired type.
         * @tparam T the desired type.
         * @return a pointer to base of trailing type @c T.
         * @note Use without caution may cause undefined behavior.
         */
        template <typename T>
        const T* GetTrailObjectBase() const {
            return reinterpret_cast<const T*>(add(static_cast<const BaseType*>(this), GetOffset<T>()));
        }

        static constexpr auto size_t_max_v = std::numeric_limits<size_t>::max();

        /**
         * @brief Get writable iterator wrapper of desired type.
         * @tparam T the desired type.
         * @param begin start index of the array. default is 0.
         * @param end end index of the array. default is max value of @c size_t.
         * @return a @c MutableTrailIterator<T>.
         */
        template <typename T>
        MutableTrailIterator<T> GetTrailObjects(size_t begin = 0, size_t end = size_t_max_v) {
            if (end == size_t_max_v) {
                end = GetCount<T>();
            }

            assert((begin <= end) && "begin > end");
            assert(((end - begin) <= GetCount<T>()) && "(end - begin) > count of T.");

            return { GetTrailObjectBase<T>() + begin, (end - begin) };
        }

        /**
         * @brief Get readonly iterator wrapper of desired type.
         * @tparam T the desired type.
         * @param begin start index of the array. default is 0.
         * @param end end index of the array. default is max value of @c size_t.
         * @return a @c TrailIterator<T>.
         */
        template <typename T>
        TrailIterator<T> GetTrailObjects(size_t begin = 0, size_t end = size_t_max_v) const {
            if (end == size_t_max_v) {
                end = GetCount<T>();
            }

            assert((begin <= end) && "begin > end");
            assert(((end - begin) <= GetCount<T>()) && "(end - begin) > count of T.");

            return { GetTrailObjectBase<T>() + begin, (end - begin) };
        }

        /**
         * @brief Get an element of given type at desired index.
         * @tparam T type of desired element.
         * @param index index of desired element.
         * @return writable reference of @c T.
         */
        template <typename T>
        T& At(size_t index) {
            assert((index < GetCount<T>()) && "overflow detected; check the code.");
            return GetTrailObjectBase<T>()[index];
        }

        /**
         * @brief Get an element of given type at desired index.
         * @tparam T type of desired element.
         * @param index index of desired element.
         * @return readonly reference of @c T.
         */
        template <typename T>
        const T& At(size_t index) const {
            assert((index < GetCount<T>()) && "overflow detected; check the code.");
            return GetTrailObjectBase<T>()[index];
        }

        /**
         * @brief Set trailing objects from content of std::array.
         * @tparam T Target type to set.
         * @tparam N count of elements in the array.
         * @param array a container.
         */
        template <typename T, size_t N>
        void SetTrailObjects(const std::array<T, N>& array) {
            SetTrailObjects(array.data(), N);
        }

        /**
         * @brief Set trailing objects from content of std::vector.
         * @tparam T Target type to set.
         * @param vector a container.
         */
        template <typename T>
        void SetTrailObjects(const std::vector<T>& vector) {
            SetTrailObjects(vector.data(), vector.size());
        }

        /**
         * @brief Set trailing objects from array.
         * @tparam T Target type to set.
         * @param array an array of T.
         * @param count a count of elements in array.
         */
        template <typename T>
        void SetTrailObjects(const T* array, size_t count) {
            assert((count <= GetCount<T>()) && "overflow detected; check the code.");
            std::uninitialized_move_n(array, count, GetTrailObjectBase<T>());
        }

        /**
         * @brief Get a sizeof tailing objects.
         * @param counts lists of count per trailing type.
         * @return a sizeof trailing objects.
         * @note Return value does not include sizeof BaseType.
         */
        static constexpr size_t GetTrailSize(expand_pack_t<TrailTypes, size_t>... counts) {
            return CalculateSize<BaseType, TrailTypes...>(1, counts...) - sizeof(BaseType);
        }
    }; // end class TrailObjects
} // end namespace BuildScript

#endif // BUILDSCRIPT_UTILS_TRAILOBJECTS_H