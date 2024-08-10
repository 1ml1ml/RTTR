#pragma once

#include "RTTRInfo/RTTRInfo.h"

namespace RTTR
{
	/// <summary>
	/// �Ա�����������Ϣ�б��Ƿ���ͬ
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
	/// ���þ�̬����
	/// </summary>
	/// <typeparam name="F">��������</typeparam>
	/// <typeparam name="...Args">������</typeparam>
	/// <param name="name">����</param>
	/// <param name="method">������</param>
	/// <param name="...args">����</param>
	/// <returns></returns>
	template<typename F, typename... Args>
	static auto invokeStaticMethod(const std::string& name, const std::string& method, Args&&... args) -> std::invoke_result_t<F>
	{
		auto methodInfos{ RTTRInfo::info(name)->staticMethod(method) };
		return std::find_if(methodInfos.begin(), methodInfos.end(), ArgsEqual<Args...>{})->invoke<F>(std::forward<Args>(args)...);
	}

	/// <summary>
	/// ������ȡ���Ա������������ģ��
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
	/// ������ͨ����
	/// </summary>
	/// <typeparam name="F">��������</typeparam>
	/// <typeparam name="...Args">������</typeparam>
	/// <param name="o">�����</param>
	/// <param name="name">����</param>
	/// <param name="method">������</param>
	/// <param name="...args">����</param>
	/// <returns></returns>
	template<typename F, typename... Args, typename Traits = member_method_traits<F>>
	static auto invokeNormalMethod(void* o, const std::string& name, const std::string& method, Args... args) -> std::invoke_result_t<F, typename Traits::class_t>
	{
		static_assert(!Traits::is_const::value, "Method is const");

		auto methodInfos{ RTTRInfo::info(name)->normalMethod(method) };
		return std::find_if(methodInfos.begin(), methodInfos.end(), ArgsEqual<Args...>{})->invoke<F>(reinterpret_cast<Traits::class_t*>(o), std::forward<Args>(args)...);
	}

	/// <summary>
	/// ����const����
	/// </summary>
	/// <typeparam name="F">��������</typeparam>
	/// <typeparam name="...Args">������</typeparam>
	/// <param name="o">�����</param>
	/// <param name="name">����</param>
	/// <param name="method">������</param>
	/// <param name="...args">����</param>
	/// <returns></returns>
	template<typename F, typename... Args, typename Traits = member_method_traits<F>>
	static auto invokeConstMethod(const void* o, const std::string& name, const std::string& method, Args... args) -> std::invoke_result_t<F, typename Traits::class_t>
	{
		static_assert(Traits::is_const::value, "Method is not const");

		auto methodInfos{ RTTRInfo::info(name)->constMethod(method) };
		return std::find_if(methodInfos.begin(), methodInfos.end(), ArgsEqual<Args...>{})->invoke<F>(reinterpret_cast<Traits::class_t*>(o), std::forward<Args>(args)...);
	}

	/// <summary>
	/// ��ȡ��̬��Ա��ֵ
	/// </summary>
	/// <typeparam name="M">��Ա����</typeparam>
	/// <param name="name">����</param>
	/// <param name="member">��Ա��</param>
	/// <returns></returns>
	template<typename M>
	static M valueStaticMember(const std::string& name, const std::string& member)
	{
		return RTTRInfo::info(name)->staticMember(member)->value<M>();
	}

	/// <summary>
	/// ��ȡ��ͨ��Ա��ֵ
	/// </summary>
	/// <typeparam name="M">��Ա����</typeparam>
	/// <param name="o">�����</param>
	/// <param name="name">����</param>
	/// <param name="member">��Ա��</param>
	/// <returns></returns>
	template<typename M>
	static M valueNormalMember(void* o, const std::string& name, const std::string& member)
	{
		return RTTRInfo::info(name)->normalMember(member)->value<M>(o);
	}
}

