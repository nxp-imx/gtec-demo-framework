# P2_PriP1_HPP

Disable on all platform per default. This package can unfortunately compile.
But its constructed in a way where it incorrectly includes a package from P1 in its public header files.
Which is incorrect as it has a private dependency on P1.

So the actual test of this error condition is done by another package.


To build this use FslBuild.py --IgnoreNotSupported

Programming error:

- P2_PriP1_HPP.hpp
  #include <FslUnitTest/TestPackages/Access/P1/P1.hpp>

Since it specified a private dependency on P1 it is not allowed to include P1 from public headers and any package dependent on this package that include P2_PriP1_HPP.hpp will not be able to compile (unless it has specified a dependency on P1)
