#include "mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) 
    : QMainWindow(parent), dbManager("ChemAnalysis.db") {
    
    setWindowTitle("Chemical Fluid Analysis Dashboard");
    resize(600, 400);

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    ingestButton = new QPushButton("🚀 Run Mass Telemetry Ingestion (10k Samples)", this);
    refreshButton = new QPushButton("🔄 Refresh Flagged Violations", this);
    
    displayArea = new QTextEdit(this);
    displayArea->setReadOnly(true);
    displayArea->setPlaceholderText("Click Refresh to load current database violations...");

    layout->addWidget(ingestButton);
    layout->addWidget(refreshButton);
    layout->addWidget(displayArea);

    setCentralWidget(centralWidget);

    // Connecting the signals to the missing slots
    connect(ingestButton, &QPushButton::clicked, this, &MainWindow::handleIngestion);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::handleRefreshViolations);
}

MainWindow::~MainWindow() {
}

// Definition 1: Fixed the undefined reference to handleIngestion
void MainWindow::handleIngestion() {
    ingestButton->setEnabled(false); 
    displayArea->append("\n[GUI] Commencing safe transaction stream via isolated cache...");
    
    try {
        dbManager.beginTransaction();
        for (int i = 1; i <= 10000; ++i) {
            dbManager.simulateHardwareInput(i);
        }
        dbManager.commitTransaction();
        
        displayArea->append("✅ Atomically committed 10,000 telemetry points to persistent storage!");
    } catch (const std::exception& e) {
        dbManager.rollbackTransaction();
        QMessageBox::critical(this, "Database Error", e.what());
    }
    
    ingestButton->setEnabled(true);
}

// Definition 2: Fixed the undefined reference to handleRefreshViolations
void MainWindow::handleRefreshViolations() {
    displayArea->clear();
    displayArea->append("Querying records exceeding safety limit threshold (> 0.5 ppm)...\n");
    displayArea->append("------------------------------------------------------------");

    std::string report = dbManager.fetchFlaggedViolations(0.5);
    if(report.empty()) {
        displayArea->append("[System Clear]: No threshold anomalies detected in database records.");
    } else {
        displayArea->append(QString::fromStdString(report));
    }
    
    displayArea->append("\nDashboard view updating completed.");
}