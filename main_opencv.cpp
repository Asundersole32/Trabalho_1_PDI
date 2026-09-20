// main_opencv.cpp  —  OpenCV 5 + Qt6 (MSYS2 UCRT64)
// Compilar: veja .vscode/tasks.json

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

static Mat hConcat(const vector<Mat>& imgs, int gap = 10) {
    int h = imgs[0].rows;
    int totalW = gap;
    for (auto& im : imgs) totalW += im.cols + gap;
    Mat canvas(h + 2 * gap, totalW, CV_8UC3, Scalar(30, 30, 30));
    int x = gap;
    for (auto& im : imgs) {
        Mat roi = canvas(Rect(x, gap, im.cols, im.rows));
        if (im.channels() == 1) cvtColor(im, roi, COLOR_GRAY2BGR);
        else                    im.copyTo(roi);
        x += im.cols + gap;
    }
    return canvas;
}

static void showSafe(const string& titulo, const Mat& img) {
    if (img.empty()) { cout << "[aviso] vazia: " << titulo << "\n"; return; }
    try { namedWindow(titulo, WINDOW_AUTOSIZE); imshow(titulo, img); }
    catch (const exception& e) { cout << "[erro " << titulo << "] " << e.what() << "\n"; }
}

int main(int argc, char** argv) {
    string caminho = (argc >= 2) ? argv[1] : "image.jpg";
    cout << "Abrindo: " << caminho << "\n";

    Mat imgBGR = imread(caminho, IMREAD_COLOR);
    if (imgBGR.empty()) {
        cerr << "Falha ao abrir: " << caminho << "\n"
             << "Coloque uma image .jpg/.png na pasta ou passe o caminho.\n";
        cout << "ENTER para sair..."; cin.get();
        return -1;
    }
    cout << "OK: " << imgBGR.cols << "x" << imgBGR.rows
         << " | canais=" << imgBGR.channels() << "\n";

    Mat imgRGB, imgHSV, imgGray;
    cvtColor(imgBGR, imgRGB,  COLOR_BGR2RGB);
    cvtColor(imgBGR, imgHSV,  COLOR_BGR2HSV);
    cvtColor(imgBGR, imgGray, COLOR_BGR2GRAY);

    vector<Mat> ch; split(imgBGR, ch);
    Mat b = ch[0], g = ch[1], r = ch[2];

    Mat noBlue = imgBGR.clone();
    vector<Mat> chA; split(noBlue, chA);
    chA[0] = Mat::zeros(noBlue.rows, noBlue.cols, CV_8UC1);
    merge(chA, noBlue);

    Mat onlyRed = Mat::zeros(imgBGR.size(), imgBGR.type());
    vector<Mat> chR(3);
    chR[0] = Mat::zeros(imgBGR.rows, imgBGR.cols, CV_8UC1);
    chR[1] = Mat::zeros(imgBGR.rows, imgBGR.cols, CV_8UC1);
    chR[2] = r;
    merge(chR, onlyRed);

    Mat brighter; imgBGR.convertTo(brighter, -1, 1.0, 50);

    Mat swapped = imgBGR.clone();
    vector<Mat> chS; split(swapped, chS);
    swap(chS[0], chS[2]); merge(chS, swapped);

    Mat panel = hConcat({imgBGR, imgRGB, imgHSV, imgGray});

    imwrite("out_01_bgr.png",      imgBGR);
    imwrite("out_02_rgb.png",      imgRGB);
    imwrite("out_03_hsv.png",      imgHSV);
    imwrite("out_04_gray.png",     imgGray);
    imwrite("out_05_noBlue.png",   noBlue);
    imwrite("out_06_onlyRed.png",  onlyRed);
    imwrite("out_07_brighter.png", brighter);
    imwrite("out_08_swapped.png",  swapped);
    imwrite("out_09_panel.png",    panel);
    cout << "Arquivos out_*.png gerados.\n";

    struct Item { string t; Mat m; };
    vector<Item> itens = {
        {"01 - Original BGR",      imgBGR},
        {"02 - RGB",               imgRGB},
        {"03 - HSV",               imgHSV},
        {"04 - Cinza",             imgGray},
        {"05 - Canal B",           b},
        {"06 - Canal G",           g},
        {"07 - Canal R",           r},
        {"08 - Sem azul",          noBlue},
        {"09 - Só vermelho",       onlyRed},
        {"10 - Brilho +50",        brighter},
        {"11 - B/R trocados",      swapped},
        {"12 - Painel comparativo",panel},
    };

    cout << "\nPressione uma tecla na janela do OpenCV para avançar.\n\n";
    for (auto& it : itens) {
        cout << "Exibindo: " << it.t << " ... "; cout.flush();
        showSafe(it.t, it.m);
        waitKey(0);
        destroyWindow(it.t);
        cout << "ok\n";
    }
    cout << "\nFim. ENTER para encerrar.\n"; cin.get();
    return 0;
}