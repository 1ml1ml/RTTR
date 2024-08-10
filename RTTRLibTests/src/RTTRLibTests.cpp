#include "RTTR.hpp"
#include "CppUnitTest.h"

#include "FakeObject/AFakeObject.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RTTRLibTests
{
	TEST_MODULE_INITIALIZE(TestModuleInitialize)
	{
		registerAFakeObject();
	}

	TEST_CLASS(RTTRLibTests)
	{
	public:
		TEST_METHOD(AFakeObject_TypeInfo_info_notNull)
		{
			Assert::IsNotNull(RTTR::RTTRInfo::info("class AFakeObject"));
		}

		TEST_METHOD(AFakeObject_RTTR_valueStaticMember_AreEqual)
		{
			auto string{ RTTR::valueStaticMember<std::string>("class AFakeObject", "s_string") };
			Assert::AreEqual(string, std::string{ "AFakeObject::s_string" });
			Logger::WriteMessage(string.c_str());
		}

		TEST_METHOD(AFakeObject_RTTR_valueNormalMember_AreEqual)
		{
			auto object{ createAFakeObject() };
			auto string{ RTTR::valueNormalMember<std::string>(object, "class AFakeObject", "m_string") };
			Assert::AreEqual(string, std::string{ "AFakeObject::m_string" });
			Logger::WriteMessage(string.c_str());
		}

		TEST_METHOD(AFakeObject_RTTR_invokeStaticMethod_AreEqual)
		{
			auto string{ RTTR::invokeStaticMethod<std::string(*)()>("class AFakeObject", "staticFunc") };
			Assert::AreEqual(string, std::string{ "AFakeObject::staticFunc()" });
			Logger::WriteMessage(string.c_str());
		}

		TEST_METHOD(AFakeObject_RTTR_invokeNormalMethod_AreEqual)
		{
			auto object{ createAFakeObject() };
			auto string{ RTTR::invokeNormalMethod<std::string(AFakeObject::*)()>(object, "class AFakeObject", "normalFunc") };
			Assert::AreEqual(string, std::string{ "AFakeObject::normalFunc()" });
			Logger::WriteMessage(string.c_str());
		}

		TEST_METHOD(AFakeObject_RTTR_invokeConstMethod_AreEqual)
		{
			auto object{ createAFakeObject() };
			auto string{ RTTR::invokeConstMethod<std::string(AFakeObject::*)() const>(object, "class AFakeObject", "constFunc") };
			Assert::AreEqual(string, std::string{ "AFakeObject::constFunc() const" });
			Logger::WriteMessage(string.c_str());
		}
	};
}
