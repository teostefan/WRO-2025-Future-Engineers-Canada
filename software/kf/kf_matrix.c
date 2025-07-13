#include "kf_matrix.h"
#include "kf_config.h"
#include <stdio.h>

KF_matrix KF_matrix_add(const KF_matrix a, const KF_matrix b) {
    if (a.rows != b.rows || a.cols != b.cols) {
        fprintf(stderr, "KF Error - Matrix dimensions do not match for addition.\n");
        return (KF_matrix){0}; // Return an empty matrix on error.
    }

    KF_matrix result;
    for (unsigned int i = 0; i < a.rows; ++i) {
        for (unsigned int j = 0; j < a.cols; ++j) {
            result.data[i][j] = a.data[i][j] + b.data[i][j];
        }
    }
    result.rows = a.rows;
    result.cols = a.cols;
    return result;
}

KF_matrix KF_matrix_subtract(const KF_matrix a, const KF_matrix b) {
    if (a.rows != b.rows || a.cols != b.cols) {
        fprintf(stderr, "KF Error - Matrix dimensions do not match for subtraction.\n");
        return (KF_matrix){0}; // Return an empty matrix on error.
    }

    KF_matrix result;
    for (unsigned int i = 0; i < a.rows; ++i) {
        for (unsigned int j = 0; j < a.cols; ++j) {
            result.data[i][j] = a.data[i][j] - b.data[i][j];
        }
    }
    result.rows = a.rows;
    result.cols = a.cols;
    return result;
}

KF_matrix KF_matrix_dot(const KF_matrix a, const KF_matrix b) {
    if (a.cols != b.rows) {
        fprintf(stderr, "KF Error - Matrix dimensions do not match for dot product.\n");
        return (KF_matrix){0}; // Return an empty matrix on error.
    }

    KF_matrix result = {0};
    result.rows = a.rows;
    result.cols = b.cols;

    for (unsigned int i = 0; i < a.rows; ++i) {
        for (unsigned int j = 0; j < b.cols; ++j) {
            for (unsigned int k = 0; k < a.cols; ++k) {
                result.data[i][j] += a.data[i][k] * b.data[k][j];
            }
        }
    }
    return result;
}

KF_matrix KF_matrix_transpose(const KF_matrix a) {
    KF_matrix result = {0};
    result.rows = a.cols;
    result.cols = a.rows;

    for (unsigned int i = 0; i < a.rows; ++i) {
        for (unsigned int j = 0; j < a.cols; ++j) {
            result.data[j][i] = a.data[i][j];
        }
    }
    return result;
}

KF_matrix KF_matrix_scale(const KF_matrix a, double scalar) {
    KF_matrix result;
    for (unsigned int i = 0; i < a.rows; ++i) {
        for (unsigned int j = 0; j < a.cols; ++j) {
            result.data[i][j] = a.data[i][j] * scalar;
        }
    }
    result.rows = a.rows;
    result.cols = a.cols;
    return result;
}

KF_matrix KF_matrix_inverse(const KF_matrix a) {
    if (a.rows != a.cols) {
        fprintf(stderr, "KF Error - Matrix must be square for inversion.\n");
        return (KF_matrix){0}; // Return an empty matrix on error.
    }

    // For simplicity, this function only supports 2x2 and 3x3 matrices, which are common in Kalman filters.
    if (a.rows == 2) {
        double det = a.data[0][0] * a.data[1][1] - a.data[0][1] * a.data[1][0];
        if (det == 0) {
            fprintf(stderr, "KF Error - Matrix is singular and cannot be inverted.\n");
            return (KF_matrix){0}; // Return an empty matrix on error.
        }
        KF_matrix result = {0};
        result.rows = 2;
        result.cols = 2;
        result.data[0][0] = a.data[1][1] / det;
        result.data[0][1] = -a.data[0][1] / det;
        result.data[1][0] = -a.data[1][0] / det;
        result.data[1][1] = a.data[0][0] / det;
        return result;
    } else if (a.rows == 3) {
        double det =
            a.data[0][0] * (a.data[1][1] * a.data[2][2] - a.data[1][2] * a.data[2][1]) -
            a.data[0][1] * (a.data[1][0] * a.data[2][2] - a.data[1][2] * a.data[2][0]) +
            a.data[0][2] * (a.data[1][0] * a.data[2][1] - a.data[1][1] * a.data[2][0]);

        if (det == 0) {
            fprintf(stderr, "KF Error - Matrix is singular and cannot be inverted.\n");
            return (KF_matrix){0}; // Return an empty matrix on error.
        }

        KF_matrix result = {0};
        result.rows = 3;
        result.cols = 3;

        // Compute the cofactor matrix and divide by determinant (adjugate/det)
        result.data[0][0] = (a.data[1][1] * a.data[2][2] - a.data[1][2] * a.data[2][1]) / det;
        result.data[0][1] = -(a.data[0][1] * a.data[2][2] - a.data[0][2] * a.data[2][1]) / det;
        result.data[0][2] = (a.data[0][1] * a.data[1][2] - a.data[0][2] * a.data[1][1]) / det;

        result.data[1][0] = -(a.data[1][0] * a.data[2][2] - a.data[1][2] * a.data[2][0]) / det;
        result.data[1][1] = (a.data[0][0] * a.data[2][2] - a.data[0][2] * a.data[2][0]) / det;
        result.data[1][2] = -(a.data[0][0] * a.data[1][2] - a.data[0][2] * a.data[1][0]) / det;

        result.data[2][0] = (a.data[1][0] * a.data[2][1] - a.data[1][1] * a.data[2][0]) / det;
        result.data[2][1] = -(a.data[0][0] * a.data[2][1] - a.data[0][1] * a.data[2][0]) / det;
        result.data[2][2] = (a.data[0][0] * a.data[1][1] - a.data[0][1] * a.data[1][0]) / det;

        return result;
    } else {
        fprintf(stderr, "KF Error - Unsupported matrix size for inversion.\n");
        return (KF_matrix){0}; // Return an empty matrix on error.
    }
}

KF_matrix KF_matrix_identity(unsigned int size) {
    if (size > KF_MAX_MATRIX_SIZE) {
        fprintf(stderr, "KF Error - Matrix size exceeds maximum allowed size.\n");
        return (KF_matrix){0}; // Return an empty matrix on error.
    }

    KF_matrix result = {0};
    result.rows = size;
    result.cols = size;

    for (unsigned int i = 0; i < size; ++i) {
        result.data[i][i] = 1.0; // Set diagonal elements to 1
    }
    return result;
}

KF_matrix KF_matrix_zero(unsigned int rows, unsigned int cols) {
    if (rows > KF_MAX_MATRIX_SIZE || cols > KF_MAX_MATRIX_SIZE) {
        fprintf(stderr, "KF Error - Matrix size exceeds maximum allowed size.\n");
        return (KF_matrix){0}; // Return an empty matrix on error.
    }

    KF_matrix result = {0};
    result.rows = rows;
    result.cols = cols;

    for (unsigned int i = 0; i < rows; ++i) {
        for (unsigned int j = 0; j < cols; ++j) {
            result.data[i][j] = 0.0; // Initialize all elements to 0
        }
    }
    return result;
}

void KF_matrix_print(const KF_matrix a) {
    printf("Matrix (%ux%u):\n", a.rows, a.cols);
    for (unsigned int i = 0; i < a.rows; ++i) {
        for (unsigned int j = 0; j < a.cols; ++j) {
            printf("%lf ", a.data[i][j]);
        }
        printf("\n");
    }
}