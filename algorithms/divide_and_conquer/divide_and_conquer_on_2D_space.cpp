// vector<vector<int>> multiply(vector<vector<int>> &A, vector<vector<int>> &B)
// {
//   int n = A.size();
//   vector<vector<int>> C(n, vector<int>(n, 0));

//   if (n == 1) {
//     C[0][0] = A[0][0] * B[0][0];
//     return C;
//   }

//   int mid = n / 2;
//   vector<vector<int>> A11(mid, vector<int>(mid)), A12(mid, vector<int>(mid)),
//       A21(mid, vector<int>(mid)), A22(mid, vector<int>(mid)),
//       B11(mid, vector<int>(mid)), B12(mid, vector<int>(mid)),
//       B21(mid, vector<int>(mid)), B22(mid, vector<int>(mid));

//   // Divide matrices into sub-matrices
//   for (int i = 0; i < mid; i++)
//     for (int j = 0; j < mid; j++) {
//       A11[i][j] = A[i][j], A12[i][j] = A[i][j + mid];
//       A21[i][j] = A[i + mid][j], A22[i][j] = A[i + mid][j + mid];
//       B11[i][j] = B[i][j], B12[i][j] = B[i][j + mid];
//       B21[i][j] = B[i + mid][j], B22[i][j] = B[i + mid][j + mid];
//     }

//   vector<vector<int>> M1 = multiply(A11, B12);
//   vector<vector<int>> M2 = multiply(A12, B22);
//   vector<vector<int>> M3 = multiply(A21, B11);
//   vector<vector<int>> M4 = multiply(A22, B21);
//   vector<vector<int>> M5 = multiply(A11, B11);
//   vector<vector<int>> M6 = multiply(A22, B22);
//   vector<vector<int>> M7 = multiply(A12, B21);

//   // Combine results
//   for (int i = 0; i < mid; i++)
//     for (int j = 0; j < mid; j++) {
//       C[i][j] = M1[i][j] + M2[i][j];
//       C[i][j + mid] = M5[i][j] + M6[i][j];
//       C[i + mid][j] = M3[i][j] + M4[i][j];
//       C[i + mid][j + mid] = M7[i][j];
//     }

//   return C;
// }
