# cppTest
a gcc/libcpp demo to illustrate ability to preprocess c/c++ source file pragrammatically

There is a tons of preparing work to make it work.
1. preparing building gcc(10.2.0 is used, other versions are possible without testing.)
a) refer:  https://gcc.gnu.org/install/build.html
b) config with non-boot-strap debug build for development:
mkdir build && cd build 
../gcc-10.2.0/configure CFLAGS=-ggdb3 -O0 CXXFLAGS=-ggdb3 -O0 LDFLAGS=-ggdb3 --prefix=[your desired gcc-10.2.0 install path] --disable-bootstrap --enable-static --disable-shared --disable-multilib --enable-languages=c++ --enable-gcc-debug --enable-cpp-debug
c) install your building is not absolutely mandatory, but it will simplify a lot. Also, it might be beneficial if we compile project with this version of gcc, especially if we explore plugin feature.

2. pickup static library for building.
a) in building directory find following static library for likning:
-lcpp -lincpath -lcommon-target -lcommon -lbacktrace -liberty
this implies we need following library: libcpp.a, libcommon-target.a, libcommon.a, libbacktrace.a, libiberty.a
The only missing static library is libincpath which is created by ourselves by archive following object files:
incpath.o
cppdefault.o
options.o
prefix.o
so, create by this command: ar -rvs libincpath.a incpath.o cppdefault.o options.o prefix.o

3. include path for building:
a) we need gcc plugin/include directory and that is why it is better to install the built gcc-10.2.0 so that 
  -I[your gcc installed path]/lib/gcc/[your gcc triplet i.e. x86_64-pc-linux-gnu]/[your gcc version, i.e. 10.2.0]/plugin/include
  
  you can find this directory in your build directory, but it also easy to find it in installed by using your gcc-10.2.0. Assume your binary 10.2.1 gcc is gcc-10.2.0, then the include path is equal: $(gcc-10.2.0 --print-file-name=plugin)/include
  So, this is the only -I include path needed.
  
4. compilation is rather simple once we have figured out the static library. as we only have one src file, it is fairly straightforward like:
[your g++ command, either default one i.e. g++ or the one we just built the gcc-10.2.0] -I[your gcc installed path]/lib/gcc/[your gcc triplet i.e. x86_64-pc-linux-gnu]/[your gcc version, i.e. 10.2.0]/plugin/include  [c++ dialet, i.e. -std=c++17] cppTest.cpp  -lcpp -lincpath -lcommon-target -lcommon -lbacktrace -liberty -o cppTest.exe

5. There are a couple hardcoded path in source file. They are done so because it is a bit difficult to get it dynamically, not impossible, but it is a big sidetrack for a demo. 
a) cppTest.cpp:  char*path=strdup("/home/nick/Downloads/gcc-10.2.0/gcc-10.2.0-debug-libcpp-install/lib/gcc/x86_64-pc-linux-gnu/10.2.0/plugin/include");
if you read the "include" path part, you will understand this is just the "plugin/include" itself. replace this with your own gcc plugin path
b) cppTest.cpp: path=strdup("/usr/include/x86_64-linux-gnu");
This is actually the system stuff, do we really need to change to the installed gcc path? maybe not because all c++ library stuff are also there. just pay attention that your "triplet" is "x86_64-linux-gnu＂　by checking this `gcc -dumpmachine`

c) const char* ptr=cpp_read_main_file(parse_in, "/home/nick/eclipse-2021/cppTest/src/cppTest.cpp");
this is obviously the "target source" file you want to preprocess, it can be any c++ source file at your choice, but passing our source itself would be nice by using "__FILE__"
d) how do I get those "cpp_define(parse_in, "__DBL_MIN_EXP__=(-1021)");"
these are acquired during debugging, but essentially they are parsed by gcc "spec", how to parse them and generate them is a little tool by checking this:

g++ -std=c++17 -dM -E -x c++ - < /dev/null

what I did is save them into a temp file and parse it by regex to generate the code. i.e.

	regex ex(R"regex(#define\s([^\s]+)\s([^\s]+)\s)regex");
	ifstream in{"/tmp/macro.txt"};
	string str(istreambuf_iterator<char>{in}, istreambuf_iterator<char>{});
	for (auto it=sregex_iterator{str.begin(), str.end(), ex}; it!=sregex_iterator{}; it++)
	{
		 std::smatch result=*it;
		 //cout<<result[1]<<endl<<result[2]<<endl<<"**************"<<endl;
		 cout<<"cpp_define(parse_in, \""<<result[1]<<"="<<result[2]<<"\");"<<endl;
	}


That is it, don't forget to change the source file name in the code 


What can you do with this demo or what is use of this work?
depending how you want to find out during preprocessing, you check token type at "if (token->type != CPP_PADDING)", you can achieve a universal "callback" of libcpp alternative to official callback. This is trivial, but the purpose is to demo how to "programmatically" programming libcpp. The biggest part is to initialize those "built-in" and "system" directories and various pre-defined switches. 
Writing a preprocessor is non-trivial job, compared with this setup, it is worth the efforts.
also "cpp_spell_token" is relying on the "buffer" size is enough for a token, pay attention of extrememly long user-defined literal strings, or comments...
