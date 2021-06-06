#include "treasureMap.h"
#include "queue.h"
using namespace std;

treasureMap::treasureMap(const PNG & baseim, const PNG & mazeim, pair<int,int> s){
    base = baseim;
    maze = mazeim;
    start = s;
}

void treasureMap::setGrey(PNG & im, pair<int,int> loc){
    RGBAPixel *pixel = im.getPixel(loc.first, loc.second);
    pixel->r = 2*((pixel->r)/4);
    pixel->g = 2*((pixel->g)/4);
    pixel->b = 2*((pixel->b)/4);
}

void treasureMap::setLOB(PNG & im, pair<int,int> loc, int d){
    RGBAPixel *pixel = im.getPixel(loc.first,loc.second);
    d = d%64;
    int r = (d >> 4) & 0b00000011;
    pixel->r = (pixel->r & 0b11111100) + r;
    int g = (d >> 2) & 0b00000011;
    pixel->g = (pixel->g & 0b11111100) + g;
    int b = d & 0b00000011;
    pixel->b = (pixel->b & 0b11111100) + b;
}

PNG treasureMap::renderMap(){
    PNG baseCopy(base);
    vector<vector<bool>> visitedLoc (baseCopy.width(), vector<bool>(baseCopy.height()));
    vector<vector<int>> shortestPath (baseCopy.width(), vector<int>(baseCopy.height()));
    Queue<pair<int,int>> q;
    visitedLoc[start.first][start.second] = true;
    shortestPath[start.first][start.second] = 0;
    setLOB(baseCopy, start, 0);
    q.enqueue(start);
    while (!q.isEmpty()) {
        pair<int,int> curr = q.dequeue();
        vector<pair<int,int>> nbrs = neighbors(curr);
        for (int i = 0; i < 4; i++) {
            pair<int,int> p = nbrs[i];
            if(good(visitedLoc, curr, p)) {
                visitedLoc[p.first][p.second] = true;
                shortestPath[p.first][p.second] = shortestPath[curr.first][curr.second] + 1;
                setLOB(baseCopy, p, shortestPath[p.first][p.second]);
                q.enqueue(p);
            }
        }
    }
    return baseCopy;
}


PNG treasureMap::renderMaze(){
    PNG baseCopy(base);
    vector<vector<bool>> visitedLoc (baseCopy.width(), vector<bool>(baseCopy.height()));
    Queue<pair<int,int>> q;
    visitedLoc[start.first][start.second] = true;
    setGrey(baseCopy, start);
    q.enqueue(start);
    while (!q.isEmpty()) {
        pair<int,int> curr = q.dequeue();
        vector<pair<int,int>> nbrs = neighbors(curr);
        for (int i = 0; i < 4; i++) {
            pair<int,int> p = nbrs[i];
            if(good(visitedLoc, curr, p)) {
                visitedLoc[p.first][p.second] = true;
                setGrey(baseCopy, p);
                q.enqueue(p);
            }
        }
    }

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            int x = start.first - 3 + i;
            int y = start.second - 3 + j;
            if (x >= 0 && y >= 0 && x < baseCopy.width() && y < baseCopy.height()) {
                RGBAPixel* pixel = baseCopy.getPixel(x, y);
                pixel->r = 255;
                pixel->g = 0;
                pixel->b = 0;
            }
        }
    }

    return baseCopy;
}

bool treasureMap::good(vector<vector<bool>> & v, pair<int,int> curr, pair<int,int> next){
    if (next.first < (int) maze.width() && next.second < (int) maze.height()) {
        if (v[next.first][next.second] == false) {
            RGBAPixel *pixelCurr = maze.getPixel(curr.first, curr.second);
            RGBAPixel *pixelNext = maze.getPixel(next.first, next.second);
            if (pixelCurr->operator==(*pixelNext)) {
                return true;
            }
        }
    }
    return false;
}

vector<pair<int,int>> treasureMap::neighbors(pair<int,int> curr) {
    vector<pair<int,int>> neighbors;
    neighbors.push_back(make_pair(curr.first-1, curr.second));
    neighbors.push_back(make_pair(curr.first, curr.second-1));
    neighbors.push_back(make_pair(curr.first+1, curr.second));
    neighbors.push_back(make_pair(curr.first, curr.second+1));
    return neighbors;
}

