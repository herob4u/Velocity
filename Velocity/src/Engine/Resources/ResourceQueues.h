#pragma once

namespace Vct
{
    class Resource;

    using OnResourcesLoaded = std::function<void(const std::vector<Resource*>&)>;

    enum LoadPriority : uint8_t
    {
        DEFAULT,
        HIGH
    };

    /*  The ResourceStreamer accepts a list of assets to batch stream and load from disk
    *   Loading is deferred to the ResourceLoader to synchronize the loading of dependencies
    *   while avoiding race conditions when modifying resource internal states.
    */
    struct ResourceStreamer
    {
        struct AsyncItem
        {
            AsyncItem(const std::vector<Resource*>& resources, OnResourcesLoaded cb)
                : Resources(resources)
                , Callback(cb)
            {
            }

            std::vector<Resource*> Resources;
            OnResourcesLoaded Callback;
        };

        ResourceStreamer(struct ResourceLoader& resourceLoader);
        void EnqueueResources(const std::vector<Resource*>& resources, OnResourcesLoaded cb, LoadPriority priority = LoadPriority::DEFAULT);

    protected:
        void Execute();

        std::list<AsyncItem> m_queue;
        std::thread m_workThread;
        std::mutex m_queueMutex;
        ResourceLoader& m_resourceLoader;
    };

    /* The resource loader is responsible for loading a single batch of stream requests */
    struct ResourceLoader
    {
        struct AsyncItem
        {
            AsyncItem(Resource* resource, void* data, size_t bytes, bool success)
                : InResource(resource)
                , Data(data)
                , NumBytes(bytes)
                , bSuccess(success)
            {
            }

            Resource* InResource;
            void* Data;
            size_t NumBytes;
            bool bSuccess;
        };

        ResourceLoader();
        void EnqueueResource(Resource* resource, void* data, size_t bytes, bool success);
        void Finish();

    protected:
        void Execute();

        std::list<AsyncItem> m_queue;
        std::thread m_workThread;
        std::mutex m_queueMutex;
    };
}