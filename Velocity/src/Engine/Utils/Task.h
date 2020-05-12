#pragma once

class Task
{
public:
    Task();
    virtual ~Task();

    void Start();
    void Stop();
    bool IsRunning() const;

protected:
    virtual void Run() = 0;
private:
    void Loop();

    std::thread m_WorkThread;
    bool m_Finished;
};

/* A Task that manages a queue of items within a Container that supports dual ended insertion and removal */
template <typename ItemType, typename Container = std::list<ItemType>>
class TQueueTask : public Task
{
public:
    void Enqueue(const ItemType& item)
    {
        std::unique_lock<std::mutex> m_queueLock(m_queueMutex);
        m_queue.push_back(item);
    }

protected:
    Container m_queue;
    std::mutex m_queueMutex;
};