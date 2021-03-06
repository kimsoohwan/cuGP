#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "../common/matrixops.h"
#include "covkernel.h"
#include "../common/cycleTimer.h"

#define INPUT_FILE "../chunked_dataset/sine_dataset_6000_10_chunk0.txt"
#define LABEL_FILE "../chunked_dataset/sine_dataset_6000_10_label0.txt"
int main()
{
	FILE *input_file, *label_file;
	int n, dim;
	double **X;
	double *y;
	
	input_file = fopen(INPUT_FILE, "r");
	label_file = fopen(LABEL_FILE, "r");

	if (input_file == NULL)
	{
		std::cout << "No input file found. Abort." << std::endl;
		return 0;
	}
	if (label_file == NULL)
	{
		std::cout << "No label file found. Abort." << std::endl;
		return 0;
	}

	// Input file has to have an additional line with n and d
	fscanf(input_file, "%d%d", &n, &dim);

	X = new double*[n];
	y = new double[n];

	for (int i = 0; i < n; i++)
		X[i] = new double[dim];

	for (int i = 0; i < n; i++)
		for (int j = 0; j < dim; j++)
			fscanf(input_file, "%lf", &X[i][j]);
	
	for (int i = 0; i < n; i++) {
		fscanf(label_file, "%lf", &y[i]);
	}

	double inithypervalues[] = {0.5, 0.5, 0.5};

	int total = 6000;
	int numtrain = 6000;
	int numtest = total - numtrain;
	Covsum kernelobj(numtrain, dim);
	kernelobj.set_loghyperparam(inithypervalues);

	double startime = CycleTimer::currentSeconds();	
	double ans = kernelobj.compute_loglikelihood(X, y);
	double endtime = CycleTimer::currentSeconds();	
	
	std::cout << ans << std::endl;
	std::cout << "time taken the entire loglikelihood computatoin = " << endtime - startime << std::endl;
//	double *grad = kernelobj.compute_gradient_loghyperparam(X, y);

	/*std::cout << grad[0] << std::endl;
	std::cout << grad[1] << std::endl;
	std::cout << grad[2] << std::endl;
	std::cout << ans << std::endl;*/

	std::cout << "Invoking solver" << std::endl;

	kernelobj.cg_solve(X, y, true);
	
	std::cout << "Done with solver" << std::endl;

	return 0;
//	double *new_hyper_params = kernelobj.get_loghyperparam();

//	std::cout << "New hyper params: " << std::endl;
//	std::cout << new_hyper_params[0] << std::endl;
//	std::cout << new_hyper_params[1] << std::endl;
//	std::cout << new_hyper_params[2] << std::endl;
//
//	ans = kernelobj.compute_loglikelihood(X, y);
//
//	std::cout << "Final answer is : " << ans << std::endl;
	
	// Just to check the testing phase
	//double acthp[] = {0.8771, 0.0786, -2.9346};
	//kernelobj.set_loghyperparam(acthp);


	double *tmeanvec = new double[numtest];
	double *tvarvec = new double[numtest];
	kernelobj.compute_test_means_and_variances(X, y, X + numtrain, tmeanvec, tvarvec, numtest);

//	printf("Now printing the test means\n");
//	print_vector(tmeanvec, numtest);
//	printf("\n\n Now priting the covariances\n");
//	print_vector(tvarvec, numtest);

	/*
	printf("Now printing the mean and var for the test set\n");
	for (int i = 0; i < numtest; i++) {
		printf("%lf %lf %lf\n", tmeanvec[i], tvarvec[i], y[numtest + i]);
	}

	*/

	printf("Call kar NLPP\n\n");

	double nlpp = kernelobj.get_negative_log_predprob(y + numtrain, tmeanvec, tvarvec, numtest);
	std::cout << "NLPP = " << nlpp << "\n";

	//delete tmeanvec;
	//delete tvarvec;

	for (int i = 0; i < n; i++)
		delete X[i];

	delete []X;
	delete y;

	fclose(input_file);
	fclose(label_file);
	
	return 0;	
}
