# BoostAsioServerWrapper
wrapper boost asio tcp server

usage step:

1 build BoostAsioServer.sln using Visual Studio 2013 (select Release Mode) 

2 change solution property, Configuration Properties -> C/C++ -> Optimization -> Whole Program Optimization -> No

3 use BoostAsioServer.sln (or ignoring step 1 & 2, directly step 3)

4 set Project "BoostAsioServerTest" as StartUp Project

5 modify "BoostAsioServerTest.cpp", set your own local server_ip & server_port
