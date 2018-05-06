// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_ATOMIC_H_
#define _SOFA_PBRPC_ATOMIC_H_

#if !defined(__i386__) && !defined(__x86_64__)
#error    "Arch not supprot!"
#endif

#include <stdint.h>

#include <boost/atomic/detail/interlocked.hpp>

namespace sofa {
namespace pbrpc {

template <typename T>
inline void atomic_inc(volatile T* n)
{
    //asm volatile ("lock; incl %0;":"+m"(*n)::"cc");
	BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD(n,1);
}
template <typename T>
inline void atomic_dec(volatile T* n)
{
    //asm volatile ("lock; decl %0;":"+m"(*n)::"cc");
	BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD(n,-1);
}
template <typename T>
inline T atomic_add_ret_old(volatile T* n, T v)
{
   // asm volatile ("lock; xaddl %1, %0;":"+m"(*n),"+r"(v)::"cc");
    return BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD(n,v);
}
template <typename T>
inline T atomic_inc_ret_old(volatile T* n)
{
    return BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD(n,1);
}
template <typename T>
inline T atomic_dec_ret_old(volatile T* n)
{
    return BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD(n,-1);
}
template <typename T>
inline T atomic_add_ret_old64(volatile T* n, T v)
{
#ifdef _WIN32
	volatile T expected = *n;
	for(;;)
	{	
		T original =_InterlockedCompareExchange64((volatile __int64 *)n,expected+v,expected);
		if (original == expected)
			return expected;
		expected = original;
	}
#else
    return BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD64(n,v);
#endif
}
template <typename T>
inline T atomic_inc_ret_old64(volatile T* n)
{
#ifdef _WIN32
	T add  = 1;
	return atomic_add_ret_old64(n,add);
#else
	return BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD64(n,1);
#endif
}
template <typename T>
inline T atomic_dec_ret_old64(volatile T* n)
{
#ifdef _WIN32
	T add  = -1;
	return atomic_add_ret_old64(n,add);
#else
	return BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD64(n,-1);
#endif
}
template <typename T>
inline void atomic_add(volatile T* n, T v)
{
	BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD(n,v);
}
template <typename T>
inline void atomic_sub(volatile T* n, T v)
{
	T tmp = 0 - v;
	BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD(n,tmp);
}
template <typename T, typename C, typename D>
inline T atomic_cmpxchg(volatile T* n, C cmp, D dest)
{
    return BOOST_ATOMIC_INTERLOCKED_COMPARE_EXCHANGE(n,dest,cmp);
}
// return old value
template <typename T>
inline T atomic_swap(volatile T* lockword, T value)
{
    return  BOOST_ATOMIC_INTERLOCKED_EXCHANGE(lockword,value);
}
template <typename T, typename E, typename C>
inline T atomic_comp_swap(volatile T* lockword, E exchange, C comperand)
{
    return atomic_cmpxchg(lockword, comperand, exchange);
}

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_ATOMIC_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
