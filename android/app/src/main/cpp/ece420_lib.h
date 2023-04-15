//
// Created by daran on 1/12/2017 to be used in ECE420 Sp17 for the first time.
// Modified by dwang49 on 1/1/2018 to adapt to Android 7.0 and Shield Tablet updates.
//

#ifndef ECE420_LIB_H
#define ECE420_LIB_H

#include <math.h>
#include <vector>
//#include "./Eigen/Dense"

float getHanningCoef(int N, int idx);
int findMaxArrayIdx(float *array, int minIdx, int maxIdx);
int findClosestIdxInArray(float *array, float value, int minIdx, int maxIdx);
int findClosestInVector(std::vector<int> vector, float value, int minIdx, int maxIdx);
float euclidean_dist(std::vector<float>A , std::vector<float>B);
float mahalanobis_dist(std::vector<float>A , std::vector<float>B);
float bhattacharya_dist(std::vector<float>A , std::vector<float>B);

#endif //ECE420_LIB_H
