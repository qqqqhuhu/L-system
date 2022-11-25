#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <stack>
#include <cstring>
#include <cmath>
#include <set>
#include <map>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

#define MY_PI 3.14159265358979323846

class Grammer {
public:
    set<string> V;
    set<string> S;
    vector<string> W;
    map<string, vector<string>> P;

    Grammer() = default;;

    Grammer(set<string> V, set<string> S, vector<string> W,
            map<string, vector<string>> P) {
        this->V = std::move(V);
        this->S = std::move(S);
        this->W = std::move(W);
        this->P = std::move(P);
    }

    // render string
    vector<string> render(const vector<string> &NW) {
        vector<string> ans;
        for (const string &v: NW) {
            if (P.find(v) == P.end()) {
                ans.push_back(v);
                continue;
            }

            vector<string> p = P.at(v);
            ans.insert(ans.end(), p.begin(), p.end());
        }
        return ans;
    };

    void display() {
        cout << "W: ";
        for (const auto &w: W) {
            cout << w;
        }
        cout << endl;

        cout << "P: ";
        for (const auto &p: P) {
            cout << p.first << " -> ";
            for (const auto &w: p.second) {
                cout << w;
            }
            cout << endl;
        }
    }
};

class Config {
public:
    Grammer grammer;
    float angleDelta{};
    int renderTimes{};
    float lineLength{};
    float figurePosX{300};
    float figurePosY{550};
    float figureAngle{90};

    Config() = default;;

    Config(const Grammer &grammer, float angleDelta, int renderTimes, float lineLength = 10) {
        this->grammer = grammer;
        this->angleDelta = angleDelta;
        this->renderTimes = renderTimes;
        this->lineLength = lineLength;
    };

    Config(const string &filename) {
        ifstream ifs;
        ifs.open(filename);

        if (!ifs.is_open()) {
            cout << "Can't open config file [" << filename << "]. " << endl;
            return;
        }

        ifs >> renderTimes;
        ifs >> angleDelta;
        ifs >> lineLength;

        {
            string W;
            ifs >> W;

            vector<string> val;
            for (char j: W) {
                string t;
                t = j;
                val.push_back(t);
            }
            this->grammer.W = val;
        }

        {
            int n = 0;
            ifs >> n;
            for (int i = 0; i < n; i++) {
                string key;
                ifs >> key;
                string valStr;
                ifs >> valStr;

                vector<string> val;
                for (char j: valStr) {
                    string t;
                    t = j;
                    val.push_back(t);
                }
                this->grammer.P.emplace(key, val);
            }
        }

        ifs.close();
    }

    void changeAngleDelta(float angleDeltaDelta) {
        float newAngleDelta = angleDelta + angleDeltaDelta;
        if (newAngleDelta >= 0 && newAngleDelta <= 180) {
            angleDelta = newAngleDelta;
        }
    }

    void changeRenderTimes(int renderTimesDelta) {
        int newRenderTimesDelta = renderTimes + renderTimesDelta;
        if (newRenderTimesDelta >= 0) {
            renderTimes = newRenderTimesDelta;
        }
    }

    void changeLineLength(float lineLengthDelta) {
        float newLineLength = 0;
        if (lineLengthDelta > 0) {
            newLineLength = lineLength * 1.3;
        } else {
            newLineLength = lineLength * (float) 0.8;
        }

        if (newLineLength >= 0) {
            lineLength = newLineLength;
        }
    }

    void changeFigurePosX(float delta) {
        figurePosX = figurePosX + delta;
    }

    void changeFigurePosY(float delta) {
        figurePosY = figurePosY + delta;
    }

    void changeFigureAngle(float delta) {
        figureAngle = figureAngle + delta;

        while (figureAngle < 0) {
            figureAngle = figureAngle + 360;
        }

        while (figureAngle > 360) {
            figureAngle = figureAngle - 360;
        }
    }

    void display() {
        cout << "===========================================" << endl;
        this->grammer.display();
        cout << "angle delta  = " << angleDelta << endl;
        cout << "render times = " << renderTimes << endl;
        cout << "line length  = " << lineLength << endl;
        cout << "figure pos x = " << figurePosX << endl;
        cout << "figure pos y = " << figurePosY << endl;
        cout << "figure angle = " << figureAngle << endl;
        cout << "===========================================" << endl;
    }
};

class ImageRender {

private:
    vector<string> grammerRender() {
        vector<string> W = this->config.grammer.W;
        for (int i = 0; i < this->config.renderTimes; i++) {
            W = this->config.grammer.render(W);
        }
        return W;
    }

    Mat imageRender(const vector<string> &W) {
        Mat img(600, 600, CV_8UC3, Scalar(255, 255, 255));

        float angle = this->config.figureAngle; // angle
        float posx = this->config.figurePosX;
        float posy = this->config.figurePosY;

        stack<vector<float>> stacks;

        for (const auto &w: W) {
            if (w == "F") {
                this->helperForward(img, angle, &posx, &posy);
            } else if (w == "+") {
                angle = angle + this->config.angleDelta;
            } else if (w == "-") {
                angle = angle - this->config.angleDelta;
            } else if (w == "[") {
                stacks.push(vector<float>{angle, posx, posy});
            } else if (w == "]") {
                vector<float> a = stacks.top();
                stacks.pop();

                angle = a[0];
                posx = a[1];
                posy = a[2];
            }
        }

        return img;
    }

    void helperForward(Mat &img, float angle, float *posx, float *posy) {

        auto lineLen = float(this->config.lineLength);

        float posx_delta = lineLen * (float) cos((angle / 180) * MY_PI);
        float posy_delta = -lineLen * (float) sin((angle / 180) * MY_PI);

        float n_posx = *posx + posx_delta;
        float n_posy = *posy + posy_delta;

        line(img, Point2f(*posx, *posy), Point2f(n_posx, n_posy), Scalar(0, 0, 0));

        *posx = n_posx;
        *posy = n_posy;
    }

public:
    Config config;

    ImageRender(const Config &config) {
        this->config = config;
    }

    Mat render() {
        return imageRender(grammerRender());
    }

    Mat renderAndShow() {
        Mat img = imageRender(grammerRender());

        namedWindow("img");
        imshow("img", img);
        waitKey(0);
        destroyAllWindows();

        return img;
    }
};


class DisplayWindow {
public:
    void run() {
        namedWindow("img");

        Config config;
        Mat img(600, 600, CV_8UC3, Scalar(255, 255, 255));

        while (true) {
            imshow("img", img);
            int key = waitKey(0);
            cout << key << endl;

            if (key == 27 || key == 113) {
                break;
            } else if (key >= 48 && key <= 57) {
                string filename;
                filename = char(key);

                // TODO: change path
                filename = R"(C:\Users\lenovo\Desktop\LSystemModel\LSystemConfigs\config)" + filename + ".config";
                config = Config(filename);
            } else if (key == 119) {
                // w: line length sub
                config.changeLineLength(-1);
            } else if (key == 101) {
                // e: line length add
                config.changeLineLength(+1);
            } else if (key == 115) {
                // s: angle sub
                config.changeAngleDelta(-1);
            } else if (key == 100) {
                // d: angle add
                config.changeAngleDelta(+1);
            } else if (key == 120) {
                // x: iter sub
                config.changeRenderTimes(-1);
            } else if (key == 99) {
                // c: iter add
                config.changeRenderTimes(+1);
            } else if (key == 106) {
                // j left
                config.changeFigurePosX(-10);
            } else if (key == 107) {
                // k down
                config.changeFigurePosY(+10);
            } else if (key == 108) {
                // l right
                config.changeFigurePosX(+10);
            } else if (key == 105) {
                // i up
                config.changeFigurePosY(-10);
            } else if (key == 117) {
                // u rotate left
                config.changeFigureAngle(+5);
            } else if (key == 111) {
                // o rotate right
                config.changeFigureAngle(-5);
            }

            config.display();
            img = ImageRender(config).render();
        }
        destroyAllWindows();
    }
};

int main() {
    DisplayWindow().run();
    return 0;
}
