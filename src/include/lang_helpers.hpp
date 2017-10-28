
#include "types.hpp"

#define RZ_COMP_GCC				1
#define RZ_COMP_LLVM			2
#define RZ_COMP_MSVC			3
// Determining the compiler
#if !defined RZ_COMP
	#if _MSC_VER && !__INTELRZ_COMPILER && !__clang__
		#define RZ_COMP RZ_COMP_MSVC
	#elif __GNUC__ && !__clang__
		#define RZ_COMP RZ_COMP_GCC
	#elif __clang__
		#define RZ_COMP RZ_COMP_LLVM
	#else
		#warning Cannot determine compiler!.
	#endif
#endif

#define RZ_ARCH_X64				1
#define RZ_ARCH_ARM_CORTEX_M4	2
#define RZ_ARCH_ARM_V6_HF		3

#define RZ_PLATF_GENERIC_WIN	1
#define RZ_PLATF_GENERIC_UNIX	2
#define RZ_PLATF_NONE			3

#undef FORCEINLINE

#if RZ_COMP == RZ_COMP_MSVC
	#define FORCEINLINE						__forceinline
	#define NOINLINE						__declspec(noinline)
	#define BUILTIN_F32_INF					((float)(1e+300 * 1e+300))
	#define BUILTIN_F64_INF					(1e+300 * 1e+300)
	#define BUILTIN_F32_QNAN				__builtin_nanf("0")
	#define BUILTIN_F64_QNAN				__builtin_nan("0")
	#define DBGBREAK						__debugbreak()
#elif RZ_COMP == RZ_COMP_LLVM
	#define FORCEINLINE						__attribute__((always_inline)) inline
	#define NOINLINE						__attribute__((noinline))
	#define BUILTIN_F32_INF					(__builtin_inff())
	#define BUILTIN_F64_INF					(__builtin_inf())
	#define BUILTIN_F32_QNAN				__builtin_nan("0")
	#define BUILTIN_F64_QNAN				__builtin_nan("0")
	#define DBGBREAK						do { asm volatile ("int3"); } while(0)
#elif RZ_COMP == RZ_COMP_GCC
	#define FORCEINLINE						__attribute__((always_inline)) inline
	#define NOINLINE						__attribute__((noinline))
	#define BUILTIN_F32_INF					(__builtin_inff())
	#define BUILTIN_F64_INF					(__builtin_inf())
	#define BUILTIN_F32_QNAN				__builtin_nan("0")
	#define BUILTIN_F64_QNAN				__builtin_nan("0")
	
	#if RZ_PLATF == RZ_PLATF_GENERIC_WIN
		#define DBGBREAK					do { __debugbreak(); } while(0)
	#elif RZ_PLATF == RZ_PLATF_GENERIC_UNIX
		#if RZ_ARCH == RZ_ARCH_ARM_V6_HF
			#define DBGBREAK				do { asm volatile ("bkpt #0"); } while(0)
		#endif
	#endif
#endif

#if RZ_PLATF == RZ_PLATF_GENERIC_WIN
	
	#if RZ_DBG // try to not use windows lib directly to simplify porting, but still allow for debugging (Sleep() for ex.)
		
		#define DWORD unsigned long
		#define BOOL int
		
		// For debugging
		__declspec(dllimport) BOOL __stdcall IsDebuggerPresent(void);
		
		__declspec(dllimport) void __stdcall Sleep(
		  DWORD dwMilliseconds
		);
		
		#define IS_DEBUGGER_PRESENT				IsDebuggerPresent()
		#define DBGBREAK_IF_DEBUGGER_PRESENT	if (IS_DEBUGGER_PRESENT) { DBGBREAK; }
		#define BREAK_IF_DEBUGGING_ELSE_STALL	if (IS_DEBUGGER_PRESENT) { DBGBREAK; } else { Sleep(100); }
		
		static void dbg_sleep (f32 sec) {
			Sleep( (DWORD)(sec * 1000.0f) );
		}
		
		#undef BOOL
		#undef DWORD
		
	#endif
	
#endif

////
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

#define STATIC_ASSERT(cond) static_assert((cond), STRINGIFY(cond))

#define ANSI_COLOUR_CODE_RED	"\033[1;31m"
#define ANSI_COLOUR_CODE_YELLOW	"\033[0;33m"
#define ANSI_COLOUR_CODE_NC		"\033[0m"


#include "assert.hpp"

//
template<typename CAST_T, typename T>
static constexpr bool _safe_cast (T x);

template<> constexpr bool _safe_cast<u32, u64> (u64 x) { return x <= 0xffffffffull; }
template<> constexpr bool _safe_cast<s32, u64> (u64 x) { return x <= 0x7fffffffull; }

#define safe_cast(cast_t, val) _safe_cast<cast_t>(val)

//
template <typename T=u32, typename AT, uptr N>
static constexpr T _arrlen (AT (& arr)[N]) {
	static_assert(safe_cast(T, N), "arrlen:: array length too large for output type!");
	return (T)N;
}
template <typename T=u32, typename AT, uptr M, uptr N>
static constexpr T _arrlen (AT (& arr)[M][N]) {
	static_assert(safe_cast(T, M*N), "arrlen:: array length too large for output type!");
	return (T)(M*N);
}
#define arrlent(T, arr) _arrlen<T>(arr)
#define arrlen(arr) _arrlen<>(arr)

static u32 strlen (utf32 const* str) {
	u32 ret = 0;
	while (*str++) ++ret;
	return ret;
}
template <u32 N>
static u32 strlen (utf32 const (& str)[N]) {
	STATIC_ASSERT(N >= 1);
	return N -1;
}

template <typename FUNC>
struct At_Scope_Exit {
	FUNC	f;
	void operator= (At_Scope_Exit &) = delete;
	
	FORCEINLINE At_Scope_Exit (FUNC f): f(f) {}
	FORCEINLINE ~At_Scope_Exit () { f(); }
};

struct _Defer_Helper {};

template<typename FUNC>
static FORCEINLINE At_Scope_Exit<FUNC> operator+(_Defer_Helper, FUNC f) {
	return At_Scope_Exit<FUNC>(f);
}

#define CONCAT(a,b) a##b

#define _defer(counter) auto CONCAT(_defer_helper, counter) = _Defer_Helper() +[&] () 
#define defer _defer(__COUNTER__)
// use like: defer { lambda code };

#undef DEFINE_ENUM_FLAG_OPS
#define DEFINE_ENUM_FLAG_OPS(TYPE, UNDERLYING_TYPE) \
	static FORCEINLINE TYPE& operator|= (TYPE& l, TYPE r) { \
		return l = (TYPE)((UNDERLYING_TYPE)l | (UNDERLYING_TYPE)r); \
	} \
	static FORCEINLINE TYPE& operator&= (TYPE& l, TYPE r) { \
		return l = (TYPE)((UNDERLYING_TYPE)l & (UNDERLYING_TYPE)r); \
	} \
	static FORCEINLINE TYPE operator| (TYPE l, TYPE r) { \
		return (TYPE)((UNDERLYING_TYPE)l | (UNDERLYING_TYPE)r); \
	} \
	static FORCEINLINE TYPE operator& (TYPE l, TYPE r) { \
		return (TYPE)((UNDERLYING_TYPE)l & (UNDERLYING_TYPE)r); \
	} \
	static FORCEINLINE TYPE operator~ (TYPE e) { \
		return (TYPE)(~(UNDERLYING_TYPE)e); \
	}

#define DEFINE_ENUM_ITER_OPS(TYPE, UNDERLYING_TYPE) \
	static FORCEINLINE TYPE& operator++ (TYPE& val) { \
		return val = (TYPE)((UNDERLYING_TYPE)val +1); \
	}

#include <initializer_list>

template <typename T, typename LEN_T=u32>
struct array {
	T*		arr;
	LEN_T	len;
	
	array () {}
	array (T* a, LEN_T l): arr{a}, len{l} {}
	constexpr array (std::initializer_list<T> l): arr{ (T*)l.begin() }, len{ (LEN_T)l.size() } {
				//static_assert(safe_cast(LEN_T, l.size()), "array<> :: initializer_list.size() out of range for len!"); // can't do this in c++
			}
	template<LEN_T N>			constexpr array (T (& a)[N]): arr{a}, len{N} {}
	
	operator array<T const> () { return {arr, len}; }
	
	static array malloc (LEN_T len) {
		return { (T*)::malloc(len*sizeof(T)), len };
	}
	void free () {
		::free(arr);
	}
	void realloc (LEN_T new_len) {
		arr = (T*)::realloc(arr, new_len*sizeof(T));
		len = new_len;
	}
	
	T cr operator[] (LEN_T indx) const {
		dbg_assert(indx >= 0 && indx < len, "array:: operator[]: indx: %d len: %d", indx, len);
		return arr[indx];
	}
	T& operator[] (LEN_T indx) {
		dbg_assert(indx >= 0 && indx < len, "array:: operator[]: indx: %d len: %d", indx, len);
		return arr[indx];
	}
	
	T*					begin () {					return arr; }
	constexpr T const*	begin () const {			return arr; }
	
	T*					end () {					return &arr[len]; }
	constexpr T const*	end () const {				return &arr[len]; }
	
	LEN_T				get_i (T const* it) {		return it -arr; }
	constexpr LEN_T		get_i (T const* it) const {	return it -arr; }
	
};

template <typename T, typename LEN_T=u32>
struct dynarr : array<T, LEN_T> {
	
	static dynarr malloc (LEN_T new_len) {
		dynarr ret = {};
		ret.realloc(new_len);
		ret.len = new_len;
		return ret;
	}
	void free () {
		::free(this->arr);
		this->len = 0;
	}
	void realloc (LEN_T new_len) {
		this->arr = (T*)::realloc(this->arr, new_len*sizeof(T));
		this->len = new_len;
	}
	
	LEN_T grow_by (LEN_T diff) {
		LEN_T old_len = this->len;
		realloc(this->len +diff);
		return old_len;
	}
	void shrink_by (LEN_T diff) {
		dbg_assert(diff <= this->len);
		realloc(this->len -diff);
	}
	
	T& push () {
		LEN_T old_len = grow_by(1);
		return this->arr[old_len];
	}
	void push (T cr val) {
		push() = val;
	}
	
	LEN_T pushn (LEN_T count) {
		return grow_by(count);
	}
	#if 0
	T* pushn (T const* val, LEN_T count) {
		LEN_T old_len = this->len;
		grow_by(count);
		return copy_values(old_len, val, count);
	}
	#endif
	
	void delete_by_moving_last (LEN_T indx) {
		dbg_assert(indx >= 0 && indx < this->len);
		if (this->len > 1) {
			this->arr[indx] = this->arr[this->len -1];
		}
		shrink_by(1);
	}
};

static void print_array (array<char>* arr, cstr format, ...) {
	
	va_list vl;
	va_start(vl, format);
	
	for (;;) {
		auto ret = vsnprintf(arr->arr, arr->len, format, vl);
		dbg_assert(ret >= 0);
		if ((u32)ret < arr->len) break;
		// buffer was to small, increase buffer size
		arr->realloc((u32)ret +1);
		// now snprintf has to succeed, so call it again
	}
	
	va_end(vl);
}
