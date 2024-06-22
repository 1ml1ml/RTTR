#pragma once

#include <any>
#include <list>
#include <string>
#include <memory>
#include <iostream>
#include <optional>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#ifdef RTTR_PROJECT
	#define RTTR_API __declspec(dllexport)
#else
	#define RTTR_API __declspec(dllimport)
#endif

namespace RTTR
{
	class TypeInfo;

	enum RTTR_API Interview : unsigned char
	{
		None = 0,
		Public,
		Protected,
		Private,
	};

	/// <summary>
	/// 超类信息
	/// </summary>
	struct RTTR_API SuperclassInfo
	{
		Interview interview{ None };
		TypeInfo* info{ nullptr };
	};

	/// <summary>
	/// 成员信息
	/// </summary>
	struct RTTR_API MemberInfo
	{
	public:
		std::string name{};
		Interview interview{ None };
		TypeInfo* info{ nullptr };
	};

	/// <summary>
	/// 静态成员信息
	/// </summary>
	struct RTTR_API StaticMemberInfo : public MemberInfo
	{
	public:
		/// <summary>
		/// 获取静态成员的值
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns></returns>
		template<typename T>
		T value() { return *reinterpret_cast<T*>(address); }

	public:
		void* address{ nullptr };
	};

	/// <summary>
	/// 普通成员信息
	/// </summary>
	struct RTTR_API NormalMemberInfo : public MemberInfo
	{
	public:
		/// <summary>
		/// 获取普通成员的值
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="M"></typeparam>
		/// <param name="o"></param>
		/// <returns></returns>
		template<typename M, typename T>
		M value(T* o) { return *reinterpret_cast<M*>((reinterpret_cast<unsigned char*>(o) + offset)); }

	public:
		int offset{ 0 };
	};

	/// <summary>
	/// 方法信息
	/// </summary>
	struct RTTR_API MethodInfo
	{
	public:
		/// <summary>
		/// 调用方法
		/// </summary>
		/// <typeparam name="F">函数类型</typeparam>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="...args"></param>
		/// <returns></returns>
		template<typename F, typename... Args>
		decltype(auto) invoke(Args&&... args) { return std::invoke(*(F*)&address, std::forward<Args>(args)...); }

	public:
		std::string name{};
		Interview interview{ None };
		TypeInfo* returnInfo{ nullptr };
		std::list<TypeInfo*> args{};
		void* address{ nullptr };
	};

	/// <summary>
	/// 静态方法信息
	/// </summary>
	struct RTTR_API StaticMethodInfo : public MethodInfo { };

	struct RTTR_API NormalMethodInfo : public MethodInfo { };

	struct RTTR_API ConstMethodInfo : public MethodInfo { };

	class TypeInfoImpl;
	class RTTR_API TypeInfo
	{
	public:
		/// <summary>
		/// 获取一个类型信息
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		static TypeInfo* info(const std::string& name);

	protected:
		/// <summary>
		/// 注册一种类型信息
		/// </summary>
		/// <param name="type"></param>
		/// <returns></returns>
		static bool registerTypeInfo(TypeInfo* info);

	protected:
		/// <summary>
		/// 获取类型大小 如果是void则返回0
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns></returns>
		template<typename T>
		static size_t Sizeof()
		{
			if constexpr (std::is_void_v<T>) return 0;
			else return sizeof(T);
		}

	public:
		TypeInfo();
		virtual ~TypeInfo();

	public:
		/// <summary>
		/// 获取类型名称
		/// </summary>
		/// <returns></returns>
		virtual std::string name() const = 0;

		/// <summary>
		/// 获取类型大小
		/// </summary>
		/// <returns></returns>
		virtual size_t size() const = 0;

		/// <summary>
		/// 获取std类型的type_info
		/// </summary>
		/// <returns></returns>
		virtual const std::type_info& std_type_info() const = 0;

	public:
		/// <summary>
		/// 注册父类信息
		/// </summary>
		/// <param name="info"></param>
		void registerSuperclass(const SuperclassInfo& info);

		/// <summary>
		/// 获取父类名称
		/// </summary>
		/// <returns></returns>
		std::list<std::string> superclassNames() const;

		/// <summary>
		/// 获取父类信息
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		std::optional<SuperclassInfo> superclass(const std::string& name) const;

		/// <summary>
		/// 注册静态成员信息
		/// </summary>
		/// <param name="info"></param>
		/// <returns></returns>
		bool registerStaticMember(const StaticMemberInfo& info);

		/// <summary>
		/// 获取所有静态成员名
		/// </summary>
		/// <returns></returns>
		std::list<std::string> staticMemberNames() const;

		/// <summary>
		/// 获取静态成员信息
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		std::optional<StaticMemberInfo> staticMember(const std::string& name) const;

		/// <summary>
		/// 注册普通成员信息
		/// </summary>
		/// <param name="info"></param>
		/// <returns></returns>
		bool registerNormalMember(const NormalMemberInfo& info);

		/// <summary>
		/// 获取所有普通成员名
		/// </summary>
		/// <returns></returns>
		std::list<std::string> normalMemberNames() const;

		/// <summary>
		/// 获取普通成员信息
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		std::optional<NormalMemberInfo> normalMember(const std::string& name) const;

		/// <summary>
		/// 注册静态方法
		/// </summary>
		/// <param name="info"></param>
		/// <returns></returns>
		bool registerStaticMethod(const StaticMethodInfo& info);

		/// <summary>
		/// 获取所有静态方法名称
		/// </summary>
		/// <returns></returns>
		std::unordered_set<std::string> staticMethodNames() const;

		/// <summary>
		/// 获取静态方法信息
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		std::list<StaticMethodInfo> staticMethod(const std::string& name) const;

		/// <summary>
		/// 注册普通方法信息
		/// </summary>
		/// <param name="info"></param>
		/// <returns></returns>
		bool registerNormalMethod(const NormalMethodInfo& info);

		/// <summary>
		/// 获取所有普通方法名称
		/// </summary>
		/// <returns></returns>
		std::unordered_set<std::string> normalMethodNames() const;

		/// <summary>
		/// 获取普通方法信息
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		std::list<NormalMethodInfo> normalMethod(const std::string& name) const;

		/// <summary>
		/// 注册const方法信息
		/// </summary>
		/// <param name="info"></param>
		/// <returns></returns>
		bool registerConstMethod(const ConstMethodInfo& info);

		/// <summary>
		/// 获取所有const方法名称
		/// </summary>
		/// <returns></returns>
		std::unordered_set<std::string> constMethodNames() const;

		/// <summary>
		/// 获取const方法信息
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		std::list<ConstMethodInfo> constMethod(const std::string& name) const;

	private:
		std::unique_ptr<TypeInfoImpl> m_impl{};
	};

	template<typename T>
	class RealTypeInfo : public TypeInfo { };

	/// <summary>
	/// 解析模板类型包返回对应的Type对象
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <returns></returns>
	template<typename... Args>
	static std::list<TypeInfo*> unpackArgs()
	{
		std::list<TypeInfo*> args;
		(args.push_back(RealTypeInfo<Args>::instance()), ...);
		return args;
	}

	template<typename F, typename... Args>
	static decltype(auto) invokeStaticMethod(const std::string& name, const std::string& method, Args... args)
	{
		if (auto info{ TypeInfo::info(name) }; info != nullptr) 
		{
			if (auto methodInfos{ TypeInfo::info(name)->staticMethod(method) }; false == methodInfos.empty())
			{
				std::list<TypeInfo*> argList{ RTTR::unpackArgs<Args...>() };
				if (auto methodInfo{ std::find_if(methodInfos.begin(), methodInfos.end(), [&argList](const StaticMethodInfo& val) { return val.args == argList; }) }; methodInfo != methodInfos.end())
					return methodInfo->invoke<F>(args...);
			}
		}
	}

	/// <summary>
	/// 用于提取类成员函数类类型的类模板
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T>
	struct member_method_traits {};

	template<typename R, typename C, typename... Args>
	struct member_method_traits<R(C::*)(Args...)> 
	{ 
		using class_t = C; 
		using is_const = std::false_type;
	};

	template<typename R, typename C, typename... Args>
	struct member_method_traits<R(C::*)(Args...) const>
	{
		using class_t = C;
		using is_const = std::true_type;
	};

	template<typename F, typename... Args>
	static decltype(auto) invokeNormalMethod(const std::string& name, const std::string& method, void* o, Args... args)
	{
		using Traits = member_method_traits<F>;
		static_assert(!Traits::is_const::value, "Method is const");

		if (auto info{ TypeInfo::info(name) }; info != nullptr)
		{
			if (auto methodInfos{ TypeInfo::info(name)->normalMethod(method) }; false == methodInfos.empty())
			{
				std::list<TypeInfo*> argList{ RTTR::unpackArgs<Args...>() };
				if (auto methodInfo{ std::find_if(methodInfos.begin(), methodInfos.end(), [&argList](const auto& val) { return val.args == argList; }) }; methodInfo != methodInfos.end())
					return methodInfo->invoke<F>(reinterpret_cast<Traits::class_t*>(o), args...);
			}
		}
	}

	template<typename F, typename... Args>
	static decltype(auto) invokeConstMethod(const std::string& name, const std::string& method, const void* o, Args... args)
	{
		using Traits = member_method_traits<F>;
		static_assert(Traits::is_const::value, "Method is not const");

		if (auto info{ TypeInfo::info(name) }; info != nullptr)
		{
			if (auto methodInfos{ TypeInfo::info(name)->constMethod(method) }; false == methodInfos.empty())
			{
				std::list<TypeInfo*> argList{ RTTR::unpackArgs<Args...>() };
				if (auto methodInfo{ std::find_if(methodInfos.begin(), methodInfos.end(), [&argList](const auto& val) { return val.args == argList; }) }; methodInfo != methodInfos.end())
					return methodInfo->invoke<F>(reinterpret_cast<const Traits::class_t*>(o), args...);
			}
		}
	}

	template<typename M>
	static decltype(auto) valueStaticMember(const std::string& name, const std::string& member)
	{
		if (auto info{ TypeInfo::info(name) }; info != nullptr)
		{
			if (auto memberInfo{ info->staticMember(member) }; memberInfo.has_value())
				return memberInfo->value<M>();
		}
	}

	template<typename M, typename T>
	static decltype(auto) valueNormalMember(const std::string& name, const std::string& member, T* o)
	{
		if (auto info{ TypeInfo::info(name) }; info != nullptr)
		{
			if (auto memberInfo{ info->normalMember(member) }; memberInfo.has_value())
				return memberInfo->value<M>(o);
		}
	}
}

//注册类型
#define RTTR_REGISTER(T) \
namespace RTTR \
{ \
	template<> \
	class RealTypeInfo<T> final : public TypeInfo \
	{ \
	public: \
		using Value = T; \
\
	public: \
		static RealTypeInfo<T>* instance() \
		{ \
			static RealTypeInfo<T> instance{}; \
			return &instance; \
		} \
\
	public: \
		std::string name() const override { return std_type_info().name(); } \
		size_t size() const override { return Sizeof<T>(); } \
		const std::type_info& std_type_info() const override { return typeid(T); } \
\
	private: \
		RealTypeInfo<T>() : TypeInfo() { registerTypeInfo(this); } \
		~RealTypeInfo<T>() = default; \
	}; \
}

//注册父类
#define RRTR_REGISTER_SUPERCLASS(T, Interview, Superclass) \
do \
{ \
	RTTR::RealTypeInfo<T>::instance()->registerSuperclass({ Interview, RTTR::RealTypeInfo<Superclass>::instance() }); \
} \
while (false)

//注册静态成员变量
#define RTTR_REGISTER_STATIC_MEMBER(T, Interview, Name) \
do \
{ \
	RTTR::RealTypeInfo<T>::instance()->registerStaticMember({ #Name, Interview, RTTR::RealTypeInfo<decltype(T::Name)>::instance(), &T::Name }); \
} \
while (false)

//注册类普通成员变量
#define RTTR_REGISTER_NORMAL_MEMBER(T, Interview, Name) \
do \
{ \
	auto offset{ &T::Name }; \
	RTTR::RealTypeInfo<T>::instance()->registerNormalMember({ #Name, Interview, RTTR::RealTypeInfo<decltype(T::Name)>::instance(), *(int*)(&offset) }); \
} \
while (false)

//注册静态方法
#define RTTR_REGISTER_STATIC_METHOD(T, Interview, R, Name, ...) \
do \
{ \
	R (*address)(__VA_ARGS__){ T::Name }; \
	RTTR::RealTypeInfo<T>::instance()->registerStaticMethod({ #Name, Interview, RTTR::RealTypeInfo<R>::instance(), RTTR::unpackArgs<__VA_ARGS__>(), address }); \
} \
while (false)

//注册普通方法
#define RTTR_REGISTER_NORMAL_METHOD(T, Interview, R, Name, ...) \
do \
{ \
	R (T::*address)(__VA_ARGS__){ &T::Name }; \
	RTTR::RealTypeInfo<T>::instance()->registerNormalMethod({ #Name, Interview, RTTR::RealTypeInfo<R>::instance(), RTTR::unpackArgs<__VA_ARGS__>(), *(void**)&address }); \
} \
while (false)

//注册const方法
#define RTTR_REGISTER_CONST_METHOD(T, Interview, R, Name, ...) \
do \
{ \
	R (T::*address)(__VA_ARGS__) const { &T::Name }; \
	RTTR::RealTypeInfo<T>::instance()->registerConstMethod({ #Name, Interview, RTTR::RealTypeInfo<R>::instance(), RTTR::unpackArgs<__VA_ARGS__>(), *(void**)&address }); \
} \
while (false)
