#pragma once

#include <assert.h>

#include <memory>
#include <optional>
#include <unordered_set>

#include "RTTRCore/RTTRCore.hpp"

namespace RTTR
{
	class RTTRInfoImpl;
	
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
}

