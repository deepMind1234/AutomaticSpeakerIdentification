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

#include <iostream>
#include <map>
#include <math.h>
#include <vector>

double getEucledianDistance(std::vector<double> A, std::vector<double> B) {
  std::vector<double> eucledianDistanceV;
  double eucledianDistance, sum = 0;
  int size = 0;
  if (A.size() <= B.size()){
	  size = A.size();
  }else{
	  size = B.size();
  }
  for (int i = 0; i < size; i++) {
    double x = pow(A[i] - B[i], 2);
    eucledianDistanceV.push_back(x);
  }
  for (std::vector<double>::iterator it = eucledianDistanceV.begin();
       it != eucledianDistanceV.end(); ++it)
    sum += *it;
  eucledianDistance = pow(sum, 0.5);
  return eucledianDistance;
}
int nearestNeighbor(
    std::vector<double> A,
    std::map<std::pair<int, int>, std::vector<double>> Recordings) {
  double current_min = 10e20;
  int current_min_id = -1;
  for (auto it = Recordings.begin(); it != Recordings.end(); ++it) {
    auto &pair = *it;
    std::pair<int, int> ids = pair.first;
    int recorderid = ids.first;
    //int recordingid = ids.second;
    std::vector<double> recordingPerPerson = pair.second;
    double dist = getEucledianDistance(A, recordingPerPerson);
    if (current_min > dist) {
      current_min = dist;
      current_min_id = recorderid;
    }
  }
  return current_min_id;
}



int kNearestNeighbors(
    std::vector<double> A,
    std::map<std::pair<int, int>, std::vector<double>> Recordings, int k) {
  std::vector<std::pair<int, double>> distances;
  for (auto it = Recordings.begin(); it != Recordings.end(); ++it) {
    auto &pair = *it;
    std::pair<int, int> ids = pair.first;
    int recorderid = ids.first;
    std::vector<double> recordingPerPerson = pair.second;
    double dist = getEucledianDistance(A, recordingPerPerson);
    distances.push_back(std::make_pair(recorderid, dist));
  }
  std::sort(
      distances.begin(), distances.end(),
      [](const std::pair<int, double> &lhs, const std::pair<int, double> &rhs) {
        return lhs.second < rhs.second;
      });
  std::map<int, int> neighbors;
  for (int i = 0; i < k; i++) {
    int recorderid = distances[i].first;
    neighbors[recorderid]++;
  }
  int max_count = 0;
  int max_recorderid = -1;
  for (auto it = neighbors.begin(); it != neighbors.end(); ++it) {
    int recorderid = it->first;
    int count = it->second;
    if (count > max_count) {
      max_count = count;
      max_recorderid = recorderid;
    }
  }
  return max_recorderid;
}