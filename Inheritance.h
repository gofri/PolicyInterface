/**
 ****************************************
 **	Inheritance.h
 **
 **	@author Gal Ofri
 **	@email	galmenashofri@gmail.com
 **	@
 ****************************************
 **/

#ifndef XXP_INHERITANCE_H_
#define XXP_INHERITANCE_H_

/**
 * ReferProtected Grants _Accessor the access to reference
 * protected data members and functions of _Object via ReferProtected, e.g.
 * &ReferProtected<Dog, MyClass>::bark;
 */
template <class _Object, class _Accessor>
struct ReferProtected final : _Object
{
	// Grant access to _Accessor
	friend _Accessor;

	// Forbid further inheritance
	ReferProtected() = delete;
};

#endif /* POLICYINTERFACE_INHERITANCE_H_ */
