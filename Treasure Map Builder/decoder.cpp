#include "decoder.h"
#include "queue.h"
#include "stack.h"
using namespace std;

decoder::decoder(const PNG & tm, pair<int,int> s)
   :start(s),mapImg(tm) {
   
   start = s;
   mapImg = tm;

   vector<vector<bool>> visitedLoc (mapImg.width(), vector<bool>(mapImg.height(), false));
   vector<vector<int>> shortestPath (mapImg.width(), vector<int>(mapImg.height(), -1));
   firstFind = vector<vector<pair<int,int>>> (mapImg.width(), vector<pair<int,int>>(mapImg.height(), make_pair(-1,-1)));
   Queue<pair<int,int>> q;
   visitedLoc[start.first][start.second] = true;
   shortestPath[start.first][start.second] = 0;
   q.enqueue(start);

   while (!q.isEmpty()) {
      pair<int,int> curr = q.dequeue();
      vector<pair<int,int>> nbrs = neighbors(curr);
      for (int i = 0; i < 4; i++) {
         pair<int,int> p = nbrs[i];
         if(good(visitedLoc, shortestPath, curr, p)) {
            visitedLoc[p.first][p.second] = true;
            shortestPath[p.first][p.second] = shortestPath[curr.first][curr.second] + 1;
            firstFind[p.first][p.second] = curr;
            q.enqueue(p);
         }
      }
   }
   
   int longestPath = -1;
   pair<int,int> locSol;
   for (int i = 0; i < mapImg.width(); i++) {
      for (int j = 0; j < mapImg.height(); j++) {
         if (shortestPath[i][j] >= longestPath) {
            longestPath = shortestPath[i][j];
            locSol.first = i;
            locSol.second = j;
         }
      }
   }

   pathPts = vector<pair<int,int>>(longestPath + 1, make_pair(0,0));
   pathPts[longestPath] = locSol;
   for (int i = longestPath - 1; i >= 0; i--) {
      pair<int,int> curr = pathPts[i+1];
      pathPts[i] = firstFind[curr.first][curr.second];
   }
}

PNG decoder::renderSolution(){
   PNG mapCopy(mapImg);
   for (int i = 0; i < pathLength(); i++) {
      int x = pathPts[i].first;
      int y = pathPts[i].second;
      RGBAPixel *pixel = mapCopy.getPixel(x, y);
      pixel->r = 255;
      pixel->g = 0;
      pixel->b = 0;
   }
   return mapCopy;
}

PNG decoder::renderMaze(){
   PNG mapCopy(mapImg);
   for (int i = 0; i < mapCopy.width(); i++) {
      for (int j = 0; j < mapCopy.height(); j++) {
         if (firstFind[i][j].first >= 0) {
            setGrey(mapCopy, make_pair(i, j));
         }
      }
   } 
    
   for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 7; j++) {
         int x = start.first - 3 + i;
         int y = start.second - 3 + j;
         if (x >= 0 && y >= 0 && x < mapCopy.width() && y < mapCopy.height()) {
            RGBAPixel* pixel = mapCopy.getPixel(x, y);
            pixel->r = 255;
            pixel->g = 0;
            pixel->b = 0;
         }
      }
   }

   return mapCopy;
}

void decoder::setGrey(PNG & im, pair<int,int> loc){
   RGBAPixel *pixel = im.getPixel(loc.first, loc.second);
   pixel->r = 2*((pixel->r)/4);
   pixel->g = 2*((pixel->g)/4);
   pixel->b = 2*((pixel->b)/4);
}

pair<int,int> decoder::findSpot(){
   return pathPts[pathPts.size() - 1];
}

int decoder::pathLength(){
   return pathPts.size();
}

bool decoder::good(vector<vector<bool>> & v, vector<vector<int>> & d, pair<int,int> curr, pair<int,int> next){
   if (next.first >= 0 && next.first < mapImg.width() && next.second >= 0 && next.second < mapImg.height()) {
      if (v[next.first][next.second] == false) {
         if (compare(*(mapImg.getPixel(next.first, next.second)), d[curr.first][curr.second])) {
            return true;
         }
      }
   }
   return false;
}

vector<pair<int,int>> decoder::neighbors(pair<int,int> curr) {
   vector<pair<int,int>> neighbors;
   neighbors.push_back(make_pair(curr.first-1, curr.second));
   neighbors.push_back(make_pair(curr.first, curr.second+1));
   neighbors.push_back(make_pair(curr.first+1, curr.second));
   neighbors.push_back(make_pair(curr.first, curr.second-1));
   return neighbors;
}


bool decoder::compare(RGBAPixel p, int d){
   int mazeVal = (p.r % 4) * 16 + (p.g % 4) * 4 + (p.b % 4);
   if (mazeVal == (d+1) % 64) {
      return true;
   }
   return false;
}
