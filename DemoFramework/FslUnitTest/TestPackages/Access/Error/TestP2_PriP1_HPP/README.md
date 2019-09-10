# TestP2_PriP1_HPP

Disable on all platform per default.
Test that we get a compilation error when including the file P2_PriP1_HPP from P2 since it incorrectly includes a file from P1 which it has a private dependency on.

To build this use FslBuild.py --IgnoreNotSupported

Expected compilation error:

- P2_PriP1_HPP.hpp
  #include <FslUnitTest/TestPackages/Access/P1/P1.hpp>

Since P2 specified a private dependency on P1 it is not allowed to include P1 from public headers.
