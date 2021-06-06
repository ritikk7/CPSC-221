#include "block.h"
#include <cmath>
#include <iostream>

int Block::height() const {
  if (width() == 0) {
    return 0;
  }
  return data[0].size();
}

int Block::width() const {
  return data.size();
}

void Block::render(PNG &im, int x) const {
  for (int i = 0; i < width(); i++) {
    for (int j = 0; j < height(); j++) {
      *(im.getPixel(x+i, j)) = data[i][j];
    }
  }

  // unsigned int bound;
  // if ((unsigned)(x + width()) > im.width())
  //   bound = im.width();
  // else
  //   bound = x + width();
  
  // for (unsigned int i = x; i < bound; i++) {
  //   for (unsigned int j = 0; j < im.height(); j++) {
  //     *(im.getPixel(i, j)) = data[j][i-(unsigned)x];
  //   }
  // }
}

void Block::build(PNG &im, int x, int width) {

  for (int i = 0; i < width; i++) {
    vector<HSLAPixel> temp;
    for (unsigned int j = 0; j < im.height(); j++) {
      temp.push_back(*(im.getPixel(x+i, j)));
    }
    data.push_back(temp);
  }

  // data.resize(im.height());
  // for (unsigned int i = 0; i < im.height(); i++) {
  //   data[i].resize(width);
  //   for (unsigned int j = x; j < (unsigned) (width + x); j++) {
  //     data[i][j-(unsigned)x] = *(im.getPixel(i, j));
  //   }
  // }
}
