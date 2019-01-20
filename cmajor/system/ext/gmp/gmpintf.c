#include "gmpintf.h"
#include "gmp.h"
#include <stdlib.h>

// integer functions:

GMP_API void* create_mpz()
{
    mpz_t* mpz = (mpz_t*)malloc(sizeof(mpz_t));
    mpz_init(*mpz);
    return mpz;
}

GMP_API void destroy_mpz(void* mpz)
{
    mpz_t* m = (mpz_t*)mpz;
    mpz_clear(*m);
    free(mpz);
}

GMP_API void assign_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_set(*left, *right);
}

GMP_API void neg_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_neg(*left, *right);
}

GMP_API void abs_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_abs(*left, *right);
}

GMP_API void assign_mpz_ui(void* mpz_left, uint32_t right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_set_ui(*left, right);
}

GMP_API void assign_mpz_si(void* mpz_left, int32_t right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_set_si(*left, right);
}

GMP_API int32_t assign_mpz_str(void* mpz_handle, const char* str, int32_t base)
{
    mpz_t* mpz = (mpz_t*)mpz_handle;
    return mpz_set_str(*mpz, str, base);
}

GMP_API void swap_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_swap(*left, *right);
}

GMP_API char* get_mpz_str(void* mpz_handle, int32_t base)
{
    mpz_t* mpz = (mpz_t*)mpz_handle;
    return mpz_get_str(NULL, base, *mpz);
}

GMP_API void free_mpz_str(char* mpz_str)
{
    free(mpz_str);
}

GMP_API void add_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_add(*target, *left, *right);
}

GMP_API void sub_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_sub(*target, *left, *right);
}

GMP_API void mul_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_mul(*target, *left, *right);
}

GMP_API void div_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_tdiv_q(*target, *left, *right);
}

GMP_API void rem_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_tdiv_r(*target, *left, *right);
}

GMP_API int32_t cmp_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    return mpz_cmp(*left, *right);
}

GMP_API void and_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_and(*target, *left, *right);
}

GMP_API void or_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_ior(*target, *left, *right);
}

GMP_API void xor_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_xor(*target, *left, *right);
}

GMP_API void cpl_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_com(*left, *right);
}

GMP_API void setbit_mpz(void* mpz_handle, uint32_t bit_index)
{
    mpz_t* mpz = (mpz_t*)mpz_handle;
    mpz_setbit(*mpz, (mp_bitcnt_t)bit_index);
}

GMP_API void clrbit_mpz(void* mpz_handle, uint32_t bit_index)
{
    mpz_t* mpz = (mpz_t*)mpz_handle;
    mpz_clrbit(*mpz, (mp_bitcnt_t)bit_index);
}

GMP_API void cplbit_mpz(void* mpz_handle, uint32_t bit_index)
{
    mpz_t* mpz = (mpz_t*)mpz_handle;
    mpz_combit(*mpz, (mp_bitcnt_t)bit_index);
}

GMP_API int32_t tstbit_mpz(void* mpz_handle, uint32_t bit_index)
{
    mpz_t* mpz = (mpz_t*)mpz_handle;
    return mpz_tstbit(*mpz, (mp_bitcnt_t)bit_index);
}

// rational functions:

GMP_API void* create_mpq()
{
    mpq_t* mpq = (mpq_t*)malloc(sizeof(mpq_t));
    mpq_init(*mpq);
    return mpq;
}

GMP_API void destroy_mpq(void* mpq)
{
    mpq_t* m = (mpq_t*)mpq;
    mpq_clear(*m);
    free(mpq);
}

GMP_API void canonicalize_mpq(void* mpq)
{
    mpq_t* subject = (mpq_t*)mpq;
    mpq_canonicalize(*subject);
}

GMP_API void assign_mpq(void* mpq_left, void* mpq_right)
{
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_set(*left, *right);
}

GMP_API int32_t assign_mpq_str(void* mpq_handle, const char* str, int32_t base)
{
    mpq_t* mpq = (mpq_t*)mpq_handle;
    int result = mpq_set_str(*mpq, str, base);
    if (result == 0)
    {
        canonicalize_mpq(mpq);
    }
    return result;
}

GMP_API void set_mpq_si(void* mpq_handle, int32_t n, int32_t d)
{
    mpq_t* mpq = (mpq_t*)mpq_handle;
    mpq_set_si(*mpq, n, d);
}

GMP_API void set_mpq_ui(void* mpq_handle, uint32_t n, uint32_t d)
{
    mpq_t* mpq = (mpq_t*)mpq_handle;
    mpq_set_ui(*mpq, n, d);
}

GMP_API char* get_mpq_str(void* mpq_handle, int32_t base)
{
    mpq_t* mpq = (mpq_t*)mpq_handle;
    return mpq_get_str(NULL, base, *mpq);
}

GMP_API void free_mpq_str(char* mpq_str)
{
    free(mpq_str);
}

GMP_API void set_mpq_z(void* mpq_left, void* mpz_right)
{
    mpq_t* left = (mpq_t*)mpq_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpq_set_z(*left, *right);
}

GMP_API void add_mpq(void* mpq_target, void* mpq_left, void* mpq_right)
{
    mpq_t* target = (mpq_t*)mpq_target;
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_add(*target, *left, *right);
    canonicalize_mpq(target);
}

GMP_API void sub_mpq(void* mpq_target, void* mpq_left, void* mpq_right)
{
    mpq_t* target = (mpq_t*)mpq_target;
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_sub(*target, *left, *right);
    canonicalize_mpq(target);
}

GMP_API void mul_mpq(void* mpq_target, void* mpq_left, void* mpq_right)
{
    mpq_t* target = (mpq_t*)mpq_target;
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_mul(*target, *left, *right);
    canonicalize_mpq(target);
}

GMP_API void div_mpq(void* mpq_target, void* mpq_left, void* mpq_right)
{
    mpq_t* target = (mpq_t*)mpq_target;
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_div(*target, *left, *right);
    canonicalize_mpq(target);
}

GMP_API void neg_mpq(void* mpq_left, void* mpq_right)
{
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_neg(*left, *right);
    canonicalize_mpq(left);
}

GMP_API void abs_mpq(void* mpq_left, void* mpq_right)
{
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_abs(*left, *right);
    canonicalize_mpq(left);
}

GMP_API int32_t cmp_mpq(void* mpq_left, void* mpq_right)
{
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    return mpq_cmp(*left, *right);
}

GMP_API int32_t equal_mpq(void* mpq_left, void* mpq_right)
{
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    return mpq_equal(*left, *right);
}

GMP_API void get_numerator_mpq(void* mpz_numerator, void* mpq_rational)
{
    mpz_t* numerator = (mpz_t*)mpz_numerator;
    mpq_t* rational = (mpq_t*)mpq_rational;
    mpq_get_num(*numerator, *rational);
}

GMP_API void get_denominator_mpq(void* mpz_denominator, void* mpq_rational)
{
    mpz_t* denominator = (mpz_t*)mpz_denominator;
    mpq_t* rational = (mpq_t*)mpq_rational;
    mpq_get_den(*denominator, *rational);
}

// float functions:

GMP_API void set_default_prec_mpf(uint32_t prec)
{
    mpf_set_default_prec(prec);
}

GMP_API uint32_t get_default_prec_mpf()
{
    return mpf_get_default_prec();
}

GMP_API void* create_mpf()
{
    mpf_t* mpf = (mpf_t*)malloc(sizeof(mpf_t));
    mpf_init(*mpf);
    return mpf;
}

GMP_API void* create_mpf_prec(uint32_t prec)
{
    mpf_t* mpf = (mpf_t*)malloc(sizeof(mpf_t));
    mpf_init2(*mpf, prec);
    return mpf;
}

GMP_API void destroy_mpf(void* mpf)
{
    mpf_t* m = (mpf_t*)mpf;
    mpf_clear(*m);
    free(mpf);
}

GMP_API uint32_t get_prec_mpf(void* mpf)
{
    mpf_t* m = (mpf_t*)mpf;
    return mpf_get_prec(*m);
}

GMP_API void set_prec_mpf(void* mpf, uint32_t prec)
{
    mpf_t* m = (mpf_t*)mpf;
    mpf_set_prec(*m, prec);
}

GMP_API void set_mpf(void* mpf_left, void* mpf_right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_t* right = (mpf_t*)mpf_right;
    mpf_set(*left, *right);
}

GMP_API void set_mpf_ui(void* mpf_left, uint32_t right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_set_ui(*left, right);
}

GMP_API void set_mpf_si(void* mpf_left, int32_t right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_set_si(*left, right);
}

GMP_API void set_mpf_d(void* mpf_left, double right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_set_d(*left, right);
}

GMP_API void set_mpf_z(void* mpf_left, void* mpz_right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpf_set_z(*left, *right);
}

GMP_API void set_mpf_q(void* mpf_left, void* mpq_right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpf_set_q(*left, *right);
}

GMP_API int32_t set_mpf_str(void* mpf_left, const char* str, int32_t base)
{
    mpf_t* left = (mpf_t*)mpf_left;
    return mpf_set_str(*left, str, base);
}

GMP_API char* get_mpf_str(void* mpf_handle, int32_t base_, uint32_t numDigits, int64_t* exponent)
{
    mpf_t* subject = (mpf_t*)mpf_handle;
    return mpf_get_str(NULL, (mp_exp_t*)exponent, base_, numDigits, *subject);
}

GMP_API void free_mpf_str(char* mpf_str)
{
    free(mpf_str);
}

GMP_API void add_mpf(void* mpf_target, void* mpf_left, void* mpf_right)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_t* right = (mpf_t*)mpf_right;
    mpf_add(*target, *left, *right);
}

GMP_API void sub_mpf(void* mpf_target, void* mpf_left, void* mpf_right)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_t* right = (mpf_t*)mpf_right;
    mpf_sub(*target, *left, *right);
}

GMP_API void mul_mpf(void* mpf_target, void* mpf_left, void* mpf_right)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_t* right = (mpf_t*)mpf_right;
    mpf_mul(*target, *left, *right);
}

GMP_API void div_mpf(void* mpf_target, void* mpf_left, void* mpf_right)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_t* right = (mpf_t*)mpf_right;
    mpf_div(*target, *left, *right);
}

GMP_API void sqrt_mpf(void* mpf_target, void* mpf_subject)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* subject = (mpf_t*)mpf_subject;
    mpf_sqrt(*target, *subject);
}

GMP_API void neg_mpf(void* mpf_target, void* mpf_subject)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* subject = (mpf_t*)mpf_subject;
    mpf_neg(*target, *subject);
}

GMP_API void abs_mpf(void* mpf_target, void* mpf_subject)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* subject = (mpf_t*)mpf_subject;
    mpf_abs(*target, *subject);
}

GMP_API int32_t cmp_mpf(void* mpf_left, void* mpf_right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_t* right = (mpf_t*)mpf_right;
    return mpf_cmp(*left, *right);
}

GMP_API void ceil_mpf(void* mpf_target, void* mpf_subject)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* subject = (mpf_t*)mpf_subject;
    mpf_ceil(*target, *subject);
}

GMP_API void floor_mpf(void* mpf_target, void* mpf_subject)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* subject = (mpf_t*)mpf_subject;
    mpf_floor(*target, *subject);
}

GMP_API void trunc_mpf(void* mpf_target, void* mpf_subject)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* subject = (mpf_t*)mpf_subject;
    mpf_trunc(*target, *subject);
}

GMP_API double get_d_mpf(void* mpf)
{
    mpf_t* m = (mpf_t*)mpf;
    return mpf_get_d(*m);
}

