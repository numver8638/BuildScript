/*
 * TrailObjects.h
 * - Variable length class.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_TRAILOBJECTS_H
#define BUILDSCRIPT_UTILS_TRAILOBJECTS_H

#include <array>
#include <vector>
#include <cstring>
#include <type_traits>

#include <BuildScript/Utils/PointerArith.h>

namespace BuildScript {
    /**
     * @brief Implements variable length class.
     *
     * @tparam BaseType Base class type inherits @c TrailObjects.
     * @tparam TrailTypes Types to be appended to end of base class.
     */
    template <typename BaseType, typename... TrailTypes>
    class TrailObjects {
    protected:
        /**
         * @brief Expand parameter pack to T.
         * @tparam T
         */
        template <typename, typename T>
        using expand_pack_t = T;

        /**
         * @brief
         * @tparam T
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
        /**
         * @brief
         * @tparam T
         */
        template <typename T>
        class TrailIterator {
            friend TrailObjects;

        private:
            T* m_base;
            size_t m_length;

            TrailIterator(T* base, size_t length)
                : m_base(base), m_length(length) {}

        public:
            struct iterator {
                friend TrailIterator;

            private:
                T* m_base;
                size_t m_index;

                iterator(const T* base, size_t index)
                    : m_base(base), m_index(index) {}

            public:
                using iterator_category = std::forward_iterator_tag;
                using value_type = T;
                using difference_type = std::ptrdiff_t;
                using pointer = value_type*;
                using reference = value_type&;
                using const_reference = const value_type&;

                iterator(const iterator&) = default;
                iterator& operator =(const iterator&) = default;

                reference operator *() const {
                    return m_base[m_index];
                }

                reference operator ->() const {
                    return m_base[m_index];
                }

                bool operator !=(const iterator& other) const {
                    return (m_base != other.m_base) || (m_index != other.m_index);
                }

                iterator& operator ++() {
                    ++m_index;
                    return *this;
                }

                iterator operator ++(int) {
                    auto old = *this;
                    operator ++();
                    return old;
                }
            };

            iterator begin() { return iterator(m_base, 0); }
            iterator end() { return iterator(m_base, m_length); }
        }; // end class TrailIterator

        TrailObjects() { static_assert(std::is_final_v<BaseType>, "Base is not final."); }
        TrailObjects(const TrailObjects&) = delete;
        TrailObjects(TrailObjects&&) = delete;
        TrailObjects& operator =(const TrailObjects&) = delete;
        TrailObjects& operator =(TrailObjects&&) = delete;

        /**
         * @brief
         * @tparam T
         * @return a pointer to base of trailing type @c T.
         */
        template <typename T>
        T* GetTrailObjectBase() { return reinterpret_cast<T*>(add(static_cast<BaseType*>(this), GetOffset<T>())); }

        /**
         * @brief
         * @tparam T
         * @return a pointer to base of trailing type @c T.
         */
        template <typename T>
        const T* GetTrailObjectBase() const {
            return reinterpret_cast<const T*>(add(static_cast<const BaseType*>(this), GetOffset<T>()));
        }

        static constexpr auto size_t_max_v = std::numeric_limits<size_t>::max();

        /**
         * @brief Get
         * @tparam T
         * @param begin
         * @param end
         * @return
         */
        template <typename T>
        TrailIterator<T> GetTrailObjects(size_t begin = 0, size_t end = size_t_max_v) {
            if (end == size_t_max_v) {
                end = GetCount<T>();
            }

            assert((begin <= end) && "begin > end");
            assert(((end - begin) <= GetCount<T>()) && "(end - begin) > count of T.");

            return { GetTrailObjectBase<T>() + begin, (end - begin) };
        }

        /**
         * @brief
         * @tparam T
         * @param begin
         * @param end
         * @return
         */
        template <typename T>
        TrailIterator<const T> GetTrailObjects(size_t begin = 0, size_t end = size_t_max_v) const {
            if (end == size_t_max_v) {
                end = GetCount<T>();
            }

            assert((begin <= end) && "begin > end");
            assert(((end - begin) <= GetCount<T>()) && "(end - begin) > count of T.");

            return { GetTrailObjectBase<T>() + begin, (end - begin) };
        }

        /**
         * @brief
         * @tparam T
         * @param index
         * @return
         */
        template <typename T>
        T& At(size_t index) {
            assert((index < GetCount<T>()) && "overflow detected; check the code.");
            return GetTrailObjectBase<T>()[index];
        }

        /**
         * @brief
         * @tparam T
         * @param index
         * @return
         */
        template <typename T>
        const T& At(size_t index) const {
            assert((index < GetCount<T>()) && "overflow detected; check the code.");
            return GetTrailObjectBase<T>()[index];
        }

        /**
         * @brief Set trail objects from content of std::array.
         * @tparam T Target type to set.
         * @tparam N count of elements in the array.
         * @param array the array contains
         */
        template <typename T, size_t N>
        void SetTrailObjects(const std::array<T, N>& array) {
            SetTrailObjects(array.data(), N);
        }

        /**
         * @brief Set trail objects from content of std::vector.
         * @tparam T Target type to set.
         * @param vector
         */
        template <typename T>
        void SetTrailObjects(const std::vector<T>& vector) {
            SetTrailObjects(vector.data(), vector.size());
        }

        /**
         * @brief Set trail objects from array.
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
         * @brief Get a size of trail objects.
         * @param counts
         * @return
         * @note Return value does not include sizeof BaseType.
         */
        static constexpr size_t GetTrailSize(expand_pack_t<TrailTypes, size_t>... counts) {
            return CalculateSize<BaseType, TrailTypes...>(1, counts...) - sizeof(BaseType);
        }
    }; // end class TrailObjects
} // end namespace BuildScript

#endif // BUILDSCRIPT_UTILS_TRAILOBJECTS_H