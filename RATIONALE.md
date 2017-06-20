Modern computers use a simple and compact representation of real, non-integral numbers, specified by and recognised as IEEE 754.

Base ten numbers with many significant fractional digits can be represented approximately in base two using a sign, an exponent part, and lastly a significand, representing a positive integer to which the exponent is applied.

The accuracy given an expected value is good, but there are a naturally uncountable number of numbers which cannot be represented exactly using this system of powers of two.

This implementation detail causes quite a lot of confusion among people coming from natural mathematics or the real world in general, because, like many other parts of computer science, it is applied within the complex systems of computers but not exposed.

Unsurprisingly, there have been countless attempts to fix this common problem of inexactness, and by extension, the element of surprise associated therein. Usually referred to as "big integer", "big number" or "decimal", these libraries or implementations use a higher-level and rather less performant implementation, that results in perfectly accurate numbers (given adequate physical resources).

Probably the simplest way to accomplish this goal of a better real number representation on computers is by storing each base ten digit, before and after the decimal separator, as individual numbers in an ordered list. The original value can be recreated exactly with no loss of precision because the value is stored exactly, and there is no practical limit on the number of significant digits because the list can grow to thousands of elements before it begins to threaten the computer's available memory.

However, such a representation is still not optimal. First, though each element in the list will only ever represent a number between 0 and 9, each element has of course a potential capacity orders of magnitude more than this. Second, in low-level languages like C, if the size of the structure is to be stored at the beginning of the homogenous structure, then the maximum usable length of the structure is the maximum storable primitive integer.

To remedy the first problem, we can use a base 256 representation of the number, since the maximum value of 8 unsigned bits is 255. This shortens dramatically the representation of all numbers since each significant digit from the right is a power of 256. In base ten, we have the ones, (10 ^ 0), tens (10 ^ 1), hundreds (10 ^ 2) places, and so on; in base 256 we have a ones place with a value up to 256, and the second digit from the right as the "256s" place, and so on.

The solution to the second is a little bit more complex: if we are restricted to storing values less than 256 in each list element, but we want to allow more than 256 digits, then the length of the digit parts as a 16-bit number (values up to 65,536) can be stored as two separate list elements whose bits are combined into one wider value when interpreted.

Students of programming and academic computer scientists alike will continue to create clever solutions to "problems" however impactful they are. Creating more big number libraries for fun when adequate ones written by smarter people already exist may seem like a pointless reinventing of the wheel or a needless duplication of effort, but without literally reinventing the wheel and the cart, engineering across the board could never innovate the way we want it to. 
