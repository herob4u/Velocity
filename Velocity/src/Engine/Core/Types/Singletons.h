#pragma once
#include "Engine/Core/Log.h"

namespace Vct
{
    template <typename Derived>
    class InstancedSingleton
    {
    public:
        static Derived& GetInstance()
        {
            static Derived s_instance;
            return s_instance;
        }
    protected:
        InstancedSingleton() {}
        virtual ~InstancedSingleton(){}
    };

    template <typename Derived>
    class Singleton
    {
    public:
        static void Init()
        {
            if(!s_instance)
            {
                s_instance = new Derived();
            }
            else
            {
                VCT_WARN("Attempted to initialize an already instanced singleton!");
            }
        }

        static void Shutdown()
        {
            if(s_instance)
            {
                delete s_instance;
                s_instance = nullptr;
            }
            else
            {
                VCT_WARN("Attempted to shutdown an invalid singleton!");
            }
        }

        static Derived* GetInstance() { return s_instance; }
        static Derived const* GetInstanceConst() { return s_instance; }
    private:
        Singleton() {}
        virtual ~Singleton() {}
        static Derived* s_instance;
    };

    template <typename Derived>
    class ScopedSingleton
    {
    public:
        ScopedSingleton()
        {
            ASSERT(!s_instance, "ScopedSingleton already instantiated elsewhere!");
            s_instance = this;
        }

        virtual ~ScopedSingleton()
        {
            ASSERT(s_instance == this, "ScopedSingleton accidentally instantiated multiple times!");
            s_instance = nullptr;
        }

        Derived* GetInstance() { return s_instance; }
        Derived const* GetInstanceConst() { return s_instance; }
    private:
        ScopedSingleton& operator=(ScopedSingleton const& Other) = delete;
        void* operator new(size_t) { VCT_ERROR("Cannot dynamically allocate scoped singleton. Must be stack allocated!"); return nullptr; }
        void operator delete(void*) { VCT_ERROR("Cannot delete scoped singleton. Must be stack allocated!"); return; }

        Derived* s_instance;
    };
}