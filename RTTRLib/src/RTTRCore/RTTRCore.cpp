#include <assert.h>

#include "RTTRCore.hpp"

RTTR::Superclass::Superclass(Interview interview, RTTRInfo* info) :
	interview(interview), info(info)
{
	assert(interview && info);
}

RTTR::MemberInfo::MemberInfo(const std::string& name, Interview interview, RTTRInfo* info) :
	name(name), interview(interview), info(info)
{
	assert(!name.empty() && interview && info);
}

RTTR::StaticMemberInfo::StaticMemberInfo(const std::string& name, Interview interview, RTTRInfo* info, const void* address) : MemberInfo(name, interview, info),
	address(address)
{
	assert(address);
}

RTTR::NormalMemberInfo::NormalMemberInfo(const std::string& name, Interview interview, RTTRInfo* info, int offset) : MemberInfo(name, interview, info),
	offset(offset)
{
}
