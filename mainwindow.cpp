#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>  // Für den Dateiauswahldialog
#include <QInputDialog>
#include <QDir>
#include<sstream>
#include <iostream>
#include <QString>
#include <QCheckBox>
#include <QSettings>
#include <QStringListModel>  // Neues Include
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialisiere Member-Variablen
    m_displayGamma = 2.2;

    // Setze den Wert für das DoubleSpinBox direkt
    ui->doubleSpinBoxDisplayGamma->setValue(m_displayGamma);

    this->setWindowTitle("Fortnite Game User Settings Tweaker");

    // Lade gespeicherte Einstellungen
    loadSettings();
}

MainWindow::~MainWindow()
{
    // Speichere die Einstellungen beim Beenden
    saveSettings();
    delete ui;
}

bool solve(std::string ip) {
    bool op;
    std::istringstream(ip) >> std::boolalpha >> op;
    return op;
}

void MainWindow::on_Dateieinlesen_clicked()
{
    QString filepath;


    if(ui->checkBoxUseDefaultPath->isChecked()){
        QString userProfile = qgetenv("USERPROFILE");
        filepath = userProfile + "\\AppData\\Local\\FortniteGame\\Saved\\Config\\WindowsClient\\GameUserSettings.ini";
    }

    else{
        filepath = QFileDialog::getOpenFileName(this,
                                                "Datei auswählen",
                                                QDir::homePath(),
                                                "Textdateien (*.txt);;Alle Dateien (*.*)");

        // Prüfen, ob der Benutzer eine Datei ausgewählt hat
        if (filepath.isEmpty()) {
            ui->textBrowser->setText("Keine Datei ausgewählt.");
            return;
        }

    }

    //Datei oeffnen
    QFile file(filepath);

    // Prüfen, ob die Datei existiert
    if (!file.exists()) {
        ui->textBrowser->setText("This file does not exist " + filepath);
        return;  // Funktion verlassen
    }

    // Versuchen, die Datei zu öffnen
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(this, "Info", "Error openig file... " + file.errorString());
        return;  // Funktion verlassen
    }

    // Datei erfolgreich geöffnet, Inhalt lesen
    QTextStream in(&file);
    QString content = in.readAll();  // Inhalt der Datei in eine QString lesen
    ui->textBrowser->setText("Filepath: \n" +filepath + "\n"+ "\n"+ "\n" + content);
    //ui->textBrowser->setText(content);  // Inhalt im TextBrowser anzeigen

    // Datei zurück an den Anfang setzen
    file.seek(0);

    // Variablen definieren, die aus der Datei ausgelesen werden sollen
    int resX = 0;
    int resY = 0;
    std::string MotionBlur;
    std::string bUseGPUCrashDebugging;
    std::string LatencyTweak1;
    int LatencyTweak2 = 0;
    int DLSSQuality = 0;
    int XeSSQuality = 0;
    double DisplayGamma = 0.0;
    std::string FortAntiAliasingMethod;
    std::string bRayTracing;
    int DesiredGlobalIlluminationQuality = 0;
    int DesiredReflectionQuality = 0;
    int PreNaniteGlobalIlluminationQuality = 0;
    int PreNaniteReflectionQuality = 0;
    int FrontendFrameRateLimit = 0;

    // Nur die benötigten Argumente übergeben
    findStuffinFile(file, resX, resY, MotionBlur, bUseGPUCrashDebugging, LatencyTweak1, LatencyTweak2, DLSSQuality, XeSSQuality, DisplayGamma, FortAntiAliasingMethod, bRayTracing, DesiredGlobalIlluminationQuality, DesiredReflectionQuality, PreNaniteGlobalIlluminationQuality, PreNaniteReflectionQuality, FrontendFrameRateLimit);

    // UI-Elemente mit den ausgelesenen Werten aktualisieren
    ui->textEdit_ResX->setPlainText(QString::number(resX));
    ui->textEdit_ResY->setPlainText(QString::number(resY));
    ui->checkBox_MotionBlur->setCheckState(StringToBool(MotionBlur));
    ui->checkBoxUseGPUCrashDebugging->setCheckState(StringToBool(bUseGPUCrashDebugging));
    ui->checkBoxLatencyTweak1->setCheckState(StringToBool(LatencyTweak1));
    ui->spinBoxLatencyTweak2->setValue(LatencyTweak2);
    ui->spinBoxDLSSQuality->setValue(DLSSQuality);
    ui->spinBox_XeSSQuality->setValue(XeSSQuality);
    ui->doubleSpinBoxDisplayGamma->setValue(DisplayGamma);

    if(FortAntiAliasingMethod == "DLSS"){
        ui->comboBoxAntiAliasingMethod->setCurrentIndex(1);
    }

    if(FortAntiAliasingMethod == "Disabled"){
        ui->comboBoxAntiAliasingMethod->setCurrentIndex(2);
    }


    ui->checkBoxRayTracing->setCheckState(StringToBool(bRayTracing));
    ui->spinBoxDesiredGlobalIlluminationQuality->setValue(DesiredGlobalIlluminationQuality);
    ui->spinBoxDesiredReflectionQuality->setValue(DesiredReflectionQuality);
    ui->spinBox_PreNaniteGlobalIlluminationQuality->setValue(PreNaniteGlobalIlluminationQuality);
    ui->spinBox_PreNaniteReflectionQuality->setValue(PreNaniteReflectionQuality);
    ui->spinBoxFrontEndFrameLimit->setValue(FrontendFrameRateLimit);




    file.close();  // Datei schließen
}

//Die Werte der GameUserSettings datei einlesen und in das GUI uebertragen
//void ValuesToUi(QFile &file){

//}

//Methode um Zeug in der ini Datei zu finden
void MainWindow::findStuffinFile(QFile &file, int &resX, int &resY, std::string &MotionBlur, std::string &bUseGPUCrashDebugging, std::string &LatencyTweak1, int &LatencyTweak2, int &DLSSQuality, int &XeSSQuality, double &DisplayGamma, std::string &FortAntiAliasingMethod, std::string &bRayTracing, int &DesiredGlobalIlluminationQuality, int &DesiredReflectionQuality, int &PreNaniteGlobalIlluminationQuality, int &PreNaniteReflectionQuality,int &FrontendFrameRateLimit) {
    QTextStream in(&file);
    QString line;

    while (!in.atEnd()) {
        line = in.readLine();

        if (line.startsWith("ResolutionSizeX=")) {
            resX = line.mid(16).toInt();
        }
        if (line.startsWith("ResolutionSizeY=")) {
            resY = line.mid(16).toInt();
        }
        if (line.startsWith("bMotionBlur=")) {
            MotionBlur = line.mid(12).toStdString();
        }
        if (line.startsWith("bUseGPUCrashDebugging=")) {
            bUseGPUCrashDebugging = line.mid(22).toStdString();
        }
        if (line.startsWith("LatencyTweak1=")) {
            LatencyTweak1 = line.mid(14).toStdString();
        }
        if (line.startsWith("LatencyTweak2=")) {
            LatencyTweak2 = line.mid(14).toInt();
        }
        if (line.startsWith("DLSSQuality=")) {
            DLSSQuality = line.mid(12).toInt();
        }
        if (line.startsWith("XeSSQuality=")) {
            XeSSQuality = line.mid(12).toInt();
        }
        if (line.startsWith("DisplayGamma=")) {
            DisplayGamma = line.mid(13).toDouble();
        }
        if (line.startsWith("FortAntiAliasingMethod=")) {
            FortAntiAliasingMethod = line.mid(23).toStdString();
        }
        if (line.startsWith("bRayTracing=")) {
            bRayTracing = line.mid(12).toStdString();
        }
        if (line.startsWith("DesiredGlobalIlluminationQuality=")) {
            DesiredGlobalIlluminationQuality = line.mid(33).toInt();
        }
        if (line.startsWith("DesiredReflectionQuality=")) {
            DesiredReflectionQuality = line.mid(26).toInt();
        }
        if (line.startsWith("PreNaniteGlobalIlluminationQuality=")) {
            PreNaniteGlobalIlluminationQuality = line.mid(35).toInt();
        }
        if (line.startsWith("PreNaniteReflectionQuality=")) {
            PreNaniteReflectionQuality = line.mid(28).toInt();
        }
        if (line.startsWith("FrontendFrameRateLimit=")) {  // New parsing
            FrontendFrameRateLimit = line.mid(23).toInt();
        }
    }
}


//Backup File generieren im selben Ordner
void MainWindow::on_pushButton_GenBackupFile_clicked()
{
    // Wenn eine Datei vom Benutzer ausgewählt wird
    QString filepath;
    if (ui->checkBoxUseDefaultPath->isChecked()) {
        filepath = "C:\\work\\Downloads\\GameUserSettings.ini";  // Standardpfad
    } else {
        filepath = QFileDialog::getOpenFileName(this,
                                                "Datei auswählen",
                                                QDir::homePath(),
                                                "Textdateien (*.ini);;Alle Dateien (*.*)");
    }

    // Überprüfen, ob der Benutzer eine Datei ausgewählt hat
    if (filepath.isEmpty()) {
        QMessageBox::warning(this, "Fehler", "Keine Datei ausgewählt.");
        return;
    }

    // Öffne die Originaldatei zum Lesen
    QFile file(filepath);
    if (!file.exists()) {
        QMessageBox::warning(this, "Fehler", "Die Originaldatei existiert nicht.");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Fehler", "Fehler beim Öffnen der Originaldatei: " + file.errorString());
        return;
    }

    // Lese den Inhalt der Originaldatei
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // Generiere den Pfad für die Backup-Datei
    QString backupFilePath = filepath + ".bak";
    int counter = 0;
    while (QFile::exists(backupFilePath)) {
        counter++;
        backupFilePath = filepath + ".bak" + QString::number(counter);
    }

    // Öffne die Backup-Datei zum Schreiben
    QFile backupFile(backupFilePath);
    if (!backupFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Fehler", "Fehler beim Erstellen der Backup-Datei: " + backupFile.errorString());
        return;
    }

    // Schreibe den Inhalt der Originaldatei in die Backup-Datei
    QTextStream out(&backupFile);
    out << content;
    backupFile.close();

    // Zeige eine Bestätigungsmeldung
    QMessageBox::information(this, "Erfolg", "Backup-Datei wurde erfolgreich erstellt: " + backupFilePath);
}




void MainWindow::on_checkBoxUseDefaultPath_checkStateChanged(const Qt::CheckState &arg1)
{

}

//Pushbutton um die recommended Settings anzupassen
void MainWindow::on_pushButton_RecSettings_clicked()
{
    // Motion Blur ausschalten
    ui->checkBox_MotionBlur->setCheckState(Qt::Unchecked);

    // GPU Crash Debugging ausschalten
    ui->checkBoxUseGPUCrashDebugging->setCheckState(Qt::Unchecked);

    // Latency Tweak 1 einschalten
    ui->checkBoxLatencyTweak1->setCheckState(Qt::Checked);

    // Latency Tweak 2 auf 1 setzen
    ui->spinBoxLatencyTweak2->setValue(1);

    // DLSS und XeSS Quality auf 0
    ui->spinBoxDLSSQuality->setValue(0);
    ui->spinBox_XeSSQuality->setValue(0);

    // Weitere Einstellungen auf 0/aus setzen
    ui->spinBoxDesiredGlobalIlluminationQuality->setValue(0);
    ui->spinBoxDesiredReflectionQuality->setValue(0);
    ui->spinBox_PreNaniteGlobalIlluminationQuality->setValue(0);
    ui->spinBox_PreNaniteReflectionQuality->setValue(0);
    ui->checkBoxRayTracing->setCheckState(Qt::Unchecked);
    ui->spinBoxFrontEndFrameLimit->setValue(120);
}

//Methode um die Werte aus der Datei von einem String zu bool fuer das UI zu konvertieren
Qt::CheckState MainWindow::StringToBool(const std::string &tmp) {
    if (tmp == "False") {
        return Qt::Unchecked;
    }
    else{
        return Qt::Checked;
    }
}



//Save Changes Button
void MainWindow::on_pushButton_SaveChanges_clicked(QString filepath)
{
    // Ersetze den Platzhalterpfad durch den tatsächlichen Pfad
    //QString filePath = "C:/path/to/your/file.txt";  // Beispielpfad

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file: " + filepath + "\nError: " + file.errorString());
        return;
    }

    QTextStream out(&file);

    // Lese die aktuellen Werte aus den UI-Elementen
    int resX = ui->textEdit_ResX->toPlainText().toInt();
    int resY = ui->textEdit_ResY->toPlainText().toInt();
    QString motionBlur = ui->checkBox_MotionBlur->isChecked() ? "True" : "False";
    QString useGPUCrashDebugging = ui->checkBoxUseGPUCrashDebugging->isChecked() ? "True" : "False";
    QString latencyTweak1 = ui->checkBoxLatencyTweak1->isChecked() ? "True" : "False";
    int latencyTweak2 = ui->spinBoxLatencyTweak2->value();
    int dlssQuality = ui->spinBoxDLSSQuality->value();
    int xessQuality = ui->spinBox_XeSSQuality->value();
    double displayGamma = ui->doubleSpinBoxDisplayGamma->value();
    QString fortAntiAliasingMethod = ui->comboBoxAntiAliasingMethod->currentText();
    QString rayTracing = ui->checkBoxRayTracing->isChecked() ? "True" : "False";
    int desiredGlobalIlluminationQuality = ui->spinBoxDesiredGlobalIlluminationQuality->value();
    int desiredReflectionQuality = ui->spinBoxDesiredReflectionQuality->value();
    int preNaniteGlobalIlluminationQuality = ui->spinBox_PreNaniteGlobalIlluminationQuality->value();
    int preNaniteReflectionQuality = ui->spinBox_PreNaniteReflectionQuality->value();
    int frontendFrameRateLimit = ui->spinBoxFrontEndFrameLimit->value();

    // Schreibe die Werte in die Datei
    out << "ResolutionSizeX=" << resX << "\n";
    out << "ResolutionSizeY=" << resY << "\n";
    out << "bMotionBlur=" << motionBlur << "\n";
    out << "bUseGPUCrashDebugging=" << useGPUCrashDebugging << "\n";
    out << "LatencyTweak1=" << latencyTweak1 << "\n";
    out << "LatencyTweak2=" << latencyTweak2 << "\n";
    out << "DLSSQuality=" << dlssQuality << "\n";
    out << "XeSSQuality=" << xessQuality << "\n";
    out << "DisplayGamma=" << QString::number(displayGamma, 'f', 6) << "\n";  // Beispiel: 6 Dezimalstellen
    out << "FortAntiAliasingMethod=" << fortAntiAliasingMethod << "\n";
    out << "bRayTracing=" << rayTracing << "\n";
    out << "DesiredGlobalIlluminationQuality=" << desiredGlobalIlluminationQuality << "\n";
    out << "DesiredReflectionQuality=" << desiredReflectionQuality << "\n";
    out << "PreNaniteGlobalIlluminationQuality=" << preNaniteGlobalIlluminationQuality << "\n";
    out << "PreNaniteReflectionQuality=" << preNaniteReflectionQuality << "\n";
    out << "FrontendFrameRateLimit=" << frontendFrameRateLimit << "\n";

    file.close();

    QMessageBox::information(this, "Success", "All Changes have been saved.");


    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error",
                              "Could not open file: " + filepath +
                                  "\nError code: " + QString::number(file.error()) +
                                  "\nError message: " + file.errorString());
        return;
    }

}



void MainWindow::on_pushButtonNewPreset_clicked()
{
    // Dialog für den Preset-Namen öffnen
    bool ok;
    QString presetName = QInputDialog::getText(this, "New Preset",
                                               "Name this preset:",
                                               QLineEdit::Normal,
                                               "", &ok);

    if (ok && !presetName.isEmpty()) {
        // Prüfe ob der Name bereits existiert
        for(int i = 0; i < ui->listWidgetSavedPresets->count(); i++) {
            if(ui->listWidgetSavedPresets->item(i)->text() == presetName) {
                QMessageBox::warning(this, "Error",
                                     "A preset with this name already exists!\nPlease choose a different name.");
                return;
            }
        }

        // Aktuelle GUI-Werte auslesen
        QString preset = presetName + "|" +
                         ui->textEdit_ResX->toPlainText() + "|" +
                         ui->textEdit_ResY->toPlainText() + "|" +
                         (ui->checkBox_MotionBlur->isChecked() ? "True" : "False") + "|" +
                         (ui->checkBoxUseGPUCrashDebugging->isChecked() ? "True" : "False") + "|" +
                         (ui->checkBoxLatencyTweak1->isChecked() ? "True" : "False") + "|" +
                         QString::number(ui->spinBoxLatencyTweak2->value()) + "|" +
                         QString::number(ui->spinBoxDLSSQuality->value()) + "|" +
                         QString::number(ui->spinBox_XeSSQuality->value()) + "|" +
                         QString::number(ui->doubleSpinBoxDisplayGamma->value()) + "|" +
                         ui->comboBoxAntiAliasingMethod->currentText() + "|" +
                         (ui->checkBoxRayTracing->isChecked() ? "True" : "False") + "|" +
                         QString::number(ui->spinBoxDesiredGlobalIlluminationQuality->value()) + "|" +
                         QString::number(ui->spinBoxDesiredReflectionQuality->value()) + "|" +
                         QString::number(ui->spinBox_PreNaniteGlobalIlluminationQuality->value()) + "|" +
                         QString::number(ui->spinBox_PreNaniteReflectionQuality->value());
        QString::number(ui->spinBoxFrontEndFrameLimit->value());

        // Neues Item zur Liste hinzufügen
        QListWidgetItem* item = new QListWidgetItem(presetName);
        item->setData(Qt::UserRole, preset);
        ui->listWidgetSavedPresets->addItem(item);
    }
}


void MainWindow::on_pushButtonLoadPreset_clicked()
{
    // Aktuell ausgewähltes Item aus der Liste holen
    QListWidgetItem* currentItem = ui->listWidgetSavedPresets->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "Error", "Select a preset first!");
        return;
    }

    // Preset-String aus den Daten des Items holen
    QString preset = currentItem->data(Qt::UserRole).toString();
    QStringList values = preset.split("|");

    // Werte in die GUI übertragen
    ui->textEdit_ResX->setText(values[1]);
    ui->textEdit_ResY->setText(values[2]);
    ui->checkBox_MotionBlur->setChecked(values[3].toLower() == "true");
    ui->checkBoxUseGPUCrashDebugging->setChecked(values[4].toLower() == "true");
    ui->checkBoxLatencyTweak1->setChecked(values[5].toLower() == "true");
    ui->spinBoxLatencyTweak2->setValue(values[6].toInt());
    ui->spinBoxDLSSQuality->setValue(values[7].toInt());
    ui->spinBox_XeSSQuality->setValue(values[8].toInt());
    ui->doubleSpinBoxDisplayGamma->setValue(values[9].toDouble());
    ui->comboBoxAntiAliasingMethod->setCurrentText(values[10]);
    ui->checkBoxRayTracing->setChecked(values[11].toLower() == "true");
    ui->spinBoxDesiredGlobalIlluminationQuality->setValue(values[12].toInt());
    ui->spinBoxDesiredReflectionQuality->setValue(values[13].toInt());
    ui->spinBox_PreNaniteGlobalIlluminationQuality->setValue(values[14].toInt());
    ui->spinBox_PreNaniteReflectionQuality->setValue(values[15].toInt());
    ui->spinBoxFrontEndFrameLimit->setValue(values[16].toInt());

    QMessageBox::information(this, "Awesome!", "Preset was loaded succesfully.");
}


void MainWindow::on_pushButtonDeletePreset_clicked()
{
    // Aktuell ausgewähltes Item aus der Liste holen
    QListWidgetItem* currentItem = ui->listWidgetSavedPresets->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "Error", "Please select a preset first!");
        return;
    }

    // Bestätigungsdialog anzeigen
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "delete preset",
        "Do you really want to delete preset  '" + currentItem->text() + "' ?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // Item aus der Liste entfernen
        delete ui->listWidgetSavedPresets->takeItem(
            ui->listWidgetSavedPresets->row(currentItem)
            );
        QMessageBox::information(this, "Success", "Preset was deleted successfully.");
    }
}

void MainWindow::on_listWidgetSavedPresets_itemDoubleClicked(QListWidgetItem *item)
{
    // Einfach die Load Preset Funktion aufrufen
    on_pushButtonLoadPreset_clicked();
}

// Neue Methode zum Speichern der Einstellungen
void MainWindow::saveSettings()
{
    QSettings settings("Duesentrieb54", "FortniteEditor");

    // Speichere die aktuellen Werte
    settings.setValue("ResolutionX", ui->textEdit_ResX->toPlainText());
    settings.setValue("ResolutionY", ui->textEdit_ResY->toPlainText());
    settings.setValue("MotionBlur", ui->checkBox_MotionBlur->isChecked());
    settings.setValue("UseGPUCrashDebugging", ui->checkBoxUseGPUCrashDebugging->isChecked());
    settings.setValue("LatencyTweak1", ui->checkBoxLatencyTweak1->isChecked());
    settings.setValue("LatencyTweak2", ui->spinBoxLatencyTweak2->value());
    settings.setValue("DLSSQuality", ui->spinBoxDLSSQuality->value());
    settings.setValue("XeSSQuality", ui->spinBox_XeSSQuality->value());
    settings.setValue("DisplayGamma", m_displayGamma);

    QString antiAliasingMethod = ui->comboBoxAntiAliasingMethod->currentText();
    settings.setValue("AntiAliasingMethod", antiAliasingMethod);

    ui->checkBoxRayTracing->isChecked();
    settings.setValue("RayTracing", ui->checkBoxRayTracing->isChecked());
    settings.setValue("DesiredGlobalIlluminationQuality", ui->spinBoxDesiredGlobalIlluminationQuality->value());
    settings.setValue("DesiredReflectionQuality", ui->spinBoxDesiredReflectionQuality->value());
    settings.setValue("PreNaniteGlobalIlluminationQuality", ui->spinBox_PreNaniteGlobalIlluminationQuality->value());
    settings.setValue("PreNaniteReflectionQuality", ui->spinBox_PreNaniteReflectionQuality->value());
    settings.setValue("FrontendFrameRateLimit", ui->spinBoxFrontEndFrameLimit->value());

    // Speichere die Presets
    QStringList presets;
    for(int i = 0; i < ui->listWidgetSavedPresets->count(); i++) {
        QListWidgetItem* item = ui->listWidgetSavedPresets->item(i);
        presets.append(item->data(Qt::UserRole).toString());
    }
    settings.setValue("savedPresets", presets);
}

// Neue Methode zum Laden der Einstellungen
void MainWindow::loadSettings()
{
    QSettings settings("Duesentrieb54", "FortniteEditor");

    // Lade die gespeicherten Werte und setze Standardwerte falls keine vorhanden sind
    ui->textEdit_ResX->setPlainText(settings.value("ResolutionX", "1920").toString());
    ui->textEdit_ResY->setPlainText(settings.value("ResolutionY", "1080").toString());
    ui->checkBox_MotionBlur->setChecked(settings.value("MotionBlur", false).toBool());
    ui->checkBoxUseGPUCrashDebugging->setChecked(settings.value("UseGPUCrashDebugging", false).toBool());
    ui->checkBoxLatencyTweak1->setChecked(settings.value("LatencyTweak1", false).toBool());
    ui->spinBoxLatencyTweak2->setValue(settings.value("LatencyTweak2", 0).toInt());
    ui->spinBoxDLSSQuality->setValue(settings.value("DLSSQuality", 0).toInt());
    ui->spinBox_XeSSQuality->setValue(settings.value("XeSSQuality", 0).toInt());

    m_displayGamma = settings.value("DisplayGamma", 2.2).toDouble();
    ui->doubleSpinBoxDisplayGamma->setValue(static_cast<int>(m_displayGamma * 100));

    QString antiAliasingMethod = settings.value("AntiAliasingMethod", "DLSS").toString();
    int index = ui->comboBoxAntiAliasingMethod->findText(antiAliasingMethod);
    if (index != -1) {
        ui->comboBoxAntiAliasingMethod->setCurrentIndex(index);
    }

    ui->checkBoxRayTracing->setChecked(settings.value("RayTracing", false).toBool());
    ui->spinBoxDesiredGlobalIlluminationQuality->setValue(settings.value("DesiredGlobalIlluminationQuality", 0).toInt());
    ui->spinBoxDesiredReflectionQuality->setValue(settings.value("DesiredReflectionQuality", 0).toInt());
    ui->spinBox_PreNaniteGlobalIlluminationQuality->setValue(settings.value("PreNaniteGlobalIlluminationQuality", 0).toInt());
    ui->spinBox_PreNaniteReflectionQuality->setValue(settings.value("PreNaniteReflectionQuality", 0).toInt());
    ui->spinBoxFrontEndFrameLimit->setValue(settings.value("FrontendFrameRateLimit", 120).toInt());

    // Lade die Presets
    QStringList presets = settings.value("savedPresets").toStringList();
    foreach(const QString &preset, presets) {
        QStringList values = preset.split("|");
        if(!values.isEmpty()) {
            QListWidgetItem* item = new QListWidgetItem(values[0]); // Der erste Wert ist der Name
            item->setData(Qt::UserRole, preset);
            ui->listWidgetSavedPresets->addItem(item);
        }
    }
}

void MainWindow::on_horizontalSlider_DisplayGamma_valueChanged(int value)
{
    m_displayGamma = static_cast<double>(value) / 100.0;
    ui->doubleSpinBoxDisplayGamma->setValue(m_displayGamma);
}


void MainWindow::on_pushButtonCopyFileContent_clicked()
{
    // Hole den Text aus dem TextBrowser
    QString content = ui->textBrowser->toPlainText();

    // Prüfe ob Inhalt vorhanden ist
    if(content.isEmpty()) {
        QMessageBox::warning(this, "Fehler", "Kein Inhalt zum Kopieren vorhanden!");
        return;
    }

    // Entferne den Dateipfad und die Leerzeilen am Anfang
    QStringList lines = content.split("\n");
    // Überspringe die ersten 4 Zeilen (Filepath + 3 Leerzeilen)
    lines.removeFirst(); // Entfernt "Filepath: "
    lines.removeFirst(); // Entfernt den Pfad
    lines.removeFirst(); // Entfernt erste Leerzeile
    lines.removeFirst(); // Entfernt zweite Leerzeile

    // Füge die restlichen Zeilen wieder zusammen
    QString cleanContent = lines.join("\n");

    // Kopiere den bereinigten Inhalt in die Zwischenablage
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(cleanContent);

    // Zeige Bestätigungsmeldung
    QMessageBox::information(this, "Erfolg", "Inhalt wurde in die Zwischenablage kopiert!");
}
