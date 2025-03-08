// main.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QProcess>
#include <QInputDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QGridLayout>
#include <QGroupBox>

class DNSChanger : public QMainWindow {
    Q_OBJECT

public:
    DNSChanger(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("DNS Changer for Linux");
        setMinimumSize(600, 400);

        // Create central widget and layout
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        setCentralWidget(centralWidget);

        // Connection selector
        QHBoxLayout *connectionLayout = new QHBoxLayout();
        QLabel *connectionLabel = new QLabel("Network Connection:");
        connectionComboBox = new QComboBox();
        QPushButton *refreshButton = new QPushButton("Refresh");
        connectionLayout->addWidget(connectionLabel);
        connectionLayout->addWidget(connectionComboBox, 1);
        connectionLayout->addWidget(refreshButton);
        mainLayout->addLayout(connectionLayout);

        // DNS Presets
        QGroupBox *presetsGroupBox = new QGroupBox("DNS Presets");
        QVBoxLayout *presetsLayout = new QVBoxLayout(presetsGroupBox);

        dnsPresets = new QListWidget();
        dnsPresets->addItem("Automatic (DHCP)");
        dnsPresets->addItem("Google DNS (8.8.8.8, 8.8.4.4)");
        dnsPresets->addItem("Cloudflare DNS (1.1.1.1, 1.0.0.1)");
        dnsPresets->addItem("OpenDNS (208.67.222.222, 208.67.220.220)");
        dnsPresets->addItem("Quad9 (9.9.9.9, 149.112.112.112)");

        presetsLayout->addWidget(dnsPresets);

        QHBoxLayout *presetButtonsLayout = new QHBoxLayout();
        QPushButton *addPresetButton = new QPushButton("Add Preset");
        QPushButton *removePresetButton = new QPushButton("Remove Preset");
        presetButtonsLayout->addWidget(addPresetButton);
        presetButtonsLayout->addWidget(removePresetButton);
        presetsLayout->addLayout(presetButtonsLayout);

        mainLayout->addWidget(presetsGroupBox);

        // Custom DNS
        QGroupBox *customGroupBox = new QGroupBox("Custom DNS Configuration");
        QGridLayout *customLayout = new QGridLayout(customGroupBox);

        QLabel *primaryDnsLabel = new QLabel("Primary DNS:");
        QLabel *secondaryDnsLabel = new QLabel("Secondary DNS:");

        primaryDnsEdit = new QLineEdit();
        secondaryDnsEdit = new QLineEdit();

        // Set validator for IP address input
        QRegularExpression ipRegex("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
        QRegularExpressionValidator *ipValidator = new QRegularExpressionValidator(ipRegex, this);
        primaryDnsEdit->setValidator(ipValidator);
        secondaryDnsEdit->setValidator(ipValidator);

        customLayout->addWidget(primaryDnsLabel, 0, 0);
        customLayout->addWidget(primaryDnsEdit, 0, 1);
        customLayout->addWidget(secondaryDnsLabel, 1, 0);
        customLayout->addWidget(secondaryDnsEdit, 1, 1);

        mainLayout->addWidget(customGroupBox);

        // Apply and Status
        QHBoxLayout *applyLayout = new QHBoxLayout();
        statusLabel = new QLabel("Status: Ready");
        QPushButton *applyButton = new QPushButton("Apply DNS Settings");
        applyLayout->addWidget(statusLabel);
        applyLayout->addStretch();
        applyLayout->addWidget(applyButton);
        mainLayout->addLayout(applyLayout);

        // Connect signals and slots
        connect(refreshButton, &QPushButton::clicked, this, &DNSChanger::refreshConnections);
        connect(dnsPresets, &QListWidget::itemClicked, this, &DNSChanger::presetSelected);
        connect(addPresetButton, &QPushButton::clicked, this, &DNSChanger::addPreset);
        connect(removePresetButton, &QPushButton::clicked, this, &DNSChanger::removePreset);
        connect(applyButton, &QPushButton::clicked, this, &DNSChanger::applyDNSSettings);

        // Initialize
        refreshConnections();
    }

private slots:
    void refreshConnections() {
        connectionComboBox->clear();
        QProcess process;
        process.start("nmcli", QStringList() << "connection" << "show");
        process.waitForFinished();
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split("\n");

        // Skip first line (header)
        for (int i = 1; i < lines.size(); ++i) {
            QString line = lines[i].trimmed();
            if (!line.isEmpty()) {
                // Extract connection name (first column)
                QString connectionName = line.split(QRegularExpression("\\s{2,}"))[0];
                if (!connectionName.isEmpty()) {
                    connectionComboBox->addItem(connectionName);
                }
            }
        }

        statusLabel->setText("Status: Connections refreshed");
    }

    void presetSelected(QListWidgetItem *item) {
        if (!item) return;

        QString presetText = item->text();
        if (presetText == "Automatic (DHCP)") {
            primaryDnsEdit->clear();
            secondaryDnsEdit->clear();
        } else if (presetText == "Google DNS (8.8.8.8, 8.8.4.4)") {
            primaryDnsEdit->setText("8.8.8.8");
            secondaryDnsEdit->setText("8.8.4.4");
        } else if (presetText == "Cloudflare DNS (1.1.1.1, 1.0.0.1)") {
            primaryDnsEdit->setText("1.1.1.1");
            secondaryDnsEdit->setText("1.0.0.1");
        } else if (presetText == "OpenDNS (208.67.222.222, 208.67.220.220)") {
            primaryDnsEdit->setText("208.67.222.222");
            secondaryDnsEdit->setText("208.67.220.220");
        } else if (presetText == "Quad9 (9.9.9.9, 149.112.112.112)") {
            primaryDnsEdit->setText("9.9.9.9");
            secondaryDnsEdit->setText("149.112.112.112");
        } else {
            // Custom preset - extract DNS values
            QRegularExpression dnsExtractor("\\((.*?),\\s*(.*?)\\)");
            QRegularExpressionMatch match = dnsExtractor.match(presetText);
            if (match.hasMatch()) {
                primaryDnsEdit->setText(match.captured(1));
                secondaryDnsEdit->setText(match.captured(2));
            }
        }
    }

    void addPreset() {
        QString name = QInputDialog::getText(this, "Add DNS Preset", "Preset Name:");
        if (name.isEmpty()) return;

        QString primaryDns = QInputDialog::getText(this, "Add DNS Preset", "Primary DNS:");
        QString secondaryDns = QInputDialog::getText(this, "Add DNS Preset", "Secondary DNS:");

        // Validate IPs
        QRegularExpression ipRegex("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
        if (!ipRegex.match(primaryDns).hasMatch() || !ipRegex.match(secondaryDns).hasMatch()) {
            QMessageBox::warning(this, "Invalid IP", "Please enter valid IP addresses");
            return;
        }

        QString presetText = QString("%1 (%2, %3)").arg(name, primaryDns, secondaryDns);
        dnsPresets->addItem(presetText);
    }

    void removePreset() {
        int row = dnsPresets->currentRow();
        if (row >= 5) { // Only allow removing custom presets
            dnsPresets->takeItem(row);
        } else {
            QMessageBox::information(this, "Cannot Remove", "Default presets cannot be removed");
        }
    }

    void applyDNSSettings() {
        QString connectionName = connectionComboBox->currentText();
        if (connectionName.isEmpty()) {
            QMessageBox::warning(this, "No Connection", "Please select a network connection");
            return;
        }

        QProcess process;
        QStringList args;
        args << "connection" << "modify" << connectionName;

        // Check if we're setting custom DNS or automatic
        if (primaryDnsEdit->text().isEmpty() && secondaryDnsEdit->text().isEmpty()) {
            // Set to automatic
            args << "ipv4.dns" << "" << "ipv4.ignore-auto-dns" << "no";
            process.start("nmcli", args);
            process.waitForFinished();

            statusLabel->setText("Status: Set to automatic DNS (DHCP)");
        } else {
            // Set custom DNS
            QString dnsServers = primaryDnsEdit->text();
            if (!secondaryDnsEdit->text().isEmpty()) {
                dnsServers += " " + secondaryDnsEdit->text();
            }

            args << "ipv4.dns" << dnsServers << "ipv4.ignore-auto-dns" << "yes";
            process.start("nmcli", args);
            process.waitForFinished();

            // Apply changes
            QProcess applyProcess;
            applyProcess.start("nmcli", QStringList() << "connection" << "up" << connectionName);
            applyProcess.waitForFinished();

            statusLabel->setText("Status: DNS settings applied");
        }
    }

private:
    QComboBox *connectionComboBox;
    QListWidget *dnsPresets;
    QLineEdit *primaryDnsEdit;
    QLineEdit *secondaryDnsEdit;
    QLabel *statusLabel;
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    DNSChanger window;
    window.show();
    return app.exec();
}
