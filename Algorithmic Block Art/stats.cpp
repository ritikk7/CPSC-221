#include "stats.h"

stats::stats(PNG & im){
    int h = im.height();
    int w = im.width();
    sumRed = vector<vector<long>> (w,vector<long> (h,0));
    sumGreen = vector<vector<long>> (w,vector<long> (h,0));
    sumBlue = vector<vector<long>> (w,vector<long> (h,0));
    sumsqRed = vector<vector<long>> (w,vector<long> (h,0));
    sumsqGreen = vector<vector<long>> (w,vector<long> (h,0));
    sumsqBlue = vector<vector<long>> (w,vector<long> (h,0));

    for (int j = 0; j < h; j++) {
        for(int i = 0; i < w; i++) {
            RGBAPixel* pixel = im.getPixel(i, j);
            if (j == 0 && i == 0) {
                sumRed[i][j] = pixel->r;
                sumGreen[i][j] = pixel->g;
                sumBlue[i][j] = pixel->b;
                sumsqRed[i][j] = (pixel->r * pixel->r);
                sumsqGreen[i][j] = (pixel->g * pixel->g);
                sumsqBlue[i][j] = (pixel->b * pixel->b);
            } else if (i == 0) {
                sumRed[i][j] = pixel->r + sumRed[i][j-1];
                sumGreen[i][j] = pixel->g + sumGreen[i][j-1];
                sumBlue[i][j] = pixel->b + sumBlue[i][j-1];
                sumsqRed[i][j] = (pixel->r * pixel->r) + sumsqRed[i][j-1];
                sumsqGreen[i][j] = (pixel->g * pixel->g) + sumsqGreen[i][j-1];
                sumsqBlue[i][j] = (pixel->b * pixel->b) + sumsqBlue[i][j-1];
            } else if (j == 0) {
                sumRed[i][j] = pixel->r + sumRed[i-1][j];
                sumGreen[i][j] = pixel->g + sumGreen[i-1][j];
                sumBlue[i][j] = pixel->b + sumBlue[i-1][j];
                sumsqRed[i][j] = (pixel->r * pixel->r) + sumsqRed[i-1][j];
                sumsqGreen[i][j] = (pixel->g * pixel->g) + sumsqGreen[i-1][j];
                sumsqBlue[i][j] = (pixel->b * pixel->b) + sumsqBlue[i-1][j];
            } else {
                sumRed[i][j] = pixel->r + sumRed[i-1][j] + sumRed[i][j-1] - sumRed[i-1][j-1];
                sumGreen[i][j] = pixel->g + sumGreen[i-1][j] + sumGreen[i][j-1] - sumGreen[i-1][j-1];
                sumBlue[i][j] = pixel->b + sumBlue[i-1][j] + sumBlue[i][j-1] - sumBlue[i-1][j-1];
                sumsqRed[i][j] = (pixel->r * pixel->r) + sumsqRed[i-1][j] + sumsqRed[i][j-1] - sumsqRed[i-1][j-1];
                sumsqGreen[i][j] = (pixel->g * pixel->g) + sumsqGreen[i-1][j] + sumsqGreen[i][j-1] - sumsqGreen[i-1][j-1];
                sumsqBlue[i][j] = (pixel->b * pixel->b) + sumsqBlue[i-1][j] + sumsqBlue[i][j-1] - sumsqBlue[i-1][j-1];
            }
        }
    }
}


long stats::getSum(char channel, pair<int,int> ul, int w, int h) {
    if (channel == 'r') {
        return sumFinder(sumRed, ul, w, h);
    } else if (channel == 'g') {
        return sumFinder(sumGreen, ul, w, h);
    } else {
        return sumFinder(sumBlue, ul, w, h);
    }
}

long stats::getSumSq(char channel, pair<int,int> ul, int w, int h) {
    if (channel == 'r') {
        return sumFinder(sumsqRed, ul, w, h);
    } else if(channel == 'g') {
        return sumFinder(sumsqGreen, ul, w, h);
    } else {
        return sumFinder(sumsqBlue, ul, w, h);
    }
}


long stats::sumFinder(vector<vector<long>> &sum, pair<int,int> ul, int w, int h) {
    if (ul.first == 0 && ul.second == 0) {
        return sum[ul.first + w-1][ul.second + h-1];
    }
    else if (ul.second == 0) {
        return sum[ul.first + w-1][ul.second + h-1] - sum[ul.first - 1][ul.second + h-1];
    }
    else if (ul.first == 0) {
        return sum[ul.first + w-1][ul.second + h-1] - sum[ul.first + w-1][ul.second - 1];
    }
    else {
        return sum[ul.first + w-1][ul.second + h-1] - sum[ul.first + w-1][ul.second - 1] - sum[ul.first - 1][ul.second + h-1] + sum[ul.first - 1][ul.second - 1];
    }
}

// given a rectangle, compute its sum of squared deviations from mean, over all color channels.
// see written specification for a description of this function.
double stats::getVar(pair<int,int> ul, int w, int h){
    double a = w * h;
    double redVal = (double) getSum('r', ul, w, h);
    double greenVal = (double) getSum('g', ul, w, h);
    double blueVal = (double) getSum('b', ul, w, h);
    double redSqVal = (double) getSumSq('r', ul, w, h);
    double greenSqVal = (double) getSumSq('g', ul, w, h);
    double blueSqVal = (double) getSumSq('b', ul, w, h);

    double finalRedVal = (double) (redSqVal - ((redVal * redVal) / a));
    double finalGreenVal = (double) (greenSqVal - ((greenVal * greenVal) / a));
    double finalBlueVal = (double) (blueSqVal - ((blueVal * blueVal) / a));
    double var =  (double) (finalRedVal + finalGreenVal + finalBlueVal);

    return var;
}
		
RGBAPixel stats::getAvg(pair<int,int> ul, int w, int h){
    RGBAPixel pixel = RGBAPixel();
    double a = w * h;
    pixel.r = getSum('r', ul, w, h) / a;
    pixel.g = getSum('g', ul, w, h) / a;
    pixel.b = getSum('b', ul, w, h) / a;
    return pixel;
}