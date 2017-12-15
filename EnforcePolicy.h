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

template <class _Access, class _Object>
struct STATIC_DERIVE;

template <class _Object>
struct STATIC_DERIVE<ACC_PUB, _Object> : public SD_UTIL<_Object>
{
};

template <class _Object>
struct STATIC_DERIVE<ACC_PRO, _Object> : protected SD_UTIL<_Object>
{
};

template <class _Object>
struct STATIC_DERIVE<ACC_PRI, _Object> : private SD_UTIL<_Object>
{
};

/**
 ********************* POLICY FUNCTIONS
 */

/**
 * Policy enforcer: enforce that PolicyClass conforms to the Policy
 * TODO add policy enforcer that expects template class HostClass & class... Policies
 */
template<template <class _PolicyClass> class _Policy, class _PolicyClass>
class PolicyEnforcer : public _PolicyClass
{
private:
	template <typename __PolicyWithClass>
	struct Check_Policy
	{
		constexpr Check_Policy()
		{
			static_assert((__PolicyWithClass(), true), "Type assertion failed");
			static_assert(sizeof(__PolicyWithClass) == 1, "Unexpected _Policy class size... please avoid using data members and virtual functions");
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
 * RefProtected Grants _Accessor the access to reference protected data members and functions of _Object
 */
template <class _Object, class _Accessor>
struct RefProtected final : STATIC_DERIVE<ACC_PRO, _Object>
{
	friend _Accessor;
};

/**
 * TODO POC enfore that function is constexpr with Macro
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

struct __CONST {};

// TODO consider making This class (Check) the base class for all policies
struct Rule
{
	using ASSERTION_IS_TRUE = bool;
	static constexpr ASSERTION_IS_TRUE _TRUE = static_cast<ASSERTION_IS_TRUE>(1);

	// General verison for static functions
	template <typename _Signature, typename _RetType, typename... _ARGS>
	static constexpr ASSERTION_IS_TRUE IsStatic(_RetType (*fp)(_ARGS...))
	{
		static_assert(std::is_same<_Signature, decltype(fp)>::value, "Not a valid static function");
		return _TRUE;
	}

	// General version for member functions
	template <typename _Signature, typename _RetType, typename... _ARGS, class _Class>
	static constexpr ASSERTION_IS_TRUE IsMember(_RetType (_Class::*fp)(_ARGS...))
	{
		static_assert(std::is_same<_Signature, decltype(fp)>::value, "Not a valid member function");
		return _TRUE;
	}

	// General version for const member functions
	template <typename _Signature, typename _RetType, typename... _ARGS, class _Class>
	static constexpr ASSERTION_IS_TRUE IsConstMember(_RetType (_Class::*fp)(_ARGS...) const)
	{
		static_assert(std::is_same<_Signature, decltype(fp)>::value, "Not a valid const member function");
		return _TRUE;
	}

	// General verison for static functions
	template <typename _Signature, typename _RetType, typename... _ARGS>
	static constexpr ASSERTION_IS_TRUE AnyFunction(_RetType (*fp)(_ARGS...))
	{
		static_assert(std::is_same<_Signature, decltype(fp)>::value, "Not a valid static function");
		return _TRUE;
	}

	// General version for member functions
	template <typename _Signature, typename _RetType, typename... _ARGS, class _Class>
	static constexpr ASSERTION_IS_TRUE AnyFunction(_RetType (_Class::*fp)(_ARGS...))
	{
		static_assert(std::is_same<_Signature, decltype(fp)>::value, "Not a valid member function");
		return _TRUE;
	}

	// General version for const member functions
	template <typename _Signature, typename _RetType, typename... _ARGS, class _Class>
	static constexpr ASSERTION_IS_TRUE AnyFunction(_RetType (_Class::*fp)(_ARGS...) const)
	{
		static_assert(std::is_same<_Signature, decltype(fp)>::value, "Not a valid const member function");
		return _TRUE;
	}

	template <typename _RetType, typename... _ARGS>
	static constexpr ASSERTION_IS_TRUE StaticFunc(RetVal<_RetType>, Args<_ARGS...>, _RetType (*fp)(_ARGS...))
	{
		return _TRUE;
	}

	template <typename _RetType, typename... _ARGS, typename _PolicyClass>
	static constexpr ASSERTION_IS_TRUE MemberFunc(	RetVal<_RetType>,
													_RetType (_PolicyClass::*funcPtr)(_ARGS...),
													Args<_ARGS...>)
	{
		return _TRUE;
	}

	template <class CONSTNESS, typename _RetType, typename... _ARGS, typename _PolicyClass>
	static constexpr ASSERTION_IS_TRUE MemberFunc(	RetVal<_RetType>,
													_RetType (_PolicyClass::*funcPtr)(_ARGS...) const,
													Args<_ARGS...>)
	{
		return _TRUE;
	}


	template <typename _RetType, typename... _ARGS, typename _PolicyClass>
	static constexpr ASSERTION_IS_TRUE NonConstMemberFunc(	RetVal<_RetType>,
															_RetType (_PolicyClass::*funcPtr)(_ARGS...),
															Args<_ARGS...>)
	{
		return _TRUE;
	}


	template <typename _RetType, typename... _ARGS, typename _PolicyClass>
	static constexpr ASSERTION_IS_TRUE ConstMemberFunc(	RetVal<_RetType>,
														_RetType (_PolicyClass::*funcPtr)(_ARGS...) const,
														Args<_ARGS...>)
	{
		return _TRUE;
	}
};

#define SET_RULE(_RULE) \
		static_assert(((_RULE), true), "Policy Rule failed: " #_RULE)

#endif /* ENFORCEPOLICY_H_ */
