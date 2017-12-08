/*
 * EnforcePolicy.h
 *
 *  Created on: Nov 29, 2017
 *      Author: menash
 */

#ifndef ENFORCEPOLICY_H_
#define ENFORCEPOLICY_H_

// TODO with Enforce(), is_base_of is redundant - type_traits are not needed anymore
#include <type_traits>

// TODO move to library, required for c++0x
template <typename T>
struct CallAssert
{
	constexpr CallAssert() { static_assert(T::Assert(), "Type assertion failed"); }
};

// TODO move to library, generally useful and required for safe unprotect
template<typename T>
struct STATIC_DERIVE : public T
{
	// Derive without the ability to create an instance - simply get access to protected functions
	STATIC_DERIVE() = delete;
};

// TODO	Find a way to ease policy syntax with inheritance from library template class.
//		the following magic tricks might be handy.

/**
 * Policy tree deriver: decide whether or not to derive from the policy itself (default = true)
 */
template<template <class PolicyClass> class Policy, class PolicyClass, bool isBaseOf>
class PolicyTreeDeriver : public PolicyClass
{
};

/**
 * Policy tree deriver: in case the policyClass does not derive from the policy - derive both
 */
template<template <class PolicyClass> class Policy, class PolicyClass>
class PolicyTreeDeriver<Policy, PolicyClass, false> : public PolicyClass, public Policy<PolicyClass>
{
};

/**
 * Policy enforcer: enforce that PolicyClass conforms to the Policy
 *
 * TODO Think of an alternative syntax that would enforce the policy without deriving from it
 * That should leave non finger prints - no compatibility issues
 * Example below...
 * The original with-derivition code is commented out.
 * This version requires unprotect
 */
template<template <class PolicyClass> class Policy, class PolicyClass>
class PolicyEnforcer : public PolicyClass
/*public PolicyTreeDeriver< 	Policy,
													PolicyClass,
													std::is_base_of< Policy<PolicyClass>, PolicyClass >::value >
													*/
{
	template <typename T>
	struct CheckPolicy
	{
		// Static-derive from policy to reach its protected members
		struct Checker : STATIC_DERIVE< Policy<PolicyClass> >
		{
			static constexpr bool Check()
			{
				static_assert(T::Enforce(), "Type assertion failed");
				static_assert(sizeof(Policy<PolicyClass>) == 1, "Unexpected Policy class size... please avoid using data members and virtual functions");

				return true;
			}
		};

		constexpr CheckPolicy()
		{
			static_assert(Checker::Check(), "Bad policy!");
		}
	};

	CheckPolicy< Policy<PolicyClass> > a;
};

/**
 * Policy duplex enforcer: Like normal enforcer, but also enforces that the policy class derives from the given policy.
 */
template<template <class PolicyClass> class Policy, class PolicyClass>
class DuplexPolicyEnforcer : public PolicyClass
{
	static_assert(std::is_base_of<Policy<PolicyClass>, PolicyClass>::value, "Policy class must derive from Policy.");
};

#endif /* ENFORCEPOLICY_H_ */
