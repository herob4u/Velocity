#include "vctPCH.h"

#include "LayerStack.h"

#include <algorithm>

using namespace Vct;

LayerStack::LayerStack()
    : m_LayerInsertIndex(0)
{
}

LayerStack::~LayerStack()
{
    for(Layer* layer : m_LayerStack)
    {
        delete layer;
        layer = nullptr;
    }

    m_LayerStack.clear();
}

void LayerStack::PushLayer(Layer* layer)
{
    m_LayerStack.insert(m_LayerStack.begin() + m_LayerInsertIndex++, layer);
}

void LayerStack::PushOverlay(Layer* overlay)
{
    m_LayerStack.push_back(overlay);
}

void LayerStack::PopLayer(Layer* layer)
{
    auto found = std::find(m_LayerStack.begin(), m_LayerStack.begin() + m_LayerInsertIndex, layer);
    if(found != m_LayerStack.end())
    {
        m_LayerStack.erase(found);
        --m_LayerInsertIndex;
    }
}

void LayerStack::PopOverlay(Layer* overlay)
{
    auto found = std::find(m_LayerStack.begin() + m_LayerInsertIndex, m_LayerStack.end(), overlay);
    if(found != m_LayerStack.end())
    {
        m_LayerStack.erase(found);
    }
}