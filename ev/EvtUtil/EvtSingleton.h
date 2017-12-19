#ifndef _EVT_SINGLETON_H_
#define _EVT_SINGLETON_H_

#include "Log.h"

template<class T>
class EvtSingleton
{
public:
    static T* GetInstance();
    static void destroy();

private:

    EvtSingleton(EvtSingleton const&){};
    EvtSingleton& operator=(EvtSingleton const&){};

protected:
    static T* m_instance;

    EvtSingleton();
    virtual ~EvtSingleton(){  };
};

template<class T>
EvtSingleton<T>::EvtSingleton()
{
    m_instance = static_cast <T*> (this);
}

template<class T>
T* EvtSingleton<T>::GetInstance()
{
    if(!m_instance)
    {
        EvtSingleton<T>::m_instance = new T();
    }

    return m_instance;
}

template<class T>
void EvtSingleton<T>::destroy()
{
    delete EvtSingleton<T>::m_instance;
    EvtSingleton<T>::m_instance = 0;
}

template<class T>
T *EvtSingleton<T>::m_instance = 0;

#endif // _EVT_SINGLETON_H_