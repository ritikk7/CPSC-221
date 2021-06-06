/**
 *
 * shifty quadtree (pa3)
 * sqtree.cpp
 * This file will be used for grading.
 *
 */

#include "sqtree.h"

// First Node constructor, given.
SQtree::Node::Node(pair<int,int> ul, int w, int h, RGBAPixel a, double v)
  :upLeft(ul),width(w),height(h),avg(a),var(v),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
{}

// Second Node constructor, given
SQtree::Node::Node(stats & s, pair<int,int> ul, int w, int h)
  :upLeft(ul),width(w),height(h),NW(NULL),NE(NULL),SE(NULL),SW(NULL) {
  avg = s.getAvg(ul,w,h);
  var = s.getVar(ul,w,h);
}

// SQtree destructor, given.
SQtree::~SQtree() {
  clear();
}

// SQtree copy constructor, given.
SQtree::SQtree(const SQtree & other) {
  copy(other);
}

// SQtree assignment operator, given.
SQtree & SQtree::operator=(const SQtree & rhs) {
  if (this != &rhs) {
    clear();
    copy(rhs);
  }
  return *this;
}

/**
 * SQtree constructor given tolerance for variance.
 */
SQtree::SQtree(PNG & imIn, double tol) {
  int h = imIn.height();
  int w =  imIn.width();
  stats s = stats(imIn);
  pair<int,int> ul = make_pair(0,0);
  root = buildTree(s, ul, w, h, tol);
}

/**
 * Helper for the SQtree constructor.
 */
SQtree::Node * SQtree::buildTree(stats & s, pair<int,int> & ul, int w, int h, double tol) {
  
  Node *curr = new Node(ul, w, h, s.getAvg(ul, w, h), s.getVar(ul, w, h));
  if (curr->var <= tol || (w == 1 && h == 1)) {
    return curr;
  }

  double var[4] = {__DBL_MIN__, __DBL_MIN__, __DBL_MIN__, __DBL_MIN__};
  double maxVariability = __DBL_MAX__;
  double maxVar;
  pair<int,int> splitPoint = ul;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      if (i == 0 && j == 0) {
        continue;
      } else if (i == 0) {
        var[0] = s.getVar(ul, w, j);
        var[1] = s.getVar(make_pair(ul.first, ul.second + j), w, h - j);
        var[2] = __DBL_MIN__;
        var[3] = __DBL_MIN__;
      } else if (j == 0) {
        var[0] = s.getVar(ul, i, h);
        var[1] = s.getVar(make_pair(ul.first+i, ul.second), w - i, h);
        var[2] = __DBL_MIN__;
        var[3] = __DBL_MIN__;
      } else {
        var[0] = s.getVar(ul, i, j);
        var[1] = s.getVar(make_pair(ul.first + i, ul.second), w - i, j);
        var[2] = s.getVar(make_pair(ul.first + i, ul.second + j), w - i, h - j);
        var[3] = s.getVar(make_pair(ul.first, ul.second + j), i, h - j);
      }
      maxVar = max(var[0], var[1]);
      maxVar = max(maxVar, var[2]);
      maxVar = max(maxVar, var[3]);
      if (maxVar <= maxVariability) {
        maxVariability = maxVar;
        splitPoint = make_pair(ul.first + i, ul.second + j);
      }
    }
  }
  
  if (splitPoint.first == ul.first) {
    pair<int,int> nw = make_pair(ul.first, ul.second);
    pair<int,int> sw = make_pair(splitPoint.first, splitPoint.second);
    curr->NW = buildTree(s, nw, w, splitPoint.second - ul.second, tol);
    curr->SW = buildTree(s, sw, w, ul.second + h - splitPoint.second, tol);
  } else if (splitPoint.second == ul.second) {
    pair<int,int> nw = make_pair(ul.first, ul.second);
    pair<int,int> ne = make_pair(splitPoint.first, splitPoint.second);
    curr->NW = buildTree(s, nw, splitPoint.first - ul.first, h, tol);
    curr->NE = buildTree(s, ne, ul.first + w - splitPoint.first, h, tol);
  } else {
    pair<int,int> nw = make_pair(ul.first, ul.second);
    pair<int,int> ne = make_pair(splitPoint.first, ul.second);
    pair<int,int> se = make_pair(splitPoint.first, splitPoint.second);
    pair<int,int> sw = make_pair(ul.first, splitPoint.second);
    curr->NW = buildTree(s, nw, splitPoint.first - ul.first, splitPoint.second - ul.second, tol);
    curr->NE = buildTree(s, ne, ul.first + w - splitPoint.first, splitPoint.second - ul.second, tol);
    curr->SE = buildTree(s, se, ul.first + w - splitPoint.first, ul.second + h - splitPoint.second, tol);
    curr->SW = buildTree(s, sw, splitPoint.first - ul.first, ul.second + h - splitPoint.second, tol);
  }
  return curr;
}
  
/**
 * Render SQtree and return the resulting image.
 */
PNG SQtree::render() {
  PNG img(root->width, root->height);
  renderImage(root, img);
  return img;
}

void SQtree::renderImage(Node *root, PNG & img) {
  if (!root) {
    return;
  }
  if (!(root->NW || root->NE || root->SE || root->SW)) {
    for (int i = 0; i < root->width; i++) {
      for (int j = 0; j < root->height; j++) {
        RGBAPixel *curr = img.getPixel(root->upLeft.first + i, root->upLeft.second + j);
        *curr = root->avg;
      }
    }
  }
  renderImage(root->NW, img);
  renderImage(root->NE, img);
  renderImage(root->SE, img);
  renderImage(root->SW, img);
}

/**
 * Delete allocated memory.
 */
void SQtree::clear() {
  clearRoot(root);
}

void SQtree::clearRoot(Node *root) {
  if (!root) {
    return;
  }
  clearRoot(root->NW);
  clearRoot(root->NE);
  clearRoot(root->SE);
  clearRoot(root->SW);
  delete root;
}

void SQtree::copy(const SQtree & other) {
  root = copyRoot(root, other.root);
}

SQtree::Node * SQtree::copyRoot(Node *root, const Node *other) {
  if (!other) {
    return NULL;
  }
  root = new Node(other->upLeft, other->width, other->height, other->avg, other->var);
  root->NW = copyRoot(root->NW, other->NW);
  root->NE = copyRoot(root->NE, other->NE);
  root->SE = copyRoot(root->SE, other->SE);
  root->SW = copyRoot(root->SW, other->SW);
  return root;
}

int SQtree::size() {
  return sizeTree(root);
}

int SQtree::sizeTree(Node *root) {
  if (!root) {
    return 0;
  }
  return sizeTree(root->NW) + sizeTree(root->NE) + sizeTree(root->SE) + sizeTree(root->SW) + 1;
}