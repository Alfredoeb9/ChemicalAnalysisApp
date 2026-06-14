#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include "DatabaseManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        private slots:
            void handleIngestion();
            void handleRefreshViolations();
        
        private:
            DatabaseManager dbManager;
            QPushButton* ingestButton;
            QPushButton* refreshButton;
            QTextEdit* displayArea;
};

#endif // MAINWINDOW_H
