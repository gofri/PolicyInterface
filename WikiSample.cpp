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
class POutputPolicy : STATIC_DERIVE_PUBLIC<_PolicyClass>
{
protected:

	/* Note:	Assuming you are willing to define an explicit interface,
	 * 			you must whitelist all versions or use the WeakPolicy (see below).
	 *
	 * Note:	You can either whitelist only one or be required to specify all allowed types in
	 *
	 * TODO find a smoother whitelisting method
	 */

	// Option #1: Policy for a single data type
	/*
	POutputPolicy(print_p1 = &_PolicyClass::print) {}
*/
	// Option #2 (commented out): Whitelist multiple types in policy, choose a single one in usage
	//		TODO POC
	// 		typedef void (_PolicyClass::*print_p2)(char);
	// 		POutputPolicy(print_p1 = &_PolicyClass::print<std::string const &>) {}
	// 		POutputPolicy(print_p2 = &_PolicyClass::print<char>) {}

	// Option #2.5 (commented out): Whitelist multiple types in policy, force to have all of them combined.
	//		TODO POC
	//		typedef void (_PolicyClass::*print_p1)(std::string const &);
	// 		typedef void (_PolicyClass::*print_p2)(char);
	// 		POutputPolicy(	print_p1 = &_PolicyClass::print<std::string const &>,
	//						print_p2 = &_PolicyClass::print<char>) {}

	// TODO explain: no support for the nothrow guarantee by standard definition

	// Template version - allow anything
	/*
	template <typename T>
	POutputPolicy(print_ps<T> = &_PolicyClass::print) {}
*/

		/**********************
		 * REQUIRED TEMPLATE SPECIFICATIONS LIST:
		// Each and every one of the following specifications are required
		static_assert(std::is_same<decltype(std::declval<_PolicyClass>().print(std::declval<std::string>())), void >::value, "valid sig");
		static_assert(std::is_same<decltype(std::declval<_PolicyClass>().print(std::declval<char>())), void >::value, "valid sig");

		static_assert(std::is_same<decltype(std::declval<_PolicyClass>().x(std::declval<int>())), void >::value, "valid sig");
		static_assert(std::is_same<decltype(std::declval<_PolicyClass>().x(std::declval<bool>())), void >::value, "valid sig");
		static_assert(std::is_same<decltype(std::declval<_PolicyClass>().x(std::declval<Fake>())), void >::value, "valid sig");
		*/

	// Option #3: Use the weak policy - only check whether a member function with the same name exist.
	// TODO add weak policy to lib
	struct WeakPolicy_t { };
	static constexpr const WeakPolicy_t WeakPolicy = {};

	// Function description...
	typedef void (_PolicyClass::*print_p1)(std::string const &) const;

    // different implementation options, chose yours...
    static constexpr bool Enforce(print_p1 = &POutputPolicy::print) {return true;}

	/* TODO Add Weak Enforcement support to PolicyEnforcer */
	constexpr POutputPolicy(const WeakPolicy_t& WeakPolicy)
	{
		static_assert(std::is_member_function_pointer<decltype(&_PolicyClass::print)>::value, "Assert name");
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
};

class LanguagePolicyGerman
{
protected:
    std::string message() const
    {
        return "Hallo Welt!";
    }
};

template<typename U, typename expr_t, expr_t Expr>
struct ClassName
{
private:
template<typename>
static constexpr std::false_type test(...);

template<typename T = U>
static decltype((Expr), std::true_type{}) test(int) ;

public:
static constexpr bool value = decltype(test<U>(0))::value;
};


// TODO try to create a whitelist but having combination of few of those:
template <typename _PC>
struct Whatever
{
	typedef void (_PC::*doThat_int)(int);
	typedef void (_PC::*doThat_char)(char);
	typedef void (_PC::*doThat_intptr)(int*);

	static constexpr bool func(doThat_int) { return true; }

	template <typename... T>
	static constexpr bool func(T...) { return false; }
};

template <typename _PC>
struct Whatever2
{
	typedef void (_PC::*doThat_char)(char);
	typedef void (_PC::*doThat_int)(int);

	static constexpr bool func(doThat_char) { return true; }

	template <typename... T>
	static constexpr bool func(T...) { return false; }
};

template <typename _PC>
struct Whatever3
{
	typedef void (_PC::*doThat_intptr)(int*);
	typedef void (_PC::*doThat_int)(int);

	static constexpr bool func(doThat_intptr) { return true; }

	template <typename... T>
	static constexpr bool func(T...) { return false; }
};

template <typename _PC>
struct Pol
{

	Pol()
	{
		printf("val: %d\n", Whatever<_PC>::func(&_PC::doThat));
		printf("val: %d\n", Whatever2<_PC>::func(&_PC::doThat));

		// TODO works only if policy class has only one overload. try to solve somehow...
		// printf("val: %d\n", Whatever3<_PC>::func(&_PC::doThat));

	}
};

struct A : Pol<A>
{
	void doThat(int) {}
	void doThat(char) {}
};

template <class _PolicyClass>
class PLanguagePolicy : STATIC_DERIVE_PUBLIC<_PolicyClass>
{
public:
		// Function description...
	    typedef std::string (_PolicyClass::*message_p)() const;

	    // different implementation options, chose yours...
	    static constexpr bool Enforce(message_p = &PLanguagePolicy::message,
	    							std::string (_PolicyClass::*)() const = &PLanguagePolicy::message) { return true; }
};

POLICY_DECL(PLanguagePolicy2,
			REQUIRE_CONST_FUNC(std::string, message)
			);

template <typename OutputPolicy, typename LanguagePolicy>
struct HelloWorld_SafeWrapped : public HelloWorld< PolicyEnforcer<POutputPolicy, OutputPolicy>, PolicyEnforcer<PLanguagePolicy2, LanguagePolicy> >
{
	HelloWorld_SafeWrapped() = default;

	HelloWorld_SafeWrapped(const HelloWorld<OutputPolicy, LanguagePolicy>& copyCtor) :
		HelloWorld< PolicyEnforcer<POutputPolicy, OutputPolicy>, PolicyEnforcer<PLanguagePolicy, LanguagePolicy> >(copyCtor)
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


// An example of function that expects HelloWorld and does not need to be modified
template<class... A>
void functionThatExpects(const HelloWorld<A...>& h)
{
	h.run();
}
// *** END OF EDIT. ***

/**
 * EDITED: 	Each paragraph in the following code section contains:
 * 			both original and wrapped version of HelloWorld Host class.
 */

struct t_POLICIES_FROM_HERE {};
static constexpr struct t_POLICIES_FROM_HERE POLICIES_FROM_HERE = {};

struct Secret
{
protected:
	void func()
	{

	}
};

struct WA
{
	static void print()
	{
		std::cout << std::is_same<decltype(&GetAccess<Secret, WA>::func), void (Secret::*)()>::value << std::endl;
	}
};

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

    A wish4thebest;

    std::cout << std::boolalpha;
    std::cout << std::endl;

    WA wa;
    wa.print();
}


