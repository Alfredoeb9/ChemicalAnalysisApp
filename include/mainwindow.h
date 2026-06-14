#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "DatabaseManager.h"

/**
 * class: MainWindow
 * @description:
 *  - The MainWindow class serves as the primary user interface for the Chemical Fluid Analysis Dashboard
 * - It inherits from QMainWindow and encapsulates all UI components, including buttons, text display areas, and menu bars.
 * - The class also manages interactions with the DatabaseManager to perform data operations based on user actions
 * - Key responsibilities include:
 *   - Initializing the main window layout and UI components
 *   - Handling user interactions through slots connected to buttons and menu actions
 *   - Displaying results and feedback in the central text area based on database queries and processing outcomes
 * 
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        private slots:
            void handleIngestion();
            void handleRefreshViolations();
            void handleSeedDatabase();
            void handleImportData();
            void handleDocumentation();
        
        private:
            DatabaseManager dbManager;
            QPushButton* ingestButton;
            QPushButton* refreshButton;
            QTextEdit* displayArea;

            // Menu Bar Pointer Targets
            QMenu* fileMenu;
            QMenu* dataMenu;
            QMenu* helpMenu;

            // Menu Action Pointer Targets for Menu Bars
            QAction* exitAction;
            QAction* seedAction;
            QAction* importAction;
            QAction* docAction;
};

#endif // MAINWINDOW_H
