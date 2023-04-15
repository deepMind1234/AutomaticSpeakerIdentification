//
// Created by daran on 1/12/2017 to be used in ECE420 Sp17 for the first time.
// Modified by dwang49 on 1/1/2018 to adapt to Android 7.0 and Shield Tablet updates.
//

#include <cmath>
#include "ece420_lib.h"

// https://en.wikipedia.org/wiki/Hann_function
float getHanningCoef(int N, int idx) {
    return (float) (0.5 * (1.0 - cos(2.0 * M_PI * idx / (N - 1))));
}

int findMaxArrayIdx(float *array, int minIdx, int maxIdx) {
    int ret_idx = minIdx;

    for (int i = minIdx; i < maxIdx; i++) {
        if (array[i] > array[ret_idx]) {
            ret_idx = i;
        }
    }

    return ret_idx;
}

int findClosestIdxInArray(float *array, float value, int minIdx, int maxIdx) {
    int retIdx = minIdx;
    float bestResid = abs(array[retIdx] - value);

    for (int i = minIdx; i < maxIdx; i++) {
        if (abs(array[i] - value) < bestResid) {
            bestResid = abs(array[i] - value);
            retIdx = i;
        }
    }

    return retIdx;
}

// TODO: These should really be templatized
int findClosestInVector(std::vector<int> vec, float value, int minIdx, int maxIdx) {
    int retIdx = minIdx;
    float bestResid = abs(vec[retIdx] - value);

    for (int i = minIdx; i < maxIdx; i++) {
        if (abs(vec[i] - value) < bestResid) {
            bestResid = abs(vec[i] - value);
            retIdx = i;
        }
    }

    return retIdx;
}


float euclidean_dist(std::vector<float>A , std::vector<float>B){
    float dist = 0.0;
    for (size_t i = 0; i < A.size(); ++i) {
        dist += std::pow(A[i] - B[i], 2);
    }
    return std::sqrt(dist);
}

/*
float mahalanobis_dist(std::vector<float>A , std::vector<float>B,Eigen::MatrixXf& covariance_matrix ){

    // Convert vectors to Eigen vectors
    Eigen::Map<const Eigen::VectorXf> eigen_v1(A.data(), A.size());
    Eigen::Map<const Eigen::VectorXf> eigen_v2(B.data(), B.size());

    // Compute the Mahalanobis distance
    double dist = std::sqrt((eigen_v1 - eigen_v2).transpose() * covariance_matrix.inverse() * (eigen_v1 - eigen_v2));

    return dist;
}


float bhattacharya_dist(std::vector<float>A , std::vector<float>B){
    return
}
*/

/*
double GetCenterFrequency(unsigned int filterBand)
{
	double centerFrequency = 0.0f;
	double exponent;

	if(filterBand == 0){
		centerFrequency = 0;
	}
	else if(filterBand >= 1 && filterBand <= 14){
		centerFrequency = (200.0f * filterBand) / 3.0f;
	}
	else{
		exponent = filterBand - 14.0f;
		centerFrequency = pow(1.0711703, exponent);
		centerFrequency *= 1073.4;
	}

	return centerFrequency;
}

double GetMagnitudeFactor(unsigned int filterBand){
	double magnitudeFactor = 0.0f;

	if(filterBand >= 1 && filterBand <= 14){
		magnitudeFactor = 0.015;
	}
	else if(filterBand >= 15 && filterBand <= 48){
		magnitudeFactor = 2.0f / (GetCenterFrequency(filterBand + 1) - GetCenterFrequency(filterBand -1));
	}

	return magnitudeFactor;
}

double GetFilterParameter(unsigned int samplingRate, unsigned int binSize, unsigned int frequencyBand, unsigned int filterBand)
{
	double filterParameter = 0.0f;

	double boundary = (frequencyBand * samplingRate) / binSize;		// k * Fs / N
	double prevCenterFrequency = GetCenterFrequency(filterBand - 1);		// fc(l - 1) etc.
	double thisCenterFrequency = GetCenterFrequency(filterBand);
	double nextCenterFrequency = GetCenterFrequency(filterBand + 1);

	if(boundary >= 0 && boundary < prevCenterFrequency)
	{
		filterParameter = 0.0f;
	}
	else if(boundary >= prevCenterFrequency && boundary < thisCenterFrequency)
	{
		filterParameter = (boundary - prevCenterFrequency) / (thisCenterFrequency - prevCenterFrequency);
		filterParameter *= GetMagnitudeFactor(filterBand);
	}
	else if(boundary >= thisCenterFrequency && boundary < nextCenterFrequency)
	{
		filterParameter = (boundary - nextCenterFrequency) / (thisCenterFrequency - nextCenterFrequency);
		filterParameter *= GetMagnitudeFactor(filterBand);
	}
	else if(boundary >= nextCenterFrequency && boundary < samplingRate)
	{
		filterParameter = 0.0f;
	}

	return filterParameter;
}

double NormalizationFactor(int NumFilters, int m)
{
	double normalizationFactor = 0.0f;

	if(m == 0)
	{
		normalizationFactor = sqrt(1.0f / NumFilters);
	}
	else
	{
		normalizationFactor = sqrt(2.0f / NumFilters);
	}

	return normalizationFactor;
}
double GetCoefficient(double*   , unsigned int samplingRate, unsigned int NumFilters, unsigned int binSize, unsigned int m)
{
	double result = 0.0f;
	double outerSum = 0.0f;
	double innerSum = 0.0f;
	unsigned int k, l;

	// 0 <= m < L
	if(m >= NumFilters)
	{
		// This represents an error condition - the specified coefficient is greater than or equal to the number of filters. The behavior in this case is undefined.
		return 0.0f;
	}

	result = NormalizationFactor(NumFilters, m);


	for(l = 1; l <= NumFilters; l++)
	{
		// Compute inner sum
		innerSum = 0.0f;
		for(k = 0; k < binSize - 1; k++)
		{
			innerSum += fabs(spectralData[k] * GetFilterParameter(samplingRate, binSize, k, l));
		}

		if(innerSum > 0.0f)
		{
			innerSum = log(innerSum); // The log of 0 is undefined, so don't use it
		}

		innerSum = innerSum * cos(((m * PI) / NumFilters) * (l - 0.5f));

		outerSum += innerSum;
	}

	result *= outerSum;

	return result;
}
*/