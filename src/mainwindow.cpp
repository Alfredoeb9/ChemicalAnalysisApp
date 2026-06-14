#include "mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QStringList>
#include <QFileDialog>
#include <QDir>
#include <QMenuBar>
#include <QMenu>
#include <QCoreApplication>

/**
 * constructor: MainWindow
 * @description:
 * - Initializes the main window, sets up the menu bar with File, Data, and Help
 * menus, and connects menu actions to their respective slots for handling user interactions.
 * - Also sets up the central widget with buttons for data ingestion and refreshing violations,
 * as well as a text area for displaying results and feedback to the user.
 * @param parent: The parent widget for the main window, defaulting to nullptr for top-level window.
 * @returns: void
 */
MainWindow::MainWindow(QWidget* parent) 
    : QMainWindow(parent), dbManager("ChemAnalysis.db") {
    
    setWindowTitle("Chemical Fluid Analysis Dashboard");
    resize(650, 450);

    QMenuBar* nativeMenuBar = this->menuBar();
    // File Menu Tab
    fileMenu = nativeMenuBar->addMenu("&File");
    exitAction = fileMenu->addAction("❌ Exit Application");
    connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    dataMenu = nativeMenuBar->addMenu("&Data");
    seedAction = dataMenu->addAction("🌱 Seed Database with Defaults...");
    importAction = dataMenu->addAction("📥 Import Data File...");
    
    connect(seedAction, &QAction::triggered, this, &MainWindow::handleSeedDatabase);
    connect(importAction, &QAction::triggered, this, &MainWindow::handleImportData);

    helpMenu = nativeMenuBar->addMenu("&Help");
    docAction = helpMenu->addAction("📖 User Documentation");
    connect(docAction, &QAction::triggered, this, &MainWindow::handleDocumentation);

    // MAIN CONTENT VIEW LAYOUT
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    ingestButton = new QPushButton("🚀 Run Mass Telemetry Ingestion (10k Samples)", this);
    refreshButton = new QPushButton("🔄 Refresh Flagged Violations", this);
    
    displayArea = new QTextEdit(this);
    displayArea->setReadOnly(true);
    displayArea->setPlaceholderText("Use the Data menu or buttons below to process system files...");

    // MODERN COAL-SLATE DIALOG STYLE SHEETS (QSS)
    displayArea->setStyleSheet(
        "QTextEdit {"
        "   background-color: #1e1e1e;"
        "   color: #d4d4d4;"
        "   font-family: 'Segoe UI', -apple-system, sans-serif;"
        "   border: 1px solid #2d2d2d;"
        "   border-radius: 6px;"
        "   padding: 12px;"
        "}"
        "QScrollBar:vertical {"
        "   border: none;"
        "   background: #1e1e1e;"
        "   width: 8px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #3e3e42;"
        "   min-height: 20px;"
        "   border-radius: 4px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: #4d4d52;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "}"
    );

    layout->addWidget(ingestButton);
    layout->addWidget(refreshButton);

    // SLATE-THEMED SEARCH BAR DESIGN
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("🔍 Filter telemetry by substance name or description keywords...");
    searchBar->setStyleSheet(
        "QLineEdit {"
        "   background-color: #252526;"
        "   color: #ffffff;"
        "   border: 1px solid #2d2d2d;"
        "   border-radius: 4px;"
        "   padding: 8px 12px;"
        "   font-size: 13px;"
        "   selection-background-color: #007acc;"
        "}"
        "QLineEdit:focus {"
        "   border: 1px solid #007acc;" // Dark IDE-blue accent ring on active focus
        "   background-color: #1e1e1e;"
        "}"
    );
    // Mount the search bar above the display area for intuitive filtering of results
    layout->addWidget(searchBar);
    layout->addWidget(displayArea);

    setCentralWidget(centralWidget);

    // Connect interactions for buttons and search bar to their respective handlers
    connect(ingestButton, &QPushButton::clicked, this, &MainWindow::handleIngestion);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::handleRefreshViolations);

    // CONNECT SEARCH BAR: Triggers instantly whenever text alters
    connect(searchBar, &QLineEdit::textChanged, this, &MainWindow::handleSearch);
}

MainWindow::~MainWindow() {
}

/**
 * function: handleSeedDatabase
 * @description:
 * - Seeds the database with initial data from a SQL script file.
 * @returns: void
 */
void MainWindow::handleSeedDatabase() {
    displayArea->append("\n[System] Searching for local development 'sql/seed_data.sql' baseline...");
    
    QStringList potentialPaths = {
        "sql/seed_data.sql",         // 1. Working directory is Project Root (e.g., running via VS Code)
        "../sql/seed_data.sql",      // 2. Working directory is build/ (e.g., running inside terminal)
        "../../sql/seed_data.sql",   // 3. Working directory is build/Debug/ or build/Release/
        QCoreApplication::applicationDirPath() + "/sql/seed_data.sql",    // 4. Sitting right next to the .exe
        QCoreApplication::applicationDirPath() + "/../sql/seed_data.sql"     // Absolute path from current working directory
    };

    QString chosenPath = "";
    for (const QString& path : potentialPaths) {
        if (QFile::exists(path)) {
            chosenPath = path;
            break; // Found it! Stop looking.
        }
    }

    // Fallback if none of the paths resolved on disk
    if (chosenPath.isEmpty()) {
        displayArea->append("❌ Error: Could not locate 'sql/seed_data.sql' automatically in any standard path.");
        QMessageBox::warning(this, "Script Not Found", 
            "Could not locate seed script automatically.\n\n"
            "Please select 'sql/seed_data.sql' manually via the file browser.");
        handleImportData();
        return;
    }

    try {
        dbManager.executeScriptFile(chosenPath.toStdString());
        displayArea->append("✅ Success! Database seeded using environment layout found at:\n   " + chosenPath);
        handleRefreshViolations(); 
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Database Seeding Error", e.what());
    }
}

/**
 * function: handleImportData
 * @description:
 * - Imports data into the database from a selected SQL script file.
 * @returns: void
 */
void MainWindow::handleImportData() {
    QString selectedFile = QFileDialog::getOpenFileName(
        this,
        "Select SQL Dataset Script to Import",
        QDir::currentPath(),
        "SQL Files (*.sql);;All Files (*.*)"
    );

    if (selectedFile.isEmpty()) {
        displayArea->append("\n[System] File import cancellation processed.");
        return;
    }

    displayArea->append("\n[Processing] Ingesting selected file target: " + selectedFile);
    
    try {
        dbManager.executeScriptFile(selectedFile.toStdString());
        QMessageBox::information(this, "Import Success", "Database transaction updates processed cleanly!");
        handleRefreshViolations();
    } catch(const std::exception& e) {
        QMessageBox::critical(this, "Import Execution Error", e.what());
    }
}

/**
 * function: handleIngestion
 * @description:
 * - Initiates the ingestion of synthetic runtime telemetry data into the database.
 * @returns: void
 */
void MainWindow::handleIngestion() {
    ingestButton->setEnabled(false); 
    displayArea->append("\n[GUI] Commencing transaction stream...");
    
    try {
        dbManager.beginTransaction();
        for (int i = 1; i <= 10000; ++i) {
            dbManager.simulateHardwareInput(i);
        }
        dbManager.commitTransaction();
        displayArea->append("✅ Atomically committed 10,000 synthetic runtime telemetry items.");
    } catch (const std::exception& e) {
        dbManager.rollbackTransaction();
        QMessageBox::critical(this, "Database Error", e.what());
    }
    ingestButton->setEnabled(true);
}

/**
 * function: handleRefreshViolations
 * @description:
 * - Refreshes the display with the latest flagged violations from the database.
 * @returns: void
 */
void MainWindow::handleRefreshViolations() {
    displayArea->clear();
    
    // Pass the search bar's current input to the query loop
    std::string reportHtml = dbManager.fetchFlaggedViolations(0.0, searchBar->text().toStdString()); 
    
    if(reportHtml.empty()) {
        displayArea->setHtml("<div style='color: #777777; text-align: center; margin-top: 40px; font-size: 14px;'>⚠️ No matching evaluation metrics found.</div>");
    } else {
        std::string finalHeader = "<div style='color: #888888; font-size: 11px; text-transform: uppercase; letter-spacing: 1px; margin-bottom: 10px;'>System Data Stream Monitoring Active</div>";
        displayArea->setHtml(QString::fromStdString(finalHeader + reportHtml));
    }
}

/**
 * function: handleDocumentation
 * @description:
 * - Displays documentation for the Chemical Fluid Analysis Dashboard.
 * @returns: void
 */
void MainWindow::handleDocumentation() {
    // QMessageBox natively parses basic HTML/CSS strings for rich text layouts
    QString docText = 
        "<h1>Chemical Fluid Analysis Dashboard</h1>"
        "<p><b>Version 1.0</b> | Developer Utility Guide</p>"
        "<hr/>"
        "<h3>Core Operations</h3>"
        "<ul>"
        "  <li><b>Mass Ingestion:</b> Atomically streams 10,000 continuous hardware sensor calibration items into the local embedded SQLite instance using safe, high-speed transactional blocks.</li>"
        "  <li><b>Refresh Violations:</b> Queries the active database for recorded toxicity metrics crossing safety limits.</li>"
        "</ul>"
        "<h3>Data Manipulation Pipeline</h3>"
        "<ul>"
        "  <li><b>File &rarr; Seed Database:</b> Flushes old tables and loads structural validation targets located at <code>sql/seed_data.sql</code>.</li>"
        "  <li><b>File &rarr; Import Data:</b> Launches the native OS file dialogue engine to read and process raw external SQL script sets.</li>"
        "</ul>"
        "<hr/>"
        "<p><i>Note: Database connections are managed concurrently inside the application root directory as <code>ChemAnalysis.db</code>.</i></p>";

    // Creates a modal message box to display the documentation content
    QMessageBox* docBox = new QMessageBox(this);
    docBox->setWindowTitle("Application Documentation");
    docBox->setTextFormat(Qt::RichText); // Forces HTML parsing
    docBox->setText(docText);
    docBox->setIcon(QMessageBox::Information);
    docBox->setStandardButtons(QMessageBox::Ok);
    // Prevents memory leaks by cleaning itself up
    docBox->setAttribute(Qt::WA_DeleteOnClose);
    docBox->exec();
}

void MainWindow::handleSearch(const QString& searchTerm) {
    // The search term is accessed directly from the search bar in handleRefreshViolations, so we mark it as unused here to avoid compiler warnings
    Q_UNUSED(searchTerm) 
    // Whenever the search term changes, we refresh the violations display with the new filter
    handleRefreshViolations();
}