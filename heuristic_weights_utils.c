#include "heuristic_weights_utils.h"

HeuristicWeights add_heuristic_weights(HeuristicWeights a, HeuristicWeights b)
{
    HeuristicWeights result = {0};
    double *a_ptr = (double *)&a;
    double *b_ptr = (double *)&b;
    double *result_ptr = (double *)&result;

    for (size_t i = 0; i < sizeof(HeuristicWeights) / sizeof(double); i++)
    {
        result_ptr[i] = a_ptr[i] + b_ptr[i];
    }

    return result;
}

HeuristicWeights subtract_heuristic_weights(HeuristicWeights a, HeuristicWeights b)
{
    HeuristicWeights result = {0};
    double *a_ptr = (double *)&a;
    double *b_ptr = (double *)&b;
    double *result_ptr = (double *)&result;

    for (size_t i = 0; i < sizeof(HeuristicWeights) / sizeof(double); i++)
    {
        result_ptr[i] = a_ptr[i] - b_ptr[i];
    }

    return result;
}

HeuristicWeights multiply_heuristic_weights(HeuristicWeights a, double b)
{
    HeuristicWeights result = {0};
    double *a_ptr = (double *)&a;
    double *result_ptr = (double *)&result;

    for (size_t i = 0; i < sizeof(HeuristicWeights) / sizeof(double); i++)
    {
        result_ptr[i] = a_ptr[i] * b;
    }

    return result;
}

HeuristicWeights divide_heuristic_weights(HeuristicWeights a, double b)
{
    HeuristicWeights result = {0};
    double *a_ptr = (double *)&a;
    double *result_ptr = (double *)&result;

    for (size_t i = 0; i < sizeof(HeuristicWeights) / sizeof(double); i++)
    {
        result_ptr[i] = a_ptr[i] / b;
    }

    return result;
}

HeuristicWeights mask_out_unused_heuristic_weights(HeuristicWeights params, HeuristicWeights weights)
{
    HeuristicWeights result = {0};
    double *params_ptr = (double *)&params;
    double *weights_ptr = (double *)&weights;
    double *result_ptr = (double *)&result;

    for (size_t i = 0; i < sizeof(HeuristicWeights) / sizeof(double); i++)
    {
        result_ptr[i] = params_ptr[i] == 0 ? 0 : weights_ptr[i];
    }

    return result;
}