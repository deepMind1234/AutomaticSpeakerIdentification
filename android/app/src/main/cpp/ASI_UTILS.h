#include <math.h>
#include <vector>
#include <map>
#include <algorithm>

double ComputeFilterBankCoefficient(double* spectralData, unsigned int samplingRate, unsigned int NumFilters, unsigned int binSize, unsigned int m);
double ComputeNormalizationFactor(int NumFilters, int m);
double calculate_center_frequency(unsigned int filterBand);
double calculate_magnitude_factor(unsigned int filterBand);
double calculate_filter_parameter(unsigned int samplingRate, unsigned int binSize, unsigned int frequencyBand, unsigned int filterBand);

 

// #include "./Eigen/Dense"

float getHanningCoef(int N, int idx);
int findMaxArrayIdx(float *array, int minIdx, int maxIdx);
int findClosestIdxInArray(float *array, float value, int minIdx, int maxIdx);
int findClosestInVector(std::vector<int> vector, float value, int minIdx, int maxIdx);
// float euclidean_dist(std::vector<float>A , std::vector<float>B);
// float mahalanobis_dist(std::vector<float>A , std::vector<float>B);
//float bhattacharya_dist(std::vector<float>A , std::vector<float>B);
// double getMahanalobisDistance(std::vector<double> A, std::vector<double> B, std::vector<std::vector<double>> cov);
//std::vector<std::vector<double>> getCovarianceMat(std::vector<double> A, std::vector<double> B);
double getEucledianDistance(std::vector<double> A, std::vector<double> B);
int nearestNeighbor(std::vector<double> A, std::map<std::pair<int, int>, std::vector<double>> Recordings);
int kNearestNeighbors( std::vector<double> A, std::map<std::pair<int, int>, std::vector<double>> Recordings, int k);