#include "ASI_UTILS.h"

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
double GetCoefficient(double* spectralData, unsigned int samplingRate, unsigned int NumFilters, unsigned int binSize, unsigned int m)
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

		innerSum = innerSum * cos(((m * M_PI) / NumFilters) * (l - 0.5f));

		outerSum += innerSum;
	}

	result *= outerSum;

	return result;
}