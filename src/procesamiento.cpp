#include "../include/procesamiento.h"
#include <cmath>
#include <algorithm>

// ===== CONVOLUCIÓN (AHORA CON INT) =====
cv::Mat convolucion(const cv::Mat& img, const std::vector<std::vector<int>>& kernel){
    int k = kernel.size() / 2;

    // CAMBIO: usar enteros con signo
    cv::Mat result = cv::Mat::zeros(img.size(), CV_32SC1);

    for(int i = k; i < img.rows - k; i++){
        for(int j = k; j < img.cols - k; j++){
            int sum = 0;

            for(int u = -k; u <= k; u++){
                for(int v = -k; v <= k; v++){
                    sum += img.at<uchar>(i+u, j+v) * kernel[u+k][v+k];
                }
            }

            result.at<int>(i,j) = sum; // CAMBIO
        }
    }

    return result;
}

// ===== GAUSSIANO =====
cv::Mat gaussianManual(const cv::Mat& img){
    std::vector<std::vector<int>> kernel = {
        {1,2,1},
        {2,4,2},
        {1,2,1}
    };

    cv::Mat conv = convolucion(img, kernel);

    cv::Mat blur(img.size(), CV_8UC1);

    for(int i=0;i<img.rows;i++){
        for(int j=0;j<img.cols;j++){
            int val = conv.at<int>(i,j) / 16;
            val = std::max(0, std::min(255, val));
            blur.at<uchar>(i,j) = val;
        }
    }

    return blur;
}

// ===== CANNY MANUAL CORREGIDO =====
cv::Mat cannyManual(const cv::Mat& img){
    cv::Mat blur = gaussianManual(img);

    std::vector<std::vector<int>> sx = {
        {-1,0,1},
        {-2,0,2},
        {-1,0,1}
    };

    std::vector<std::vector<int>> sy = {
        {-1,-2,-1},
        {0,0,0},
        {1,2,1}
    };

    // AHORA SON MATRICES DE INT
    cv::Mat gx = convolucion(blur, sx);
    cv::Mat gy = convolucion(blur, sy);

    cv::Mat mag = cv::Mat::zeros(img.size(), CV_8UC1);

    for(int i=0;i<img.rows;i++){
        for(int j=0;j<img.cols;j++){

            // CAMBIO CLAVE
            int gx_val = gx.at<int>(i,j);
            int gy_val = gy.at<int>(i,j);

            int val = std::sqrt(gx_val*gx_val + gy_val*gy_val);

            if(val > 80) // puedes ajustar: 60–100
                mag.at<uchar>(i,j) = 255;
        }
    }

    return mag;
}

// ===== GEOMETRÍA =====
double distancia(cv::Point a, cv::Point b){
    return std::sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}

int orientacion(cv::Point a, cv::Point b, cv::Point c){
    int val = (b.y - a.y)*(c.x - b.x) - (b.x - a.x)*(c.y - b.y);

    if(val == 0) return 0;
    return (val > 0) ? 1 : 2;
}

std::vector<cv::Point> grahamScan(std::vector<cv::Point> points){
    if(points.size() < 3) return points;

    int ymin = 0;
    for(int i = 1; i < points.size(); i++){
        if(points[i].y < points[ymin].y)
            ymin = i;
    }

    std::swap(points[0], points[ymin]);
    cv::Point p0 = points[0];

    std::sort(points.begin()+1, points.end(), [p0](cv::Point a, cv::Point b){
        int o = orientacion(p0, a, b);
        if(o == 0)
            return distancia(p0, a) < distancia(p0, b);
        return o == 2;
    });

    std::vector<cv::Point> hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);
    hull.push_back(points[2]);

    for(int i = 3; i < points.size(); i++){
        while(hull.size() > 1 &&
              orientacion(hull[hull.size()-2], hull.back(), points[i]) != 2){
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }

    return hull;
}