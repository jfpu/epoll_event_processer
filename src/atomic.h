#ifndef __ARCH_I386_ATOMIC__
#define __ARCH_I386_ATOMIC__

/*
 * Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc..
 */
#define CONFIG_SMP 1

#ifdef CONFIG_SMP
#define LOCK "lock ; "
#else
#define LOCK ""
#endif

/*
 * Make sure gcc doesn't try to be clever and move things around
 * on us. We need to use _exactly_ the address the user gave us,
 * not some alias that contains the same information.
 */
typedef struct { volatile int counter; } atomic_t;

#define ATOMIC_INIT(i)  ( (atomic_t) { (i) } )  // { (i) }

#define atomic_read(v)  ((v)->counter)
#define atomic_set(v, i) (((v)->counter) = (i))

static __inline__ void atomic_add(int i, atomic_t *v) {
  __asm__ __volatile__(
    LOCK "addl %1,%0"
    :"=m" (v->counter)
    :"ir" (i), "m" (v->counter));
}

static __inline__ void atomic_sub(int i, atomic_t *v) {
  __asm__ __volatile__(
    LOCK "subl %1,%0"
    :"=m" (v->counter)
    :"ir" (i), "m" (v->counter));
}

static __inline__ int atomic_sub_and_test(int i, atomic_t *v) {
  unsigned char c;

  __asm__ __volatile__(
    LOCK "subl %2,%0; sete %1"
    :"=m" (v->counter), "=qm" (c)
    :"ir" (i), "m" (v->counter) : "memory");
  return c;
}

static __inline__ void atomic_inc(atomic_t *v) {
  __asm__ __volatile__(
    LOCK "incl %0"
    :"=m" (v->counter)
    :"m" (v->counter));
}

static __inline__ void atomic_dec(atomic_t *v) {
  __asm__ __volatile__(
    LOCK "decl %0"
    :"=m" (v->counter)
    :"m" (v->counter));
}

static __inline__ int atomic_dec_and_test(atomic_t *v) {
  unsigned char c;

  __asm__ __volatile__(
    LOCK "decl %0; sete %1"
    :"=m" (v->counter), "=qm" (c)
    :"m" (v->counter) : "memory");
  return c != 0;
}

static __inline__ int atomic_inc_and_test(atomic_t *v) {
  unsigned char c;

  __asm__ __volatile__(
    LOCK "incl %0; sete %1"
    :"=m" (v->counter), "=qm" (c)
    :"m" (v->counter) : "memory");
  return c != 0;
}

static __inline__ int atomic_add_negative(int i, atomic_t *v) {
  unsigned char c;

  __asm__ __volatile__(
    LOCK "addl %2,%0; sets %1"
    :"=m" (v->counter), "=qm" (c)
    :"ir" (i), "m" (v->counter) : "memory");
  return c;
}

/**
 * atomic_add_return - add and return
 * @i: integer value to add
 * @v: pointer of type atomic_t
 *
 * Atomically adds @i to @v and returns @i + @v
 */
static __inline__ int atomic_add_return(int i, atomic_t *v) {
  int __i = i;
  __asm__ __volatile__(
    LOCK "xaddl %0, %1"
    :"+r" (i), "+m" (v->counter)
    : : "memory");
  return i + __i;
}

static __inline__ int atomic_sub_return(int i, atomic_t *v) {
  return atomic_add_return(-i, v);
}

#define atomic_inc_return(v)  (atomic_add_return(1, v))
#define atomic_dec_return(v)  (atomic_sub_return(1, v))


/* These are x86-specific, used by some header files */
#define atomic_clear_mask(mask, addr) \
  __asm__ __volatile__(LOCK "andl %0, %1" \
  : : "r" (~(mask)), "m" (*addr) : "memory")

#define atomic_set_mask(mask, addr) \
  __asm__ __volatile__(LOCK "orl %0, %1" \
  : : "r" (mask), "m" (*addr) : "memory")

#endif
