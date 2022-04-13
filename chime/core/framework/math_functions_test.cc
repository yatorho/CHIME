// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/math_functions.hpp"

namespace chime {

class MathFunctionsTest : public ::testing::Test {};

TEST_F(MathFunctionsTest, TestChimeCpuGemm) {
  utens_t m;
  utens_t n;
  utens_t k;
  // ***************** Test float32 no trans ********************** //
  {
    utens_t m = 4ul;
    utens_t n = 3ul;
    utens_t k = 5ul;
    auto arr1 = static_cast<float32 *>(malloc(m * k * sizeof(float32)));
    auto arr2 = static_cast<float32 *>(malloc(k * n * sizeof(float32)));
    auto res = static_cast<float32 *>(malloc(m * n * sizeof(float32)));
    auto res_c = static_cast<float32 *>(malloc(m * n * sizeof(float32)));

    for (utens_t i = 0; i < m * k; i++) arr1[i] = i;
    for (utens_t i = 0; i < k * n; i++) arr2[i] = i;

    for (utens_t i = 0; i < m; i++) {
      for (utens_t j = 0; j < n; j++) {
        float32 temp = 0.f;
        for (utens_t l = 0; l < k; l++) {
          temp += arr1[i * k + l] * arr2[l * n + j];
        }
        res_c[i * n + j] = temp;
      }
    }

    chime_cpu_gemm<float32>(CblasNoTrans, CblasNoTrans, m, n, k, 1.f, arr1,
                            arr2, 0.f, res);
    for (utens_t i = 0; i < m * n; i++) EXPECT_EQ(res[i], res_c[i]);
    free(arr1);
    free(arr2);
    free(res);
    free(res_c);
  }

  // ***************** Test float64 no trans ********************** //
  {
    m = 6ul;
    n = 7ul;
    k = 8ul;

    auto arr1 = static_cast<float64 *>(malloc(m * k * sizeof(float64)));
    auto arr2 = static_cast<float64 *>(malloc(k * n * sizeof(float64)));
    auto res = static_cast<float64 *>(malloc(m * n * sizeof(float64)));
    auto res_c = static_cast<float64 *>(malloc(m * n * sizeof(float64)));

    for (utens_t i = 0; i < m * k; i++) arr1[i] = i * i;
    for (utens_t i = 0; i < k * n; i++) arr2[i] = i * i;

    for (utens_t i = 0; i < m; i++) {
      for (utens_t j = 0; j < n; j++) {
        float32 temp = 0.f;
        for (utens_t l = 0; l < k; l++) {
          temp += arr1[i * k + l] * arr2[l * n + j];
        }
        res_c[i * n + j] = temp;
      }
    }
    chime_cpu_gemm<float64>(CblasNoTrans, CblasNoTrans, m, n, k, 1., arr1, arr2,
                            0., res);
    for (utens_t i = 0; i < m * n; i++) EXPECT_EQ(res[i], res_c[i]);
    free(arr1);
    free(arr2);
    free(res);
    free(res_c);
  }

  // ***************** Test float64 with transA ********************** //
  {
    m = 7ul;
    n = 9ul;
    k = 9ul;

    auto arr1 = static_cast<float64 *>(malloc(m * k * sizeof(float64)));
    auto arr1_t = static_cast<float64 *>(malloc(m * k * sizeof(float64)));
    auto arr2 = static_cast<float64 *>(malloc(k * n * sizeof(float64)));
    auto res = static_cast<float64 *>(malloc(m * n * sizeof(float64)));
    auto res_c = static_cast<float64 *>(malloc(m * n * sizeof(float64)));
    for (utens_t i = 0; i < m * k; i++) arr1[i] = i;
    for (utens_t i = 0; i < k * n; i++) arr2[i] = i * i;

    // transA operation
    for (utens_t i = 0; i < m; i++) {
      for (utens_t j = 0; j < k; j++) arr1_t[i * k + j] = arr1[j * m + i];
    }

    for (utens_t i = 0; i < m; i++) {
      for (utens_t j = 0; j < n; j++) {
        float32 temp = 0.f;
        for (utens_t l = 0; l < k; l++) {
          temp += arr1_t[i * k + l] * arr2[l * n + j];
        }
        res_c[i * n + j] = temp;
      }
    }

    chime_cpu_gemm<float64>(CblasTrans, CblasNoTrans, m, n, k, 1., arr1, arr2,
                            0., res);
    for (utens_t i = 0; i < m * n; i++) EXPECT_EQ(res[i], res_c[i]);
    free(arr1);
    free(arr1_t);
    free(arr2);
    free(res);
    free(res_c);
  }

  // ***************** Test float32 with transB alpha ********************** //
  {

    m = 6ul;
    n = 12ul;
    k = 10ul;

    float32 alpha = -3.f;

    auto arr1 = static_cast<float32 *>(malloc(m * k * sizeof(float32)));
    auto arr2 = static_cast<float32 *>(malloc(k * n * sizeof(float32)));
    auto arr2_t = static_cast<float32 *>(malloc(k * n * sizeof(float32)));
    auto res = static_cast<float32 *>(malloc(m * n * sizeof(float32)));
    auto res_c = static_cast<float32 *>(malloc(m * n * sizeof(float32)));

    for (utens_t i = 0; i < m * k; i++) arr1[i] = -static_cast<float32>(i);
    for (utens_t i = 0; i < k * n; i++) arr2[i] = i * i;

    // transB operation
    for (utens_t i = 0; i < k; i++) {
      for (utens_t j = 0; j < n; j++) arr2_t[i * n + j] = arr2[j * k + i];
    }

    for (utens_t i = 0; i < m; i++) {
      for (utens_t j = 0; j < n; j++) {
        float32 temp = 0.f;
        for (utens_t l = 0; l < k; l++) {
          temp += arr1[i * k + l] * arr2_t[l * n + j];
        }
        res_c[i * n + j] = alpha * temp;
      }
    }

    chime_cpu_gemm<float32>(CblasNoTrans, CblasTrans, m, n, k, alpha, arr1,
                            arr2, 0.f, res);
    for (utens_t i = 0; i < m * n; i++) EXPECT_EQ(res[i], res_c[i]);
    free(arr1);
    free(arr2);
    free(arr2_t);
    free(res);
    free(res_c);
  }

  // ********** Test float32 with transA transB alpha beta ************** //
  {

    m = 13ul;
    n = 12ul;
    k = 6ul;

    float32 alpha = 2.f;
    float32 beta = -1.f;

    auto arr1 = static_cast<float32 *>(malloc(m * k * sizeof(float32)));
    auto arr1_t = static_cast<float32 *>(malloc(m * k * sizeof(float32)));
    auto arr2 = static_cast<float32 *>(malloc(k * n * sizeof(float32)));
    auto arr2_t = static_cast<float32 *>(malloc(k * n * sizeof(float32)));
    auto res = static_cast<float32 *>(malloc(m * n * sizeof(float32)));
    auto res_c = static_cast<float32 *>(malloc(m * n * sizeof(float32)));

    // set res value
    for (utens_t i = 0; i < m * n; i++) {
      res[i] = i;
      res_c[i] = i;
    }

    for (utens_t i = 0; i < m * k; i++) arr1[i] = -static_cast<float32>(i);
    for (utens_t i = 0; i < k * n; i++) arr2[i] = i * i;

    // transA operation
    for (utens_t i = 0; i < m; i++) {
      for (utens_t j = 0; j < k; j++) arr1_t[i * k + j] = arr1[j * m + i];
    }
    // transB operation
    for (utens_t i = 0; i < k; i++) {
      for (utens_t j = 0; j < n; j++) arr2_t[i * n + j] = arr2[j * k + i];
    }

    for (utens_t i = 0; i < m; i++) {
      for (utens_t j = 0; j < n; j++) {
        float32 temp = 0.f;
        for (utens_t l = 0; l < k; l++) {
          temp += arr1_t[i * k + l] * arr2_t[l * n + j];
        }
        res_c[i * n + j] = alpha * temp + beta * res_c[i * n + j];
      }
    }

    chime_cpu_gemm<float32>(CblasTrans, CblasTrans, m, n, k, alpha, arr1, arr2,
                            beta, res);
    for (utens_t i = 0; i < m * n; i++) EXPECT_EQ(res[i], res_c[i]);
    free(arr1);
    free(arr1_t);
    free(arr2);
    free(arr2_t);
    free(res);
    free(res_c);
  }
}

} // namespace chime
