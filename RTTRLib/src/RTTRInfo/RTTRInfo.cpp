#include <algorithm>
#include <unordered_map>

#include "RTTRInfo.h"

namespace RTTR
{
	class RTTRInfoImpl
	{
	public:
		struct AddressEqual
		{
		public:
			explicit AddressEqual(const void* address) : address(address) {}

		public:
			template<typename T>
			bool operator ()(const T& value) { return value.address == address; }

		private:
			const void* address;
		};

	public:
		static inline std::unordered_map<std::string, RTTRInfo*> s_infos{};

	public:
		RTTRInfoImpl() = default;

	public:
		std::list<Superclass> superclass;										//父类

		std::unordered_map<std::string, StaticMemberInfo> staticMembers{};		//静态成员变量
		std::unordered_map<std::string, NormalMemberInfo> normalMembers{};		//普通成员变量

		std::unordered_multimap<std::string, StaticMethodInfo> staticMethods{};	//静态方法
		std::unordered_multimap<std::string, NormalMethodInfo> normalMethods{};	//普通方法
		std::unordered_multimap<std::string, ConstMethodInfo> constMethods{};	//const方法
	};
}

RTTR::RTTRInfo* RTTR::RTTRInfo::info(const std::string& name)
{
	auto find{ RTTRInfoImpl::s_infos.find(name) };
	return find != RTTRInfoImpl::s_infos.end() ? find->second : nullptr;
}

bool RTTR::RTTRInfo::registerRTTRInfo(RTTRInfo* info)
{
	assert(info != nullptr);

	if (nullptr == RTTRInfo::info(info->name()))
	{
		RTTRInfoImpl::s_infos.insert({ info->name(), info });
		return true;
	}
	return false;
}

RTTR::RTTRInfo::RTTRInfo() :
	m_impl(new RTTRInfoImpl{})
{
}

RTTR::RTTRInfo::~RTTRInfo() = default;

void RTTR::RTTRInfo::registerSuperclass(const Superclass& info)
{
	if (false == superclass(info.info->name()).has_value()) m_impl->superclass.push_back(info);
}

std::list<std::string> RTTR::RTTRInfo::superclassNames() const
{
	std::list<std::string> names;
	for (const auto& val : m_impl->superclass) names.push_back(val.info->name());
	return names;
}

std::optional<RTTR::Superclass> RTTR::RTTRInfo::superclass(const std::string& name) const
{
	auto find{ std::find_if(m_impl->superclass.begin(), m_impl->superclass.end(), [&name](const Superclass& val) { return val.info->name() == name; }) };
	return find != m_impl->superclass.end() ? std::optional{ *find } : std::nullopt;
}

bool RTTR::RTTRInfo::registerStaticMember(const StaticMemberInfo& info)
{
	if (false == staticMember(info.name).has_value())
	{
		m_impl->staticMembers.insert({ info.name, info });
		return true;
	}
	return false;
}

std::list<std::string> RTTR::RTTRInfo::staticMemberNames() const
{
	std::list<std::string> names;
	for (const auto& [name, info] : m_impl->staticMembers) names.push_back(name);
	return names;
}

std::optional<RTTR::StaticMemberInfo> RTTR::RTTRInfo::staticMember(const std::string& name) const
{
	auto find{ m_impl->staticMembers.find(name) };
	return find != m_impl->staticMembers.end() ? std::optional{ find->second } : std::nullopt;
}

bool RTTR::RTTRInfo::registerNormalMember(const NormalMemberInfo& info)
{
	if (false == normalMember(info.name).has_value())
	{
		m_impl->normalMembers.insert({ info.name, info });
		return true;
	}
	return false;
}

std::list<std::string> RTTR::RTTRInfo::normalMemberNames() const
{
	std::list<std::string> names;
	for (const auto& [name, info] : m_impl->normalMembers) names.push_back(name);
	return names;
}

std::optional<RTTR::NormalMemberInfo> RTTR::RTTRInfo::normalMember(const std::string& name) const
{
	auto find{ m_impl->normalMembers.find(name) };
	return find != m_impl->normalMembers.end() ? std::optional{ find->second } : std::nullopt;
}

bool RTTR::RTTRInfo::registerStaticMethod(const StaticMethodInfo& info)
{
	if (auto methods{ staticMethod(info.name) }; std::find_if(methods.begin(), methods.end(), RTTRInfoImpl::AddressEqual{ info.address }) == methods.end())
	{
		m_impl->staticMethods.insert({ info.name, info });
		return true;
	}
	return false;
}

std::unordered_set<std::string> RTTR::RTTRInfo::staticMethodNames() const
{
	std::unordered_set<std::string> names;
	for (const auto& [name, info] : m_impl->staticMethods) names.insert(name);
	return names;
}

std::list<RTTR::StaticMethodInfo> RTTR::RTTRInfo::staticMethod(const std::string& name) const
{
	std::list<RTTR::StaticMethodInfo> methods;
	auto pair{ m_impl->staticMethods.equal_range(name) };
	while (pair.first != pair.second) methods.push_back(pair.first++->second);
	return methods;
}

bool RTTR::RTTRInfo::registerNormalMethod(const NormalMethodInfo& info)
{
	if (auto methods{ normalMethod(info.name) }; std::find_if(methods.begin(), methods.end(), RTTRInfoImpl::AddressEqual{ info.address }) == methods.end())
	{
		m_impl->normalMethods.insert({ info.name, info });
		return true;
	}
	return false;
}

std::unordered_set<std::string> RTTR::RTTRInfo::normalMethodNames() const
{
	std::unordered_set<std::string> names{};
	for (const auto& [name, info] : m_impl->normalMethods) names.insert(name);
	return names;
}

std::list<RTTR::NormalMethodInfo> RTTR::RTTRInfo::normalMethod(const std::string& name) const
{
	std::list<RTTR::NormalMethodInfo> methods;
	auto pair{ m_impl->normalMethods.equal_range(name) };
	while (pair.first != pair.second) methods.push_back(pair.first++->second);
	return methods;
}

bool RTTR::RTTRInfo::registerConstMethod(const ConstMethodInfo& info)
{
	if (auto methods{ constMethod(info.name) }; std::find_if(methods.begin(), methods.end(), RTTRInfoImpl::AddressEqual{ info.address }) == methods.end())
	{
		m_impl->constMethods.insert({ info.name, info });
		return true;
	}
	return false;
}

std::unordered_set<std::string> RTTR::RTTRInfo::constMethodNames() const
{
	std::unordered_set<std::string> names{};
	for (const auto& [name, info] : m_impl->constMethods) names.insert(name);
	return names;
}

std::list<RTTR::ConstMethodInfo> RTTR::RTTRInfo::constMethod(const std::string& name) const
{
	std::list<RTTR::ConstMethodInfo> methods;
	auto pair{ m_impl->constMethods.equal_range(name) };
	while (pair.first != pair.second) methods.push_back(pair.first++->second);
	return methods;
}
