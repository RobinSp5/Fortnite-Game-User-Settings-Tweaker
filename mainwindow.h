#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QSettings>
#include <QFile>
#include <QListWidgetItem>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString filepath;

private slots:
    void on_horizontalSlider_DisplayGamma_valueChanged(int value);
    void on_Dateieinlesen_clicked();
    void on_pushButton_SaveChanges_clicked();
    void on_pushButton_GenBackupFile_clicked();
    void on_checkBoxUseDefaultPath_checkStateChanged(const Qt::CheckState &arg1);
    void on_pushButton_RecSettings_clicked();
    void on_pushButtonNewPreset_clicked();
    void on_pushButtonLoadPreset_clicked();
    void on_pushButtonDeletePreset_clicked();
    void on_listWidgetSavedPresets_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButtonCopyFileContent_clicked();

private:
    void saveSettings();
    void loadSettings();
    void findStuffinFile(QFile &file, int &resX, int &resY, 
                        std::string &MotionBlur, std::string &bUseGPUCrashDebugging,
                        std::string &LatencyTweak1, int &LatencyTweak2, 
                        int &DLSSQuality, int &XeSSQuality, double &DisplayGamma,
                        std::string &FortAntiAliasingMethod, std::string &bRayTracing,
                        int &DesiredGlobalIlluminationQuality, int &DesiredReflectionQuality,
                        int &PreNaniteGlobalIlluminationQuality, int &PreNaniteReflectionQuality, int &FrontendFrameRateLimit);
    Qt::CheckState StringToBool(const std::string &tmp);

    Ui::MainWindow *ui;
    double m_displayGamma;
};

#endif // MAINWINDOW_H
