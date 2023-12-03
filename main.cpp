

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cmath>


#include <iostream>
using namespace cv;
using namespace std;


// Histogram oluşturma işlevi
void CreateHistogram(int**& h) {// iki buyutlu bir buyut bitlik için ikincisi ranklar için (r,g,b)
    h = new int* [3]; // rankları için 
    for (int i = 0; i < 3; i++) {
        h[i] = new int[256]; // bitlik 8 === 2^8 =256
        for (int j = 0; j < 256; j++) { // 0 dan 255 kadar çalışacak 
            h[i][j] = 0; // defult olarak 0 atar
        }
    }
}


class Img { // resim okuma ve işlemleri sağlanan sınıf  
private:
    int** HistogramInput;// giriş histogram saklamak için 
    int** HistogramOutput; // histogram sonuçları saklamak için 
    float** CDF;// yovarlana diğerleri için 
    float** Sk;// her pixel sığlıklığı hisaplamak için 
    int cols;// resim genişliği 
    int rows;// resim yüksekliği 
    cv::Mat img;// giriş resim 
    cv::Mat* histogramImg;// histogram yaptıktan sonra resim
public:
    Img(string imageSours) {//kurucu 
        this->img = cv::imread(imageSours); //resim okuma 
        this->rows = this->img.rows;// satır saısı 
        this->cols = this->img.cols;// sütün sayısı
        this->CreateHistogram(this->HistogramInput); // yer açma 
        this->CreateHistogram(this->HistogramOutput);// yer açma 
        this->CreatePr(CDF);// yer açma 
        this->CreateSk(Sk);//yer açma 
        this->CalculatHistograms(); // histogram hisaplama fon.
        this->histogramImg = new cv::Mat(this->rows, this->cols, CV_8UC3);// resim oluşturma 
        this->setHistogramImg();// resim ayarlama 

    }
    void setHistogramImg() { //resim ayarlama 
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                this->histogramImg->at<cv::Vec3b>(i, j) = cv::Vec3b(this->HistogramOutput[0][this->img.at<cv::Vec3b>(i, j)[0]], HistogramOutput[1][this->img.at<cv::Vec3b>(i, j)[1]], HistogramOutput[2][this->img.at<cv::Vec3b>(i, j)[2]]);// sonuç histogramdan gelen değeriyle resim yerleşilir 
            }
        }
    }
    void PrCalculat(int**& h, float**& CDF, int pixelNumber) { //pr değeri hisaplama 
        for (int i = 0; i < 256; i++) { /// 8 bitlik olduğu için 0<->255 ile gezecek  
            CDF[0][i] = float(h[0][i]) / float(pixelNumber); // pixel olasağı hisaplar 
            CDF[1][i] = float(h[1][i]) / float(pixelNumber);
            CDF[2][i] = float(h[2][i]) / float(pixelNumber);
            if (i != 0) {
                CDF[0][i] += CDF[0][i - 1];//öncek değerle toplanır 
                CDF[1][i] += CDF[1][i - 1];
                CDF[2][i] += CDF[2][i - 1];
            }

        }


    }
    void SkCalculat(float**& CDF, float**& sk, int**& histogram) {// Sk değeri hesaplanır 

        for (int i = 0; i < 256; i++) { /// 8 bitlik olduğu için 0<->255 ile gezecek
            sk[0][i] = (CDF[0][i] * 255);/// sk değeri alır  255 ile çarplanır 8 bitlik olduğu için 
            sk[1][i] = (CDF[1][i] * 255);
            sk[2][i] = (CDF[2][i] * 255);
            histogram[0][i] = round(sk[0][i]); // yovarlama yapılır 
            histogram[1][i] = round(sk[1][i]);
            histogram[2][i] = round(sk[2][i]);

        }
  
    }
    void CreateHistogram(int**& h) { //histogram oluşturma 
        h = new int* [3];
        for (int i = 0; i < 3; i++) {
            h[i] = new int[256];
            for (int j = 0; j < 256; j++) {
                h[i][j] = 0;
            }
        }
    }
    void CreatePr(float**& pr) { //  pr dezi oluşturma 
        pr = new float* [3];
        for (int i = 0; i < 3; i++) {
            pr[i] = new float[256];
            for (int j = 0; j < 256; j++) {
                pr[i][j] = 0.0;
            }
        }
    }  
    void CreateSk(float**& sk) {// sk  dizi oluşturma 
        sk = new float* [3];
        for (int i = 0; i < 3; i++) {
            sk[i] = new float[256];
            for (int j = 0; j < 256; j++) {
                sk[i][j] = 0.0;
            }
        }
    }
    void CalculatHistograms() { // histogram hisaplama 
        for (int i = 0; i < this->rows; i++)
        {
            for (int j = 0; j < this->cols; j++) { // resimdeki olan pexil okulur ve HistogramInput içinde toplanılır  
                cv::Vec3b index = this->img.at<cv::Vec3b>(i, j);
                this->HistogramInput[0][index[0]]++; // resimdeki olan pexil değeri yoluyle sayısın toplanır 
                this->HistogramInput[1][index[1]]++;
                this->HistogramInput[2][index[2]]++;
            }
        }
        this->PrCalculat(this->HistogramInput, this->CDF, rows * cols);// pr hisaplanır 
        this->SkCalculat(this->CDF, this->Sk, this->HistogramOutput); // sk hesaplanır 
    }

    void showImg(string title = "title") { // resim güsterme fon.
        cv::namedWindow(title, cv::WINDOW_AUTOSIZE);
        cv::imshow(title, this->img);
        cv::waitKey(0);

    }
    void showHistoImg(string title = "histo_title") {// resmin yeni halı gösterme fon.
        cv::namedWindow(title, cv::WINDOW_AUTOSIZE);
        cv::imshow(title, *this->histogramImg);
        cv::waitKey(0);

    }
    cv::Mat getImg() { // resim alma 
        return this->img;
    }
    cv::Mat getHistogramImg() { // resim yeni halı alma 
        return *this->histogramImg;
    }
    int** getHistogramInput() { // resim giriş histogramı alma 
        return this->HistogramInput;
    }
    int** getHistogramOutput() {// resmin sonuç historam alma 
        return this->HistogramOutput;
    }
    int getRows() { return this->rows; }// satır sayı alma 
    int getCols() { return this->cols; }// sütün sayı alam 

    
    ~Img() { // yıkıcı 
        delete this->histogramImg;
        for (int i = 0; i < 3; i++) {
            delete this->CDF[i];
            delete this->HistogramInput[i];
            delete this->HistogramOutput[i];
            delete this->Sk[i];
        }
        delete this->CDF;
        delete this->HistogramInput;
        delete this->HistogramOutput;
        delete this->Sk;

    }
};




int getIndex(int sourceIndex, int referenceArray[]) { // giriş histogram ile referens histogram karşlaştırma yaparak index değeri dönüş yapar  
    int index = sourceIndex;
    int i = 0;
    for (int i = 0; i < 256; i++) {
        if (sourceIndex <= referenceArray[i]) { // ğer referens değeri kaynak index ile büyük veya eşit ise referns indexi geri alır 
            index = i;
            break;
        }
    }
    return index;

}

void getTargetHistogram(int** sourceHisto, int** referenceHisto, int**& targetHisto) {// hedef değerleri bulmayı yapar 
    CreateHistogram(targetHisto);
    for (int i = 0; i < 256; i++) {

        targetHisto[0][i] = getIndex(sourceHisto[0][i], referenceHisto[0]);// değeri bulur
        targetHisto[1][i] = getIndex(sourceHisto[1][i], referenceHisto[1]);
        targetHisto[2][i] = getIndex(sourceHisto[2][i], referenceHisto[2]);
    }

}


void CreateOutputImg(Img& sourceImg, Img& referenceImg) {// hedef (sonuç resim hisaplanır )
    int rows = sourceImg.getRows();
    int cols = sourceImg.getCols();
    cv::Mat source_Img = sourceImg.getImg();// kaynak resim 
    cv::Mat outputImg(rows, cols, CV_8UC3);// sonuç için deful resim oluşturma 
    int** targetHistogram;

    getTargetHistogram(sourceImg.getHistogramOutput(), referenceImg.getHistogramOutput(), targetHistogram);


    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            outputImg.at<cv::Vec3b>(i, j) = cv::Vec3b(targetHistogram[0][source_Img.at<cv::Vec3b>(i, j)[0]], targetHistogram[1][source_Img.at<cv::Vec3b>(i, j)[1]], targetHistogram[2][source_Img.at<cv::Vec3b>(i, j)[2]]);// targrt (hedef) değerlerine göre resim ayarlanır  
        }
    }


    cv::namedWindow("output img", cv::WINDOW_AUTOSIZE);//resim gösterme işlemleri 
    cv::imshow("output img", outputImg);
    cv::waitKey(0);

    delete targetHistogram;
}

int main()
{


    Img sourcImg("./input.jpg");// giriş resim okuma ve hisaplama 
    Img referenceImg("./reference.jpg");// referens resim okuma ve hisaplama

    sourcImg.showImg("input img"); // giriş resim gösterme 
    sourcImg.showHistoImg("Histogram input img");//  giriş resim hisaplama sonuçu gösterme 

    referenceImg.showImg("reference img");// referenc resim gösterme
    referenceImg.showHistoImg("Histogram reference img"); // referens resim hisaplama sonuçu gösterme 



    CreateOutputImg(sourcImg, referenceImg);// çıkış (sonuç) resim hesaplama ve gösterme   









    return 0;
}