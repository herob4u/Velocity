#include "vctPCH.h"
#include "ModuleMgr.h"
#include "Module.h"

namespace Vct
{
    void ModuleMgr::RegisterModule(IModule* module)
    {
        if(!module)
        {
            VCT_WARN("Failed to register module - module was invalid");
            return;
        }

        StringId moduleId = module->GetTypeId();
        if(!ModuleExist(moduleId))
        {
            GraphEntry newEntry;
            newEntry.Module = module;
            newEntry.Adjacency; // ... module->GetDependencies()
            m_moduleGraph.push_back(newEntry);

            // Only interested in cycles in this entry's connected component
            {
                std::vector<bool> visitList;
                std::unordered_set<int> pendingList;
                if(ContainsCycle(static_cast<int>(m_moduleGraph.size() - 1), pendingList, visitList))
                {
                    // Cycle detected, remove the element. Failed registration
                    VCT_ERROR("Failed to register module - cyclic dependency introduced");
                    m_moduleGraph.pop_back();
                    return;
                }
            }

            module->OnRegister();
        }
    }

    void ModuleMgr::UnregisterModule(IModule* module)
    {
        if(!module)
        {
            VCT_WARN("Failed to unregister module - module was invalid");
            return;
        }

        StringId moduleId = module->GetTypeId();
        for(int v = 0; v < m_moduleGraph.size(); ++v)
        {
            if(m_moduleGraph[v].Module->GetTypeId() == moduleId)
            {
                m_moduleGraph.erase(m_moduleGraph.begin() + v);
            }
        }

        // Fix up dangling dependencies...
        // ... 

        module->OnUnregister();
    }

    void ModuleMgr::LoadModules()
    {
        std::vector<IModule*> modules = GetToposort();
        for(IModule* module : modules)
        {
            LoadModule(module);
        }
    }

    void ModuleMgr::UnloadModules()
    {
        // In reverse topo order to satisfy dependency chain
        std::vector<IModule*> modules = GetToposort();
        for(auto it = modules.rbegin(); it != modules.rend(); ++it)
        {
            UnloadModule(*it);
        }
    }

    void ModuleMgr::LoadModule(StringId const& moduleId)
    {
        LoadModule(GetModule(moduleId));
    }

    void ModuleMgr::UnloadModule(StringId const& moduleId)
    {
        UnloadModule(GetModule(moduleId));
    }

    bool ModuleMgr::ModuleExist(StringId const& moduleId) const
    {
        return GetModule(moduleId) != nullptr;
    }

    void ModuleMgr::Print()
    {
        for(int v = 0; v < m_moduleGraph.size(); ++v)
        {
            VCT_INFO("[{0}] {1}, Dependencies: ", v, m_moduleGraph[v].Module->GetTypeId().ToStringRef());
            for(int e = 0; e < m_moduleGraph[v].Adjacency.size(); ++e)
            {
                int v2 = m_moduleGraph[v].Adjacency[e];
                if(v2 < m_moduleGraph.size())
                {
                    VCT_INFO(">{0}", m_moduleGraph[v2].Module->GetTypeId().ToStringRef());
                }

                VCT_INFO("\n");
            }
        }
    }

    void ModuleMgr::LoadModule(IModule* module)
    {
        if(module && !module->IsLoaded())
        {
            module->Load();
        }
    }

    void ModuleMgr::UnloadModule(IModule* module)
    {
        if(module && module->IsLoaded())
        {
            module->Unload();
        }
    }

    IModule* ModuleMgr::GetModule(StringId const& moduleId) const
    {
        for(GraphEntry const& entry : m_moduleGraph)
        {
            if(entry.Module->GetTypeId() == moduleId)
            {
                return entry.Module;
            }
        }

        return nullptr;
    }

    std::vector<IModule*> ModuleMgr::GetToposort()
    {
        std::vector<IModule*> toposort;
        toposort.resize(m_moduleGraph.size(), nullptr);

        std::vector<bool> visitList(m_moduleGraph.size(), false); // Flag per vertex on whether it was visited
        std::unordered_set<int> pendingList; // Vertex IDs we are processing

        bool success = true;
        for(int v = 0; v < m_moduleGraph.size() && success; ++v)
        {
            if(!visitList[v])
            {
                success &= GetToposort(v, pendingList, visitList, toposort);
            }
        }

        ASSERT(success, "Failed to toposort modules! Cycle was found - failure to clear cycle may load modules in invalid ordering that may lead to crash");

        // Failed to toposort, at worst case, just return the graph in-order.
        if(!success)
        {
            for(int v = 0; v < m_moduleGraph.size(); ++v)
            {
                toposort[v] = m_moduleGraph[v].Module;
            }
        }

        return toposort;
    }

    bool ModuleMgr::GetToposort(int vertex, std::unordered_set<int>& pendingList, std::vector<bool>& visitList, std::vector<IModule*>& outModules)
    {
        // Reached a vertex, but already processing it! i.e a cycle
        if(pendingList.find(vertex) != pendingList.end())
        {
            VCT_WARN("Failed to topoligically sort module graph. Cycle was found at vertex {0}, typeid={1}", vertex, m_moduleGraph[vertex].Module->GetTypeId().ToString().c_str());
            return false;
        }

        visitList[vertex] = true;
    }

    bool ModuleMgr::ContainsCycle(int vertex, std::unordered_set<int>& pendingList, std::vector<bool>& visitList)
    {
        if(pendingList.find(vertex) != pendingList.end())
        {
            VCT_WARN("Cycle found at vertex {0}, moduleId={1}", vertex, m_moduleGraph[vertex].Module->GetTypeId().ToStringRef());
            return true;   
        }

        for(int v : m_moduleGraph[vertex].Adjacency)
        {
            if(ContainsCycle(v, pendingList, visitList))
            {
                return true;
            }
        }

        pendingList.erase(vertex);
        return false;
    }
}