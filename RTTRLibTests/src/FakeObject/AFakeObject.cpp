#include <string>

#include "RTTR.h"
#include "AFakeObject.h"

RTTR_REGISTER(std::string)

class AFakeObject
{
public:
	static inline std::string s_string{ "AFakeObject::s_string" };

public:
	static std::string staticFunc();

public:
	AFakeObject();

public:
	std::string normalFunc();
	std::string constFunc() const;

public:
	std::string m_string{ "AFakeObject::m_string" };
};
RTTR_REGISTER(AFakeObject);

void registerAFakeObject()
{
	RTTR::RealRTTRInfo<AFakeObject>::instance();
	RTTR_REGISTER_STATIC_MEMBER(AFakeObject, RTTR::Public, s_string);
	RTTR_REGISTER_STATIC_METHOD(AFakeObject, RTTR::Public, staticFunc);
}

AFakeObject* createAFakeObject()
{
	auto object{ new AFakeObject{} };
	return object;
}

std::string AFakeObject::staticFunc()
{
	return "AFakeObject::staticFunc()";
}

AFakeObject::AFakeObject()
{
	RTTR_REGISTER_NORMAL_MEMBER(RTTR::Public, m_string);

	RTTR_REGISTER_NORMAL_METHOD(RTTR::Public, std::string, normalFunc);

	RTTR_REGISTER_CONST_METHOD(RTTR::Public, std::string, constFunc);
}

std::string AFakeObject::normalFunc()
{
	return "AFakeObject::normalFunc()";
}

std::string AFakeObject::constFunc() const
{
	return "AFakeObject::constFunc() const";
}
