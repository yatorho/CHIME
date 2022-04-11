// Copyright by 2022.4 chime
// author: yatorho

#ifndef CORE_FRAMEWORK_MATH_FUNCTIONS_HPP_
#define CORE_FRAMEWORK_MATH_FUNCTIONS_HPP_

#include "common.hpp"

#include <openblas/cblas.h>


namespace chime {

#if USE_BLAS_LIB == OPEN_BLAS



// chime gemmm provides a simpler interface to the gemm functions, with the
// limitation that the data has to be contiguous in the memory
template <typename Dtype>
void chime_cpu_gemm(CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB, utens_t m,
                  utens_t n, utens_t k, Dtype alpha, const Dtype *A,
                  const Dtype *B, Dtype beta, Dtype *C);

template <typename Dtype>
void chime_cpu_gemv(CBLAS_TRANSPOSE transA, utens_t m, utens_t n, Dtype alpha,
                   const Dtype *A, const Dtype *x, Dtype beta, Dtype *y);

template <typename Dtype>
void chime_cpu_axpy(utens_t N, Dtype alpha, const Dtype *x, Dtype *y);

template <typename Dtype>
void chime_cpu_axpby(utens_t N, Dtype alpha, const Dtype *x, Dtype beta,
                   Dtype *y);

template <typename Dtype>
void chime_cpu_copy(Dtype *y, const Dtype *x, utens_t n);

template <typename Dtype> 
void chime_cpu_set(Dtype *y, Dtype alpha, utens_t n);

template <typename Dtype> 
Dtype chime_cpu_asum(utens_t n, const Dtype *x);

template <typename Dtype>
Dtype chime_cpu_dot(utens_t n, const Dtype *x, const Dtype *y);

template <typename Dtype>
Dtype chime_cpu_strided_dot(utens_t n, const Dtype *x, utens_t incx,
                          const Dtype *y, utens_t incy);

template <typename Dtype>
void chime_cpu_scal(utens_t n, Dtype alpha, Dtype *x);

template <typename Dtype>
void chime_cpu_scal(utens_t n, Dtype alpha, const Dtype *x, Dtype *y);

template <typename Dtype>
void chime_cpu_rng_uniform(utens_t n, Dtype a, Dtype b, Dtype *r);

template <typename Dtype>
void chime_cpu_rng_gaussian(utens_t n , Dtype mu, Dtype sigma, Dtype *r);

template <typename Dprob, typename Dtype>
void chime_cpu_rng_bernoulli(utens_t n, Dprob p, Dtype *r);

template <typename Dtype>
void chime_cpu_exp(utens_t n, const Dtype *x, Dtype *y);

template <typename Dtype>
void chime_cpu_div(utens_t n, const Dtype *a, const Dtype *b, Dtype *y);

template <typename Dtype>
void chime_cpu_mul(utens_t n, const Dtype *a, const Dtype *b, Dtype *y);

template <typename Dtype>
void chime_cpu_add(utens_t n, const Dtype *a, const Dtype *b, Dtype *y);

template <typename Dtype>
void chime_cpu_sub(utens_t n, const Dtype *a, const Dtype *b, Dtype *y);

template <typename Dtype>
void chime_cpu_sign(utens_t n, const Dtype *a, Dtype *b);


#elif USE_BLAS_LIB == EIGEN

#endif

} // namespace chime

#endif // CORE_FRAMEWORK_MATH_FUNCTIONS_HPP_
