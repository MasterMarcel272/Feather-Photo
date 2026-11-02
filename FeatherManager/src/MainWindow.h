#pragma once

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <QString>

class QLabel;
class QByteArray;
class QJsonObject;
class QNetworkAccessManager;
class QNetworkReply;
class QProgressBar;
class QPushButton;
class QVBoxLayout;
class QFrame;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    struct AppEntry
    {
        QString id;
        QString name;
        QString description;
        QString repository;
        QString installerAsset;
        QString installedRegistryKey;
        QString latestVersion;
        QString downloadUrl;
        QString sha256;
        QString releaseError;
        QPointer<QFrame> card;
        QPointer<QLabel> versionLabel;
        QPointer<QProgressBar> progressBar;
        QPointer<QPushButton> installButton;
    };

    QNetworkAccessManager* m_network = nullptr;
    QWidget* m_appList = nullptr;
    QVBoxLayout* m_appLayout = nullptr;
    QLabel* m_statusLabel = nullptr;
    QPushButton* m_refreshButton = nullptr;
    QMap<QString, AppEntry> m_apps;

    void loadBundledCatalog();
    void refreshCatalog();
    void populateCatalog(const QByteArray& json);
    void addApp(const QJsonObject& object);
    void fetchLatestRelease(const QString& appId);
    void updateAppCard(AppEntry& app);
    void startDownload(const QString& appId);
    void handleReply(QNetworkReply* reply);
    QString installedVersion(const AppEntry& app) const;
    static QString normalizedVersion(QString version);
};
