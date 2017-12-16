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
struct PolicyEnforcer : public _PolicyClass
{
	constexpr PolicyEnforcer()
	{
		static_assert((_Policy<_PolicyClass>(), true), "Type assertion failed");
		static_assert(sizeof(_Policy<_PolicyClass>) == 1, "Unexpected _Policy class size... please avoid using data members and virtual functions in policies.");
	}
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

template <typename... ARGS>
struct Args
{
       constexpr Args() = default;
};

template <template <class> class... _Policies>
struct PolicyList
{
       constexpr PolicyList() = default;
};

template <class... _Policies>
struct PolicyClassList
{
       constexpr PolicyClassList() = default;
};

template <typename _Type>
struct RetVal
{
       constexpr RetVal() = default;
};

template < template <class> class _Policy, class _PolicyClass>
struct PolicyUnit
{
       constexpr PolicyUnit() = default;
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

	// Temp fallback for template functions - cannot deduct with the methods above
	template <typename _Signature>
	static constexpr ASSERTION_IS_TRUE AnyFunction(_Signature)
	{
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

	// TODO change interface (function last)

	template <typename _RetType, typename... _ARGS, typename _PolicyClass>
	static constexpr ASSERTION_IS_TRUE ConstMemberFunc(	RetVal<_RetType>,
														_RetType (_PolicyClass::*funcPtr)(_ARGS...) const,
														Args<_ARGS...>)
	{
		return _TRUE;
	}
};

#define SET_RULE(_RULE) \
		static_assert(((_RULE), true), "Policy Rule failed.")

/**
 * TODO solve yarden maymon's problem with Split<T, pol1, pol2> in a way that allows using Class A{x(); y();} for 2 different X(), Y() policies
 * 		First try to solve assuming you expect the splitted one
 * 		Then try to make a general<...> that would solve that internally
 *		Idea: implmenet Unify<Args...> that would output unique classes (no duplicates)
 */


template <	class _FirstCls,
			class... _RestCls,
			template <class> class _FirstPlc,
			template <class> class... _RestPlc
			>
static constexpr bool Match(PolicyClassList<_FirstCls, _RestCls...>, PolicyList<_FirstPlc, _RestPlc...>)
{
	static_assert(sizeof...(_RestCls) == sizeof...(_RestPlc), "Number of policies does not match number of policy-classes.");
	return 	Match(PolicyClassList<_FirstCls>(), PolicyList<_FirstPlc>()) &&
			Match(PolicyClassList<_RestCls...>(), PolicyList<_RestPlc...>());

}

// TODO unify API (which type is first)

template <class _Cls, template <class> class _Plc>
static constexpr bool Match(PolicyClassList<_Cls>, PolicyList<_Plc>)
{
	static_assert((PolicyEnforcer<_Plc, _Cls>(), true), "Mismatch between policy and policy-class");
	return true;
}

template <class _First, class... Rest>
struct DriveOnce : virtual _First, DriveOnce<Rest...> {};

template <class _Last>
struct DriveOnce<_Last> : virtual _Last{};

template <class... _Policies>
struct ExtendingPolicyClassList : PolicyClassList<_Policies...>, DriveOnce<_Policies...>
{
       constexpr ExtendingPolicyClassList() = default;
};

// TODO check whether EnforcePolicy would work too in case using virtual inheritance
template <class _plcClsList, class _plcList>
struct DeriveMaster;

template <class... _plcClsList, template <class> class... _plcList>
struct DeriveMaster<ExtendingPolicyClassList<_plcClsList...>, PolicyList<_plcList...>> : ExtendingPolicyClassList<_plcClsList...> // TODO add class that expects PolicyClassList and derives from its members. consider using function call + decltype (GetPolicyClasses that returns DeriveOnce<args...>)
{
	constexpr DeriveMaster()
	{
		static_assert(Match(PolicyClassList<_plcClsList...>(), PolicyList<_plcList...>()), "Full match");
	}
};


#endif /* ENFORCEPOLICY_H_ */
