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
#include "Inheritance.h"
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
	typedef void (_PolicyClass::*print_p1)(std::string const &) const;

	using Access = ReferProtected<_PolicyClass, POutputPolicy>;

	constexpr POutputPolicy()
	{
		// static_assert(std::is_base_of<POutputPolicy<_PolicyClass>, _PolicyClass>::value, "base");

		// TODO find a smoother solution
    	SET_RULE(Rule::ConstMemberFunc<Access, void, std::string const &>(&Access::print));
		SET_RULE(Rule::AnyFunction<print_p1>(&Access::print));
	}
};

class OutputPolicyWriteToCout
{
public:

    template<typename MessageType>
    void print(MessageType const &message) const
    {
        std::cout << message << std::endl;
    }

    std::string message() const
    {
    	return "fuck the system";
    }
};

class LanguagePolicyEnglish
{
public:
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
public:
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

	    using Access = ReferProtected<_PolicyClass, PLanguagePolicy>;

	    // different implementation options, chose yours...
	    // TODO change Enforce to return ALWAYS_TRUE (change entire API to ALWAYS_TRUE, and rename ALWAYS_TRUE better)
	    constexpr PLanguagePolicy()
	    {
	    	//SET_RULE(Rule::ConstMemberFunc<Access, std::string>(&Access::message));
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

	    using Access = ReferProtected<_PolicyClass, PLanguagePolicy2>;

	    constexpr PLanguagePolicy2()
	    {
	    	/**
	    	 * Rule Desc...
	    	 */
	    	// SET_RULE(Rule::Templated<std::string>::ConstMemberFunc(&Access::message));

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

// Bad idea to use parameter pack - cannot choose which function in case of collision between different policies
template <class... plcCls>
using HelloWorldPolicy = DeriveMaster<	PolicyClassList<plcCls...>,
										PolicyList<POutputPolicy, PLanguagePolicy> >;

template <class OutputPolicy, class LanguagePolicy>
struct HelloWorldBeautiful : HelloWorldPolicy<OutputPolicy, LanguagePolicy>
{
	//using OutputPolicy::print;
	//using LanguagePolicy::message;

	using HelloWorldPolicy<OutputPolicy, LanguagePolicy>::Get;

	void run() const
	{
		const OutputPolicy& p = Get(Args<OutputPolicy>());
		const LanguagePolicy& p2 = Get(Args<LanguagePolicy>());
		std::cout << "Works?!" << std::endl;
		p.print(p2.message());
		std::cout << "Damm!!!!" << std::endl;
	}
};

void func(Args<OutputPolicyWriteToCout, LanguagePolicyEnglish>)
{
	std::cout << "yay!" << std::endl;
}

struct S
{

};

/**
 * EDITED: 	Each paragraph in the following code section contains:
 * 			both original and wrapped version of HelloWorld Host class.
 */
#include <typeinfo>
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
    //DeriveMaster< 	ExtendingPolicyClassList<OutputPolicyWriteToCout, LanguagePolicyEnglish, LanguagePolicyEnglish>,
	//				PolicyList<POutputPolicy, PLanguagePolicy2, PLanguagePolicy> >();

    DeriveOnce<OutputPolicyWriteToCout, LanguagePolicyEnglish, LanguagePolicyEnglish, OutputPolicyWriteToCout, LanguagePolicyGerman, LanguagePolicyEnglish>();

    /**
     * TODO add explanation:
     * 		in the following scenario, there is no solution - virtual inheritance breaks the purpose of policies:
     *			Derived1 : Base
     *			Derived2 : Base
     *		This would make Base ambiguous, which would cause a compile error.
     *		This is equivalent to either:
     *			func(Base* purpose1, Base* purpose2)
     *			func(Derived1* purpose1, Derived2* purpose2)
     *		which would work perfectly with strategy, but badly with policies.
     * TODO POC: implementation suggestion to solve:
     *
     *
     */
    HelloWorldBeautiful<OutputPolicyWriteToCout, LanguagePolicyEnglish> hwb;
    hwb.run();

    Args<LanguagePolicyEnglish, OutputPolicyWriteToCout> x;
    Args<OutputPolicyWriteToCout, LanguagePolicyEnglish, LanguagePolicyEnglish, LanguagePolicyEnglish> x2;
    DeriveOnce<OutputPolicyWriteToCout, LanguagePolicyEnglish, LanguagePolicyEnglish, LanguagePolicyEnglish>::List y;
    DeriveOnce<LanguagePolicyEnglish, OutputPolicyWriteToCout, LanguagePolicyEnglish, OutputPolicyWriteToCout, LanguagePolicyEnglish, LanguagePolicyEnglish>::List y2;
    std::cout << typeid(decltype(x)).name() << std::endl;
    std::cout << decltype(x)::Length << std::endl;
    std::cout << typeid(decltype(x2)).name() << std::endl;
    std::cout << decltype(x2)::Length << std::endl;
    std::cout << typeid(decltype(y)).name() << std::endl;
    std::cout << decltype(y)::Length << std::endl;
    std::cout << typeid(decltype(y2)).name() << std::endl;
    std::cout << decltype(y2)::Length << std::endl;

    std::cout << std::endl;

    std::cout << typeid(
    		decltype(
    				Args<>::AddUnique(x, Args<OutputPolicyWriteToCout>())
					)
			).name() << std::endl;

    std::cout << typeid(
    		decltype(
    				Args<>::AddUnique(x, Args<LanguagePolicyGerman>())
					)
			).name() << std::endl;

    std::cout << Args<>::IsIn(Args<int, char>(), Args<int>()) << std::endl;
}



