#include "txv_795_elgamal.h"
#include "ui_txv_795_elgamal.h"
#include <QPalette>
#include <QLineEdit>
#include <QTextEdit>
#include <iostream>
#include <random>
#include <string>
#include <cmath>
#include <QMessageBox>
#include <utility>
#include <QTextDocument>
#include <QTextCursor>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QProcess>
#include <QTemporaryFile>
#include <QTextCharFormat>
TXV_795_Elgamal::TXV_795_Elgamal(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TXV_795_Elgamal)
{
    ui->setupUi(this);
    setupLineEdit();
    connect(ui->random, &QPushButton::clicked, this, &TXV_795_Elgamal::generateRandomKey);
    connect(ui->taoKey, &QPushButton::clicked, this, &TXV_795_Elgamal::generateKey);
    connect(ui->deleteKey, &QPushButton::clicked, this, &TXV_795_Elgamal::DeleteKey);
    connect(ui->fileBR, &QPushButton::clicked, this,&TXV_795_Elgamal::openFile1);
    connect(ui->fileBM, &QPushButton::clicked, this,&TXV_795_Elgamal::openFile2);
    connect(ui->maHoa, &QPushButton::clicked, this,&TXV_795_Elgamal::convertAndEncrypt);
    connect(ui->chuyen, &QPushButton::clicked, this,&TXV_795_Elgamal::buttonChuyen);
    connect(ui->giaiMa, &QPushButton::clicked, this,&TXV_795_Elgamal::convertAndDecrypt);
    connect(ui->luuBM, &QPushButton::clicked, this,&TXV_795_Elgamal::saveFileMH);
    connect(ui->luuBR, &QPushButton::clicked, this,&TXV_795_Elgamal::saveFileGM);
}

TXV_795_Elgamal::~TXV_795_Elgamal()
{
    delete ui;
}
// Cài đặt chỉ đọc cho một số lineEdit và textEdit
void TXV_795_Elgamal::setupLineEdit()
{
    ui->editBeta->setReadOnly(true);
    ui->editKpub->setReadOnly(true);
    ui->editKpr->setReadOnly(true);
    ui->textEdit_MH2->setReadOnly(true);
    ui->textEdit_GM2->setReadOnly(true);
    // Không cho con trỏ hoạt động trong vùng này!
    ui->editBeta->setCursor(Qt::ArrowCursor);
    ui->editKpub->setCursor(Qt::ArrowCursor);
    ui->editKpr->setCursor(Qt::ArrowCursor);
    ui->textEdit_MH2->setCursor(Qt::ArrowCursor);
    ui->textEdit_GM2->setCursor(Qt::ArrowCursor);
}
void  TXV_795_Elgamal::setupLine(bool a)
{
    ui->editP->setReadOnly(a);
    ui->editAlpha->setReadOnly(a);
    ui->editA->setReadOnly(a);
}
void  TXV_795_Elgamal::setupTextEiditMH(bool a)
{
    ui->textEdit_MH1->setReadOnly(a);
}
void  TXV_795_Elgamal::setupTextEiditGM(bool a)
{
    ui->textEdit_GM1->setReadOnly(a);
}
// Các thuật toán cần dùng:
    // Thuật toán bình phương và nhân để tìm lũy thừa lớn: Hàm tính a^b mod m
    int TXV_795_Elgamal:: power(int base, int exponent, int mod) {
        long result = 1;
        base = base % mod;
        while (exponent > 0) {
            if (exponent % 2 == 1) {
                result = (result * base) % mod;
            }
            base = (base * base) % mod;
            exponent = exponent / 2;
        }
        return result;
    }
    // Hàm kiểm tra một số có phải nguyên tố hay không
    bool TXV_795_Elgamal::checkPrime(int n) {
        if (n <= 1) return false;
        if (n <= 3) return true;
        if (n % 2 == 0 || n % 3 == 0) return false;
        for (int i = 5; i * i <= n; i += 6) {
            if (n % i == 0 || n % (i + 2) == 0) return false;
        }
        return true;
    }
    // Hàm sinh ngẫu nhiên một số nguyên tố
    int TXV_795_Elgamal::generateLargePrime() {

        bool isPrimeNumber = false;
        int prime;

        while (!isPrimeNumber) {
            prime = generateRandomNumber(8000,15000);
            if (checkPrime(prime)) {
                isPrimeNumber = true;
            }
        }

        return prime;
    }
    // Sinh một số nguyên ngẫu nhiên trong khoảng từ a đến b
    int TXV_795_Elgamal::generateRandomNumber(int a, int b) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(a, b);
        return dist(gen);
    }
    // Kiểm tra xem một số có phải là phần tử nguyên thủy của một số nguyên tố hay không
    bool TXV_795_Elgamal::isPrimitiveRoot(int g, int p) {
        std::vector<bool> seen(p, false);
        int order = 0;

        for (int i = 1; i < p; ++i) {
            int val = power(g, i, p);
            if (seen[val]) {
                return false;  // Không phải phần tử nguyên thủy
            }
            seen[val] = true;
            ++order;
        }

        // Một phần tử nguyên thủy phải có bậc là phi(p) = p - 1
        return order == p - 1;
    }
    // Tìm phần tử nguyên thủy của một số nguyên tố
    int TXV_795_Elgamal::findPrimitiveRoot(int p) {
        for (int g = 2; g < p; ++g) {
            if (isPrimitiveRoot(g, p)) {
                return g;
            }
        }
        return -1;  // Không tìm thấy phần tử nguyên thủy
    }

    //Thuật toán Euclid mở rộng để tìm phần tử nghịch đảo của r1 mod r0
    int TXV_795_Elgamal::extendedEuclidean(int a, int m) {
        int m0= m;
        int y =0, x =1;
        if(m==1){
            return 0;
        }
        while(a>1){
            int q = a/m;
            int t = m;
            m = a%m;
            a =t;
            t=y;
            y = x-q*y;
            x=t;
        }
        if(x<0){
            x+=m0;
        }
        return x;
    }
    // Thuật toán tìm UCLN của hai số
    int TXV_795_Elgamal::findGCD(int a, int b) {
        while (b != 0) {
            int temp = a;
            a = b;
            b = temp % b;
        }
        return a;
    }
// Slot tạo khóa tự động.
void TXV_795_Elgamal:: generateRandomKey()
{
    int Alpha, p;
    do{
        // Chọn số nguyên tố lớn p
        p = generateLargePrime();

        // Chọn một số nguyên g là phần tử sinh trong trường modulo p
        Alpha = findPrimitiveRoot(p);
    }while(Alpha == -1);

    // Chọn khóa bí mật thứ nhất
    int A = generateRandomNumber(1, p - 2);

    // Tính khóa công khai
    int Beta = power(Alpha, A, p);
    // Chọn khóa bí mật thứ 2
    int k = generateRandomNumber(1, p - 2);
    // Hiển thị các giá trị trên giao diện
    ui->editP->setText(QString::number(p));
    ui->editAlpha->setText(QString::number(Alpha));
    ui->editA->setText(QString::number(A));
    ui->editK->setText(QString::number(k));
    ui->editBeta->setText(QString::number(Beta));
    ui->editKpr->setText(QString::number(A));
    ui->editKpub->setText(QString::number(p) + "; " + QString::number(Alpha) + "; " + QString::number(Beta));
    setupLine(true);
}
// Slot tạo key khi nhập
void TXV_795_Elgamal:: generateKey()
{
// Mở chế độ cho nhập:
    setupLine(false);

    // Kiểm tra xem người dùng đã nhập đủ các biến hay chưa
    bool isInputValid = true;

    // Lấy giá trị P từ nguồn dữ liệu người dùng
    bool ok1, ok2, ok3;
    int userInputPp = ui->editP->text().toInt(&ok1);
    int userInputAlphal = ui->editAlpha->text().toInt(&ok2);
    int userInputAa = ui->editA->text().toInt(&ok3);

    // Kiểm tra xem người dùng đã nhập đủ biến P, Alpha và A hay chưa
    if (!ok1) {
        QMessageBox::information(this, "Warning", "<html><b>Chưa nhập giá trị P!</b></html>");
        isInputValid = false;
    }
    else{
        if(userInputPp < 8000){
               QMessageBox::information(this, "Warning", "<html><b>Giá trị của P chưa đủ lớn! (P>8000)</b></html>");
               isInputValid = false;
       }
   }

    if (!ok2) {
        QMessageBox::information(this, "Warning", "<html><b>Chưa nhập giá trị Alpha!</b></html>");
        isInputValid = false;
    }

    if (!ok3) {
        QMessageBox::information(this, "Warning", "<html><b>Chưa nhập giá trị A!</b></html>");
        isInputValid = false;
    }

    // Kiểm tra xem tất cả các biến đã được nhập đúng định dạng hay chưa
    if (!isInputValid) {
        return;
    }

    // Kiểm tra tính nguyên tố của P
    if (!checkPrime(userInputPp)) {
        QMessageBox::information(this, "Warning", "<html><b>Giá trị P không phải là số nguyên tố!</b></html>");
        return;
    }

    // Kiểm tra tính nguyên sinh của Alpha
    if (!isPrimitiveRoot(userInputAlphal, userInputPp)) {
        QMessageBox::information(this, "Warning", "<html><b>Alpha không phải là phần tử nguyên sinh của số nguyên tố P!</b></html>");
        return;
    }

    // Kiểm tra giá trị A
    if (userInputAa <= 0 || userInputAa >= userInputPp - 1) {
        QMessageBox::information(this, "Warning", "<html><b>Số bí mật thứ nhất không thỏa mãn!</b></html>");
        return;
    }

    // Nếu tất cả các kiểm tra trên đều thành công, thực hiện phần còn lại của hàm
    ui->editP->setText(QString::number(userInputPp));
    ui->editAlpha->setText(QString::number(userInputAlphal));
    ui->editA->setText(QString::number(userInputAa));
    ui->editBeta->setText(QString::number(power(userInputAlphal, userInputAa, userInputPp)));
    ui->editKpr->setText(QString::number(userInputAa));
    ui->editKpub->setText(QString::number(userInputPp) + "; " + QString::number(userInputAlphal) + "; " + QString::number(power(userInputAlphal, userInputAa, userInputPp)));
    setupLine(true);
}
// Slot khi xóa khóa
void TXV_795_Elgamal:: DeleteKey()
{
    ui->editP->clear();
    ui->editAlpha->clear();
    ui->editA->clear();
    ui->editBeta->clear();
    ui->editKpub->clear();
    ui->editKpr->clear();
    ui->editK->clear();
    ui->textEdit_MH2->clear();
    ui->textEdit_MH1->clear();
    ui->textEdit_GM2->clear();
    ui->textEdit_GM1->clear();
    setupLine(false);
    setupTextEiditGM(false);
    setupTextEiditMH(false);
}

// Hàm đọc và hiện thị nội dung của 1 file
void TXV_795_Elgamal::readAndDisplayDocx(const QString& filePath, QTextEdit* textEdit)
{
    // Tạo tệp tin tạm để lưu nội dung đã chuyển đổi
    QString tempFilePath = QDir::tempPath() + "/temp.html";

    // Chuyển đổi tệp tin .docx sang HTML bằng Pandoc
    QString pandocPath = "C:\\Program Files\\pandoc\\pandoc.exe";
    QStringList arguments;
    arguments << "-s" << filePath << "-o" << tempFilePath;

    QProcess process;
    process.setProgram(pandocPath);
    process.setArguments(arguments);
    process.start();
    process.waitForFinished();

    // Không chuyển đổi được sang HTML thì đọc trực tiếp trong lớp QtEdit
    if (process.exitCode() != 0)
    {
        // Đọc nội dung của file .docx
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly))
        {
               QByteArray docxData = file.readAll();
               file.close();

               // Hiển thị nội dung thuần túy trong QTextEdit
               QString plainText = QString::fromUtf8(docxData);
               textEdit->setPlainText(plainText);
        }
    }else{
        // Đọc nội dung HTML đã chuyển đổi và hiển thị trong QTextEdit
        QFile file(tempFilePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
               QTextStream stream(&file);
               QString html = stream.readAll();

               // Hiển thị HTML trong QTextEdit
               textEdit->setHtml(html);

               file.close();
        }

        // Xóa tệp tin tạm
        QFile::remove(tempFilePath);
    }
}

// Xây dựng slot chọn file để đọc và hiển thị nội dung vào QTextEdit textEdit_MH1
void TXV_795_Elgamal::openFile1()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Chọn file", "", "Text Files (*.txt *.docx *.doc)");

    if (!filePath.isEmpty())
    {
        readAndDisplayDocx(filePath, ui->textEdit_MH1);
        setupTextEiditMH(true);
    }
}
// Xây dựng slot chọn file để đọc và hiển thị nội dung vào QTextEdit textEdit_GM1
void TXV_795_Elgamal::openFile2()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Chọn file", "", "Text Files (*.txt *.docx *.doc)");

    if (!filePath.isEmpty())
    {
        readAndDisplayDocx(filePath, ui->textEdit_GM1);
        setupTextEiditMH(true);
    }
}
// Thuật toán mã hóa Elgamal
QList<std::pair<int, int>> TXV_795_Elgamal::ElGamalEncrypt(const QList<int>& plaintext)
{
    bool ok, ok1, ok2, ok3;
    int k = ui->editK->text().toInt(&ok);
    int p = ui->editP->text().toInt(&ok1);
    int g = ui->editAlpha->text().toInt(&ok2);
    int beta = ui->editBeta->text().toInt(&ok3);

    QList<std::pair<int, int>> encryptedList;

    for (int num : plaintext)
    {
        int y1 = power(g, k, p);
        int y2 = (num * power(beta, k, p)) % p;

        encryptedList.append(std::make_pair(y1, y2));
    }

    return encryptedList;
}
void TXV_795_Elgamal::buttonChuyen()
{
    QString ciphertext = ui->textEdit_MH2->toPlainText();
    ui->textEdit_GM1->setPlainText(ciphertext);
    setupTextEiditGM(true);
}
// Thuật toán giải mã Elgamal
QList<int> TXV_795_Elgamal::ElGamalDecrypt(const QList<std::pair<int, int>>& ciphertext)
{
    bool ok, ok1;
    int a = ui->editA->text().toInt(&ok);
    int p = ui->editP->text().toInt(&ok1);
    QList<int> decryptedList;

    for (const auto& pair : ciphertext)
    {
        int y1 = pair.first;
        int y2 = pair.second;

        // Giải mã cặp (y1, y2)
        int num = (y2 * extendedEuclidean(power(y1, a, p), p)) % p;
        decryptedList.append(num);
    }

    return decryptedList;
}
// Slot mã hóa văn bản trong textEditMH1 và hiển thị trong textEditMH2
void TXV_795_Elgamal::convertAndEncrypt()
{
    bool isInputValid = true;
    bool isInputValid2 = true;
    bool ok, ok1;
    int p = ui->editP->text().toInt(&ok1);
    int k = ui->editK->text().toInt(&ok);
    if (!ok1) {
        QMessageBox::information(this, "Warning", "<html><b>Chưa tạo khóa!</b></html>");
        isInputValid = false;
    }
    if (!isInputValid) {
        return;
    }
    if (!ok) {
        QMessageBox::information(this, "Warning", "<html><b>Chưa nhập số bí mật thứ 2!</b></html>");
        isInputValid2 = false;
    }
    if (!isInputValid2) {
        return;
    }
    if (k <= 0 || k >= p - 1) {
        QMessageBox::information(this, "Warning", "<html><b>Số bí mật thứ hai không thỏa mãn!</b></html>");
        return;
    }
     QString plaintext = ui->textEdit_MH1->toPlainText().replace("\n", "\\n"); // khi sử dụng mỗi toPlainText thì dấu xuống dòng sẽ bị bỏ thay vào là khoảng trắng
    // Chuyển đổi văn bản thành một dãy số nguyên
    QList<int> intList = convertTextToIntList(plaintext);

    // Mã hóa dãy số nguyên sử dụng ElGamal
    QList<std::pair<int, int>> encryptedData = ElGamalEncrypt(intList);

    // Tạo chuỗi mã hóa để hiển thị
    QString ciphertext;
    QList<int> list;
    for (const auto& pair : encryptedData) {
        list.append(pair.first);
        list.append(pair.second);
    }
    ciphertext = encodeBase64(list);
    // Hiển thị chuỗi mã hóa trong textEdit_MH2
    ui->textEdit_MH2->setPlainText(ciphertext);
}

// Slot giải hóa văn bản trong textEditGM1 và hiển thị trong textEditGM2
void TXV_795_Elgamal::convertAndDecrypt()
{
    bool ok1,ok2;
    bool isInputValid = true;
    QString ciphertext = ui->textEdit_GM1->toPlainText();
    ui->editP->text().toInt(&ok1);
    ui->editA->text().toInt(&ok2);
    if (!ok1) {
        QMessageBox::information(this, "Warning", "<html><b>Chưa nhập p!</b></html>");
        isInputValid = false;
    }
    if (!ok2) {
        QMessageBox::information(this, "Warning", "<html><b>Chưa nhập Kpr!</b></html>");
        isInputValid = false;
    }
    if (!isInputValid) {
        return;
    }
    // Chuyển đổi chuỗi mã hóa thành dãy các cặp số nguyên
    QList<std::pair<int, int>> intPairs = convertCiphertextToIntPairs(ciphertext);

    // Giải mã ElGamal với dãy cặp số nguyên này thành dãy số nguyên
    QList<int> decryptedList = ElGamalDecrypt(intPairs);

    QString plaintext;

    for (int num : decryptedList)
    {
        QString character; // Ánh xạ số nguyên thành ký tự tương ứng
        if(num == -2){
               character = "\\n";
        }else{
               character = convertToChar(num);
        }
        plaintext += character;
    }
    // Khôi phục ký tự xuống dòng
    plaintext = plaintext.replace("\\n", "\n");

    // Hiển thị văn bản giải mã trong textEdit_GM2
    ui->textEdit_GM2->setPlainText(plaintext);
}

// Chuyển đổi văn bản thành một dãy số nguyên trong trường modulo p
QList<int> TXV_795_Elgamal::convertTextToIntList(const QString& text)
{
    QList<int> intList;

    for (const QChar& ch : text)
    {
        int intValue; // Ánh xạ ký tự thành số nguyên trong trường modulo p
        // Không được viết là if(ch =="\\n") vì trình biên dịch sẽ hiểu là \\n = \ và \n
        if(ch == QChar('\\') && ch == QChar('n')){
               intValue = -2; // định nghĩa xuống dòng là -1
        }else{
               intValue = convertToInt(ch);
        }
        intList.append(intValue);
    }

    return intList;
}

// Chuyển đổi chuỗi mã hóa thành dãy các cặp số nguyên trong trường modulo p
QList<std::pair<int, int>> TXV_795_Elgamal::convertCiphertextToIntPairs(const QString& ciphertext)
{
    QList<std::pair<int, int>> intPairs;
    QList<int> intList = decodeBase64(ciphertext);

    for (int i = 0; i < intList.size(); i += 2) {
        int firstNum = intList[i];
        int secondNum = intList[i + 1];
        intPairs.append(std::make_pair(firstNum, secondNum));
    }
    return intPairs;
}
// Ánh xạ số nguyên mã hóa thành ký tự tương ứng trong trường modulo p
QString TXV_795_Elgamal::convertToChar(int num)
{
    int convertedValue = num + 32; // Chuyển số nguyên thành giá trị Unicode (32-126)

    return QString(QChar(convertedValue));
}

// Ánh xạ ký tự thành số nguyên tương ứng trong trường modulo p
int TXV_795_Elgamal::convertToInt(QChar ch)
{
    int unicode = ch.unicode(); // Lấy giá trị Unicode của ký tự
    int intValue = unicode - 32; // Chuyển giá trị Unicode thành số nguyên trong trường modulo p

    return intValue;
}
// Hàm mã hóa dãy số nguyên thành chuỗi Base64
QString TXV_795_Elgamal::encodeBase64(const QList<int>& intList)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    for (int num : intList) {
        stream << num;
    }

    QByteArray encodedData = data.toBase64();
    return QString(encodedData);
}
QList<int> TXV_795_Elgamal::decodeBase64(const QString& base64String)
{
    QByteArray decodedData = QByteArray::fromBase64(base64String.toUtf8());

    QList<int> intList;
    QDataStream stream(decodedData);

    while (!stream.atEnd()) {
        int num;
        stream >> num;
        intList.append(num);
    }

    return intList;
}
// Slot để xử lý khi nhấn vào QPushButton luuBM
void TXV_795_Elgamal::saveFileMH()
{
    // Lấy nội dung từ QTextEdit
    QString content = ui->textEdit_MH2->toPlainText();

    // Hiển thị hộp thoại lưu file
        QString filePath = QFileDialog::getSaveFileName(this, "Lưu file", "", "Text Files (*.txt);;Word Files (*.docx)");

        // Kiểm tra xem người dùng đã chọn đường dẫn lưu trữ hay chưa
        if (!filePath.isEmpty())
    {
        // Tạo và mở file để ghi nội dung vào
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
                QTextStream stream(&file);
                stream << content;
                file.close();

                QMessageBox::information(this, "Thông báo", "Đã lưu file thành công!");
        }
        else
        {
                QMessageBox::critical(this, "Lỗi", "Không thể lưu file!");
        }
    }
}
// Slot để xử lý khi nhấn vào QPushButton luuBM
void TXV_795_Elgamal::saveFileGM()
{
    // Lấy nội dung từ QTextEdit
    QString content = ui->textEdit_GM2->toPlainText();

    // Hiển thị hộp thoại lưu file
    QString filePath = QFileDialog::getSaveFileName(this, "Lưu file", "", "Text Files (*.txt);;Word Files (*.docx)");

        // Kiểm tra xem người dùng đã chọn đường dẫn lưu trữ hay chưa
        if (!filePath.isEmpty())
    {
        // Tạo và mở file để ghi nội dung vào
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
                QTextStream stream(&file);
                stream << content;
                file.close();

                QMessageBox::information(this, "Thông báo", "Đã lưu file thành công!");
        }
        else
        {
                QMessageBox::critical(this, "Lỗi", "Không thể lưu file!");
        }
    }
}
