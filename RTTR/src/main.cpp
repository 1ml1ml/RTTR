#include <iostream>

#include "RTTR/RTTR.hpp"

RTTR_REGISTER(int)
RTTR_REGISTER(void)
RTTR_REGISTER(std::string)

struct ExampleSuperclass
{
public:
	static inline std::string s_i{ "s_i" };

public:
	static void func1(int)
	{
		std::cout << "func1_int" << std::endl;
	}

	static void func1(int, int)
	{
		std::cout << "func1_int_in" << std::endl;
	}

public:
	ExampleSuperclass();

public:
	virtual void func2(int)
	{
		std::cout << "func2_int" << std::endl;
	}

	void func2(int, int)
	{
		std::cout << "func2_int_int" << std::endl;
	}

public:
	std::string m_i{ "m_i" };
};
RTTR_REGISTER(ExampleSuperclass)

struct Example : public ExampleSuperclass
{
public:
	Example();

public:
	void func2(int) override
	{
		std::cout << "override func2_int" << std::endl;
	}
};
RTTR_REGISTER(Example)

int main()
{
	ExampleSuperclass es{};

	auto esT{ RTTR::TypeInfo::info("struct ExampleSuperclass") };
	std::cout << esT->name() << " " << esT->size() << " " << std::endl;

	std::cout << esT->staticMember("s_i")->value<std::string>() << std::endl;
	std::cout << esT->normalMember("m_i")->value<ExampleSuperclass, std::string>(&es) << std::endl;

	esT->staticMethod("func1").front().invoke<void(*)(int)>(0);
	esT->staticMethod("func1").back().invoke<void(*)(int, int)>(0, 0);

	esT->normalMethod("func2").front().invoke<void(ExampleSuperclass::*)(int)>(&es, 0);
	esT->normalMethod("func2").back().invoke<void(ExampleSuperclass::*)(int, int)>(&es, 0, 0);

	Example e{};

	auto eT{ RTTR::TypeInfo::info("struct Example") };
	eT->normalMethod("func2").front().invoke<void(Example::*)(int)>(&e, 0);

	esT = eT->superclass("struct ExampleSuperclass").value().info;
	std::cout << esT->name() << " " << esT->size() << " " << std::endl;

	RTTR::invokeStaticMethod<ExampleSuperclass, void(*)(int)>("func1", 0);
	RTTR::invokeNormalMethod<ExampleSuperclass, void(ExampleSuperclass::*)(int)>("func2", &es, 0);
	RTTR::invokeNormalMethod<Example, void(Example::*)(int)>("func2", &e, 0);

	std::cout << RTTR::valueStaticMember<ExampleSuperclass, std::string>("s_i") << std::endl;
	std::cout << RTTR::valueNormalMember<ExampleSuperclass, std::string>("m_i", &es) << std::endl;

	return 0;
}

ExampleSuperclass::ExampleSuperclass()
{
	RTTR_REGISTER_STATIC_MEMBER(RTTR::Public, s_i);
	RTTR_REGISTER_NORMAL_MEMBER(RTTR::Public, m_i);

	RTTR_REGISTER_STATIC_METHOD(RTTR::Public, void, func1, int);
	RTTR_REGISTER_STATIC_METHOD(RTTR::Public, void, func1, int, int);

	RTTR_REGISTER_NORMAL_METHOD(RTTR::Public, void, func2, int);
	RTTR_REGISTER_NORMAL_METHOD(RTTR::Public, void, func2, int, int);
}

Example::Example() : ExampleSuperclass()
{
	RRTR_REGISTER_SUPERCLASS(RTTR::Public, ExampleSuperclass);

	RTTR_REGISTER_NORMAL_METHOD(RTTR::Public, void, func2, int);
}