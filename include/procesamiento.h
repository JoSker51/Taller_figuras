#ifndef PROCESAMIENTO_H
#define PROCESAMIENTO_H

#include <opencv2/opencv.hpp>
#include <vector>

// ===== FILTRADO =====
cv::Mat gaussianManual(const cv::Mat& img);
cv::Mat convolucion(const cv::Mat& img, const std::vector<std::vector<int>>& kernel);

// ===== BORDES =====
cv::Mat cannyManual(const cv::Mat& img);

// ===== GEOMETRÍA =====
int orientacion(cv::Point a, cv::Point b, cv::Point c);
std::vector<cv::Point> grahamScan(std::vector<cv::Point> points);

// ===== UTILIDADES =====
double distancia(cv::Point a, cv::Point b);

#endif