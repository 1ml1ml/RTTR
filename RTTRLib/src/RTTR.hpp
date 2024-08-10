#pragma once

#include <assert.h>

#include <any>
#include <list>
#include <string>
#include <memory>
#include <optional>
#include <typeinfo>
#include <type_traits>
#include <unordered_set>

namespace RTTR
{
	class RTTRInfo;
	class RTTRInfoImpl;

	enum Interview : unsigned char
	{
		None = 0,
		Public,
		Protected,
		Private,
		Virtual = 0b1000,
	};

	/// <summary>
	/// 超类信息
	/// </summary>
	struct Superclass
	{
	public:
		Superclass(Interview interview, RTTRInfo* info);

	public:
		const Interview interview;
		RTTRInfo* const info;
	};

	/// <summary>
	/// 成员信息
	/// </summary>
	struct MemberInfo
	{
	public:
		MemberInfo(const std::string& name, Interview interview, RTTRInfo* info);

	public:
		const std::string name;
		const Interview interview;
		RTTRInfo* const info;
	};

	/// <summary>
	/// 静态成员信息
	/// </summary>
	struct StaticMemberInfo : public MemberInfo
	{
	public:
		StaticMemberInfo(const std::string& name, Interview interview, RTTRInfo* info, const void* address);

	public:
		/// <summary>
		/// 获取静态成员的值
		/// </summary>
		/// <typeparam name="M"></typeparam>
		/// <returns></returns>
		template<typename M>
		M value() const { return *reinterpret_cast<M*>(const_cast<void*>(address)); }

	public:
		const void* const address{};
	};

	/// <summary>
	/// 普通成员信息
	/// </summary>
	struct NormalMemberInfo : public MemberInfo
	{
	public:
		NormalMemberInfo(const std::string& name, Interview interview, RTTRInfo* info, int offset);

	public:
		/// <summary>
		/// 获取普通成员的值
		/// </summary>
		/// <typeparam name="M"></typeparam>
		/// <param name="o"></param>
		/// <returns></returns>
		template<typename M>
		M value(void* o) const { return *reinterpret_cast<M*>((reinterpret_cast<unsigned char*>(o) + offset)); }

	public:
		const int offset{ 0 };
	};

	/// <summary>
	/// 方法信息
	/// </summary>
	struct MethodInfo
	{
	public:
		template<typename F>
		MethodInfo(const std::string& name, Interview interview, RTTRInfo* returnInfo, F callable, const std::list<RTTRInfo*>& args) :
			name(name), interview(interview), returnInfo(returnInfo), callable(callable), args(args), address(*reinterpret_cast<void**>(&callable))
		{
			assert(!this->name.empty() && this->interview && this->returnInfo && this->callable.has_value() && this->address);
		}

	public:
		/// <summary>
		/// 调用方法
		/// </summary>
		/// <typeparam name="F">函数类型</typeparam>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="...args"></param>
		/// <returns></returns>
		template<typename F, typename... Args>
		decltype(auto) invoke(Args... args) const { return std::invoke(std::any_cast<F>(callable), std::forward<Args>(args)...); }

	public:
		const std::string name;
		const Interview interview;
		RTTRInfo* const returnInfo;
		const std::any callable;
		const std::list<RTTRInfo*> args;
		void* const address;
	};

	/// <summary>
	/// 静态方法信息
	/// </summary>
	struct StaticMethodInfo : public MethodInfo 
	{
	public:
		using MethodInfo::MethodInfo;
	};

	/// <summary>
	/// 普通方法信息
	/// </summary>
	struct NormalMethodInfo : public MethodInfo 
	{
	public:
		using MethodInfo::MethodInfo;
	};

	/// <summary>
	/// const方法信息
	/// </summary>
	struct ConstMethodInfo : public MethodInfo 
	{
	public:
		using MethodInfo::MethodInfo;
	};

	/// <summary>
	/// 类型信息
	/// </summary>
	class RTTRInfo
	{
	public:
		/// <summary>
		/// 获取一个类型信息
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		static RTTRInfo* info(const std::string& name);

	protected:
		/// <summary>
		/// 注册一种类型信息
		/// </summary>
		/// <param name="type"></param>
		/// <returns></returns>
		static bool registerRTTRInfo(RTTRInfo* info);

	public:
		RTTRInfo();
		virtual ~RTTRInfo();

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

	public:
		/// <summary>
		/// 注册父类信息
		/// </summary>
		/// <param name="info"></param>
		void registerSuperclass(const Superclass& info);

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
		std::optional<Superclass> superclass(const std::string& name) const;

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
		std::unique_ptr<RTTRInfoImpl> m_impl{};
	};

	template<typename T>
	class RealRTTRInfo : public RTTRInfo { };

	/// <summary>
	/// 对比两个参数信息列表是否相同
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	template<typename... Args>
	struct ArgsEqual
	{
	public:
		template<typename T>
		bool operator ()(const T& value) { return value.args == args; }

	private:
		std::list<RTTRInfo*> args{ unpackArgsInfo<Args...>() };
	};

	/// <summary>
	/// 解包参数包获取参数信息
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <returns></returns>
	template<typename... Args>
	static std::list<RTTRInfo*> unpackArgsInfo() { return { RealRTTRInfo<Args>::instance()... }; }

	/// <summary>
	/// 调用静态方法
	/// </summary>
	/// <typeparam name="F">函数类型</typeparam>
	/// <typeparam name="...Args">参数包</typeparam>
	/// <param name="name">类名</param>
	/// <param name="method">方法名</param>
	/// <param name="...args">参数</param>
	/// <returns></returns>
	template<typename F, typename... Args>
	static auto invokeStaticMethod(const std::string& name, const std::string& method, Args&&... args) -> std::invoke_result_t<F>
	{
		auto methodInfos{ RTTRInfo::info(name)->staticMethod(method) };
		return std::find_if(methodInfos.begin(), methodInfos.end(), ArgsEqual<Args...>{})->invoke<F>(std::forward<Args>(args)...);
	}

	/// <summary>
	/// 用于提取类成员函数特征的类模板
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
		using class_t = const C;
		using is_const = std::true_type;
	};

	/// <summary>
	/// 调用普通方法
	/// </summary>
	/// <typeparam name="F">函数类型</typeparam>
	/// <typeparam name="...Args">参数包</typeparam>
	/// <param name="o">类对象</param>
	/// <param name="name">类名</param>
	/// <param name="method">方法名</param>
	/// <param name="...args">参数</param>
	/// <returns></returns>
	template<typename F, typename... Args, typename Traits = member_method_traits<F>>
	static auto invokeNormalMethod(void* o, const std::string& name, const std::string& method, Args... args) -> std::invoke_result_t<F, typename Traits::class_t>
	{
		static_assert(!Traits::is_const::value, "Method is const");

		auto methodInfos{ RTTRInfo::info(name)->normalMethod(method) };
		return std::find_if(methodInfos.begin(), methodInfos.end(), ArgsEqual<Args...>{})->invoke<F>(reinterpret_cast<Traits::class_t*>(o), std::forward<Args>(args)...);
	}

	/// <summary>
	/// 调用const方法
	/// </summary>
	/// <typeparam name="F">函数类型</typeparam>
	/// <typeparam name="...Args">参数包</typeparam>
	/// <param name="o">类对象</param>
	/// <param name="name">类名</param>
	/// <param name="method">方法名</param>
	/// <param name="...args">参数</param>
	/// <returns></returns>
	template<typename F, typename... Args, typename Traits = member_method_traits<F>>
	static auto invokeConstMethod(const void* o, const std::string& name, const std::string& method, Args... args) -> std::invoke_result_t<F, typename Traits::class_t>
	{
		static_assert(Traits::is_const::value, "Method is not const");

		auto methodInfos{ RTTRInfo::info(name)->constMethod(method) };
		return std::find_if(methodInfos.begin(), methodInfos.end(), ArgsEqual<Args...>{})->invoke<F>(reinterpret_cast<Traits::class_t*>(o), std::forward<Args>(args)...);
	}

	/// <summary>
	/// 获取静态成员的值
	/// </summary>
	/// <typeparam name="M">成员类型</typeparam>
	/// <param name="name">类名</param>
	/// <param name="member">成员名</param>
	/// <returns></returns>
	template<typename M>
	static M valueStaticMember(const std::string& name, const std::string& member)
	{
		return RTTRInfo::info(name)->staticMember(member)->value<M>();
	}

	/// <summary>
	/// 获取普通成员的值
	/// </summary>
	/// <typeparam name="M">成员类型</typeparam>
	/// <param name="o">类对象</param>
	/// <param name="name">类名</param>
	/// <param name="member">成员名</param>
	/// <returns></returns>
	template<typename M>
	static M valueNormalMember(void* o, const std::string& name, const std::string& member)
	{
		return RTTRInfo::info(name)->normalMember(member)->value<M>(o);
	}
}

#include "RTTRRegister.hpp"
