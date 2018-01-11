#pragma once


template<typename T> class Singleton
{
public:
	static T* GetSingleton()
	{
		if (!m_Singleton)
		{
			m_Singleton = new T;
		}
		return m_Singleton;
	}
	static void DestroySingleton()
	{
		if (m_Singleton)
		{
			delete m_Singleton;
			m_Singleton = nullptr;
		}
	}

protected:
	static T* m_Singleton;
};


template<typename T> T* Singleton<T>::m_Singleton = nullptr;
