#pragma once
#include "Engine/Core/Types/StringId.h"
#include "ModuleMgr.h"

#define DECLARE_MODULE(className)\
public:\
static className* GetInstance() { return s_instance; }\
static StringId const& GetStaticTypeId() { return s_typeId; }\
virtual StringId const& GetTypeId() { return GetStaticTypeId(); }\
private:\
static void SetInstance(className##* instance) { ASSERT(!instance, "Module already set!"); s_instance = instance; }\
static className* s_instance;\
static StringId s_typeId;

#define IMPLEMENT_MODULE(className)\
static StringId className::s_typeId(#className);

// @todo: use a typelist, foreach and add ModuleClass::GetStaticTypeId() to module graph
#define MODULE_DEPENDENCY(...)

namespace Vct
{
    enum class EModuleState
    {
        UNLOADED,
        LOADED
    };

    // A module houses a set of functionalities - i.e FileIO module, Rendering module
    // Gives potential for static initialization of systems, and sharing of resource
    //
    // A model can be valid regardless of registration. Registration is what allows modules to load
    class IModule
    {
    public:
        friend class ModuleMgr;

        void Load()
        {
            OnLoad();
            m_state = EModuleState::LOADED;
        }

        void Unload()
        {
            OnUnload();
            m_state = EModuleState::UNLOADED;
        }

        virtual void OnRegister() = 0;
        virtual void OnUnregister() = 0;
        virtual void OnLoad() = 0;
        virtual void OnUnload() = 0;

        virtual StringId const& GetTypeId() const { return StringId::NONE; }

        bool IsLoaded() const { return m_state == EModuleState::LOADED;}

        template<typename ModuleClass>
        static ModuleClass* Register()
        {
            auto moduleMgr = ModuleMgr::GetInstance();
            if(!moduleMgr.ModuleExist(ModuleClass::GetStaticTypeId()))
            {
                ModuleClass* module = new ModuleClass();
                moduleMgr.RegisterModule(module);

                // Set instance
                module->s_instance = module;
            }
        }

        template<typename ModuleClass>
        static void Unregister()
        {
            auto moduleMgr = ModuleMgr::GetInstance();
            if(moduleMgr.ModuleExist(ModuleClass::GetStaticTypeId()))
            {
                ModuleClass* module = ModuleClass::GetInstance(); 
                ASSERT(!module->IsLoaded(), "It makes little sense to unregister a module while it's loaded...?");
                moduleMgr.UnregisterModule(module);

                // Clear instance
                ASSERT(module->s_instance == module, "Huh..?");
                module->s_instance = nullptr;
                delete module;
            }
        }
    protected:
        IModule(){}
        ~IModule(){}
    private:
        EModuleState m_state;
    };
}