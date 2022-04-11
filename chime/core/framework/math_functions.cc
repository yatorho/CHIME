// Copyright by 2022.4 chime
// author: yatorho

#include "math_functions.hpp"

#include <random>

namespace chime {

template <>
void chime_cpu_gemm<float32>(CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                             utens_t m, utens_t n, utens_t k, float32 alpha,
                             const float32 *A, const float32 *B, float32 beta,
                             float32 *C) {
  blasint lda = static_cast<blasint>((transA == CblasNoTrans) ? k : m);
  blasint ldb = static_cast<blasint>((transB == CblasNoTrans) ? n : k);
  cblas_sgemm(CblasRowMajor, transA, transB, static_cast<blasint>(m),
              static_cast<blasint>(n), static_cast<blasint>(k), alpha, A, lda,
              B, ldb, beta, C, static_cast<blasint>(n));
}

template <>
void chime_cpu_gemm<float64>(CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                             utens_t m, utens_t n, utens_t k, float64 alpha,
                             const float64 *A, const float64 *B, float64 beta,
                             float64 *C) {
  blasint lda = static_cast<blasint>((transA == CblasNoTrans) ? k : m);
  blasint ldb = static_cast<blasint>((transB == CblasNoTrans) ? n : k);
  cblas_dgemm(CblasRowMajor, transA, transB, static_cast<blasint>(m),
              static_cast<blasint>(n), static_cast<blasint>(k), alpha, A, lda,
              B, ldb, beta, C, static_cast<blasint>(n));
}

template <>
void chime_cpu_gemm<float128>(CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                              utens_t m, utens_t n, utens_t k, float128 alpha,
                              const float128 *A, const float128 *B,
                              float128 beta, float128 *C) {
#ifndef NOT_IMPLEMENTED
  blasint lda = static_cast<blasint>((transA == CblasNoTrans) ? k : m);
  blasint ldb = static_cast<blasint>((transB == CblasNoTrans) ? n : k);
#endif
  NOT_IMPLEMENTED;
}

template <>
void chime_cpu_gemv<float32>(CBLAS_TRANSPOSE transA, utens_t m, utens_t n,
                             float32 alpha, const float32 *A, const float32 *x,
                             float32 beta, float32 *y) {
  cblas_sgemv(CblasRowMajor, transA, static_cast<blasint>(m),
              static_cast<blasint>(n), alpha, A, static_cast<blasint>(n), x,
              static_cast<blasint>(1), beta, y, static_cast<blasint>(1));
}

template <>
void chime_cpu_gemv<float64>(CBLAS_TRANSPOSE transA, utens_t m, utens_t n,
                             float64 alpha, const float64 *A, const float64 *x,
                             float64 beta, float64 *y) {
  cblas_dgemv(CblasRowMajor, transA, static_cast<blasint>(m),
              static_cast<blasint>(n), alpha, A, static_cast<blasint>(n), x,
              static_cast<blasint>(1), beta, y, static_cast<blasint>(1));
}

template <>
void chime_cpu_gemv<float128>(CBLAS_TRANSPOSE transA, utens_t m, utens_t n,
                              float128 alpha, const float128 *A,
                              const float128 *x, float128 beta, float128 *y) {
  NOT_IMPLEMENTED;
}

template <>
void chime_cpu_axpy<float32>(utens_t N, float32 alpha, const float32 *x,
                             float32 *y) {
  cblas_saxpy(static_cast<blasint>(N), alpha, x, static_cast<blasint>(1), y,
              static_cast<blasint>(1));
}

template <>
void chime_cpu_axpy<float64>(utens_t N, float64 alpha, const float64 *x,
                             float64 *y) {
  cblas_daxpy(static_cast<blasint>(N), alpha, x, static_cast<blasint>(1), y,
              static_cast<blasint>(1));
}

template <>
void chime_cpu_axpy<float128>(utens_t N, float128 alpha, const float128 *x,
                              float128 *y) {
  NOT_IMPLEMENTED;
}

template <>
void chime_cpu_axpby<float32>(utens_t N, float32 alpha, const float32 *x,
                              float32 beta, float32 *y) {
  cblas_saxpby(static_cast<blasint>(N), alpha, x, static_cast<blasint>(1), beta,
               y, static_cast<blasint>(1));
}

template <>
void chime_cpu_axpby<float64>(utens_t N, float64 alpha, const float64 *x,
                              float64 beta, float64 *y) {
  cblas_daxpby(static_cast<blasint>(N), alpha, x, static_cast<blasint>(1), beta,
               y, static_cast<blasint>(1));
}

template <>
void chime_cpu_axpby<float128>(utens_t N, float128 alpha, const float128 *x,
                               float128 beta, float128 *y) {
  NOT_IMPLEMENTED;
}

template <>
void chime_cpu_copy<float32>(float32 *y, const float32 *x, utens_t n) {
  DCHECK_NE(x, y);
  cblas_scopy(static_cast<blasint>(n), x, static_cast<blasint>(1), y,
              static_cast<blasint>(1));
}

template <>
void chime_cpu_copy<float64>(float64 *y, const float64 *x, utens_t n) {
  DCHECK_NE(x, y);
  cblas_dcopy(static_cast<blasint>(n), x, static_cast<blasint>(1), y,
              static_cast<blasint>(1));
}

template <>
void chime_cpu_copy<float128>(float128 *y, const float128 *x, utens_t n) {
  NOT_IMPLEMENTED;
}

template <typename Dtype>
void chime_cpu_copy(Dtype *y, const Dtype *x, utens_t n) {
  DCHECK_NE(x, y);
  std::memcpy(y, x, n * sizeof(Dtype));
}

template void chime_cpu_copy<int8>(int8 *y, const int8 *x, utens_t n);
template void chime_cpu_copy<int16>(int16 *y, const int16 *x, utens_t n);
template void chime_cpu_copy<int32>(int32 *y, const int32 *x, utens_t n);
template void chime_cpu_copy<int64>(int64 *y, const int64 *x, utens_t n);
template void chime_cpu_copy<uint8>(uint8 *y, const uint8 *x, utens_t n);
template void chime_cpu_copy<uint16>(uint16 *y, const uint16 *x, utens_t n);
template void chime_cpu_copy<uint32>(uint32 *y, const uint32 *x, utens_t n);
template void chime_cpu_copy<uint64>(uint64 *y, const uint64 *x, utens_t n);

template <typename Dtype> void chime_cpu_set(Dtype *y, Dtype alpha, utens_t n) {
  DCHECK(y);
  if (alpha == 0) {
    std::memset(y, 0, static_cast<size_t>(n) * sizeof(Dtype));
    return;
  } else {
    for (utens_t i = 0; i < n; i++) {
      y[i] = alpha;
    }
  }
}

template void chime_cpu_set<int8>(int8 *y, int8 alpha, utens_t n);
template void chime_cpu_set<int16>(int16 *y, int16 alpha, utens_t n);
template void chime_cpu_set<int32>(int32 *y, int32 alpha, utens_t n);
template void chime_cpu_set<int64>(int64 *y, int64 alpha, utens_t n);
template void chime_cpu_set<uint8>(uint8 *y, uint8 alpha, utens_t n);
template void chime_cpu_set<uint16>(uint16 *y, uint16 alpha, utens_t n);
template void chime_cpu_set<uint32>(uint32 *y, uint32 alpha, utens_t n);
template void chime_cpu_set<uint64>(uint64 *y, uint64 alpha, utens_t n);
template void chime_cpu_set<float32>(float32 *y, float32 alpha, utens_t n);
template void chime_cpu_set<float64>(float64 *y, float64 alpha, utens_t n);
template void chime_cpu_set<float128>(float128 *y, float128 alpha, utens_t n);

template <> float32 chime_cpu_asum<float32>(utens_t n, const float32 *x) {
  return cblas_sasum(static_cast<blasint>(n), x, static_cast<blasint>(1));
}

template <> float64 chime_cpu_asum<float64>(utens_t n, const float64 *x) {
  return cblas_dasum(static_cast<blasint>(n), x, static_cast<blasint>(1));
}

template <> float128 chime_cpu_asum<float128>(utens_t n, const float128 *x) {
  NOT_IMPLEMENTED;
}

template <>
float32 chime_cpu_strided_dot<float32>(utens_t n, const float32 *x,
                                       utens_t incx, const float32 *y,
                                       utens_t incy) {
  return cblas_sdot(static_cast<blasint>(n), x, static_cast<blasint>(incx), y,
                    static_cast<blasint>(incy));
}

template <>
float64 chime_cpu_strided_dot<float64>(utens_t n, const float64 *x,
                                       utens_t incx, const float64 *y,
                                       utens_t incy) {
  return cblas_ddot(static_cast<blasint>(n), x, static_cast<blasint>(incx), y,
                    static_cast<blasint>(incy));
}

template <>
float128 chime_cpu_strided_dot<float128>(utens_t n, const float128 *x,
                                         utens_t incx, const float128 *y,
                                         utens_t incy) {
  NOT_IMPLEMENTED;
}

template <typename Dtype>
Dtype chime_cpu_dot(utens_t n, const Dtype *x, const Dtype *y) {
  return chime_cpu_strided_dot(n, x, static_cast<utens_t>(1), y,
                               static_cast<utens_t>(1));
}

template float32 chime_cpu_dot(utens_t n, const float32 *x, const float32 *y);
template float64 chime_cpu_dot(utens_t n, const float64 *x, const float64 *y);

template <> void chime_cpu_scal<float32>(utens_t n, float32 alpha, float32 *x) {
  cblas_sscal(static_cast<blasint>(n), alpha, x, static_cast<blasint>(1));
}

template <> void chime_cpu_scal<float64>(utens_t n, float64 alpha, float64 *x) {
  cblas_dscal(static_cast<blasint>(n), alpha, x, static_cast<blasint>(1));
}

template <>
void chime_cpu_scal<float128>(utens_t n, float128 alpha, float128 *x) {
  NOT_IMPLEMENTED;
}

template <>
void chime_cpu_scal<float32>(utens_t n, float32 alpha, const float32 *x,
                             float32 *y) {
  cblas_scopy(static_cast<blasint>(n), x, static_cast<blasint>(1), y,
              static_cast<blasint>(1));
  cblas_sscal(static_cast<blasint>(n), alpha, y, static_cast<blasint>(1));
}

template <>
void chime_cpu_scal<float64>(utens_t n, float64 alpha, const float64 *x,
                             float64 *y) {
  cblas_dcopy(static_cast<blasint>(n), x, static_cast<blasint>(1), y,
              static_cast<blasint>(1));
  cblas_dscal(static_cast<blasint>(n), alpha, y, static_cast<blasint>(1));
}

template <>
void chime_cpu_scal<float128>(utens_t n, float128 alpha, const float128 *x,
                              float128 *y) {
  NOT_IMPLEMENTED;
}

template <typename Dtype>
void chime_cpu_rng_uniform(utens_t n, Dtype a, Dtype b, Dtype *r) {
  DCHECK_LE(n, UTENS_MAX);
  DCHECK(r);
  DCHECK_LE(a, b);

  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_real_distribution<Dtype> dis(a, b);
  for (utens_t i = 0; i < n; i++) {
    r[i] = dis(gen);
  }
}

template void chime_cpu_rng_uniform<float32>(utens_t n, float32 a, float32 b,
                                             float32 *r);
template void chime_cpu_rng_uniform<float64>(utens_t n, float64 a, float64 b,
                                             float64 *r);
template void chime_cpu_rng_uniform<float128>(utens_t n, float128 a, float128 b,
                                              float128 *r);

template <typename Dtype>
void chime_cpu_rng_gaussian(utens_t n, Dtype mu, Dtype sigma, Dtype *r) {
  DCHECK(r);

  std::random_device rd{};
  std::mt19937 gen{rd()};
  std::normal_distribution<Dtype> random_distribution(mu, sigma);
  for (utens_t i = 0; i < n; i++) {
    r[i] = random_distribution(gen);
  }
}

template void chime_cpu_rng_gaussian<float32>(utens_t n, float32 mu,
                                              float32 sigma, float32 *r);
template void chime_cpu_rng_gaussian<float64>(utens_t n, float64 mu,
                                              float64 sigma, float64 *r);
template void chime_cpu_rng_gaussian<float128>(utens_t n, float128 mu,
                                               float128 sigma, float128 *r);

template <typename Dprob, typename Dtype>
void chime_cpu_rng_bernoulli(utens_t n, Dprob p, Dtype *r) {
  DCHECK(r);
  DCHECK_LE(p, static_cast<Dprob>(1));

  std::bernoulli_distribution random_distribution(p);

  std::random_device rd{};
  std::mt19937 gen{rd()};
  for (utens_t i = 0; i < n; i++) {
    r[i] = static_cast<Dtype>(random_distribution(gen));
  }
}

template void chime_cpu_rng_bernoulli<float32, int8>(utens_t n, float32 p,
                                                     int8 *r);
template void chime_cpu_rng_bernoulli<float64, int8>(utens_t n, float64 p,
                                                     int8 *r);
template void chime_cpu_rng_bernoulli<float32, int16>(utens_t n, float32 p,
                                                      int16 *r);
template void chime_cpu_rng_bernoulli<float64, int16>(utens_t n, float64 p,
                                                      int16 *r);
template void chime_cpu_rng_bernoulli<float32, int32>(utens_t n, float32 p,
                                                      int32 *r);
template void chime_cpu_rng_bernoulli<float64, int32>(utens_t n, float64 p,
                                                      int32 *r);
template void chime_cpu_rng_bernoulli<float32, int64>(utens_t n, float32 p,
                                                      int64 *r);
template void chime_cpu_rng_bernoulli<float64, int64>(utens_t n, float64 p,
                                                      int64 *r);
template void chime_cpu_rng_bernoulli<float32, uint8>(utens_t n, float32 p,
                                                      uint8 *r);
template void chime_cpu_rng_bernoulli<float64, uint8>(utens_t n, float64 p,
                                                      uint8 *r);
template void chime_cpu_rng_bernoulli<float32, uint16>(utens_t n, float32 p,
                                                       uint16 *r);
template void chime_cpu_rng_bernoulli<float64, uint16>(utens_t n, float64 p,
                                                       uint16 *r);
template void chime_cpu_rng_bernoulli<float32, uint32>(utens_t n, float32 p,
                                                       uint32 *r);
template void chime_cpu_rng_bernoulli<float64, uint32>(utens_t n, float64 p,
                                                       uint32 *r);
template void chime_cpu_rng_bernoulli<float32, uint64>(utens_t n, float32 p,
                                                       uint64 *r);
template void chime_cpu_rng_bernoulli<float64, uint64>(utens_t n, float64 p,
                                                       uint64 *r);

template <>
void chime_cpu_add<float32>(utens_t n, const float32 *a, const float32 *b,
                            float32 *y) {
  cblas_scopy(static_cast<blasint>(n), a, static_cast<blasint>(1), y,
              static_cast<blasint>(1));
  cblas_saxpy(static_cast<blasint>(n), static_cast<float32>(1.f), b,
              static_cast<blasint>(1), y, static_cast<blasint>(1));
}

template <>
void chime_cpu_add<float64>(utens_t n, const float64 *a, const float64 *b,
                            float64 *y) {
  cblas_dcopy(static_cast<blasint>(n), a, static_cast<blasint>(1), y,
              static_cast<blasint>(1));
  cblas_daxpy(static_cast<blasint>(n), static_cast<float64>(1.f), b,
              static_cast<blasint>(1), y, static_cast<blasint>(1));
}

} // namespace chime
