#include "kf.h"
#include "kf_config.h"
#include "kf_matrix.h"

KF_bayesian_state KF_predict(const KF_bayesian_state *state, const KF_matrix *transition_matrix, const KF_matrix *process_noise, const KF_matrix *input, const KF_matrix *input_effect_matrix) {
    KF_bayesian_state predicted_state;

    // Predict the state
    predicted_state.state = KF_matrix_add(KF_matrix_dot(*transition_matrix, state->state), KF_matrix_dot(*input_effect_matrix, *input));

    // Predict the covariance
    predicted_state.covariance = KF_matrix_add(KF_matrix_dot(*transition_matrix, KF_matrix_dot(state->covariance, KF_matrix_transpose(*transition_matrix))), *process_noise);

    return predicted_state;
}

KF_bayesian_state KF_update(const KF_bayesian_state *predicted_state, const KF_matrix *observation_matrix, const KF_matrix *measurement_noise, const KF_matrix *measurement) {
    KF_bayesian_state updated_state;

    // Innovation (residual)
    KF_matrix innovation = KF_matrix_subtract(*measurement, KF_matrix_dot(*observation_matrix, predicted_state->state));

    // Innovation covariance S = H P H^T + R
    KF_matrix S = KF_matrix_add(
        KF_matrix_dot(*observation_matrix, KF_matrix_dot(predicted_state->covariance, KF_matrix_transpose(*observation_matrix))),
        *measurement_noise);

    KF_matrix K;
    if (S.rows == 1 && S.cols == 1) {
        // Scalar measurement: use division instead of matrix inversion
        double s_val = S.data[0][0];
        KF_matrix PHt = KF_matrix_dot(predicted_state->covariance, KF_matrix_transpose(*observation_matrix));
        K = PHt;
        // Scale all elements of K by 1/s_val
        for (unsigned int i = 0; i < K.rows; ++i)
            for (unsigned int j = 0; j < K.cols; ++j)
                K.data[i][j] /= s_val;
    } else {
        // Vector/matrix measurement: use matrix inversion
        KF_matrix S_inv = KF_matrix_inverse(S);
        K = KF_matrix_dot(
            KF_matrix_dot(predicted_state->covariance, KF_matrix_transpose(*observation_matrix)),
            S_inv);
    }

    // Updated state: x = x' + K y
    updated_state.state = KF_matrix_add(predicted_state->state, KF_matrix_dot(K, innovation));

    // Updated covariance: P = (I - K H) P'
    KF_matrix I = KF_matrix_identity(predicted_state->covariance.rows);
    KF_matrix KH = KF_matrix_dot(K, *observation_matrix);
    KF_matrix I_minus_KH = KF_matrix_subtract(I, KH);
    updated_state.covariance = KF_matrix_dot(I_minus_KH, predicted_state->covariance);

    return updated_state;
}