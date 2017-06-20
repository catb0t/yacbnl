Modern computers use a simple and compact representation of real, non-integral numbers, specified by and recognised as IEEE 754.

Base ten numbers with many significant fractional digits can be represented approximately in base two using a sign, an exponent part, and lastly a significand, representing a positive integer to which the exponent is applied.

The accuracy given an expected value is good, but there are a naturally uncountable number of numbers which cannot be represented exactly using this system of powers of two.

This implementation detail causes quite a lot of confusion among people coming from natural mathematics or the real world in general, because, like many other parts of computer science, it is applied within the complex systems of computers but not exposed.

Unsurprisingly, there have been countless attempts to fix this common problem of inexactness, and by extension, the element of surprise associated therein. Usually referred to as "big integer", "big number" or "decimal", these libraries or implementations use a higher-level and rather less performant implementation, that results in perfectly accurate numbers (given adequate physical resources).

Probably the simplest way to accomplish this goal of a better real number representation on computers is by storing each base ten digit, before and after the decimal separator, as individual numbers in an ordered list. The original value can be recreated exactly with no loss of precision because the value is stored exactly, and there is no practical limit on the number of significant digits because the list can grow to thousands of elements before it begins to threaten the computer's available memory.

However, such a representation is still not optimal. First, though each element in the list will only ever represent a number between 0 and 9, each element has of course a capacity orders of magnitude more than this. Second, in low-level languages like C, if the size of the structure is to be stored at the beginning of the homogenous structure, then the maximum usable length of the structure is the maximum storable primitive integer.

