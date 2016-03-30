#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QNetworkAccessManager;
class QJsonValueRef;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void updateView(QJsonObject jsonObject, const QString &parentKey = QString());

	void updateLabel(QLabel *label, const QJsonValueRef &value);
private:
	void setToken(const QString &token);

	QString buildUrl(const QString &extraPath);
	QByteArray buildData(const QString &extraPath);

	QString mServerUrl;
	QString mUserName;
	QString mUserPassword;
	QString mToken;

	QNetworkAccessManager *networkManager;
};

#endif // MAINWINDOW_H
