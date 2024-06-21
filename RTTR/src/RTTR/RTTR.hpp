#pragma once

#include <list>
#include <string>
#include <memory>
#include <optional>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace RTTR
{
	class TypeInfo;

	enum Interview : unsigned char
	{
		None = 0,
		Public,
		Protected,
		Private,
	};

	/// <summary>
	/// 超类信息
	/// </summary>
	struct SuperclassInfo
	{
		Interview interview{ None };
		TypeInfo* info{ nullptr };
	};

	/// <summary>
	/// 成员信息
	/// </summary>
	struct MemberInfo
	{
	public:
		std::string name{};
		Interview interview{ None };
		TypeInfo* info{ nullptr };
	};

	/// <summary>
	/// 静态成员信息
	/// </summary>
	struct StaticMemberInfo : public MemberInfo
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
	struct NormalMemberInfo : public MemberInfo
	{
	public:
		/// <summary>
		/// 获取普通成员的值
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="M"></typeparam>
		/// <param name="o"></param>
		/// <returns></returns>
		template<typename T, typename M>
		M value(T* o) { return *reinterpret_cast<M*>((reinterpret_cast<unsigned char*>(o) + offset)); }

	public:
		int offset{ 0 };
	};

	/// <summary>
	/// 方法信息
	/// </summary>
	struct MethodInfo
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
	struct StaticMethodInfo : public MethodInfo { };

	struct NormalMethodInfo : public MethodInfo { };

	class TypeInfoImpl;
	class TypeInfo
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

	/// <summary>
	/// 调用指定静态方法
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="F"></typeparam>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="name"></param>
	/// <param name="...args"></param>
	/// <returns></returns>
	template<typename T, typename F, typename... Args>
	static decltype(auto) invokeStaticMethod(const std::string& name, Args... args)
	{
		std::list<TypeInfo*> argList{ RTTR::unpackArgs<Args...>() };

		auto infos{ RealTypeInfo<T>::instance()->staticMethod(name) };
		if (auto info{ std::find_if(infos.begin(), infos.end(), [&argList](const auto& val) { return val.args == argList; }) }; info != infos.end())
			return info->invoke<F>(args...);
	}

	/// <summary>
	/// 调用指定普通方法
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="F"></typeparam>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="name"></param>
	/// <param name="o"></param>
	/// <param name="...args"></param>
	/// <returns></returns>
	template<typename T, typename F, typename... Args>
	static decltype(auto) invokeNormalMethod(const std::string& name, T* o, Args... args)
	{
		std::list<TypeInfo*> argList{ RTTR::unpackArgs<Args...>() };

		auto infos{ RealTypeInfo<T>::instance()->normalMethod(name) };
		if (auto info{ std::find_if(infos.begin(), infos.end(), [&argList](const auto& val) { return val.args == argList; }) }; info != infos.end())
			return info->invoke<F>(o, args...);
	}

	/// <summary>
	/// 获取指定静态成员的值
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="M"></typeparam>
	/// <param name="name"></param>
	/// <returns></returns>
	template<typename T, typename M>
	static decltype(auto) valueStaticMember(const std::string& name)
	{
		if (auto info{ RealTypeInfo<T>::instance()->staticMember(name) }; info.has_value())
			return info->value<M>();
	}

	/// <summary>
	/// 获取指定普通成员的值
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="M"></typeparam>
	/// <param name="name"></param>
	/// <param name="o"></param>
	/// <returns></returns>
	template<typename T, typename M>
	static decltype(auto) valueNormalMember(const std::string& name, T* o)
	{
		if (auto info{ RealTypeInfo<T>::instance()->normalMember(name) }; info.has_value())
			return info->value<T, M>(o);
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
#define RRTR_REGISTER_SUPERCLASS(Interview, Superclass) \
do \
{ \
	using T = std::remove_reference_t<decltype(*this)>; \
	RTTR::RealTypeInfo<T>::instance()->registerSuperclass({ Interview, RTTR::RealTypeInfo<Superclass>::instance() }); \
} \
while (false)

//注册静态成员变量
#define RTTR_REGISTER_STATIC_MEMBER(Interview, Name) \
do \
{ \
	using T = std::remove_reference_t<decltype(*this)>; \
	RTTR::RealTypeInfo<T>::instance()->registerStaticMember({ #Name, Interview, RTTR::RealTypeInfo<decltype(T::Name)>::instance(), &T::Name }); \
} \
while (false)

//注册类普通成员变量
#define RTTR_REGISTER_NORMAL_MEMBER(Interview, Name) \
do \
{ \
	using T = std::remove_reference_t<decltype(*this)>; \
	auto offset{ &T::Name }; \
	RTTR::RealTypeInfo<T>::instance()->registerNormalMember({ #Name, Interview, RTTR::RealTypeInfo<decltype(T::Name)>::instance(), *(int*)(&offset) }); \
} \
while (false)

//注册静态方法
#define RTTR_REGISTER_STATIC_METHOD(Interview, R, Name, ...) \
do \
{ \
	using T = std::remove_reference_t<decltype(*this)>; \
	R (*address)(__VA_ARGS__){ T::Name }; \
	RTTR::RealTypeInfo<T>::instance()->registerStaticMethod({ #Name, Interview, RTTR::RealTypeInfo<R>::instance(), RTTR::unpackArgs<__VA_ARGS__>(), address }); \
} \
while (false)

//注册普通方法
#define RTTR_REGISTER_NORMAL_METHOD(Interview, R, Name, ...) \
do \
{ \
	using T = std::remove_reference_t<decltype(*this)>; \
	R (T::*address)(__VA_ARGS__){ &T::Name }; \
	RTTR::RealTypeInfo<T>::instance()->registerNormalMethod({ #Name, Interview, RTTR::RealTypeInfo<R>::instance(), RTTR::unpackArgs<__VA_ARGS__>(), *(void**)&address }); \
} \
while (false)
