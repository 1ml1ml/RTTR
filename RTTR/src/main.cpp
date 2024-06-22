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
	static int func1(int n)
	{
		std::cout << "static_func1_int" << std::endl;
		return n;
	}

	static void func1(int, int)
	{
		std::cout << "static_func1_int_in" << std::endl;
	}

public:
	ExampleSuperclass();

public:
	virtual void func2(int)
	{
		std::cout << "virtual_func2_int" << std::endl;
	}

	void func2(int, int)
	{
		std::cout << "func2_int_int" << std::endl;
	}

	void func2(int, int) const
	{
		std::cout << "const_func2_int_int" << std::endl;
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
		std::cout << "override_func2_int" << std::endl;
	}
};
RTTR_REGISTER(Example)

int main()
{
	Example e{};
	ExampleSuperclass es{};

	std::cout << RTTR::invokeStaticMethod<int(*)(int)>("struct ExampleSuperclass", "func1", 0) << std::endl;
	RTTR::invokeNormalMethod<void(ExampleSuperclass::*)(int)>("struct ExampleSuperclass", "func2", &es, 0);
	RTTR::invokeNormalMethod<void(ExampleSuperclass::*)(int, int)>("struct ExampleSuperclass", "func2", &es, 0, 0);
	RTTR::invokeConstMethod<void(ExampleSuperclass::*)(int, int) const>("struct ExampleSuperclass", "func2", &es, 0, 0);

	std::cout << RTTR::valueStaticMember<std::string>("struct ExampleSuperclass", "s_i") << std::endl;
	std::cout << RTTR::valueNormalMember<std::string>("struct ExampleSuperclass", "m_i", &es) << std::endl;

	//调用重写函数
	ExampleSuperclass* eptr{ &e };
	RTTR::invokeNormalMethod<void(Example::*)(int)>("struct Example","func2", eptr, 0);

	return 0;
}

ExampleSuperclass::ExampleSuperclass()
{
	RTTR_REGISTER_STATIC_MEMBER(ExampleSuperclass, RTTR::Public, s_i);
	RTTR_REGISTER_NORMAL_MEMBER(ExampleSuperclass,RTTR::Public, m_i);

	RTTR_REGISTER_STATIC_METHOD(ExampleSuperclass,RTTR::Public, int, func1, int);
	RTTR_REGISTER_STATIC_METHOD(ExampleSuperclass,RTTR::Public, void, func1, int, int);

	RTTR_REGISTER_NORMAL_METHOD(ExampleSuperclass,RTTR::Public, void, func2, int);
	RTTR_REGISTER_NORMAL_METHOD(ExampleSuperclass,RTTR::Public, void, func2, int, int);
	RTTR_REGISTER_CONST_METHOD(ExampleSuperclass,RTTR::Public, void, func2, int, int);
}

Example::Example() : ExampleSuperclass()
{
	RRTR_REGISTER_SUPERCLASS(Example, RTTR::Public, ExampleSuperclass);

	RTTR_REGISTER_NORMAL_METHOD(Example, RTTR::Public, void, func2, int);
}