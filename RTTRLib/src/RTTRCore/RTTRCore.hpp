#pragma once

#include <any>
#include <list>
#include <string>
#include <type_traits>

namespace RTTR
{
	class RTTRInfo;

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
}
