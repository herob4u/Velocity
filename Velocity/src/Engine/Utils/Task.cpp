#include "vctPCH.h"
#include "Task.h"

Task::Task()
    : m_Finished(false)
{
}

Task::~Task()
{
}

void Task::Start()
{
    ASSERT(!m_Finished, "Cannot restart elapsed thread");
    m_WorkThread = std::thread(&Task::Loop, this);
}

void Task::Stop()
{
    m_Finished = true;
}

bool Task::IsRunning() const
{
    return !m_Finished;
}

void Task::Loop()
{
    while(!m_Finished)
    {
        Run();
    }
}
