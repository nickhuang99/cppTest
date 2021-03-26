//============================================================================
// Name        : cppTest.cpp
// Author      : nick huang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "glimits.h"
#include "auto-host.h"
#include "system.h"
#include "ansidecl.h"
#include "libiberty.h"
#include "cpplib.h"
#include "coretypes.h"
#include "incpath.h"
#include "diagnostic.h"

using namespace std;

extern void cpp_init_iconv (cpp_reader *pfile);
static size_t round_alloc_size (size_t s)
{
  return s;
}
bool diagnostic_cb (cpp_reader *, enum cpp_diagnostic_level errtype,
	       enum cpp_warning_reason, rich_location *richloc,
	       const char *msg, va_list *ap)
{
  const line_map_ordinary *map;
  location_t location = richloc->get_loc ();
  linemap_resolve_location (line_table, location, LRK_SPELLING_LOCATION, &map);
  expanded_location loc = linemap_expand_location (line_table, map, location);
  fprintf (stderr, "%s:%d:%d %s: ", loc.file, loc.line, loc.column,
	   (errtype == CPP_DL_WARNING) ? "warning" : "error");
  vfprintf (stderr, msg, *ap);
  fprintf (stderr, "\n");
  FILE *f = fopen (loc.file, "r");
  if (f)
    {
      char buf[128];
      while (loc.line > 0)
	{
	  if (!fgets (buf, 128, f))
	    goto notfound;
	  if (buf[strlen (buf) - 1] != '\n')
	    {
	      if (loc.line > 1)
		loc.line++;
	    }
	  loc.line--;
	}
      fprintf (stderr, "%s", buf);
      for (int i = 0; i < loc.column - 1; ++i)
	fputc (' ', stderr);
      fputc ('^', stderr);
      fputc ('\n', stderr);
notfound:
      fclose (f);
    }

  if (errtype == CPP_DL_FATAL)
    exit (1);
  return false;
}
const char* my_missing_header(cpp_reader *parse_in, const char *header, cpp_dir ** ppdir)
{
//	cout<<"missing header:"<<header<<endl<<"dir:"<<(*ppdir)->name<<endl;
	return NULL;
}
void cb_include (cpp_reader * parse_in, location_t loc, const unsigned char * dirName,
		   const char * fname, int angle_brackets, const cpp_token **ptoken)
{
	cerr<<"loc:"<<loc<<endl<<"dirname:"<<dirName<<endl<<"fname:"<<fname<<endl<<"angle_bracket:"<<angle_brackets<<endl;

//	cpp_buffer* pbuffer=cpp_get_buffer(parse_in);
//	_cpp_file*pfile=cpp_get_file(pbuffer);
//	cout<<"path:"<<cpp_get_path(pfile)<<endl;
	expanded_location exploc=linemap_client_expand_location_to_spelling_point(loc, LOCATION_ASPECT_CARET);
	cerr<<"file:"<<exploc.file<<endl<<"line:"<<exploc.line<<":column"<<exploc.column<<endl<<"************************"<<endl;

	//cpp_warning_with_line(parse_in, CPP_W_NONE, loc, 0, "warning from nick");
}
//unsigned char buffer[2048];
string cppTokenTypeName(const cpp_token* token)
{
	if (token->type == CPP_KEYWORD)
	{
		return "KEYWORD";
	}
	if (token->type == CPP_NAME)
	{
		return "IDENT";
	}

	if (token->type >= CPP_EQ && token->type < CPP_NAME)
	{
		return "OPERATOR";
	}
	if (token->type>=CPP_NUMBER && token->type<=CPP_COMMENT)
	{
		return "LITERAL";
	}
	if (token->type==CPP_MACRO_ARG)
	{
		return "MACRO_ARG";
	}
	if (token->type>=CPP_PRAGMA && token->type<=CPP_PRAGMA_EOL)
	{
		return "PRAGMA";
	}
	if (token->type==CPP_PADDING)
	{
		return "PADDING";
	}
	return "ERROR";
}

struct Token_Name
{
	cpp_ttype t;
	unsigned char* name;
};
#define OP(e, s) { CPP_##e,    (unsigned char*) #s },
#define TK(e, s) { CPP_##e,    (unsigned char*) #e },
static const struct Token_Name token_names[N_TTYPES] = { TTYPE_TABLE };
#undef OP
#undef TK
#define TOKEN_NAME(token) (token_names[(token)->type].name)

int main()
{
	//std::set_terminate([](){ std::cout << "Unhandled exception" << std::endl; abort();});
	//std::set_terminate(__gnu_cxx::__verbose_terminate_handler);
	diagnostic_initialize (global_dc, 0);
	line_table = XCNEW (class line_maps);
	linemap_init (line_table, BUILTINS_LOCATION);
	line_table->reallocator = xrealloc;
	line_table->round_alloc_size = round_alloc_size;
	cpp_reader* parse_in=cpp_create_reader(CLK_CXX17, nullptr, line_table);
	if (parse_in)
	{
		cpp_options * poption=cpp_get_options(parse_in);
		poption->debug=1;
		poption->cplusplus_comments=1;
		char*path=strdup("/home/nick/Downloads/gcc-10.2.0/gcc-10.2.0-debug-libcpp-install/lib/gcc/x86_64-pc-linux-gnu/10.2.0/plugin/include");
		add_path(path, INC_BRACKET, 1, 1);
		path=strdup("/usr/include/x86_64-linux-gnu");
		add_path(path, INC_SYSTEM, 1, 1);
		//register_include_chains(parse_in, "/tmp,/usr/include/x86_64-linux-gnu", nullptr, nullptr, 1, 1, 1);
		register_include_chains(parse_in, nullptr, nullptr, nullptr, 1, 1, 1);
//		cpp_dir *dir = XCNEW (cpp_dir);
//		dir->name = ASTRDUP ("/home/nick/eclipse-2021/cppTest/src");
//		cpp_set_include_chains (parse_in, dir, NULL, true);
		const char* ptr=cpp_read_main_file(parse_in, "/home/nick/eclipse-2021/cppTest/src/cppTest.cpp");
		//const char* ptr=cpp_read_main_file(parse_in, "/tmp/test.c");
		if (ptr)
		{
			cpp_init_builtins(parse_in, 1);
			cpp_init_iconv(parse_in);
			cerr<<"successful!"<<ptr<<endl;
			cpp_callbacks*pCallback=cpp_get_callbacks(parse_in);
			pCallback->include=cb_include;
			pCallback->diagnostic=diagnostic_cb;
			pCallback->missing_header=my_missing_header;
			cpp_set_callbacks(parse_in, pCallback);
			cpp_define(parse_in, "__DBL_MIN_EXP__=(-1021)");
			cpp_define(parse_in, "__UINT_LEAST16_MAX__=0xffff");
			cpp_define(parse_in, "__ATOMIC_ACQUIRE=2");
			cpp_define(parse_in, "__FLT128_MAX_10_EXP__=4932");
			cpp_define(parse_in, "__FLT_MIN__=1.17549435082228750796873653722224568e-38F");
			cpp_define(parse_in, "__GCC_IEC_559_COMPLEX=2");
			cpp_define(parse_in, "__UINT_LEAST8_TYPE__=unsigned");
			cpp_define(parse_in, "__SIZEOF_FLOAT80__=16");
			cpp_define(parse_in, "__INTMAX_C(c)=c");
			cpp_define(parse_in, "__CHAR_BIT__=8");
			cpp_define(parse_in, "__UINT8_MAX__=0xff");
			cpp_define(parse_in, "__SCHAR_WIDTH__=8");
			cpp_define(parse_in, "__WINT_MAX__=0xffffffffU");
			cpp_define(parse_in, "__FLT32_MIN_EXP__=(-125)");
			cpp_define(parse_in, "__ORDER_LITTLE_ENDIAN__=1234");
			cpp_define(parse_in, "__SIZE_MAX__=0xffffffffffffffffUL");
			cpp_define(parse_in, "__WCHAR_MAX__=0x7fffffff");
			cpp_define(parse_in, "__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1=1");
			cpp_define(parse_in, "__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2=1");
			cpp_define(parse_in, "__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4=1");
			cpp_define(parse_in, "__DBL_DENORM_MIN__=((double)4.94065645841246544176568792868221372e-324L)");
			cpp_define(parse_in, "__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8=1");
			cpp_define(parse_in, "__GCC_ATOMIC_CHAR_LOCK_FREE=2");
			cpp_define(parse_in, "__GCC_IEC_559=2");
			cpp_define(parse_in, "__FLT32X_DECIMAL_DIG__=17");
			cpp_define(parse_in, "__FLT_EVAL_METHOD__=0");
			cpp_define(parse_in, "__FLT64_DECIMAL_DIG__=17");
			cpp_define(parse_in, "__GCC_ATOMIC_CHAR32_T_LOCK_FREE=2");
			cpp_define(parse_in, "__UINT_FAST64_MAX__=0xffffffffffffffffUL");
			cpp_define(parse_in, "__SIG_ATOMIC_TYPE__=int");
			cpp_define(parse_in, "__DBL_MIN_10_EXP__=(-307)");
			cpp_define(parse_in, "__FINITE_MATH_ONLY__=0");
			cpp_define(parse_in, "__FLT32X_MAX_EXP__=1024");
			cpp_define(parse_in, "__FLT32_HAS_DENORM__=1");
			cpp_define(parse_in, "__UINT_FAST8_MAX__=0xff");
			cpp_define(parse_in, "__FLT32_MAX_10_EXP__=38");
			cpp_define(parse_in, "__DEC64_MAX_EXP__=385");
			cpp_define(parse_in, "__INT8_C(c)=c");
			cpp_define(parse_in, "__INT_LEAST8_WIDTH__=8");
			cpp_define(parse_in, "__UINT_LEAST64_MAX__=0xffffffffffffffffUL");
			cpp_define(parse_in, "__SHRT_MAX__=0x7fff");
			cpp_define(parse_in, "__LDBL_MAX__=1.18973149535723176502126385303097021e+4932L");
			cpp_define(parse_in, "__FLT64X_MAX_10_EXP__=4932");
			cpp_define(parse_in, "__FLT64X_HAS_QUIET_NAN__=1");
			cpp_define(parse_in, "__UINT_LEAST8_MAX__=0xff");
			cpp_define(parse_in, "__GCC_ATOMIC_BOOL_LOCK_FREE=2");
			cpp_define(parse_in, "__FLT128_DENORM_MIN__=6.47517511943802511092443895822764655e-4966F128");
			cpp_define(parse_in, "__UINTMAX_TYPE__=long");
			cpp_define(parse_in, "__linux=1");
			cpp_define(parse_in, "__DEC32_EPSILON__=1E-6DF");
			cpp_define(parse_in, "__FLT_EVAL_METHOD_TS_18661_3__=0");
			cpp_define(parse_in, "__unix=1");
			cpp_define(parse_in, "__UINT32_MAX__=0xffffffffU");
			cpp_define(parse_in, "__FLT128_MIN_EXP__=(-16381)");
			cpp_define(parse_in, "__WINT_MIN__=0U");
			cpp_define(parse_in, "__FLT128_MIN_10_EXP__=(-4931)");
			cpp_define(parse_in, "__INT_LEAST16_WIDTH__=16");
			cpp_define(parse_in, "__SCHAR_MAX__=0x7f");
			cpp_define(parse_in, "__FLT128_MANT_DIG__=113");
			cpp_define(parse_in, "__WCHAR_MIN__=(-__WCHAR_MAX__");
			cpp_define(parse_in, "__INT64_C(c)=c");
			cpp_define(parse_in, "__GCC_ATOMIC_POINTER_LOCK_FREE=2");
			cpp_define(parse_in, "__FLT32X_MANT_DIG__=53");
			cpp_define(parse_in, "__FLT64X_EPSILON__=1.08420217248550443400745280086994171e-19F64x");
			//cpp_define(parse_in, "__STDC_HOSTED__=1");
			cpp_define(parse_in, "__DEC64_MIN_EXP__=(-382)");
			cpp_define(parse_in, "__DBL_DIG__=15");
			cpp_define(parse_in, "__FLT32_DIG__=6");
			cpp_define(parse_in, "__FLT_EPSILON__=1.19209289550781250000000000000000000e-7F");
			cpp_define(parse_in, "__SHRT_WIDTH__=16");
			cpp_define(parse_in, "__LDBL_MIN__=3.36210314311209350626267781732175260e-4932L");
			//cpp_define(parse_in, "__STDC_UTF_16__=1");
			cpp_define(parse_in, "__DEC32_MAX__=9.999999E96DF");
			cpp_define(parse_in, "__FLT64X_DENORM_MIN__=3.64519953188247460252840593361941982e-4951F64x");
			cpp_define(parse_in, "__FLT32X_HAS_INFINITY__=1");
			cpp_define(parse_in, "__INT32_MAX__=0x7fffffff");
			cpp_define(parse_in, "__unix__=1");
			cpp_define(parse_in, "__INT_WIDTH__=32");
			cpp_define(parse_in, "__SIZEOF_LONG__=8");
			cpp_define(parse_in, "__STDC_IEC_559__=1");
			cpp_define(parse_in, "__STDC_ISO_10646__=201706L");
			cpp_define(parse_in, "__UINT16_C(c)=c");
			cpp_define(parse_in, "__DECIMAL_DIG__=21");
			cpp_define(parse_in, "__STDC_IEC_559_COMPLEX__=1");
			cpp_define(parse_in, "__FLT64_EPSILON__=2.22044604925031308084726333618164062e-16F64");
			cpp_define(parse_in, "__gnu_linux__=1");
			cpp_define(parse_in, "__FLT64X_MIN_10_EXP__=(-4931)");
			cpp_define(parse_in, "__LDBL_HAS_QUIET_NAN__=1");
			cpp_define(parse_in, "__FLT64_MANT_DIG__=53");
			cpp_define(parse_in, "__FLT64X_MANT_DIG__=64");
			cpp_define(parse_in, "__GNUC__=10");
			cpp_define(parse_in, "__MMX__=1");
			cpp_define(parse_in, "__FLT_HAS_DENORM__=1");
			cpp_define(parse_in, "__SIZEOF_LONG_DOUBLE__=16");
			cpp_define(parse_in, "__BIGGEST_ALIGNMENT__=16");
			cpp_define(parse_in, "__FLT64_MAX_10_EXP__=308");
			cpp_define(parse_in, "__DBL_MAX__=((double)1.79769313486231570814527423731704357e+308L)");
			cpp_define(parse_in, "__INT_FAST32_MAX__=0x7fffffffffffffffL");
			cpp_define(parse_in, "__DBL_HAS_INFINITY__=1");
			cpp_define(parse_in, "__SIZEOF_FLOAT__=4");
			cpp_define(parse_in, "__HAVE_SPECULATION_SAFE_VALUE=1");
			cpp_define(parse_in, "__DEC32_MIN_EXP__=(-94)");
			cpp_define(parse_in, "__INTPTR_WIDTH__=64");
			cpp_define(parse_in, "__FLT64X_HAS_INFINITY__=1");
			cpp_define(parse_in, "__UINT_LEAST32_MAX__=0xffffffffU");
			cpp_define(parse_in, "__FLT32X_HAS_DENORM__=1");
			cpp_define(parse_in, "__INT_FAST16_TYPE__=long");
			cpp_define(parse_in, "__MMX_WITH_SSE__=1");
			cpp_define(parse_in, "__LDBL_HAS_DENORM__=1");
			cpp_define(parse_in, "__FLT128_HAS_INFINITY__=1");
			cpp_define(parse_in, "__DEC32_MIN__=1E-95DF");
			cpp_define(parse_in, "__DBL_MAX_EXP__=1024");
			cpp_define(parse_in, "__WCHAR_WIDTH__=32");
			cpp_define(parse_in, "__FLT32_MAX__=3.40282346638528859811704183484516925e+38F32");
			cpp_define(parse_in, "__DEC128_EPSILON__=1E-33DL");
			cpp_define(parse_in, "__SSE2_MATH__=1");
			cpp_define(parse_in, "__ATOMIC_HLE_RELEASE=131072");
			cpp_define(parse_in, "__PTRDIFF_MAX__=0x7fffffffffffffffL");
			cpp_define(parse_in, "__amd64=1");
			cpp_define(parse_in, "__STDC_NO_THREADS__=1");
			cpp_define(parse_in, "__ATOMIC_HLE_ACQUIRE=65536");
			cpp_define(parse_in, "__LONG_LONG_MAX__=0x7fffffffffffffffLL");
			cpp_define(parse_in, "__SIZEOF_SIZE_T__=8");
			cpp_define(parse_in, "__FLT64X_MIN_EXP__=(-16381)");
			cpp_define(parse_in, "__SIZEOF_WINT_T__=4");
			cpp_define(parse_in, "__LONG_LONG_WIDTH__=64");
			cpp_define(parse_in, "__FLT32_MAX_EXP__=128");
			cpp_define(parse_in, "__GXX_ABI_VERSION=1014");
			cpp_define(parse_in, "__FLT_MIN_EXP__=(-125)");
			cpp_define(parse_in, "__GCC_HAVE_DWARF2_CFI_ASM=1");
			cpp_define(parse_in, "__INT16_MAX__=0x7fff");
			cpp_define(parse_in, "__x86_64=1");
			cpp_define(parse_in, "__INT_FAST64_TYPE__=long");
			cpp_define(parse_in, "__FLT64_DENORM_MIN__=4.94065645841246544176568792868221372e-324F64");
			cpp_define(parse_in, "__DBL_MIN__=((double)2.22507385850720138309023271733240406e-308L)");
			cpp_define(parse_in, "__FLT128_EPSILON__=1.92592994438723585305597794258492732e-34F128");
			cpp_define(parse_in, "__FLT64X_NORM_MAX__=1.18973149535723176502126385303097021e+4932F64x");
			cpp_define(parse_in, "__SIZEOF_POINTER__=8");
			cpp_define(parse_in, "__LP64__=1");
			cpp_define(parse_in, "__DBL_HAS_QUIET_NAN__=1");
			cpp_define(parse_in, "__FLT32X_EPSILON__=2.22044604925031308084726333618164062e-16F32x");
			cpp_define(parse_in, "__DECIMAL_BID_FORMAT__=1");
			cpp_define(parse_in, "__FLT64_MIN_EXP__=(-1021)");
			cpp_define(parse_in, "__FLT64_MIN_10_EXP__=(-307)");
			cpp_define(parse_in, "__FLT64X_DECIMAL_DIG__=21");
			cpp_define(parse_in, "__DEC128_MIN__=1E-6143DL");
			cpp_define(parse_in, "__UINT16_MAX__=0xffff");
			cpp_define(parse_in, "__DBL_HAS_DENORM__=1");
			cpp_define(parse_in, "__LDBL_HAS_INFINITY__=1");
			cpp_define(parse_in, "__FLT32_MIN__=1.17549435082228750796873653722224568e-38F32");
			cpp_define(parse_in, "__UINT8_TYPE__=unsigned");
			cpp_define(parse_in, "__FLT_DIG__=6");
			cpp_define(parse_in, "__NO_INLINE__=1");
			cpp_define(parse_in, "__DEC_EVAL_METHOD__=2");
			cpp_define(parse_in, "__DEC128_MAX__=9.999999999999999999999999999999999E6144DL");
			cpp_define(parse_in, "__FLT_MANT_DIG__=24");
			cpp_define(parse_in, "__LDBL_DECIMAL_DIG__=21");
			cpp_define(parse_in, "__VERSION__=\"10.2.0\"");
			cpp_define(parse_in, "__UINT64_C(c)=c");
			cpp_define(parse_in, "_STDC_PREDEF_H=1");
			cpp_define(parse_in, "__INT_LEAST32_MAX__=0x7fffffff");
			cpp_define(parse_in, "__GCC_ATOMIC_INT_LOCK_FREE=2");
			cpp_define(parse_in, "__FLT128_MAX_EXP__=16384");
			cpp_define(parse_in, "__FLT32_MANT_DIG__=24");
			cpp_define(parse_in, "__FLOAT_WORD_ORDER__=__ORDER_LITTLE_ENDIAN__");
			cpp_define(parse_in, "__FLT128_HAS_DENORM__=1");
			cpp_define(parse_in, "__FLT32_DECIMAL_DIG__=9");
			cpp_define(parse_in, "__FLT128_DIG__=33");
			cpp_define(parse_in, "__INT32_C(c)=c");
			cpp_define(parse_in, "__DEC64_EPSILON__=1E-15DD");
			cpp_define(parse_in, "__ORDER_PDP_ENDIAN__=3412");
			cpp_define(parse_in, "__DEC128_MIN_EXP__=(-6142)");
			cpp_define(parse_in, "__INT_FAST32_TYPE__=long");
			cpp_define(parse_in, "__UINT_LEAST16_TYPE__=short");
			cpp_define(parse_in, "unix=1");
			cpp_define(parse_in, "__SIZE_TYPE__=long");
			cpp_define(parse_in, "__UINT64_MAX__=0xffffffffffffffffUL");
			cpp_define(parse_in, "__FLT64X_DIG__=18");
			cpp_define(parse_in, "__INT8_TYPE__=signed");
			cpp_define(parse_in, "__ELF__=1");
			cpp_define(parse_in, "__GCC_ASM_FLAG_OUTPUTS__=1");
			cpp_define(parse_in, "__UINT32_TYPE__=unsigned");
			cpp_define(parse_in, "__FLT_RADIX__=2");
			cpp_define(parse_in, "__INT_LEAST16_TYPE__=short");
			cpp_define(parse_in, "__LDBL_EPSILON__=1.08420217248550443400745280086994171e-19L");
			cpp_define(parse_in, "__UINTMAX_C(c)=c");
			cpp_define(parse_in, "__SSE_MATH__=1");
			cpp_define(parse_in, "__k8=1");
			cpp_define(parse_in, "__FLT32X_MIN__=2.22507385850720138309023271733240406e-308F32x");
			cpp_define(parse_in, "__SIG_ATOMIC_MAX__=0x7fffffff");
			cpp_define(parse_in, "__GCC_ATOMIC_WCHAR_T_LOCK_FREE=2");
			cpp_define(parse_in, "__SIZEOF_PTRDIFF_T__=8");
			cpp_define(parse_in, "__LDBL_DIG__=18");
			cpp_define(parse_in, "__x86_64__=1");
			cpp_define(parse_in, "__FLT32X_MIN_EXP__=(-1021)");
			cpp_define(parse_in, "__DEC32_SUBNORMAL_MIN__=0.000001E-95DF");
			cpp_define(parse_in, "__INT_FAST16_MAX__=0x7fffffffffffffffL");
			cpp_define(parse_in, "__FLT64_DIG__=15");
			cpp_define(parse_in, "__UINT_FAST32_MAX__=0xffffffffffffffffUL");
			cpp_define(parse_in, "__UINT_LEAST64_TYPE__=long");
			cpp_define(parse_in, "__FLT_HAS_QUIET_NAN__=1");
			cpp_define(parse_in, "__FLT_MAX_10_EXP__=38");
			cpp_define(parse_in, "__LONG_MAX__=0x7fffffffffffffffL");
			cpp_define(parse_in, "__FLT64X_HAS_DENORM__=1");
			cpp_define(parse_in, "__DEC128_SUBNORMAL_MIN__=0.000000000000000000000000000000001E-6143DL");
			cpp_define(parse_in, "__FLT_HAS_INFINITY__=1");
			cpp_define(parse_in, "__UINT_FAST16_TYPE__=long");
			cpp_define(parse_in, "__DEC64_MAX__=9.999999999999999E384DD");
			cpp_define(parse_in, "__INT_FAST32_WIDTH__=64");
			cpp_define(parse_in, "__CHAR16_TYPE__=short");
			cpp_define(parse_in, "__PRAGMA_REDEFINE_EXTNAME=1");
			cpp_define(parse_in, "__SIZE_WIDTH__=64");
			cpp_define(parse_in, "__SEG_FS=1");
			cpp_define(parse_in, "__INT_LEAST16_MAX__=0x7fff");
			cpp_define(parse_in, "__DEC64_MANT_DIG__=16");
			cpp_define(parse_in, "__INT64_MAX__=0x7fffffffffffffffL");
			cpp_define(parse_in, "__SEG_GS=1");
			cpp_define(parse_in, "__FLT32_DENORM_MIN__=1.40129846432481707092372958328991613e-45F32");
			cpp_define(parse_in, "__SIG_ATOMIC_WIDTH__=32");
			cpp_define(parse_in, "__INT_LEAST64_TYPE__=long");
			cpp_define(parse_in, "__INT16_TYPE__=short");
			cpp_define(parse_in, "__INT_LEAST8_TYPE__=signed");
			cpp_define(parse_in, "__STDC_VERSION__=201710L");
			cpp_define(parse_in, "__SIZEOF_INT__=4");
			cpp_define(parse_in, "__DEC32_MAX_EXP__=97");
			cpp_define(parse_in, "__INT_FAST8_MAX__=0x7f");
			cpp_define(parse_in, "__FLT128_MAX__=1.18973149535723176508575932662800702e+4932F128");
			cpp_define(parse_in, "__INTPTR_MAX__=0x7fffffffffffffffL");
			cpp_define(parse_in, "linux=1");
			cpp_define(parse_in, "__FLT64_HAS_QUIET_NAN__=1");
			cpp_define(parse_in, "__FLT32_MIN_10_EXP__=(-37)");
			cpp_define(parse_in, "__FLT32X_DIG__=15");
			cpp_define(parse_in, "__PTRDIFF_WIDTH__=64");
			cpp_define(parse_in, "__LDBL_MANT_DIG__=64");
			cpp_define(parse_in, "__FLT64_HAS_INFINITY__=1");
			cpp_define(parse_in, "__FLT64X_MAX__=1.18973149535723176502126385303097021e+4932F64x");
			cpp_define(parse_in, "__SIG_ATOMIC_MIN__=(-__SIG_ATOMIC_MAX__");
			cpp_define(parse_in, "__code_model_small__=1");
			cpp_define(parse_in, "__GCC_ATOMIC_LONG_LOCK_FREE=2");
			cpp_define(parse_in, "__DEC32_MANT_DIG__=7");
			cpp_define(parse_in, "__k8__=1");
			cpp_define(parse_in, "__INTPTR_TYPE__=long");
			cpp_define(parse_in, "__UINT16_TYPE__=short");
			cpp_define(parse_in, "__WCHAR_TYPE__=int");
			cpp_define(parse_in, "__UINTPTR_MAX__=0xffffffffffffffffUL");
			cpp_define(parse_in, "__INT_FAST64_WIDTH__=64");
			cpp_define(parse_in, "__INT_FAST64_MAX__=0x7fffffffffffffffL");
			cpp_define(parse_in, "__GCC_ATOMIC_TEST_AND_SET_TRUEVAL=1");
			cpp_define(parse_in, "__FLT_NORM_MAX__=3.40282346638528859811704183484516925e+38F");
			cpp_define(parse_in, "__FLT32_HAS_INFINITY__=1");
			cpp_define(parse_in, "__FLT64X_MAX_EXP__=16384");
			cpp_define(parse_in, "__UINT_FAST64_TYPE__=long");
			cpp_define(parse_in, "__INT_MAX__=0x7fffffff");
			cpp_define(parse_in, "__linux__=1");
			cpp_define(parse_in, "__INT64_TYPE__=long");
			cpp_define(parse_in, "__FLT_MAX_EXP__=128");
			cpp_define(parse_in, "__ORDER_BIG_ENDIAN__=4321");
			cpp_define(parse_in, "__DBL_MANT_DIG__=53");
			cpp_define(parse_in, "__SIZEOF_FLOAT128__=16");
			cpp_define(parse_in, "__INT_LEAST64_MAX__=0x7fffffffffffffffL");
			cpp_define(parse_in, "__GCC_ATOMIC_CHAR16_T_LOCK_FREE=2");
			cpp_define(parse_in, "__DEC64_MIN__=1E-383DD");
			cpp_define(parse_in, "__WINT_TYPE__=unsigned");
			cpp_define(parse_in, "__UINT_LEAST32_TYPE__=unsigned");
			cpp_define(parse_in, "__SIZEOF_SHORT__=2");
			cpp_define(parse_in, "__FLT32_NORM_MAX__=3.40282346638528859811704183484516925e+38F32");
			cpp_define(parse_in, "__SSE__=1");
			cpp_define(parse_in, "__LDBL_MIN_EXP__=(-16381)");
			cpp_define(parse_in, "__FLT64_MAX__=1.79769313486231570814527423731704357e+308F64");
			cpp_define(parse_in, "__amd64__=1");
			cpp_define(parse_in, "__WINT_WIDTH__=32");
			cpp_define(parse_in, "__INT_LEAST8_MAX__=0x7f");
			cpp_define(parse_in, "__INT_LEAST64_WIDTH__=64");
			cpp_define(parse_in, "__LDBL_MAX_EXP__=16384");
			cpp_define(parse_in, "__FLT32X_MAX_10_EXP__=308");
			cpp_define(parse_in, "__SIZEOF_INT128__=16");
			cpp_define(parse_in, "__LDBL_MAX_10_EXP__=4932");
			cpp_define(parse_in, "__ATOMIC_RELAXED=0");
			cpp_define(parse_in, "__DBL_EPSILON__=((double)2.22044604925031308084726333618164062e-16L)");
			cpp_define(parse_in, "__FLT128_MIN__=3.36210314311209350626267781732175260e-4932F128");
			cpp_define(parse_in, "_LP64=1");
			cpp_define(parse_in, "__UINT8_C(c)=c");
			cpp_define(parse_in, "__FLT64_MAX_EXP__=1024");
			cpp_define(parse_in, "__INT_LEAST32_TYPE__=int");
			cpp_define(parse_in, "__SIZEOF_WCHAR_T__=4");
			cpp_define(parse_in, "__UINT64_TYPE__=long");
			cpp_define(parse_in, "__GNUC_PATCHLEVEL__=0");
			cpp_define(parse_in, "__FLT128_NORM_MAX__=1.18973149535723176508575932662800702e+4932F128");
			cpp_define(parse_in, "__FLT64_NORM_MAX__=1.79769313486231570814527423731704357e+308F64");
			cpp_define(parse_in, "__FLT128_HAS_QUIET_NAN__=1");
			cpp_define(parse_in, "__INTMAX_MAX__=0x7fffffffffffffffL");
			cpp_define(parse_in, "__INT_FAST8_TYPE__=signed");
			cpp_define(parse_in, "__FLT64X_MIN__=3.36210314311209350626267781732175260e-4932F64x");
			cpp_define(parse_in, "__GNUC_STDC_INLINE__=1");
			cpp_define(parse_in, "__FLT64_HAS_DENORM__=1");
			cpp_define(parse_in, "__FLT32_EPSILON__=1.19209289550781250000000000000000000e-7F32");
			cpp_define(parse_in, "__DBL_DECIMAL_DIG__=17");
			//cpp_define(parse_in, "__STDC_UTF_32__=1");
			cpp_define(parse_in, "__INT_FAST8_WIDTH__=8");
			cpp_define(parse_in, "__FXSR__=1");
			cpp_define(parse_in, "__FLT32X_MAX__=1.79769313486231570814527423731704357e+308F32x");
			cpp_define(parse_in, "__DBL_NORM_MAX__=((double)1.79769313486231570814527423731704357e+308L)");
			cpp_define(parse_in, "__BYTE_ORDER__=__ORDER_LITTLE_ENDIAN__");
			cpp_define(parse_in, "__INTMAX_WIDTH__=64");
			cpp_define(parse_in, "__UINT32_C(c)=c");
			cpp_define(parse_in, "__FLT_DENORM_MIN__=1.40129846432481707092372958328991613e-45F");
			cpp_define(parse_in, "__INT8_MAX__=0x7f");
			cpp_define(parse_in, "__LONG_WIDTH__=64");
			cpp_define(parse_in, "__UINT_FAST32_TYPE__=long");
			cpp_define(parse_in, "__FLT32X_NORM_MAX__=1.79769313486231570814527423731704357e+308F32x");
			cpp_define(parse_in, "__CHAR32_TYPE__=unsigned");
			cpp_define(parse_in, "__FLT_MAX__=3.40282346638528859811704183484516925e+38F");
			cpp_define(parse_in, "__SSE2__=1");
			cpp_define(parse_in, "__INT32_TYPE__=int");
			cpp_define(parse_in, "__SIZEOF_DOUBLE__=8");
			cpp_define(parse_in, "__FLT_MIN_10_EXP__=(-37)");
			cpp_define(parse_in, "__FLT64_MIN__=2.22507385850720138309023271733240406e-308F64");
			cpp_define(parse_in, "__INT_LEAST32_WIDTH__=32");
			cpp_define(parse_in, "__INTMAX_TYPE__=long");
			cpp_define(parse_in, "__DEC128_MAX_EXP__=6145");
			cpp_define(parse_in, "__FLT32X_HAS_QUIET_NAN__=1");
			cpp_define(parse_in, "__ATOMIC_CONSUME=1");
			cpp_define(parse_in, "__GNUC_MINOR__=2");
			cpp_define(parse_in, "__INT_FAST16_WIDTH__=64");
			cpp_define(parse_in, "__UINTMAX_MAX__=0xffffffffffffffffUL");
			cpp_define(parse_in, "__FLT32X_DENORM_MIN__=4.94065645841246544176568792868221372e-324F32x");
			cpp_define(parse_in, "__DBL_MAX_10_EXP__=308");
			cpp_define(parse_in, "__LDBL_DENORM_MIN__=3.64519953188247460252840593361941982e-4951L");
			cpp_define(parse_in, "__INT16_C(c)=c");
			//cpp_define(parse_in, "__STDC__=1");
			cpp_define(parse_in, "__PTRDIFF_TYPE__=long");
			cpp_define(parse_in, "__ATOMIC_SEQ_CST=5");
			cpp_define(parse_in, "__FLT32X_MIN_10_EXP__=(-307)");
			cpp_define(parse_in, "__UINTPTR_TYPE__=long");
			cpp_define(parse_in, "__DEC64_SUBNORMAL_MIN__=0.000000000000001E-383DD");
			cpp_define(parse_in, "__DEC128_MANT_DIG__=34");
			cpp_define(parse_in, "__LDBL_MIN_10_EXP__=(-4931)");
			cpp_define(parse_in, "__SIZEOF_LONG_LONG__=8");
			cpp_define(parse_in, "__FLT128_DECIMAL_DIG__=36");
			cpp_define(parse_in, "__GCC_ATOMIC_LLONG_LOCK_FREE=2");
			cpp_define(parse_in, "__FLT32_HAS_QUIET_NAN__=1");
			cpp_define(parse_in, "__FLT_DECIMAL_DIG__=9");
			cpp_define(parse_in, "__UINT_FAST16_MAX__=0xffffffffffffffffUL");
			cpp_define(parse_in, "__LDBL_NORM_MAX__=1.18973149535723176502126385303097021e+4932L");
			cpp_define(parse_in, "__GCC_ATOMIC_SHORT_LOCK_FREE=2");
			cpp_define(parse_in, "__UINT_FAST8_TYPE__=unsigned");
			cpp_define(parse_in, "__ATOMIC_ACQ_REL=4");
			cpp_define(parse_in, "__ATOMIC_RELEASE=3");
			unsigned char buffer[1024];
			const cpp_token*token=nullptr;
			token=cpp_get_token(parse_in);
			while (token && token->type!=CPP_EOF)
			{
				if (token->type != CPP_PADDING)
				{
					cout<<"token-type:"<<TOKEN_NAME(token)<<" of : ";
					unsigned char*ptr=cpp_spell_token(parse_in, token, buffer, true);
					cout<<string((const char*)(buffer), ptr-buffer)<<endl;
				}
				token=cpp_get_token(parse_in);
			}

			//location_t loc;
			//FILE*output=fopen("/tmp/output.txt", "w+b");
			//FILE*output=stdout;
//			char*buffer=NULL;
//			size_t bufSize=0;
//			size_t curSize=0;
//			FILE* output=open_memstream(&buffer, &bufSize);
//			token=cpp_get_token(parse_in);
//			while (token && token->type!=CPP_EOF)
//			{
//				//cout<<"."<<endl;
////				unsigned char*ptr=cpp_spell_token(parse_in, token, buffer, true);
////				cout<<ptr<<endl;
//				//cout<<cpp_token_as_text(parse_in, token)<<endl;
//				//cpp_error_at(parse_in, CPP_DL_NOTE, loc, "%s", cpp_token_as_text(parse_in, token));
//				cpp_output_token(token, output);
//				fflush(output);
//				string str(buffer+curSize, bufSize-curSize);
//				cout<<"token-type:"<<cppTokenTypeName(token)<<" of " <<str<<endl;
//				curSize=bufSize;
//				token=cpp_get_token(parse_in);
//			}
//			free(buffer);
			//fclose(output);
		}
	}
	return 0;
}
