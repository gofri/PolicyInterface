/*
 * EnforcePolicy.h
 *
 *  Created on: Nov 29, 2017
 *      Author: menash
 */

#ifndef ENFORCEPOLICY_H_
#define ENFORCEPOLICY_H_

struct ACC_PUB;
struct ACC_PRO;
struct ACC_PRI;

/**
 *********************** STATIC DERIVE FUNCTIONS
 */
// TODO move to library, generally useful and required for safe unprotect
template <class _Object>
struct SD_UTIL : public _Object
{
	// Derive without the ability to create an instance - simply get access to protected functions
	SD_UTIL() = delete;
};

template <class _Object, class _Access>
struct SD;

template <class _Object>
struct SD<_Object, ACC_PUB> : public SD_UTIL<_Object>
{
};

template <class _Object>
struct SD<_Object, ACC_PRO> : protected SD_UTIL<_Object>
{
};

template <class _Object>
struct SD<_Object, ACC_PRI> : private SD_UTIL<_Object>
{
};

template<typename T>
struct STATIC_DERIVE_PUBLIC : public T
{
};

template<typename T>
struct STATIC_DERIVE_PROTECTED : protected T
{
	// Derive without the ability to create an instance - simply get access to protected functions
	STATIC_DERIVE_PROTECTED() = delete;
};


template<typename T>
struct STATIC_DERIVE_PRIVATE : private T
{
	// Derive without the ability to create an instance - simply get access to protected functions
	STATIC_DERIVE_PRIVATE() = delete;
};

/**
 ********************* POLICY FUNCTIONS
 */

/**
 * Policy enforcer: enforce that PolicyClass conforms to the Policy
 */
template<template <class _PolicyClass> class _Policy, class _PolicyClass>
class PolicyEnforcer : public _PolicyClass
{
private:
	template <typename __PolicyWithClass>
	struct Check_Policy
	{
		// Static-derive from policy to reach its protected members
		struct Checker : STATIC_DERIVE_PUBLIC< _Policy<_PolicyClass> >
		{
			static constexpr bool Check()
			{
				static_assert(__PolicyWithClass::Enforce(), "Type assertion failed");
				static_assert(sizeof(_Policy<_PolicyClass>) == 1, "Unexpected _Policy class size... please avoid using data members and virtual functions");

				return true;
			}
		};

		constexpr Check_Policy()
		{
			static_assert(Checker::Check(), "Bad policy!");
		}
	};

	Check_Policy< _Policy<_PolicyClass> > TRIGGER_CTOR;
};

/**
 * Policy duplex enforcer: Like normal enforcer, but also enforces that the policy class derives from the given policy.
 */
template<template <class PolicyClass> class Policy, class PolicyClass>
class DuplexPolicyEnforcer : public PolicyEnforcer<Policy, PolicyClass>
{
	static_assert(std::is_base_of<Policy<PolicyClass>, PolicyClass>::value, "Policy-class must derive from Policy.");
};

/**
 * AccessProtected allows referencing
 */
template <class _Object, class _Accessor>
struct RefProtected final : protected _Object
{
	friend _Accessor;
};

/**
 * TODO POC enfore that function is constexpr
 * TODO add support for FORCE-INHERITANCE for policies
 */

/**
 ***************** Templat Simple API
 */

template <typename _Type>
struct RetVal
{
	constexpr RetVal() = default;
};

template <typename... ARGS>
struct Args
{
	constexpr Args() = default;
};

struct T_CONST {};
struct T_NON_CONST {};

struct Check
{
	struct ASSERTION_IS_TRUE {};

	template <typename _RetType, typename... _ARGS>
	static constexpr ASSERTION_IS_TRUE StaticFunc(RetVal<_RetType>, Args<_ARGS...>, _RetType (*fp)(_ARGS...))
	{
		return ASSERTION_IS_TRUE();
	}

	template <typename _RetType, typename... _ARGS, typename _PolicyClass>
	static constexpr ASSERTION_IS_TRUE NonConstMemberFunc(RetVal<_RetType>, _RetType (_PolicyClass::*funcPtr)(_ARGS...), Args<_ARGS...>)
	{
		return ASSERTION_IS_TRUE();
	}

	template <typename _RetType, typename... _ARGS, typename _PolicyClass>
	static constexpr ASSERTION_IS_TRUE ConstMemberFunc(RetVal<_RetType>, _RetType (_PolicyClass::*funcPtr)(_ARGS...) const, Args<_ARGS...>)
	{
		return ASSERTION_IS_TRUE();
	}

	template <typename T>
	static constexpr ASSERTION_IS_TRUE Func(T)
	{
		return ASSERTION_IS_TRUE();
	}


	// TODO consider moving "Is" into CTOR
	// TODO consider adding some typedef for easier non const
	// TODO consider making This class (Check) the base class for all policies
	template <typename T = T_NON_CONST>
	struct MemberFunc
	{
		template <typename _RetType, typename... _ARGS, typename _PolicyClass>
		static constexpr ASSERTION_IS_TRUE Is(	RetVal<_RetType>,
														_RetType (_PolicyClass::*funcPtr)(_ARGS...),
														Args<_ARGS...>)
		{
			static_assert(std::is_same<T, T_NON_CONST>::value, "Unexpected Constness type.");
			return ASSERTION_IS_TRUE();
		}
	};
};

template <>
struct Check::MemberFunc<T_CONST>
{
	template <typename _RetType, typename... _ARGS, typename _PolicyClass>
	static constexpr ASSERTION_IS_TRUE Is(	RetVal<_RetType>,
													_RetType (_PolicyClass::*funcPtr)(_ARGS...) const,
													Args<_ARGS...>)
	{
		return ASSERTION_IS_TRUE();
	}
};


#endif /* ENFORCEPOLICY_H_ */
