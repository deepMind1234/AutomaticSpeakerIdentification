#include "ASI_UTILS.h"

double calculate_center_frequency(unsigned int filter_band) {
    double center_frequency = 0.0;
    double exp_value;

    if (filter_band == 0) {
        center_frequency = 0.0;
    }
    else if (filter_band >= 1 && filter_band <= 14) {
        center_frequency = (200.0 * filter_band) / 3.0;
    }
    else {
        exp_value = filter_band - 14.0;
        center_frequency = pow(1.0711703, exp_value) * 1073.4;
    }

    return center_frequency;
}

double calculate_magnitude_factor(unsigned int filter_band) {
    double magnitude_factor = 0.0;

    if (filter_band >= 1 && filter_band <= 14) {
        magnitude_factor = 0.015;
    }
    else if (filter_band >= 15 && filter_band <= 48) {
        double prev_center_frequency = calculate_center_frequency(filter_band - 1);
        double next_center_frequency = calculate_center_frequency(filter_band + 1);
        magnitude_factor = 2.0 / (next_center_frequency - prev_center_frequency);
    }

    return magnitude_factor;
}

double calculate_filter_parameter(unsigned int sampling_rate, unsigned int bin_size, unsigned int frequency_band, unsigned int filter_band) {
    double filter_parameter = 0.0;

    double boundary = (frequency_band * sampling_rate) / bin_size;
    double prev_center_frequency = calculate_center_frequency(filter_band - 1);
    double this_center_frequency = calculate_center_frequency(filter_band);
    double next_center_frequency = calculate_center_frequency(filter_band + 1);

    if (boundary >= 0 && boundary < prev_center_frequency) {
        filter_parameter = 0.0;
    }
    else if (boundary >= prev_center_frequency && boundary < this_center_frequency) {
        double numerator = boundary - prev_center_frequency;
        double denominator = this_center_frequency - prev_center_frequency;
        filter_parameter = numerator / denominator;
        filter_parameter *= calculate_magnitude_factor(filter_band);
    }
    else if (boundary >= this_center_frequency && boundary < next_center_frequency) {
        double numerator = boundary - next_center_frequency;
        double denominator = this_center_frequency - next_center_frequency;
        filter_parameter = numerator / denominator;
        filter_parameter *= calculate_magnitude_factor(filter_band);
    }
    else if (boundary >= next_center_frequency && boundary < sampling_rate) {
        filter_parameter = 0.0;
    }

    return filter_parameter;
}

double ComputeNormalizationFactor(int numFilters, int m)
{
    double normalizationFactor = (m == 0) ? sqrt(1.0 / numFilters) : sqrt(2.0 / numFilters);
    return normalizationFactor;
}

double ComputeFilterBankCoefficient(double* spectralData, unsigned int samplingRate, unsigned int numFilters, unsigned int binSize, unsigned int m)
{
    double filterBankCoefficient = 0.0;
    double normalizationFactor = ComputeNormalizationFactor(numFilters, m);
    double outerSum = 0.0;

    if(m >= numFilters)
    {
        return filterBankCoefficient; // undefined behavior
    }

    for(unsigned int l = 1; l <= numFilters; l++)
    {
        double innerSum = 0.0;

        for(unsigned int k = 0; k < binSize - 1; k++)
        {
            double filterParameter = GetFilterParameter(samplingRate, binSize, k, l);
            innerSum += fabs(spectralData[k] * filterParameter);
        }

        if(innerSum > 0.0)
        {
            innerSum = log(innerSum);
        }

        double cosineFactor = cos(((m * M_PI) / numFilters) * (l - 0.5));
        innerSum *= cosineFactor;
        outerSum += innerSum;
    }

    filterBankCoefficient = normalizationFactor * outerSum;
    return filterBankCoefficient;
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
	double alpha = 0.7;
	dist = exp(-1*alpha*dist);
    distances.push_back(std::make_pair(recorderid, dist));
  }
  std::sort(
      distances.begin(), distances.end(),
      [](const std::pair<int, double> &lhs, const std::pair<int, double> &rhs) {
        return lhs.second > rhs.second;
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

// #include "Eigen/Dense"
// std::vector<std::vector<double>> getCovarianceMat(std::vector<double> A, std::vector<double> B) {
//     double mean_A = 0, mean_B = 0;
//     double sum_A = 0, sum_B = 0, sum_cov = 0;
//     int n = A.size();  // assuming A and B have the same size
//     std::vector<std::vector<double>> covariance(n, std::vector<double>(n, 0));
//     for (int i = 0; i < n; i++) {
//         sum_A += A[i];
//         sum_B += B[i];
//     }
//     mean_A = sum_A / n;
//     mean_B = sum_B / n;
//     for (int i = 0; i < n; i++) {
//         sum_cov += (A[i] - mean_A) * (B[i] - mean_B);
//     }
//     double cov = sum_cov / n;
//     // fill the covariance matrix
//     for (int i = 0; i < n; i++) {
//         covariance[i][i] = cov;
//     }
//     return covariance;
// }

// double getMahanalobisDistance(std::vector<double> A, std::vector<double> B, std::vector<std::vector<double>> cov) {
//     Eigen::Map<Eigen::VectorXd> eigen_A(A.data(), A.size());
//     Eigen::Map<Eigen::VectorXd> eigen_B(B.data(), B.size());
//     Eigen::MatrixXd covariance(A.size(), A.size());
//     for (int i = 0; i < A.size(); i++) {
//         covariance.row(i) = Eigen::Map<Eigen::VectorXd>(cov[i].data(), cov[i].size());
//     }
//     double squared_dist = (eigen_A - eigen_B).transpose() * covariance.inverse() * (eigen_A - eigen_B);
//     double MahalanobisDistance = sqrt(squared_dist);
//     return MahalanobisDistance;
// }
