#ifndef KF_H
#define KF_H

#include "kf_config.h"
#include "kf_matrix.h"
#include <stdio.h>

#ifndef KF_MAX_MATRIX_SIZE
#define KF_MAX_MATRIX_SIZE 3 // Default maximum matrix size
#warning "Using default maximum matrix size: 3x3. Define KF_MAX_MATRIX_SIZE in kf_config.h for a specific value."
#endif

/**
 * @brief A struct representing a Kalman filter state and its covariance.
 */
typedef struct KF_bayesian_state {
    KF_matrix state;      // State vector
    KF_matrix covariance; // Covariance matrix
} KF_bayesian_state;

/**
 * @brief Predicts the next state of a system using the Kalman filter equations.
 *
 * @param state The current state & covariance of the system.
 * @param transition_matrix The state transition matrix.
 * @param process_noise The process noise covariance matrix.
 * @param input The control input vector.
 * @param input_effect_matrix The input effect matrix.
 * @return Returns the predicted state and covariance.
 */
KF_bayesian_state KF_predict(const KF_bayesian_state *state, const KF_matrix *transition_matrix, const KF_matrix *process_noise, const KF_matrix *input, const KF_matrix *input_effect_matrix);

/**
 * @brief Updates the predicted state with a new measurement using the Kalman filter equations.
 *
 * @param predicted_state The predicted state & covariance from the KF_predict function.
 * @param observation_matrix The observation matrix.
 * @param measurement_noise The measurement noise covariance matrix.
 * @param measurement The measurement vector.
 * @return Returns the updated state and covariance.
 */
KF_bayesian_state KF_update(const KF_bayesian_state *predicted_state, const KF_matrix *observation_matrix, const KF_matrix *measurement_noise, const KF_matrix *measurement);

#endif // KF_H