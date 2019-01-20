#ifndef __GMP_INTF_H__
#define __GMP_INTF_H__
#include <stdint.h>

#ifdef __WIN32
#ifndef CMRT_IMPORT
#define GMP_API __declspec(dllexport)
#else
#define GMP_API
#endif
#else
#define GMP_API
#endif

#if defined (__cplusplus)
extern "C" {
#endif

// integer functions:

GMP_API void* create_mpz();
GMP_API void destroy_mpz(void* mpz);
GMP_API void assign_mpz(void* mpz_left, void* mpz_right);
GMP_API void neg_mpz(void* mpz_left, void* mpz_right);
GMP_API void abs_mpz(void* mpz_left, void* mpz_right);
GMP_API void assign_mpz_ui(void* mpz_left, uint32_t right);
GMP_API void assign_mpz_si(void* mpz_left, int32_t right);
GMP_API int32_t assign_mpz_str(void* mpz_handle, const char* str, int32_t base);
GMP_API void swap_mpz(void* mpz_left, void* mpz_right);
GMP_API char* get_mpz_str(void* mpz_handle, int32_t base);
GMP_API void free_mpz_str(char* mzp_str);
GMP_API void add_mpz(void* mpz_target, void* mpz_left, void* mpz_right);
GMP_API void sub_mpz(void* mpz_target, void* mpz_left, void* mpz_right);
GMP_API void mul_mpz(void* mpz_target, void* mpz_left, void* mpz_right);
GMP_API void div_mpz(void* mpz_target, void* mpz_left, void* mpz_right);
GMP_API void rem_mpz(void* mpz_target, void* mpz_left, void* mpz_right);
GMP_API int32_t cmp_mpz(void* mpz_left, void* mpz_right);
GMP_API void and_mpz(void* mpz_target, void* mpz_left, void* mpz_right);
GMP_API void or_mpz(void* mpz_target, void* mpz_left, void* mpz_right);
GMP_API void xor_mpz(void* mpz_target, void* mpz_left, void* mpz_right);
GMP_API void cpl_mpz(void* mpz_left, void* mpz_right);
GMP_API void setbit_mpz(void* mpz_handle, uint32_t bit_index);
GMP_API void clrbit_mpz(void* mpz_handle, uint32_t bit_index);
GMP_API void cplbit_mpz(void* mpz_handle, uint32_t bit_index);
GMP_API int32_t tstbit_mpz(void* mpz_handle, uint32_t bit_index);

// rational functions:

GMP_API void* create_mpq();
GMP_API void destroy_mpq(void* mpq);
GMP_API void canonicalize_mpq(void* mpq);
GMP_API void assign_mpq(void* mpq_left, void* mpq_right);
GMP_API int32_t assign_mpq_str(void* mpq_handle, const char* str, int32_t base);
GMP_API void set_mpq_si(void* mpq_handle, int32_t n, int32_t d);
GMP_API void set_mpq_ui(void* mpq_handle, uint32_t n, uint32_t d);
GMP_API char* get_mpq_str(void* mpq_handle, int32_t base);
GMP_API void free_mpq_str(char* mpq_str);
GMP_API void set_mpq_z(void* mpq_left, void* mpz_right);
GMP_API void add_mpq(void* mpq_target, void* mpq_left, void* mpq_right);
GMP_API void sub_mpq(void* mpq_target, void* mpq_left, void* mpq_right);
GMP_API void mul_mpq(void* mpq_target, void* mpq_left, void* mpq_right);
GMP_API void div_mpq(void* mpq_target, void* mpq_left, void* mpq_right);
GMP_API void neg_mpq(void* mpq_left, void* mpq_right);
GMP_API void abs_mpq(void* mpq_left, void* mpq_right);
GMP_API int32_t cmp_mpq(void* mpq_left, void* mpq_right);
GMP_API int32_t equal_mpq(void* mpq_left, void* mpq_right);
GMP_API void get_numerator_mpq(void* mpz_numerator, void* mpq_rational);
GMP_API void get_denominator_mpq(void* mpz_denominator, void* mpq_rational);

// float functions:

GMP_API void set_default_prec_mpf(uint32_t prec);
GMP_API uint32_t get_default_prec_mpf();
GMP_API void* create_mpf();
GMP_API void* create_mpf_prec(uint32_t prec);
GMP_API void destroy_mpf(void* mpf);
GMP_API uint32_t get_prec_mpf(void* mpf);
GMP_API void set_prec_mpf(void* mpf, uint32_t prec);
GMP_API void set_mpf(void* mpf_left, void* mpf_right);
GMP_API void set_mpf_ui(void* mpf_left, uint32_t right);
GMP_API void set_mpf_si(void* mpf_left, int32_t right);
GMP_API void set_mpf_d(void* mpf_left, double right);
GMP_API void set_mpf_z(void* mpf_left, void* mpz_right);
GMP_API void set_mpf_q(void* mpf_left, void* mpq_right);
GMP_API int32_t set_mpf_str(void* mpf_left, const char* str, int32_t base);
GMP_API char* get_mpf_str(void* mpf_handle, int32_t base_, uint32_t numDigits, int64_t* exponent);
GMP_API void free_mpf_str(char* mpf_str);
GMP_API void add_mpf(void* mpf_target, void* mpf_left, void* mpf_right);
GMP_API void sub_mpf(void* mpf_target, void* mpf_left, void* mpf_right);
GMP_API void mul_mpf(void* mpf_target, void* mpf_left, void* mpf_right);
GMP_API void div_mpf(void* mpf_target, void* mpf_left, void* mpf_right);
GMP_API void sqrt_mpf(void* mpf_target, void* mpf_subject);
GMP_API void neg_mpf(void* mpf_target, void* mpf_subject);
GMP_API void abs_mpf(void* mpf_target, void* mpf_subject);
GMP_API int32_t cmp_mpf(void* mpf_left, void* mpf_right);
GMP_API void ceil_mpf(void* mpf_target, void* mpf_subject);
GMP_API void floor_mpf(void* mpf_target, void* mpf_subject);
GMP_API void trunc_mpf(void* mpf_target, void* mpf_subject);
GMP_API double get_d_mpf(void* mpf);

#if defined (__cplusplus)
}
#endif

#endif // __GMP_INTF_H__
