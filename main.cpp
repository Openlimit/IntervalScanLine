#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Edge {
    float x, dx;
    int ymax, ymin, poly_id;
};

struct Poly {
    int poly_id, color;
    bool flag;
    float a, b, c, d;
};


void getEdge(vector<Edge> &ET, const float *s, int poly_id) {
    for (int i = 0; i < 3; ++i) {
        int next = i + 3;
        if (i + 1 == 3)
            next = 0;
        Edge e;
        e.poly_id = poly_id;
        e.dx = (s[i + 1] - s[next + 1]) / (s[i] - s[next]);

        if (s[i + 1] > s[next + 1]) {
            e.ymax = (int) s[i + 1];
            e.ymin = (int) s[next + 1];
            e.x = s[i];
        } else {
            e.ymax = (int) s[next + 1];
            e.ymin = (int) s[i + 1];
            e.x = s[next];
        }
        ET.emplace_back(e);
    }
}

void getPoly(vector<Poly> &PL, const float *s, int color) {
    Poly p;
    p.poly_id = (int) PL.size();
    p.color = color;
    p.flag = 0;

    p.a = (s[4] - s[1]) * (s[8] - s[2]) - (s[5] - s[2]) * (s[7] - s[1]);
    p.b = (s[5] - s[2]) * (s[6] - s[0]) - (s[3] - s[0]) * (s[8] - s[2]);
    p.c = (s[3] - s[0]) * (s[7] - s[1]) - (s[4] - s[1]) * (s[6] - s[0]);
    p.d = -(p.a * s[0] + p.b * s[1] + p.c * s[2]);
    PL.emplace_back(p);
}

bool cmp(const Edge &a, const Edge &b) {
    return a.x < b.x;
}

int main() {
    float s1[9] = {1, 2, 0, 2, 6, 0, 4, 3, 0};
    float s2[9] = {3, 4, 1, 5, 7, 1, 6, 1, 1};
    vector<Edge> ET;
    vector<Poly> PL;

    getPoly(PL, s1, 50);
    getPoly(PL, s2, 150);
    getEdge(ET, s1, 0);
    getEdge(ET, s2, 1);

    vector<Edge> AET;
    vector<Poly> IPL;
    int ymin = 0;
    int ymax = 10;

    float zmax = -1e9;
    for (int y = ymax; y >= ymin; --y) {
        for (int i = 0; i < ET.size(); ++i) {
            if (ET[i].ymax >= ET[i].ymin && y == ET[i].ymax) {
                AET.emplace_back(ET[i]);
                ET[i].ymax--;
                ET[i].x -= ET[i].dx;
            }
        }
        sort(AET.begin(), AET.end(), cmp);
        for (int i = 0; i < AET.size(); ++i) {
            int id1 = AET[i].poly_id;
            PL[id1].flag = !PL[id1].flag;
            if (PL[id1].flag) {
                float x = (AET[i].x + AET[i + 1].x) / 2;
                float z = -(PL[id1].a * x + PL[id1].b * y + PL[id1].d) / PL[id1].c;
            }
        }
    }


    return 0;
}