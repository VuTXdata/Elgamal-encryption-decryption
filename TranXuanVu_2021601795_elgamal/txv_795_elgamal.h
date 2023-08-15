#ifndef TXV_795_ELGAMAL_H
#define TXV_795_ELGAMAL_H

#include <QMainWindow>
#include <QTextEdit>
QT_BEGIN_NAMESPACE
namespace Ui { class TXV_795_Elgamal; }
QT_END_NAMESPACE

class TXV_795_Elgamal : public QMainWindow
{
    Q_OBJECT

public:
    TXV_795_Elgamal(QWidget *parent = nullptr);
    ~TXV_795_Elgamal();
    // Khai báo các hàm
    void setupLineEdit();
    void setupLine(bool a);
    void setupTextEiditMH(bool a);
    void setupTextEiditGM(bool a);
    int power(int base, int exponent, int mod);
    bool checkPrime(int n);
    int extendedEuclidean(int a, int m);
    int generateLargePrime();
    bool isPrimitiveRoot(int g, int p);
    int findPrimitiveRoot(int p);
    int generateRandomNumber(int a, int b);
    int findGCD(int a, int b);
    QList<std::pair<int, int>>ElGamalEncrypt(const QList<int>& plaintext);
    void readAndDisplayDocx(const QString& filePath, QTextEdit* textEdit);
    QList<int>convertTextToIntList(const QString& text);
    QList<int>ElGamalDecrypt(const QList<std::pair<int, int>>& ciphertext);
    QList<std::pair<int, int>>convertCiphertextToIntPairs(const QString& ciphertext);
    QString convertToChar(int num);
    int convertToInt(QChar ch);
    QString encodeBase64(const QList<int>& intList);
    QList<int> decodeBase64(const QString& base64String);
    // Khai báo các slots
private slots:
    void generateRandomKey();
    void generateKey();
    void DeleteKey();
    void openFile1();
    void openFile2();
    void convertAndEncrypt();
    void buttonChuyen();
    void convertAndDecrypt();
    void saveFileMH();
    void saveFileGM();
    // Khai báo các biến cần dùng

private:
    Ui::TXV_795_Elgamal *ui;
};
#endif // TXV_795_ELGAMAL_H
