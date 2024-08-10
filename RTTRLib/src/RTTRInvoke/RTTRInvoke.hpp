#pragma once

#include "RTTRInfo/RTTRInfo.h"

namespace RTTR
{
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

