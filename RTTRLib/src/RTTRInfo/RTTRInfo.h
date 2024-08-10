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
}

