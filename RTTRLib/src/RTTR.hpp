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
	/// ������Ϣ
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
	/// ��Ա��Ϣ
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
	/// ��̬��Ա��Ϣ
	/// </summary>
	struct StaticMemberInfo : public MemberInfo
	{
	public:
		StaticMemberInfo(const std::string& name, Interview interview, RTTRInfo* info, const void* address);

	public:
		/// <summary>
		/// ��ȡ��̬��Ա��ֵ
		/// </summary>
		/// <typeparam name="M"></typeparam>
		/// <returns></returns>
		template<typename M>
		M value() const { return *reinterpret_cast<M*>(const_cast<void*>(address)); }

	public:
		const void* const address{};
	};

	/// <summary>
	/// ��ͨ��Ա��Ϣ
	/// </summary>
	struct NormalMemberInfo : public MemberInfo
	{
	public:
		NormalMemberInfo(const std::string& name, Interview interview, RTTRInfo* info, int offset);

	public:
		/// <summary>
		/// ��ȡ��ͨ��Ա��ֵ
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
	/// ������Ϣ
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
		/// ���÷���
		/// </summary>
		/// <typeparam name="F">��������</typeparam>
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
	/// ��̬������Ϣ
	/// </summary>
	struct StaticMethodInfo : public MethodInfo 
	{
	public:
		using MethodInfo::MethodInfo;
	};

	/// <summary>
	/// ��ͨ������Ϣ
	/// </summary>
	struct NormalMethodInfo : public MethodInfo 
	{
	public:
		using MethodInfo::MethodInfo;
	};

	/// <summary>
	/// const������Ϣ
	/// </summary>
	struct ConstMethodInfo : public MethodInfo 
	{
	public:
		using MethodInfo::MethodInfo;
	};

	/// <summary>
	/// ������Ϣ
	/// </summary>
	class RTTRInfo
	{
	public:
		/// <summary>
		/// ��ȡһ��������Ϣ
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		static RTTRInfo* info(const std::string& name);

	protected:
		/// <summary>
		/// ע��һ��������Ϣ
		/// </summary>
		/// <param name="type"></param>
		/// <returns></returns>
		static bool registerRTTRInfo(RTTRInfo* info);

	public:
		RTTRInfo();
		virtual ~RTTRInfo();

	public:
		/// <summary>
		/// ��ȡ��������
		/// </summary>
		/// <returns></returns>
		virtual std::string name() const = 0;

		/// <summary>
		/// ��ȡ���ʹ�С
		/// </summary>
		/// <returns></returns>
		virtual size_t size() const = 0;

	public:
		/// <summary>
		/// ע�Ḹ����Ϣ
		/// </summary>
		/// <param name="info"></param>
		void registerSuperclass(const Superclass& info);

		/// <summary>
		/// ��ȡ��������
		/// </summary>
		/// <returns></returns>
		std::list<std::string> superclassNames() const;

		/// <summary>
		/// ��ȡ������Ϣ
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		std::optional<Superclass> superclass(const std::string& name) const;

		/// <summary>
		/// ע�ᾲ̬��Ա��Ϣ
		/// </summary>
		/// <param name="info"></param>
		/// <returns></returns>
		bool registerStaticMember(const StaticMemberInfo& info);

		/// <summary>
		/// ��ȡ���о�̬��Ա��
		/// </summary>
		/// <returns></returns>
		std::list<std::string> staticMemberNames() const;

		/// <summary>
		/// ��ȡ��̬��Ա��Ϣ
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		std::optional<StaticMemberInfo> staticMember(const std::string& name) const;

		/// <summary>
		/// ע����ͨ��Ա��Ϣ
		/// </summary>
		/// <param name="info"></param>
		/// <returns></returns>
		bool registerNormalMember(const NormalMemberInfo& info);

		/// <summary>
		/// ��ȡ������ͨ��Ա��
		/// </summary>
		/// <returns></returns>
		std::list<std::string> normalMemberNames() const;

		/// <summary>
		/// ��ȡ��ͨ��Ա��Ϣ
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		std::optional<NormalMemberInfo> normalMember(const std::string& name) const;

		/// <summary>
		/// ע�ᾲ̬����
		/// </summary>
		/// <param name="info"></param>
		/// <returns></returns>
		bool registerStaticMethod(const StaticMethodInfo& info);

		/// <summary>
		/// ��ȡ���о�̬��������
		/// </summary>
		/// <returns></returns>
		std::unordered_set<std::string> staticMethodNames() const;

		/// <summary>
		/// ��ȡ��̬������Ϣ
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		std::list<StaticMethodInfo> staticMethod(const std::string& name) const;

		/// <summary>
		/// ע����ͨ������Ϣ
		/// </summary>
		/// <param name="info"></param>
		/// <returns></returns>
		bool registerNormalMethod(const NormalMethodInfo& info);

		/// <summary>
		/// ��ȡ������ͨ��������
		/// </summary>
		/// <returns></returns>
		std::unordered_set<std::string> normalMethodNames() const;

		/// <summary>
		/// ��ȡ��ͨ������Ϣ
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		std::list<NormalMethodInfo> normalMethod(const std::string& name) const;

		/// <summary>
		/// ע��const������Ϣ
		/// </summary>
		/// <param name="info"></param>
		/// <returns></returns>
		bool registerConstMethod(const ConstMethodInfo& info);

		/// <summary>
		/// ��ȡ����const��������
		/// </summary>
		/// <returns></returns>
		std::unordered_set<std::string> constMethodNames() const;

		/// <summary>
		/// ��ȡconst������Ϣ
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
	/// �����������ȡ������Ϣ
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <returns></returns>
	template<typename... Args>
	static std::list<RTTRInfo*> unpackArgsInfo() { return { RealRTTRInfo<Args>::instance()... }; }

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

#include "RTTRRegister.hpp"
