#include <SFML/Graphics.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <cmath>

using namespace std;

inline int flat(int x, int y, int w, int h) {
  return x * h + y;
}

inline int flat2x(int f, int w, int h) {
  return f / h;
}

inline int flat2y(int f, int w, int h) {
  return f % h;
}

class Test {
  public:
    sf::Image mask, src, target, masked;
    vector<int> margin;
    vector<double> b[3];
    string prefix, suffix;

    Test() {}
    Test(string _prefix, string _suffix): prefix(_prefix), suffix(_suffix) {
      prefix = "./img/" + prefix;
      mask.loadFromFile(prefix + "_mask." + suffix);
      src.loadFromFile(prefix + "_src." + suffix);
      target.loadFromFile(prefix + "_target." + suffix);
    }
    ~Test() {}

    void getMasked() {
      int mask_x = mask.getSize().x;
      int mask_y = mask.getSize().y;

      sf::Image norm_mask;
      norm_mask.create(mask_x, mask_y);

      int max_x = 0, max_y = 0;
      int min_x = mask_x, min_y = mask_y;

      int nx[4] = {0, -1, 1, 0};
      int ny[4] = {-1, 0, 0, 1};

      for (int i = 0; i < mask_x; ++i)
        for (int j = 0; j < mask_y; ++j)
          if (mask.getPixel(i, j).r > 200) {
            max_x = max(max_x, i); max_y = max(max_y, j);
            min_x = min(min_x, i); min_y = min(min_y, j);
            norm_mask.setPixel(i, j, src.getPixel(i, j));
          }

      int masked_w = max_x - min_x + 5;
      int masked_h = max_y - min_y + 5;
      masked.create(masked_w, masked_h);
      masked.copy(norm_mask, 0, 0, sf::IntRect(min_x - 1, min_y - 1, masked_w, masked_h));

      for (int i = 1; i < masked_w - 1; ++i)
        for (int j = 1; j < masked_h - 1; ++j)
          if (masked.getPixel(i, j).r > 0 || masked.getPixel(i, j).g > 0 || masked.getPixel(i, j).b > 0)
            for (int k = 0; k < 4; ++k)
              if (masked.getPixel(i + nx[k], j + ny[k]).r == 0 &&
                  masked.getPixel(i + nx[k], j + ny[k]).g == 0 &&
                  masked.getPixel(i + nx[k], j + ny[k]).b == 0 ) {
                margin.push_back(flat(i, j, masked_w, masked_h));
                break;
              }

      // masked.saveToFile("./img/testest.jpg");
    }

    void getGrad() {
      int masked_x = masked.getSize().x;
      int masked_y = masked.getSize().y;
      for (int i = 0; i < 3; ++i)
        b[i].resize(masked_x * masked_y);

      for (int i = 1; i < masked_x - 1; ++i) {
        for (int j = 1; j < masked_y - 1; ++j) {
          if (masked.getPixel(i - 1, j) != sf::Color(0, 0, 0)) {
            b[0][flat(i, j, masked_x, masked_y)] += masked.getPixel(i, j).r - masked.getPixel(i - 1, j).r;
            b[1][flat(i, j, masked_x, masked_y)] += masked.getPixel(i, j).g - masked.getPixel(i - 1, j).g;
            b[2][flat(i, j, masked_x, masked_y)] += masked.getPixel(i, j).b - masked.getPixel(i - 1, j).b;
          }
          if (masked.getPixel(i + 1, j) != sf::Color(0, 0, 0)) {
            b[0][flat(i, j, masked_x, masked_y)] += masked.getPixel(i, j).r - masked.getPixel(i + 1, j).r;
            b[1][flat(i, j, masked_x, masked_y)] += masked.getPixel(i, j).g - masked.getPixel(i + 1, j).g;
            b[2][flat(i, j, masked_x, masked_y)] += masked.getPixel(i, j).b - masked.getPixel(i + 1, j).b;
          }
          if (masked.getPixel(i, j - 1) != sf::Color(0, 0, 0)) {
            b[0][flat(i, j, masked_x, masked_y)] += masked.getPixel(i, j).r - masked.getPixel(i, j - 1).r;
            b[1][flat(i, j, masked_x, masked_y)] += masked.getPixel(i, j).g - masked.getPixel(i, j - 1).g;
            b[2][flat(i, j, masked_x, masked_y)] += masked.getPixel(i, j).b - masked.getPixel(i, j - 1).b;
          }
          if (masked.getPixel(i, j + 1) != sf::Color(0, 0, 0)) {
            b[0][flat(i, j, masked_x, masked_y)] += masked.getPixel(i, j).r - masked.getPixel(i, j + 1).r;
            b[1][flat(i, j, masked_x, masked_y)] += masked.getPixel(i, j).g - masked.getPixel(i, j + 1).g;
            b[2][flat(i, j, masked_x, masked_y)] += masked.getPixel(i, j).b - masked.getPixel(i, j + 1).b;
          }
          // b[0][flat(i, j, masked_x, masked_y)] = 4 * masked.getPixel(i, j).r
          // - masked.getPixel(i - 1, j).r - masked.getPixel(i + 1, j).r
          // - masked.getPixel(i, j - 1).r - masked.getPixel(i, j + 1).r;
          // b[1][flat(i, j, masked_x, masked_y)] = 4 * masked.getPixel(i, j).g
          // - masked.getPixel(i - 1, j).g - masked.getPixel(i + 1, j).g
          // - masked.getPixel(i, j - 1).g - masked.getPixel(i, j + 1).g;
          // b[2][flat(i, j, masked_x, masked_y)] = 4 * masked.getPixel(i, j).b
          // - masked.getPixel(i - 1, j).b - masked.getPixel(i + 1, j).b
          // - masked.getPixel(i, j - 1).b - masked.getPixel(i, j + 1).b;
        }
      }

      // for (int i = 0; i < b[0].size(); ++i) {
      //   // sf::Color tmp(b[0][i], b[1][i], b[2][i]);
      //   sf::Color tmp(b[0][i], 0, 0);
      //   masked.setPixel(flat2x(i, masked_x, masked_y), flat2y(i, masked_x, masked_y), tmp);
      // }
      // masked.saveToFile("./img/testest2.jpg");
    }

    void getFused(int pos_x, int pos_y) {
      int masked_x = masked.getSize().x;
      int masked_y = masked.getSize().y;
      int size = masked_x * masked_y;

      double x[3][size];
      double mm[masked_x][masked_y][3];

      for (int i = 0; i < masked_x; ++i)
        for (int j = 0; j < masked_y; ++j) {
          x[0][flat(i, j, masked_x, masked_y)] = mm[i][j][0] = masked.getPixel(i, j).r;
          x[1][flat(i, j, masked_x, masked_y)] = mm[i][j][1] = masked.getPixel(i, j).g;
          x[2][flat(i, j, masked_x, masked_y)] = mm[i][j][2] = masked.getPixel(i, j).b;
        }

        for (auto m: margin) {
          sf::Color pixel = target.getPixel(flat2x(m, masked_x, masked_y) + pos_x, flat2y(m, masked_x, masked_y) + pos_y);
          // sf::Color pixel = target.getPixel(flat2x(m, masked_x, masked_y), flat2y(m, masked_x, masked_y));
          x[0][m] = pixel.r;
          x[1][m] = pixel.g;
          x[2][m] = pixel.b;
        }

      for (int _ = 0; _ < 10000; ++_) {
        for (int i = 0; i < size; ++i) {
          if (find(margin.begin(), margin.end(), i) != margin.end()) continue;
          int px = flat2x(i, masked_x, masked_y);
          int py = flat2y(i, masked_x, masked_y);

          if (masked.getPixel(px, py) == sf::Color()) continue;

          for (int j = 0; j < 3; ++j) {
            double ax = 0;
            double cnt = 0.0;
            // if (px > 0 && (mm[px - 1][py][0] > 0 || mm[px - 1][py][1] > 0 || mm[px - 1][py][2] > 0))
            // { cnt += 1.0; ax += x[j][flat(px - 1, py, masked_x, masked_y)]; }
            // if (py > 0 && (mm[px][py - 1][0] > 0 || mm[px][py - 1][1] > 0 || mm[px][py - 1][2] > 0))
            // { cnt += 1.0; ax += x[j][flat(px, py -  1, masked_x, masked_y)]; }
            // if (px < masked_x - 1 && (mm[px + 1][py][0] > 0 || mm[px + 1][py][1] > 0 || mm[px + 1][py][2] > 0))
            // { cnt += 1.0; ax += x[j][flat(px + 1, py, masked_x, masked_y)]; }
            // if (py < masked_y - 1 && (mm[px][py + 1][0] > 0 || mm[px][py + 1][1] > 0 || mm[px][py + 1][2] > 0))
            // { cnt += 1.0; ax += x[j][flat(px, py + 1, masked_x, masked_y)]; }
            if (px > 0 && (masked.getPixel(px - 1, py) != sf::Color(0, 0, 0)))
              { cnt += 1.0; ax += x[j][i - masked_y]; }
            if (py > 0 && (masked.getPixel(px, py - 1) != sf::Color(0, 0, 0)))
              { cnt += 1.0; ax += x[j][i - 1]; }
            if (px < masked_x - 1 && (masked.getPixel(px + 1, py) != sf::Color(0, 0, 0)))
              { cnt += 1.0; ax += x[j][i + masked_y]; }
            if (py < masked_y - 1 && (masked.getPixel(px, py + 1) != sf::Color(0, 0, 0)))
              { cnt += 1.0; ax += x[j][i + 1]; }
            ax += b[j][i];
            ax /= cnt;
            x[j][i] = ax;
          }
        }
      }

      for (int i = 0; i < size; ++i) {
        if (x[0][i] > 0 || x[1][i] > 0 || x[2][i] > 0) {
          int px = flat2x(i, masked_x, masked_y);
          int py = flat2y(i, masked_x, masked_y);
          target.setPixel(px + pos_x, py + pos_y, sf::Color(min(255, max(0, int(x[0][i]))), min(255, max(0, int(x[1][i] + 0.5))), min(255, max(0, int(x[2][i] + 0.5)))));
        }
        // masked.setPixel(px, py, sf::Color(x[0][i], 0, 0));
        // masked.setPixel(px, py, sf::Color(0, x[1][i], 0));
        // masked.setPixel(px, py, sf::Color(0, 0, x[2][i]));
        // masked.setPixel(px, py, sf::Color(b[0][i], 0, 0));
        // masked.setPixel(px, py, sf::Color(0, b[1][i], 0));
        // masked.setPixel(px, py, sf::Color(0, 0, b[2][i]));
        // if (x[0][i] > 255) cout << x[0][i] << endl;
        // cout << b[2][i] << endl;
      }
      target.saveToFile(prefix + "_result." + suffix);
    }
};
