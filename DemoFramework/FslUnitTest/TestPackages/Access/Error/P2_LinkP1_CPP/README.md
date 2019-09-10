# P2_LinkP1_CPP

Disable on all platform per default.
As this package intentionally can not compile since its trying to access headers it don't have access to. Which is the thing we are trying to verify.

To build this use FslBuild.py --IgnoreNotSupported

Expected error:

- P2_LinkP1_CPP.cpp
  #include <FslUnitTest/TestPackages/Access/P1/P1.hpp> no such file or directory

Since it specified a link dependency on P1 it should not be able to include its files!
