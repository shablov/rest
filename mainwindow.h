#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QNetworkAccessManager;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	void setToken(const QString &token);
	void setRequest(const QString &url, const QString &data);

	QString buildUrl(const QString &extraPath);
	QString buildData(const QString &extraPath);

	QString mServerUrl;
	QString mUserName;
	QString mUserPassword;
	QString mToken;

	QNetworkAccessManager *networkManager;
};

#endif // MAINWINDOW_H
