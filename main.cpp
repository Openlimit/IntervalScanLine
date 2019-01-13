#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>

using namespace std;

struct Edge {
    float x, dx;
    int ymax, ymin, poly_id;
};

struct Poly {
    int poly_id;
    bool flag;
    float a, b, c, d;
};

struct Point {
    float x, y, z;

    Point(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Triangle {
    int a, b, c;

    Triangle(int a, int b, int c) : a(a), b(b), c(c) {}
};


void getEdge(vector<Edge> &ET, vector<Point> &points, Triangle t, int poly_id) {
    int p_ids[3] = {t.a, t.b, t.c};
    for (int i = 0; i < 3; ++i) {
        int cur = p_ids[i];
        int next;
        if (i == 2)
            next = p_ids[0];
        else
            next = p_ids[i + 1];
        Edge e;
        e.poly_id = poly_id;
        if (points[cur].y != points[next].y)
            e.dx = (points[cur].x - points[next].x) / (points[cur].y - points[next].y);
        else
            e.dx = 0;

        if (points[cur].y > points[next].y) {
            e.ymax = (int) points[cur].y;
            e.ymin = (int) points[next].y;
            e.x = points[cur].x;
        } else {
            e.ymax = (int) points[next].y;
            e.ymin = (int) points[cur].y;
            e.x = points[next].x;
        }
        ET.emplace_back(e);
    }
}

void getPoly(vector<Poly> &PL, vector<Point> &points, Triangle t) {
    Poly p;
    p.poly_id = (int) PL.size();
    p.flag = false;

    p.a = (points[t.b].y - points[t.a].y) * (points[t.c].z - points[t.a].z) -
          (points[t.b].z - points[t.a].z) * (points[t.c].y - points[t.a].y);
    p.b = (points[t.b].z - points[t.a].z) * (points[t.c].x - points[t.a].x) -
          (points[t.b].x - points[t.a].x) * (points[t.c].z - points[t.a].z);
    p.c = (points[t.b].x - points[t.a].x) * (points[t.c].y - points[t.a].y) -
          (points[t.b].y - points[t.a].y) * (points[t.c].x - points[t.a].x);
    p.d = -(p.a * points[t.a].x + p.b * points[t.a].y + p.c * points[t.a].z);
    PL.emplace_back(p);
}

float getZ(Poly &poly, float x, int y) {
    if (poly.c == 0)
        return -1e9;
    float z = -(poly.a * x + poly.b * y + poly.d) / poly.c;
    return z;
}

bool cmp(const Edge &a, const Edge &b) {
    return a.x < b.x;
}

void drawLine(ofstream &fs, float x1, float x2, int y, Poly &poly, bool newLine) {
    int x11 = (int) x1;
    int x22 = (int) x2;
    for (int i = x11; i < x22; ++i) {
        int z = (int) getZ(poly, i, y);
        fs << z << " ";
    }
    if (newLine)
        fs << endl;
}

void load_obj(const string &name, vector<Edge> &ET, vector<Poly> &PL, int height) {
    ifstream fs(name, ios::in);
    int vm, fm;
    fs >> vm >> fm;
    char s;
    float x, y, z;
    vector<Point> points;
    vector<Triangle> faces;
    float ymax = -1e9;
    float ymin = 1e9;
    float xmean = 0;
    float ymean = 0;
    float zmean = 0;
    for (int i = 0; i < vm; ++i) {
        fs >> s >> x >> y >> z;
        Point p(x, y, z);
        points.emplace_back(p);
        if (y > ymax)
            ymax = y;
        if (y < ymin)
            ymin = y;
        xmean += x;
        ymean += y;
        zmean += z;
    }
    int a, b, c;
    for (int i = 0; i < fm; ++i) {
        fs >> s >> a >> b >> c;
        Triangle t(a - 1, b - 1, c - 1);
        faces.emplace_back(t);
    }
    fs.close();

    float time = height * 0.8 / (ymax - ymin);
    xmean /= vm;
    ymean /= vm;
    zmean /= vm;
    for (int i = 0; i < points.size(); ++i) {
        points[i].x = (points[i].x - xmean) * time + height / 2.0;
        points[i].y = (points[i].y - ymean) * time + height / 2.0;
        points[i].z = (points[i].z - zmean) * time*2 + height / 2.0;
    }

    for (int i = 0; i < faces.size(); ++i) {
        getEdge(ET, points, faces[i], i);
        getPoly(PL, points, faces[i]);
    }
}

int main() {
    int height = 500;
    int width = 500;
    int ymin = 0;
    int ymax = height - 1;
    vector<Edge> ET;
    vector<Poly> PL;
    load_obj("C:\\Users\\wly\\Desktop\\homer_large.obj", ET, PL, height);

    Poly bg;
    bg.a = bg.b = 0;
    bg.c = 1;
    bg.d = height;
    bg.flag = false;
    PL.emplace_back(bg);
    int bg_id = PL.size() - 1;

    Edge bg_e1;
    bg_e1.dx = 0;
    bg_e1.poly_id = bg_id;
    bg_e1.x = 0;
    bg_e1.ymax = ymax;
    bg_e1.ymin = ymin;
    ET.emplace_back(bg_e1);

    Edge bg_e2;
    bg_e2.dx = 0;
    bg_e2.poly_id = bg_id;
    bg_e2.x = width;
    bg_e2.ymax = ymax;
    bg_e2.ymin = ymin;
    ET.emplace_back(bg_e2);

    vector<Edge> AET;
    set<int> IPL;

    ofstream fs("C:\\Users\\wly\\Desktop\\image.pgm");
    fs << "P2" << endl;
    fs << width << " " << height << endl;
    fs << height << endl;

    for (int y = ymax; y >= ymin; --y) {
        for (int i = 0; i < ET.size(); ++i) {
            if (ET[i].ymax >= ET[i].ymin && y == ET[i].ymax) {
                if (ET[i].ymax > ET[i].ymin)
                    AET.emplace_back(ET[i]);
                ET[i].ymax--;
                ET[i].x -= ET[i].dx;
            }
        }
        if (AET.empty())
            continue;
        sort(AET.begin(), AET.end(), cmp);
        for (int i = 0; i < AET.size() - 1; ++i) {
            int id1 = AET[i].poly_id;
            PL[id1].flag = !PL[id1].flag;
            if (PL[id1].flag) {
                IPL.insert(id1);
            } else {
                IPL.erase(id1);
            }

            float x = (AET[i].x + AET[i + 1].x) / 2;
            float zmax = -1e9;
            int cur_id = -1;
            for (auto it = IPL.begin(); it != IPL.end(); it++) {
                int id = *it;
                float z = getZ(PL[id], x, y);
                if (z > zmax) {
                    zmax = z;
                    cur_id = id;
                }
            }
            drawLine(fs, AET[i].x, AET[i + 1].x, y, PL[cur_id], i + 2 == AET.size());
        }

        for (auto it = IPL.begin(); it != IPL.end(); it++) {
            PL[*it].flag = false;
        }
        AET.clear();
        IPL.clear();
    }

    return 0;
}