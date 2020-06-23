#pragma once

//#define ASYNC_LOADER

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

        ResourceStreamer(struct SyncResourceLoader& resourceLoader);
        void EnqueueResources(const std::vector<Resource*>& resources, OnResourcesLoaded cb, LoadPriority priority = LoadPriority::DEFAULT);

    protected:
        void Execute();

        std::list<AsyncItem> m_queue;
        std::thread m_workThread;
        std::mutex m_queueMutex;
        SyncResourceLoader& m_resourceLoader;
    };

    /*  An Asynchronous Resource Loader is designed to load resources that are inherently thread safe.
    *   Callbacks enqueue resource data within the queue, the the loader continuously processes queue
    *   elements as they arrive.
    */    
    struct AsyncResourceLoader
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

        AsyncResourceLoader();
        virtual ~AsyncResourceLoader() {}

        void EnqueueResource(Resource* resource, void* data, size_t bytes, bool success);
        void Finish();

    protected:
        void Execute();

        std::thread m_workThread;
        std::list<AsyncItem> m_queue;
        std::mutex m_queueMutex;
    };

    #define SYNC_LOAD_BATCH_SIZE 4
    /*  A Synchronous Resource Loader is designed to execute resource loads on the main thread.
    *   Asynchronous callbacks enqueue resource data within the queue, and the application polls
    *   the loader occassionally to perform a flush in which all queued resources are loaded.
    */
    struct SyncResourceLoader
    {
        struct SyncItem
        {
            SyncItem(Resource* resource, void* data, size_t bytes, bool success)
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

        SyncResourceLoader();

        /* Enqueues a resource load associated with a specified batchId. */
        void EnqueueResource(int batchId, Resource* resource, void* data, size_t numBytes, bool success);

        /*  Makes a new batch of items to load. Once a batch of complete, the associated callback is invoked */
        /*  If all IDs are occupied, the calling thread is blocked until a slot is freed up 
        *   Returns the newly alloted batch ID so that the batch can be subsequently ammended by calls
        *   to EnqueueResource
        */
        int MakeNewLoadBatch(size_t batchSize, OnResourcesLoaded OnBatchLoaded);

        void Handle();

        protected:
        int GetNextBatchId() const;
        void FinishBatch(int batchId);

        typedef std::array<std::list<SyncItem>, SYNC_LOAD_BATCH_SIZE> BatchQueue;
        BatchQueue m_queue;
        std::mutex m_queueMutex;

        struct BatchHeader
        {
            size_t Remaining;
            OnResourcesLoaded Callback;
            std::vector<Resource*> Loaded;
        };

        std::array<BatchHeader, SYNC_LOAD_BATCH_SIZE> m_batchHeader;
    };
}