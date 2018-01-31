#pragma once
#include <memory>


	/* 单例模式的实现：一、模板 */
	
	//严格来说，该模板并不算真正的单例模式，真正的单例模式设计需要满足： 不可被创建（构造函数私有），不可被赋值（赋值和复制构造函数私有）
	//	其次，继承该类的子类 如果没有将自己的构造函数和赋值函数 设为私有，其子类还是可以被单独创建和赋值
	template<typename T>
	class ZSingleton
	{
	public:
		static std::auto_ptr<T> m_ptrInstance;		//实例对象
		//static ZCS m_cs;							//临界区		

	public:
		ZSingleton() {}
		virtual ~ZSingleton(){}

		//获取实例
		static T *GetInstance()
		{
			if (NULL == m_ptrInstance.get())
			{
				//m_cs.Enter();
				if (NULL == m_ptrInstance.get())
				{
					 m_ptrInstance = std::auto_ptr<T>(new T());  //智能指针管理该实例
				}
				//m_cs.Leave();
			}

			return m_ptrInstance.get();
		}
	};

	//实例句柄
	template<typename T>
	std::auto_ptr<T> ZSingleton<T>::m_ptrInstance;

	////临界区
	//template<typename T>
	//ZCS ZSingleton<T>::m_cs;


	/*
		比较规范的单例模式的模板实现：

		template<typename T>
		class Singleton
		{
			public:
			inline static T& GetInstance()
			{
				static T s_instance;
				return s_instance;
			}

			private:
			Singleton(){}
			Singleton(Singleton const&): Singleton(){}
			Singleton& operator= (Singleton const&){return *this;}
			~Singleton(){}
		};

		//使用示例：
		#include "ZSingleton.h"
		class Example
		{

		};
		typedef Singleton<Example> SglExample;
		#define SGL_EXAMPLE SglExample::GetInstance()
	
		//该实现的优缺点：
		优点：逻辑上符合了单例模式的设计
		缺点：基本没有什么拓展性，比如 往模板添加成员 将难以维护
	*/


	/* 单例模式的实现：二、宏定义 */

	//基类有默认构造函数的情况可使用
#define SINGLETON_0(_CLASS_)								\
public:														\
	inline static _CLASS_* GetInstance()					\
	{														\
		static _CLASS_ s_instance;							\
		return &s_instance;									\
	}														\
private:													\
	_CLASS_();												\
	_CLASS_(_CLASS_ const&){}					\
	_CLASS_& operator= (_CLASS_ const&) { return *this; }	\
	~_CLASS_();

	//基类有1个没有默认构造函数的情况可使用
#define SINGLETON_1(_CLASS_, _BASE_CLASS_, PARAMETER)						\
public:																		\
	inline static _CLASS_* GetInstance()									\
	{																		\
		static _CLASS_ s_instance;											\
		return &s_instance;													\
	}																		\
private:																	\
	_CLASS_();																\
	_CLASS_(_CLASS_ const&) : _BASE_CLASS_(PARAMETER) {}					\
	_CLASS_& operator= (_CLASS_ const&) { return *this; }					\
	~_CLASS_();

	//基类有2个没有默认构造函数的情况可使用
#define SINGLETON_2(_CLASS_, _BASE_CLASS_1, PARAMETER_1,_BASE_CLASS_2, PARAMETER_2)					\
public:																								\
	inline static _CLASS_* GetInstance()															\
	{																								\
		static _CLASS_ s_instance;																	\
		return &s_instance;																			\
	}																								\
private:																							\
	_CLASS_();																						\
	_CLASS_(_CLASS_ const&) :_BASE_CLASS_1(PARAMETER_1),_BASE_CLASS_2(PARAMETER_2) {}				\
	_CLASS_& operator= (_CLASS_ const&) { return *this; }											\
	~_CLASS_();


	//单例类的构造与析构函数（如果构造函数和析构函数不想自己实现，可以直接在函数外定义该宏）  
#define SINGLETON_C_D_0(_CLASS_)			\
	_CLASS_::_CLASS_() {}					\
	_CLASS_::~_CLASS_() {}    

#define SINGLETON_C_D_1(_CLASS_, _BASE_CLASS_, PARAMETER)			\
	_CLASS_::_CLASS_():_BASE_CLASS_(PARAMETER) {}					\
	_CLASS_::~_CLASS_() {}    

#define SINGLETON_C_D_3(_CLASS_, _BASE_CLASS_1, PARAMETER_1,_BASE_CLASS_2, PARAMETER_2)			\
	_CLASS_::_CLASS_():_BASE_CLASS_1(PARAMETER_1),_BASE_CLASS_2(PARAMETER_2) {}					\
	_CLASS_::~_CLASS_() {}    

	/*

	//使用示例：
	#include "ZSingleton.h"  
  
	class Example  
	{  
		SINGLETON_0(Example);  
	};  
	#define SGL_EXAMPLE Example::GetInstance() 

	//.cpp 文件中使用
	SINGLETON_C_D_0(Example)


	//该实现的优缺点：
	优点：逻辑上符合了单例模式的设计，且具有很好的拓展性
	缺点：实现确实有点丑

	*/

