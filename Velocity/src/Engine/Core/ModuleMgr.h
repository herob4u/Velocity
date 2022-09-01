#pragma once
#include "Engine/Core/Types/StringId.h"
#include "Engine/Core/Types/Singletons.h"

namespace Vct
{
    class IModule;

    // Maintains a graph of all modules and their dependencies.
    // Loads them in order upon engine initialization
    class ModuleMgr : public InstancedSingleton<ModuleMgr>
    {
        struct GraphEntry
        {
            IModule* Module;
            std::vector<int> Adjacency;
        };

    public:
        void RegisterModule(IModule* module);
        void UnregisterModule(IModule* module);

        // Loads/Unloads all registered modules in topological order
        void LoadModules();
        void UnloadModules();

        void LoadModule(StringId const& moduleId);
        void UnloadModule(StringId const& moduleId);

        bool ModuleExist(StringId const& moduleId) const;
        
        void Print();
    private:
        void LoadModule(IModule* module);
        void UnloadModule(IModule* module);

        IModule* GetModule(StringId const& moduleId) const;

        // Topologically sorts the module lists to resolve load dependencies
        std::vector<IModule*> GetToposort();
        bool GetToposort(int vertex, std::unordered_set<int>& pendingList, std::vector<bool>& visitList, std::vector<IModule*>& outModules);
        bool ContainsCycle(int vertex, std::unordered_set<int>& pendingList, std::vector<bool>& visitList);
        std::vector<GraphEntry> m_moduleGraph;
    };
}