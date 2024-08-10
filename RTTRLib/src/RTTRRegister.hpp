#pragma once

#include "RTTR.hpp"

//注册类型
#define RTTR_REGISTER(T) \
namespace RTTR \
{ \
	template<> \
	class RealRTTRInfo<T> final : public RTTRInfo \
	{ \
	public: \
		static RealRTTRInfo<T>* instance() \
		{ \
			static RealRTTRInfo<T> s_instance{}; \
			return &s_instance; \
		} \
\
	public: \
		std::string name() const override { return typeid(T).name(); } \
		size_t size() const override { if constexpr (std::is_void_v<T>) return 0; else return sizeof(T); } \
\
	private: \
		RealRTTRInfo<T>() : RTTRInfo() { registerRTTRInfo(this); } \
		~RealRTTRInfo<T>() = default; \
	}; \
}

//注册父类
#define RRTR_REGISTER_SUPERCLASS(Interview, Superclass) \
do \
{ \
	using T = decltype(*this); \
	RTTR::RealRTTRInfo<T>::instance()->registerSuperclass({ Interview, RTTR::RealRTTRInfo<Superclass>::instance() }); \
} \
while (false)

//注册静态成员变量
#define RTTR_REGISTER_STATIC_MEMBER(T, Interview, Name) \
do \
{ \
	RTTR::RealRTTRInfo<T>::instance()->registerStaticMember({ #Name, Interview, RTTR::RealRTTRInfo<decltype(T::Name)>::instance(), &T::Name }); \
} \
while (false)

//注册类普通成员变量
#define RTTR_REGISTER_NORMAL_MEMBER(Interview, Name) \
do \
{ \
	using T = std::remove_const_t<std::remove_reference_t<decltype(*this)>>; \
	auto offset{ &T::Name }; \
	RTTR::RealRTTRInfo<T>::instance()->registerNormalMember({ #Name, Interview, RTTR::RealRTTRInfo<decltype(T::Name)>::instance(), *(int*)(&offset) }); \
} \
while (false)

//注册静态方法
#define RTTR_REGISTER_STATIC_METHOD(T, Interview, Name, ...) \
do \
{ \
	using R = std::invoke_result_t<decltype(T::Name), __VA_ARGS__>; \
	R (*address)(__VA_ARGS__){ T::Name }; \
	RTTR::RealRTTRInfo<T>::instance()->registerStaticMethod({ #Name, Interview, RTTR::RealRTTRInfo<R>::instance(), address, RTTR::unpackArgsInfo<__VA_ARGS__>() }); \
} \
while (false)

//注册普通方法
#define RTTR_REGISTER_NORMAL_METHOD(Interview, R, Name, ...) \
do \
{ \
	using T = std::remove_const_t<std::remove_reference_t<decltype(*this)>>; \
	R (T::*address)(__VA_ARGS__){ &T::Name }; \
	RTTR::RealRTTRInfo<T>::instance()->registerNormalMethod({ #Name, Interview, RTTR::RealRTTRInfo<R>::instance(), address, RTTR::unpackArgsInfo<__VA_ARGS__>() }); \
} \
while (false)

//注册const方法
#define RTTR_REGISTER_CONST_METHOD(Interview, R, Name, ...) \
do \
{ \
	using T = std::remove_const_t<std::remove_reference_t<decltype(*this)>>; \
	R (T::*address)(__VA_ARGS__) const { &T::Name }; \
	RTTR::RealRTTRInfo<T>::instance()->registerConstMethod({ #Name, Interview, RTTR::RealRTTRInfo<R>::instance(), address, RTTR::unpackArgsInfo<__VA_ARGS__>() }); \
} \
while (false)
