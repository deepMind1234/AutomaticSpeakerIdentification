#include <math.h>
#include <vector>
#include <map>

double GetCoefficient(double* spectralData, unsigned int samplingRate, unsigned int NumFilters, unsigned int binSize, unsigned int m);
double NormalizationFactor(int NumFilters, int m);
double GetCenterFrequency(unsigned int filterBand);
double GetMagnitudeFactor(unsigned int filterBand);
double GetFilterParameter(unsigned int samplingRate, unsigned int binSize, unsigned int frequencyBand, unsigned int filterBand);

 

// #include "./Eigen/Dense"

float getHanningCoef(int N, int idx);
int findMaxArrayIdx(float *array, int minIdx, int maxIdx);
int findClosestIdxInArray(float *array, float value, int minIdx, int maxIdx);
int findClosestInVector(std::vector<int> vector, float value, int minIdx, int maxIdx);
float euclidean_dist(std::vector<float>A , std::vector<float>B);
float mahalanobis_dist(std::vector<float>A , std::vector<float>B);
float bhattacharya_dist(std::vector<float>A , std::vector<float>B);
