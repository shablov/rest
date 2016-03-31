#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAbstractButton;
class QLabel;
class QNetworkAccessManager;
class QJsonValueRef;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);

private slots:
	void onButtonClicked(QAbstractButton *button);

private:
	void setToken(const QString &token);

	QString buildUrl(const QString &extraPath);
	QByteArray buildData(const QString &extraPath);

	void updateView(QJsonObject jsonObject, const QString &parentKey = QString());
	void updateLabel(QLabel *label, const QJsonValueRef &value);
	QString mServerUrl;
	QString mUserName;
	QString mUserPassword;
	QString mToken;

	QNetworkAccessManager *networkManager;
};

#endif // MAINWINDOW_H
