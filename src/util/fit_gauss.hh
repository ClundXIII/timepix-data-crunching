#ifndef PDC_gauss_hh_included
#define PDC_gauss_hh_included

#include <dlib/optimization.h>
#include <iostream>
#include <vector>

#include "../classes/structs.hh"

typedef dlib::matrix<double,2,1> input_vector;
typedef dlib::matrix<double,3,1> parameter_vector;

// ----------------------------------------------------------------------------------------

// We will use this function to generate data.  It represents a function of 2 variables
// and 3 parameters.   The least squares procedure will be used to infer the values of
// the 3 parameters based on a set of input/output pairs.
double model (
    const input_vector& input,
    const parameter_vector& params
)
{
    const double p0 = params(0);
    const double p1 = params(1);
    const double p2 = params(2);

    const double i0 = input(0);
    const double i1 = input(1);

    const double temp = p0*i0 + p1*i1 + p2;

    return temp*temp;
}

// ----------------------------------------------------------------------------------------

// This function is the "residual" for a least squares problem.   It takes an input/output
// pair and compares it to the output of our model and returns the amount of error.  The idea
// is to find the set of parameters which makes the residual small on all the data pairs.
double residual (
    const std::pair<input_vector, double>& data,
    const parameter_vector& params
)
{
    return model(data.first, params) - data.second;
}

// ----------------------------------------------------------------------------------------

// This function is the derivative of the residual() function with respect to the parameters.
parameter_vector residual_derivative (
    const std::pair<input_vector, double>& data,
    const parameter_vector& params
)
{
    parameter_vector der;

    const double p0 = params(0);
    const double p1 = params(1);
    const double p2 = params(2);

    const double i0 = data.first(0);
    const double i1 = data.first(1);

    const double temp = p0*i0 + p1*i1 + p2;

    der(0) = i0*2*temp;
    der(1) = i1*2*temp;
    der(2) = 2*temp;

    return der;
}

PDC::gaussParameter fitGauss(std::vector<double>& xVals, std::vector<double>& yValsData, PDC::gaussParameter initialParams) {
    try {
        // randomly pick a set of parameters to use in this example
        const parameter_vector params = 10*dlib::randm(3,1);
        std::cout << "params: " << trans(params) << std::endl;


        // Now let's generate a bunch of input/output pairs according to our model.
        std::vector<std::pair<input_vector, double> > data_samples;
        input_vector input;
        for (int i = 0; i < 1000; ++i)
        {
            input = 10*dlib::randm(2,1);
            const double output = model(input, params);

            // save the pair
            data_samples.push_back(std::make_pair(input, output));
        }

        // Before we do anything, let's make sure that our derivative function defined above matches
        // the approximate derivative computed using central differences (via derivative()).
        // If this value is big then it means we probably typed the derivative function incorrectly.
        std::cout << "derivative error: " << length(residual_derivative(data_samples[0], params) -
                                               derivative(residual)(data_samples[0], params) ) << std::endl;





        // Now let's use the solve_least_squares_lm() routine to figure out what the
        // parameters are based on just the data_samples.
        parameter_vector x;
        x = 1;

        std::cout << "Use Levenberg-Marquardt" << std::endl;
        // Use the Levenberg-Marquardt method to determine the parameters which
        // minimize the sum of all squared residuals.
        dlib::solve_least_squares_lm(dlib::objective_delta_stop_strategy(1e-7).be_verbose(),
                               residual,
                               residual_derivative,
                               data_samples,
                               x);

        // Now x contains the solution.  If everything worked it will be equal to params.
        std::cout << "inferred parameters: "<< trans(x) << std::endl;
        std::cout << "solution error:      "<< length(x - params) << std::endl;
        std::cout << std::endl;




        x = 1;
        std::cout << "Use Levenberg-Marquardt, approximate derivatives" << std::endl;
        // If we didn't create the residual_derivative function then we could
        // have used this method which numerically approximates the derivatives for you.
        dlib::solve_least_squares_lm(dlib::objective_delta_stop_strategy(1e-7).be_verbose(),
                               residual,
                               derivative(residual),
                               data_samples,
                               x);

        // Now x contains the solution.  If everything worked it will be equal to params.
        std::cout << "inferred parameters: "<< trans(x) << std::endl;
        std::cout << "solution error:      "<< length(x - params) << std::endl;
        std::cout << std::endl;




        x = 1;
        std::cout << "Use Levenberg-Marquardt/quasi-newton hybrid" << std::endl;
        // This version of the solver uses a method which is appropriate for problems
        // where the residuals don't go to zero at the solution.  So in these cases
        // it may provide a better answer.
        dlib::solve_least_squares(dlib::objective_delta_stop_strategy(1e-7).be_verbose(),
                            residual,
                            residual_derivative,
                            data_samples,
                            x);

        // Now x contains the solution.  If everything worked it will be equal to params.
        std::cout << "inferred parameters: "<< trans(x) << std::endl;
        std::cout << "solution error:      "<< length(x - params) << std::endl;

    }
    catch (std::exception& e) {
        std::cout << "Error while fitting: " << e.what() << std::endl;
    }
}

#endif /* PDC_gauss_hh_included */
