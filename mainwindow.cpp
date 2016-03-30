#include "mainwindow.h"


#include <QButtonGroup>
#include <QDebug>
#include <QPushButton>
#include <QVariant>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHttpMultiPart>
#include <QBuffer>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent), mServerUrl("http://stas.pubsandbox.eterhost.ru/rest-test/public/api"),
	mUserName("testuser"), mUserPassword("testuser"), networkManager(0)
{
	QPushButton *infoReqPB = new QPushButton(tr("Info request"));
	infoReqPB->setObjectName("info");
	QPushButton *authReqPB = new QPushButton(tr("Authenticate request"));
	authReqPB->setObjectName("authenticate");
	QPushButton *userInfoReqPB = new QPushButton(tr("User Info request"));
	userInfoReqPB->setObjectName("userinfo");
	userInfoReqPB->setDisabled(true);

	QButtonGroup *buttonGroup = new QButtonGroup(this);
	buttonGroup->addButton(infoReqPB);
	buttonGroup->addButton(authReqPB);
	buttonGroup->addButton(userInfoReqPB);
	connect(buttonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
			[=] (QAbstractButton *button) {
		QString requestUrl = buildUrl(button->objectName());
		QString data = buildData(button->objectName());
		setRequest(requestUrl, data);
	});

	QWidget *window = new QWidget(this);
	setCentralWidget(window);
	QVBoxLayout *mainLayout = new QVBoxLayout(window);
	foreach (QAbstractButton *button, buttonGroup->buttons())
	{
		mainLayout->addWidget(button);
	}

	networkManager = new QNetworkAccessManager(this);
	connect(networkManager, &QNetworkAccessManager::finished, [=] (QNetworkReply *reply) {
		QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
		QJsonObject jsonObject = jsonDoc.object();
		qDebug() << jsonObject;
		if (jsonObject.contains("token"))
		{
			setToken(jsonObject.value("token").toString());
		}
	});
}

QString MainWindow::buildUrl(const QString &extraPath)
{
	return  mServerUrl +
			((mServerUrl.right(1) != "/") ? "/" : "") +
			extraPath;
}

QString MainWindow::buildData(const QString &extraPath)
{
	if (extraPath == "authenticate")
	{
		return QString("email=%0&password=%1").arg(mUserName, mUserPassword);
	}
	if (extraPath == "userinfo")
	{
		return QString("Bearer %0").arg(mToken);
	}
	return QString();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setToken(const QString &token)
{
	mToken = token;
	if (QAbstractButton *button = findChild<QAbstractButton*>("userinfo"))
	{
		button->setDisabled(mToken.isEmpty());
	}
}

void MainWindow::setRequest(const QString &url, const QString &data)
{
	qDebug() << url << data;
	QString requestType = data.isEmpty() ? "GET" : "POST";
	QNetworkRequest request = QNetworkRequest(QUrl(url));
	if (requestType == "GET")
	{
		networkManager->get(request);
	}
	else
	{
		if (url.contains("userinfo"))
		{
			request.setHeader(QNetworkRequest::ContentTypeHeader, "Authorization");
			request.setRawHeader("Authorization", data.toUtf8());
			networkManager->get(request);
		}
		else
		{
			request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
			networkManager->post(request, data.toUtf8());
		}
	}
}
