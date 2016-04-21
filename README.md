# BoostAsioServerWrapper
wrapper boost asio tcp server

usage step:

1 prepare ACE6.3.0 & BOOST1.59 well

2 use gen_vc12_vs2013.cmd to generate "BoostAsioServer.sln"

3 use BoostAsioServer.sln (or ignoring step 1 & 2, directly step 3)

4 change all project property, Configuration Properties -> C/C++ -> Optimization -> Whole Program Optimization -> No

5 set Project "BoostAsioServerTest" as StartUp Project

6 modify "BoostAsioServerTest.cpp", set your own local server_ip & server_port

7 build BoostAsioServer.sln using Visual Studio 2013 (select Release Mode) 
