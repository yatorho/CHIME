// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/framework/math_functions.hpp"

#include <cmath>

#include "chime/core/framework/common.hpp"

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
        float64 temp = 0.;
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
        float64 temp = 0.;
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

TEST_F(MathFunctionsTest, TestChimeCpuGemv) {
  utens_t m;
  utens_t n;
  // **************** test float32 no trans *******************//
  {
    m = 12;
    n = 10;
    auto mat = static_cast<float32 *>(malloc(m * n * sizeof(float32)));
    auto vec = static_cast<float32 *>(malloc(n * sizeof(float32)));
    auto res = static_cast<float32 *>(malloc(m * sizeof(float32)));
    auto res_c = static_cast<float32 *>(malloc(m * sizeof(float32)));

    for (utens_t i = 0; i < m * n; i++) mat[i] = static_cast<float32>(i);
    for (utens_t i = 0; i < n; i++) vec[i] = -static_cast<float32>(i);

    for (utens_t i = 0; i < m; i++) {
      float32 temp = 0.f;
      for (utens_t j = 0; j < n; j++) temp += mat[i * n + j] * vec[j];
      res_c[i] = temp;
    }

    chime_cpu_gemv<float32>(CblasNoTrans, m, n, 1.f, mat, vec, 0.f, res);
    for (utens_t i = 0; i < m; i++) EXPECT_EQ(res[i], res_c[i]);

    free(mat);
    free(vec);
    free(res);
    free(res_c);
  }

  // **************** test float64 with trans alpha *******************//
  {
    m = 24;
    n = 12;

    float64 alpha = -2.3;

    auto mat = static_cast<float64 *>(malloc(m * n * sizeof(float64)));
    auto mat_t = static_cast<float64 *>(malloc(m * n * sizeof(float64)));
    auto vec = static_cast<float64 *>(malloc(n * sizeof(float64)));
    auto res = static_cast<float64 *>(malloc(m * sizeof(float64)));
    auto res_c = static_cast<float64 *>(malloc(m * sizeof(float64)));

    for (utens_t i = 0; i < m * n; i++) mat[i] = static_cast<float64>(i);
    for (utens_t i = 0; i < n; i++) vec[i] = -static_cast<float64>(i);

    // transport operation
    for (utens_t i = 0; i < m; i++) {
      for (utens_t j = 0; j < n; j++) mat_t[i * n + j] = mat[j * m + i];
    }

    for (utens_t i = 0; i < m; i++) {
      float64 temp = 0.;
      for (utens_t j = 0; j < n; j++) temp += mat_t[i * n + j] * vec[j];
      res_c[i] = alpha * temp;
    }

    chime_cpu_gemv<float64>(CblasTrans, n, m, alpha, mat, vec, 0., res);
    for (utens_t i = 0; i < m; i++) EXPECT_EQ(res[i], res_c[i]);

    free(mat);
    free(mat_t);
    free(vec);
    free(res);
    free(res_c);
  }

  // **************** test float32 with trans alpha beta*******************//
  {
    m = 24;
    n = 12;
    float32 alpha = -2.3f;
    float32 beta = 1.2f;

    auto mat = static_cast<float32 *>(malloc(m * n * sizeof(float32)));
    auto mat_t = static_cast<float32 *>(malloc(m * n * sizeof(float32)));
    auto vec = static_cast<float32 *>(malloc(n * sizeof(float32)));
    auto res = static_cast<float32 *>(malloc(m * sizeof(float32)));
    auto res_c = static_cast<float32 *>(malloc(m * sizeof(float32)));

    for (utens_t i = 0; i < m; i++) {
      res[i] = static_cast<float32>(i);
      res_c[i] = static_cast<float32>(i);
    }

    for (utens_t i = 0; i < m * n; i++) mat[i] = static_cast<float32>(i);
    for (utens_t i = 0; i < n; i++) vec[i] = -static_cast<float32>(i);

    // transport operation
    for (utens_t i = 0; i < m; i++) {
      for (utens_t j = 0; j < n; j++) mat_t[i * n + j] = mat[j * m + i];
    }

    for (utens_t i = 0; i < m; i++) {
      float32 temp = 0.f;
      for (utens_t j = 0; j < n; j++) temp += mat_t[i * n + j] * vec[j];
      res_c[i] = alpha * temp + beta * res_c[i];
    }

    chime_cpu_gemv<float32>(CblasTrans, n, m, alpha, mat, vec, beta, res);
    for (utens_t i = 0; i < m; i++) EXPECT_EQ(res[i], res_c[i]);

    free(mat);
    free(mat_t);
    free(vec);
    free(res);
    free(res_c);
  }
}

TEST_F(MathFunctionsTest, TestChimeCpuAxpy) {
  utens_t N;
  {  //  ********************* float32 ****************** //
    N = 100;
    float32 alpha = 0.4f;
    auto x = (float32 *) (malloc(N * sizeof(float32)));
    auto y_1 = (float32 *) (malloc(N * sizeof(float32)));
    auto y_2 = (float32 *) (malloc(N * sizeof(float32)));

    for (utens_t i = 0; i < N; i++) { x[i] = (float32) i; }
    for (utens_t i = 0; i < N; i++) {
      y_1[i] = (float32) i;
      y_2[i] = (float32) i;
    }

    for (utens_t i = 0; i < N; i++) { y_1[i] = alpha * x[i] + y_1[i]; }

    chime_cpu_axpy(N, alpha, x, y_2);
    float err = 0.f;
    for (utens_t i; i < N; i++) { err += std::fabs(y_1[i] - y_2[i]); }

    EXPECT_LT(err, 1e-5);
    free(x);
    free(y_1);
    free(y_2);
  }

  {  //  ********************* float64 ****************** //

    N = 100;
    float64 alpha = 0.4f;
    auto x = (float64 *) (malloc(N * sizeof(float64)));
    auto y_1 = (float64 *) (malloc(N * sizeof(float64)));
    auto y_2 = (float64 *) (malloc(N * sizeof(float64)));

    for (utens_t i = 0; i < N; i++) { x[i] = (float64) (i); }
    for (utens_t i = 0; i < N; i++) {
      y_1[i] = (float64) (i);
      y_2[i] = (float64) (i);
    }

    for (utens_t i = 0; i < N; i++) { y_1[i] = alpha * x[i] + y_1[i]; }

    chime_cpu_axpy(N, alpha, x, y_2);
    float err = 0.f;
    for (utens_t i; i < N; i++) { err += std::fabs(y_1[i] - y_2[i]); }

    EXPECT_LT(err, 1e-100);
    free(x);
    free(y_1);
    free(y_2);
  }
}

TEST_F(MathFunctionsTest, TestChimeCpuAsum) {
  utens_t n;
  //  ********************* float32 ****************** //
  {
    n = 100;
    auto arr = static_cast<float32 *>(malloc(n * sizeof(float32)));
    float32 res;
    float32 res_c = 0.f;

    for (utens_t i = 0; i < n; i++) arr[i] = 1.f;

    for (utens_t i = 0; i < n; i++) { res_c += arr[i]; }

    res = chime_cpu_asum<float32>(n, arr);
    EXPECT_EQ(res_c, res);
    EXPECT_EQ(res, 100.f);

    free(arr);
  }

  //  ********************* float64 ****************** //
  {
    n = 200;
    auto arr = static_cast<float64 *>(malloc(n * sizeof(float64)));
    float64 res;
    float64 res_c = 0.;

    for (utens_t i = 0; i < n; i++) arr[i] = 1.;

    for (utens_t i = 0; i < n; i++) { res_c += arr[i]; }

    res = chime_cpu_asum<float64>(n, arr);
    EXPECT_EQ(res_c, res);
    EXPECT_EQ(res, 200.);

    free(arr);
  }
}

}  // namespace chime
