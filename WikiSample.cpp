/*
 * WikiSample.cpp
 *
 *  Created on: Nov 29, 2017
 *      Author: menash
 */

#include <iostream>
#include <string>

// *** EDITED: not from original source code. ***
#include "EnforcePolicy.h"
// *** END OF EDIT. ***

struct Fake { int x; };

template <typename OutputPolicy, typename LanguagePolicy>
class HelloWorld : private OutputPolicy, private LanguagePolicy
{
    using OutputPolicy::print;
    using LanguagePolicy::message;

public:

    // Behaviour method
    void run() const
    {
        // Two policy methods
        print(message());
    }
};

// *** EDITED: not from original source code. ***
template <class _PolicyClass>
struct POutputPolicy
{
	// TODO explain: no support for the nothrow guarantee by standard definition
	// TODO explain: White/Black-listing limitations due to language support - known issue
	// TODO explain: Template functions limitations due to language support

	typedef void (_PolicyClass::*print_p1)(std::string const &) const;

	using Access = RefProtected<_PolicyClass, POutputPolicy>;

	constexpr POutputPolicy(print_p1 = &Access::print)
	{
    	// SET_RULE(Rule::ConstMemberFunc(RetVal<void>(), &Access::print, Args<std::string const &>()));
		SET_RULE(Rule::AnyFunction<print_p1>(&Access::print));
	}
};

class OutputPolicyWriteToCout
{
protected:

    template<typename MessageType>
    void print(MessageType const &message) const
    {
        std::cout << message << std::endl;
    }
};

class LanguagePolicyEnglish
{
protected:
    std::string message() const
    {
        return "Hello, World!";
    }
    void nonConst()
	{
	}
};

class LanguagePolicyGerman
{
protected:
    std::string message() const
    {
        return "Hallo Welt!";
    }

    void nonConst()
	{
	}
};

template <class _PolicyClass>
class PLanguagePolicy
{
public:
		// Function description...
	    typedef std::string (_PolicyClass::*message_p)() const;

	    using Access = RefProtected<_PolicyClass, PLanguagePolicy>;

	    // different implementation options, chose yours...
	    // TODO change Enforce to return ALWAYS_TRUE (change entire API to ALWAYS_TRUE, and rename ALWAYS_TRUE better)
	    constexpr PLanguagePolicy()
	    {
	    	SET_RULE(Rule::ConstMemberFunc(RetVal<std::string>(), &Access::message, Args<>()));
	    	SET_RULE(Rule::MemberFunc<__CONST>(RetVal<std::string>(), &Access::message, Args<>()));
	    	SET_RULE(Rule::MemberFunc(RetVal<void>(), &Access::nonConst, Args<>()));
	    	SET_RULE(Rule::AnyFunction<message_p>(&Access::message));
	    }
};

template <class _PolicyClass>
class PLanguagePolicy2
{
public:
		// Function description...
	    typedef std::string (_PolicyClass::*message_p)() const;

	    using Access = RefProtected<_PolicyClass, PLanguagePolicy2>;

	    constexpr PLanguagePolicy2()
	    {
	    	/**
	    	 * Rule Desc...
	    	 */
	    	SET_RULE(Rule::ConstMemberFunc(RetVal<std::string>(), &Access::message, Args<>()));

	    	/**
	    	 * Rule Desc...
	    	 */
	    	SET_RULE(Rule::MemberFunc<__CONST>(RetVal<std::string>(), &Access::message, Args<>()));

	    	/**
	    	 * Rule Desc...
	    	 */
	    	SET_RULE(Rule::MemberFunc(RetVal<void>(), &Access::nonConst, Args<>()));

	    	/**
	    	 * Rule Desc...
	    	 */
	    	SET_RULE(Rule::AnyFunction<message_p>(&Access::message));
	    }
};

// TODO make it look better...
template <typename OutputPolicy, typename LanguagePolicy>
struct HelloWorld_SafeWrapped : public HelloWorld< PolicyEnforcer<POutputPolicy, OutputPolicy>, PolicyEnforcer<PLanguagePolicy2, LanguagePolicy> >
{
	HelloWorld_SafeWrapped() = default;

	HelloWorld_SafeWrapped(const HelloWorld<OutputPolicy, LanguagePolicy>& copyCtor) :
		HelloWorld< PolicyEnforcer<POutputPolicy, OutputPolicy>, PolicyEnforcer<PLanguagePolicy2, LanguagePolicy> >(copyCtor)
	{
	}

	operator const HelloWorld<OutputPolicy, LanguagePolicy>() const
	{
		return *static_cast<const HelloWorld<OutputPolicy, LanguagePolicy>*>(this);
	}

	operator HelloWorld<OutputPolicy, LanguagePolicy>()
	{
		return *static_cast<HelloWorld<OutputPolicy, LanguagePolicy>*>(this);;
	}
};
// Optional: force usage of HelloWorld_SafeWrapped by text replacement
// #define HelloWorld HelloWorld_SafeWrapped

template < template <class> class _Policy, class _PolicyClass>
constexpr PolicyUnit<_Policy, _PolicyClass> ToPair(const _Policy<_PolicyClass>&)
{
}

// An example of function that expects HelloWorld and does not need to be modified
template<class... A>
void functionThatExpects(const HelloWorld<A...>& h)
{
	h.run();
}
// *** END OF EDIT. ***

#include <typeinfo>
template <typename first, typename... rest>
static bool print(Args<first, rest...>)
{
	print(Args<first>());
	print(Args<rest...>());
	return true;
}
template <typename first>
static bool print(Args<first>)
{
	std::cout << typeid(first).name() << std::endl;
	return true;
}

template <	class _FirstCls,
			class... _RestCls,
			template <class> class _FirstPlc,
			template <class> class... _RestPlc
			>
static bool Match(PolicyClassList<_FirstCls, _RestCls...>, PolicyList<_FirstPlc, _RestPlc...>)
{
	static_assert(sizeof...(_RestCls) == sizeof...(_RestPlc), "Number of policies does not match number of policy-classes.");
	return 	Match(PolicyClassList<_FirstCls>(), PolicyList<_FirstPlc>()) &&
			Match(PolicyClassList<_RestCls...>(), PolicyList<_RestPlc...>());

}

// TODO unify API (which type is first)
template <class _Cls, template <class> class _Plc>
static bool Match(PolicyClassList<_Cls>, PolicyList<_Plc>)
{
	std::cout << typeid(_Cls).name() << " ";
	std::cout << typeid(_Plc<_Cls>).name() << std::endl;
	static_assert((PolicyEnforcer<_Plc, _Cls>(), true), "woohoo");
	return true;
}

// TODO use Match mechanism in conjunction with policies unifier

template <class _First, class... Rest>
struct DriveOnce : virtual _First, DriveOnce<Rest...> {};

template <class _Last>
struct DriveOnce<_Last> : virtual _Last{};

template <class _plcClsList, class _plcList>
struct DeriveMaster
{
	DeriveMaster()
	{
		Match(_plcClsList(), _plcList());
	}
};

/**
 * EDITED: 	Each paragraph in the following code section contains:
 * 			both original and wrapped version of HelloWorld Host class.
 */
int main()
{
    /* Example 1 */
    typedef HelloWorld<OutputPolicyWriteToCout, LanguagePolicyEnglish> HelloWorldEnglish;
    typedef HelloWorld_SafeWrapped<OutputPolicyWriteToCout, LanguagePolicyEnglish> HelloWorldEnglish_SafeWrapped;

    HelloWorldEnglish hello_world;
    hello_world.run(); // prints "Hello, World!"
    HelloWorldEnglish_SafeWrapped hello_world_SafeWrapper;
    functionThatExpects(hello_world); // prints "Hallo Welt!"

    /* Example 2
     * Does the same, but uses another language policy */
    typedef HelloWorld<OutputPolicyWriteToCout, LanguagePolicyGerman> HelloWorldGerman;
    typedef HelloWorld_SafeWrapped<OutputPolicyWriteToCout, LanguagePolicyGerman> HelloWorldGerman_SafeWrapped;

    HelloWorldGerman hello_world2;
    hello_world2.run(); // prints "Hallo Welt!"
    HelloWorldGerman_SafeWrapped hello_world2_SafeWrapped;
    functionThatExpects(hello_world2_SafeWrapped); // prints "Hallo Welt!"

    //std::cout << std::boolalpha;
    std::cout << std::endl;

    //std::cout << std::noboolalpha;

    Match(PolicyClassList<OutputPolicyWriteToCout, LanguagePolicyEnglish>(), PolicyList<POutputPolicy, PLanguagePolicy2>());
    // DeriveMaster< PolicyClassList<OutputPolicyWriteToCout>, PolicyList<POutputPolicy> >();
}


