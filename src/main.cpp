#include <opencv2/opencv.hpp>
#include <iostream>
#include "../include/procesamiento.h"

int main(){
    cv::VideoCapture cap("../data/video.mp4");

    if(!cap.isOpened()){
        std::cout << "Error abriendo video\n";
        return -1;
    }

    cv::Mat frame;

    while(cap.read(frame)){

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        cv::Mat edges = cannyManual(gray);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Buscar contorno más grande (fondo)
        int maxIndex = 0;
        double maxArea = 0;

        for(int i = 0; i < contours.size(); i++){
            double area = cv::contourArea(contours[i]);
            if(area > maxArea){
                maxArea = area;
                maxIndex = i;
            }
        }

        for(int i = 0; i < contours.size(); i++){

            if(i == maxIndex)
                continue;

            double area = cv::contourArea(contours[i]);

            if(area < 2000)
                continue;

            std::vector<cv::Point> hull = grahamScan(contours[i]);

            int lados = hull.size();

            std::string figura;

            if(lados == 3)
                figura = "Triangulo";
            else if(lados == 4)
                figura = "Cuadrilatero";
            else if(lados == 5)
                figura = "Pentagono";
            else if(lados > 6)
                figura = "Circulo";
            else
                continue;

            // Calcular centro
            cv::Moments m = cv::moments(contours[i]);

            if(m.m00 == 0)
                continue;

            int cx = int(m.m10 / m.m00);
            int cy = int(m.m01 / m.m00);

            // Dibujar contorno
            cv::polylines(frame, hull, true, cv::Scalar(0,255,0), 2);

            // Dibujar centro
            cv::circle(frame, cv::Point(cx, cy), 3, cv::Scalar(255,0,0), -1);

            // Dibujar texto
            cv::putText(
                frame,
                figura,
                cv::Point(cx - 40, cy),
                cv::FONT_HERSHEY_SIMPLEX,
                0.6,
                cv::Scalar(0,0,255),
                2
            );
        }

        cv::imshow("Video", frame);
        cv::imshow("Edges", edges);

        if(cv::waitKey(30) == 27)
            break;
    }

    return 0;
}