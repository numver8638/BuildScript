/*
 * Handle.h
 * - Reference of garbage collected object.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_GC_HANDLE_H
#define BUILDSCRIPT_GC_HANDLE_H

#include <type_traits>

#include <BuildScript/Config.h>

namespace BuildScript {
    class GCObject;
    class ScriptValue;
    class GCTracer;
    class GCHeap;

    using GCObjectRef = GCObject* const *;

    template <typename T>
    class GCHandle;

    namespace Internal {
        class GCHandleBase {
            friend class BuildScript::GCHeap;

        private:
            GCObject* m_ptr;

        protected:
            explicit GCHandleBase(GCObject* ptr)
                : m_ptr(ptr) {}

            void SetRawPointer(GCObject* ptr) { m_ptr = ptr; }
            GCObject* GetRawPointer() const { return m_ptr; }
            GCObjectRef GetReference() const { return &m_ptr; }

            static void RegisterGlobal(Internal::GCHandleBase*);
            static void UnregisterGlobal(Internal::GCHandleBase*);

            static void WriteBarrier(Internal::GCHandleBase*, GCObject*);
        }; // end class GCHandleBase

        class GCLocalBase {
            friend class BuildScript::GCHeap;

        private:
            // This function is not part of API, but must be exported to track GCed objects.
            // Use function rather than direct thread_local due to
            // export of thread_local is not allowed on MSVC.
            static EXPORT_API GCLocalBase*& GetRoot();

            GCLocalBase* m_prev = nullptr;
            GCObject* m_ptr;

        protected:
            explicit GCLocalBase(GCObject* ptr)
               : m_ptr(ptr), m_prev(GetRoot()) {
                GetRoot() = this;
            }

           ~GCLocalBase() noexcept {
               GetRoot() = m_prev;
            }

            void SetRawPointer(GCObject* ptr) { m_ptr = ptr; }
            GCObject* GetRawPointer() const { return m_ptr; }
            GCObjectRef GetReference() const { return &m_ptr; }
        }; // end class GCLocalBase
    } // end namespace Internal

    /**
     * @brief Represents a pointer of garbage collected object out of the GC heap.
     * @tparam T a type of the pointer.
     *
     * GCGlobal<T> is ...
     */
    template <typename T>
    class GCGlobal : Internal::GCHandleBase {
        friend class GCTracer;

    public:
        GCGlobal()
            : GCGlobal(nullptr) {}

        GCGlobal(std::nullptr_t)
            : Internal::GCHandleBase(nullptr) { RegisterGlobal(this); }

        template <typename U>
        GCGlobal(U* ptr)
            : Internal::GCHandleBase(ptr) {
            static_assert(std::is_convertible_v<U*, T*>, "type check.");
            RegisterGlobal(this);
        }

        template <typename U>
        GCGlobal(GCHandle<U> handle);

        ~GCGlobal() noexcept { UnregisterGlobal(this); }

        // copy and move constructions are forbidden.
        GCGlobal(const GCGlobal&) = delete;
        GCGlobal(GCGlobal&&) = delete;

        // copy and move assignments are forbidden.
        GCGlobal& operator =(const GCGlobal&) = delete;
        GCGlobal& operator =(GCGlobal&&) = delete;

        template <typename U>
        GCGlobal& operator =(U* ptr) {
            static_assert(std::is_convertible_v<U*, T*>, "type check.");
            SetRawPointer(ptr);
            return *this;
        }

        template <typename U>
        GCGlobal& operator =(GCHandle<U> handle);

        GCHandle<T> operator *() const { return ToHandle(); }

        operator bool() const { return GetRawPointer() != nullptr; }

        T* operator ->() { return static_cast<T*>(GetRawPointer()); }

        const T* operator ->() const { return static_cast<const T*>(GetRawPointer()); }

        GCHandle<T> ToHandle() const { return { GetReference() }; }

        template <typename U>
        GCHandle<U> Cast() {
            static_assert(std::is_convertible_v<U*, T*>, "type check.");
            return { GetReference() };
        }

        T* Unwrap() {
            return static_cast<T*>(GetRawPointer());
        }

        T* Unwrap() const {
            return static_cast<const T*>(GetRawPointer());
        }
    }; // end class GCGlobal

    /**
     * @brief Represents a pointer of garbage collected object in the machine stack.
     * @tparam T a type of the pointer.
     *
     * GCLocal<T> is ...
     */
    template <typename T>
    class GCLocal : Internal::GCLocalBase {
        friend class GCTracer;

    public:
        GCLocal()
            : GCLocal(nullptr) {}

        GCLocal(std::nullptr_t)
            : Internal::GCLocalBase(nullptr) {}

        template <typename U>
        GCLocal(U* ptr)
            : Internal::GCLocalBase(ptr) {
            static_assert(std::is_convertible_v<U*, T*>, "type check.");
       }

        template <typename U>
        GCLocal(GCHandle<U> handle);

        // copy and move constructions are forbidden.
        GCLocal(const GCLocal&) = delete;
        GCLocal(GCLocal&&) = delete;

        // copy and move assignments are forbidden.
        GCLocal& operator =(const GCLocal&) = delete;
        GCLocal& operator =(GCLocal&&) = delete;

        template <typename U>
        GCLocal& operator =(U* ptr) {
            static_assert(std::is_convertible_v<U*, T*>, "type check.");
            SetRawPointer(ptr);
            return *this;
        }

        template <typename U>
        GCLocal& operator =(GCHandle<U> handle);

        GCHandle<T> operator *() const { return ToHandle(); }

        operator bool() const { return GetRawPointer() != nullptr; }

        T* operator ->() { return static_cast<T*>(GetRawPointer()); }

        const T* operator ->() const { return static_cast<const T*>(GetRawPointer()); }

        GCHandle<T> ToHandle() const { return { GetReference() }; }

        template <typename U>
        GCHandle<U> Cast() {
            static_assert(std::is_convertible_v<U*, T*>, "type check.");
            return { GetReference() };
        }

        T* Unwrap() {
            return static_cast<T*>(GetRawPointer());
        }

        T* Unwrap() const {
            return static_cast<T*>(GetRawPointer());
        }
    }; // end class GCLocal

    /**
     * @brief Represents a pointer of garbage collected object in the GC heap.
     * @tparam T a type of the pointer.
     *
     * GCMember<T> is ...
     */
    template <typename T>
    class GCMember : Internal::GCHandleBase {
        friend class GCTracer;

    public:
        GCMember()
            : GCMember(nullptr) {}

        GCMember(std::nullptr_t)
            : Internal::GCHandleBase(nullptr) {}

        template <typename U>
        GCMember(U* ptr)
            : Internal::GCHandleBase(nullptr) {
            static_assert(std::is_convertible_v<U*, T*>, "type check.");
            WriteBarrier(this, ptr);
        }

        template <typename U>
        GCMember(GCHandle<U> handle);

        // copy and move constructions are forbidden.
        GCMember(const GCMember&) = delete;
        GCMember(GCMember&&) = delete;

        // copy and move assignments are forbidden.
        GCMember& operator =(const GCMember&) = delete;
        GCMember& operator =(GCMember&&) = delete;

        template <typename U>
        GCMember& operator =(U* ptr) {
            static_assert(std::is_convertible_v<U*, T*>, "type check.");
            WriteBarrier(this, ptr);
            return *this;
        }

        GCMember& operator =(std::nullptr_t) {
            WriteBarrier(this, nullptr);
            return *this;
        }

        template <typename U>
        GCMember& operator =(GCHandle<U> handle);

        GCLocal<T> operator *() const { return ToLocal(); }

        operator bool() const { return GetRawPointer() != nullptr; }

        T* operator ->() { return static_cast<T*>(GetRawPointer()); }

        const T* operator ->() const { return static_cast<const T*>(GetRawPointer()); }

        GCLocal<T> ToLocal() const { return { static_cast<T*>(GetRawPointer()) }; }

        T* Unwrap() {
            return static_cast<T*>(GetRawPointer());
        }

        T* Unwrap() const {
            return static_cast<T*>(GetRawPointer());
        }
    }; // end class GCMember

    /**
     * @brief Represents a reference of garbage collected object.
     * @tparam T
     */
    template <typename T>
    class GCHandle {
        template <typename>
        friend class GCHandle;
        template <typename>
        friend class GCGlobal;
        template <typename>
        friend class GCLocal;
        template <typename>
        friend class GCMember;

    private:
        GCObjectRef m_ref;

        GCHandle(GCObjectRef ref)
            : m_ref(ref) {}

    public:
        GCHandle()
            : GCHandle(nullptr) {}

        GCHandle(std::nullptr_t)
            : m_ref(nullptr) {}

        template <typename U>
        GCHandle(GCHandle<U> other)
            : m_ref(other.m_ref) {
            static_assert(std::is_convertible_v<U*, T*>, "type check.");
        }

        template <typename U>
        GCHandle& operator =(GCHandle<U> other) {
            static_assert(std::is_convertible_v<U*, T*>, "type check.");
            m_ref = other.m_ref;
            return *this;
        }

        T* operator ->() { return Unwrap(); }
        const T* operator ->() const { return Unwrap(); }

        operator bool() const { return m_ref != nullptr; }

        template <typename U>
        GCHandle<U> Cast() {
            static_assert(std::is_convertible_v<U*, T*>, "type check.");
            return { m_ref };
        }

        T* Unwrap() {
            return static_cast<T*>(*m_ref);
        }

        T* Unwrap() const {
            return static_cast<T*>(*m_ref);
        }
    }; // end class GCHandle

    template <typename T>
    template <typename U>
    inline GCGlobal<T>::GCGlobal(GCHandle<U> handle)
        : Internal::GCHandleBase(*(handle.m_ref)) {
        static_assert(std::is_convertible_v<U*, T*>, "type check.");
        RegisterGlobal(this);
    }

    template <typename T>
    template <typename U>
    inline GCGlobal<T>& GCGlobal<T>::operator =(GCHandle<U> handle) {
        static_assert(std::is_convertible_v<U*, T*>, "type check.");
        SetRawPointer(*(handle.m_ref));
        return *this;
    }

    template <typename T>
    template <typename U>
    inline GCLocal<T>::GCLocal(GCHandle<U> handle)
        : Internal::GCLocalBase(*(handle.m_ref)) {
        static_assert(std::is_convertible_v<U*, T*>, "type check.");
    }

    template <typename T>
    template <typename U>
    inline GCLocal<T>& GCLocal<T>::operator =(GCHandle<U> handle) {
        static_assert(std::is_convertible_v<U*, T*>, "type check.");
        SetRawPointer(*(handle.m_ref));
        return *this;
    }

    template <typename T>
    template <typename U>
    inline GCMember<T>::GCMember(GCHandle<U> handle)
        : Internal::GCHandleBase(nullptr) {
        static_assert(std::is_convertible_v<U*, T*>, "type check.");
        WriteBarrier(this, *(handle.m_ref));
    }

    template <typename T>
    template <typename U>
    inline GCMember<T>& GCMember<T>::operator =(GCHandle<U> handle) {
        static_assert(std::is_convertible_v<U*, T*>, "type check.");
        WriteBarrier(this, *(handle.m_ref));
        return *this;
    }
} // end namespace BuildScript

#endif // BUILDSCRIPT_GC_HANDLE_H