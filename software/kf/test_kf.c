#include "kf.h"
#include "kf_config.h"
#include "kf_matrix.h"
#include "test_kf_inverse_cumulative_gaussian.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DT 0.1
#define TOTAL_STEPS 500
#define SIGMA_Z 50 // Measurement noise standard deviation
#define SIGMA_A 15 // Acceleration process noise standard deviation

int main() {
    srand(time(NULL));

    // State: [position; velocity; acceleration]
    double x0_data[3][1] = {{0}, {0}, {1}}; // Initial position 0, velocity 0, acceleration 1
    KF_matrix x0 = {.rows = 3, .cols = 1, .data = {{x0_data[0][0]}, {x0_data[1][0]}, {x0_data[2][0]}}};

    // Initial covariance
    KF_matrix P0 = KF_matrix_identity(3);

    // Transition matrix for constant acceleration
    double F_data[3][3] = {
        {1, DT, 0.5 * DT * DT},
        {0, 1, DT},
        {0, 0, 1}};
    KF_matrix F = {.rows = 3, .cols = 3, .data = {{F_data[0][0], F_data[0][1], F_data[0][2]}, {F_data[1][0], F_data[1][1], F_data[1][2]}, {F_data[2][0], F_data[2][1], F_data[2][2]}}};

    // Observation matrix (we only observe position)
    double H_data[1][3] = {{1, 0, 0}};
    KF_matrix H = {.rows = 1, .cols = 3, .data = {{H_data[0][0], H_data[0][1], H_data[0][2]}}};

    // Process noise covariance Q (for acceleration noise)
    double dt2 = DT * DT, dt3 = dt2 * DT, dt4 = dt3 * DT;
    double q00 = dt4 / 4.0 * SIGMA_A * SIGMA_A;
    double q01 = dt3 / 2.0 * SIGMA_A * SIGMA_A;
    double q02 = dt2 / 2.0 * SIGMA_A * SIGMA_A;
    double q11 = dt2 * SIGMA_A * SIGMA_A;
    double q12 = DT * SIGMA_A * SIGMA_A;
    double q22 = SIGMA_A * SIGMA_A;
    KF_matrix Q = {
        .rows = 3, .cols = 3, .data = {{q00, q01, q02}, {q01, q11, q12}, {q02, q12, q22}}};

    // Measurement noise covariance (scalar)
    double R_data[1][1] = {{SIGMA_Z * SIGMA_Z}};
    KF_matrix R = {.rows = 1, .cols = 1, .data = {{R_data[0][0]}}};

    // No control input
    KF_matrix u = KF_matrix_zero(3, 1);
    KF_matrix B = KF_matrix_zero(3, 3);

    // Initial state
    KF_bayesian_state state = {.state = x0, .covariance = P0};

    // Open a pipe to gnuplot
    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        fprintf(stderr, "Could not open pipe to gnuplot\n");
        return 1;
    }

    // Prepare data arrays
    double ideal_pos[TOTAL_STEPS] = {0};
    double measurements[TOTAL_STEPS] = {0};
    double filtered_pos[TOTAL_STEPS], filtered_vel[TOTAL_STEPS], filtered_acc[TOTAL_STEPS];

    double time = 0.0;
    for (int t = 0; t < TOTAL_STEPS; ++t) {
        // Simulate a measurement with some noise (real = 0.5 * a * t^2, a = 1)
        ideal_pos[t] = 0.5 * time * time;
        measurements[t] = ideal_pos[t] + (double)normsinv((double)rand() / RAND_MAX) * SIGMA_Z;
        time += DT;
    }

    for (int t = 0; t < TOTAL_STEPS; ++t) {
        // Predict
        state = KF_predict(&state, &F, &Q, &u, &B);

        // Measurement vector
        double z_data[1][1] = {{measurements[t]}};
        KF_matrix z = {.rows = 1, .cols = 1, .data = {{z_data[0][0]}}};

        // Update
        state = KF_update(&state, &H, &R, &z);

        filtered_pos[t] = state.state.data[0][0];
        filtered_vel[t] = state.state.data[1][0];
        filtered_acc[t] = state.state.data[2][0];
    }

    // Send plot commands and data to gnuplot
    fprintf(gp, "set title 'Kalman Filter: Position Tracking (Constant Acceleration)'\n");
    fprintf(gp, "set xlabel 'Time'\n");
    fprintf(gp, "set ylabel 'Position'\n");
    fprintf(gp, "plot '-' with lines title 'KF Position', '-' with points title 'Measurement', '-' with lines title 'Ideal Position'\n");

    // Send filtered position data
    for (int t = 0; t < TOTAL_STEPS; ++t)
        fprintf(gp, "%f %f\n", DT * t, filtered_pos[t]);
    fprintf(gp, "e\n");

    // Send measurement data
    for (int t = 0; t < TOTAL_STEPS; ++t)
        fprintf(gp, "%f %f\n", DT * t, measurements[t]);
    fprintf(gp, "e\n");

    // Send ideal position data
    for (int t = 0; t < TOTAL_STEPS; ++t)
        fprintf(gp, "%f %f\n", DT * t, ideal_pos[t]);
    fprintf(gp, "e\n");

    fflush(gp);
    pclose(gp);

    return 0;
}