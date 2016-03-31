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
#include <QLabel>
#include <QFormLayout>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent), mServerUrl("http://stas.pubsandbox.eterhost.ru/rest-test/public/api"),
	mUserName("testuser"), mUserPassword("testuser"), networkManager(0)
{
	networkManager = new QNetworkAccessManager(this);
	connect(networkManager, &QNetworkAccessManager::finished, [=] (QNetworkReply *reply) {
		QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
		QJsonObject jsonObject = jsonDoc.object();
		qDebug() << jsonObject;
		if (jsonObject.contains("token"))
		{
			setToken(jsonObject.value("token").toString());
		}
		updateView(jsonObject);
	});

	QPushButton *infoReqPB = new QPushButton(tr("Info request"));
	infoReqPB->setObjectName("info");
	infoReqPB->setProperty("requestType", QVariant::fromValue(QNetworkAccessManager::GetOperation));
	QPushButton *authReqPB = new QPushButton(tr("Authenticate request"));
	authReqPB->setObjectName("authenticate");
	authReqPB->setProperty("requestType", QVariant::fromValue(QNetworkAccessManager::PostOperation));
	QPushButton *userInfoReqPB = new QPushButton(tr("User Info request"));
	userInfoReqPB->setObjectName("userinfo");
	userInfoReqPB->setProperty("requestType", QVariant::fromValue(QNetworkAccessManager::GetOperation));
	userInfoReqPB->setDisabled(true);

	QButtonGroup *buttonGroup = new QButtonGroup(this);
	buttonGroup->addButton(infoReqPB);
	buttonGroup->addButton(authReqPB);
	buttonGroup->addButton(userInfoReqPB);
	connect(buttonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
			this, &MainWindow::onButtonClicked);

	QWidget *window = new QWidget(this);
	setCentralWidget(window);
	QVBoxLayout *mainLayout = new QVBoxLayout(window);
	foreach (QAbstractButton *button, buttonGroup->buttons())
	{
		mainLayout->addWidget(button);
	}

	QLabel *nameServerLbl = new QLabel;
	nameServerLbl->setObjectName("nameserver");
	QLabel *versionLbl = new QLabel;
	versionLbl->setObjectName("version");

	QLabel *createdAtLbl = new QLabel;
	createdAtLbl->setObjectName("created_at");
	QLabel *emailLbl = new QLabel;
	emailLbl->setObjectName("email");
	QLabel *firstNameLbl = new QLabel;
	firstNameLbl->setObjectName("first_name");
	QLabel *lastNameLbl = new QLabel;
	lastNameLbl->setObjectName("last_name");
	QLabel *middleNameLbl = new QLabel;
	middleNameLbl->setObjectName("middle_name");
	QLabel *idLbl = new QLabel;
	idLbl->setObjectName("id");

	QLabel *sipPasswordLbl = new QLabel;
	sipPasswordLbl->setObjectName("sip.password");
	QLabel *sipRegistrationUriLbl = new QLabel;
	sipRegistrationUriLbl->setObjectName("sip.registration_uri");
	QLabel *sipUriLbl = new QLabel;
	sipUriLbl->setObjectName("sip.uri");
	QLabel *sipUserNameLbl = new QLabel;
	sipUserNameLbl->setObjectName("sip.username");

	QLabel *updatedAtLbl = new QLabel;
	updatedAtLbl->setObjectName("updated_at");

	QFormLayout *parametersLayout = new QFormLayout;
	parametersLayout->addRow("Name server", nameServerLbl);
	parametersLayout->addRow("Version", versionLbl);
	parametersLayout->addRow("Created at", createdAtLbl);
	parametersLayout->addRow("Email", emailLbl);
	parametersLayout->addRow("First name", firstNameLbl);
	parametersLayout->addRow("Last name", lastNameLbl);
	parametersLayout->addRow("Middle name", middleNameLbl);
	parametersLayout->addRow("Id", idLbl);
	parametersLayout->addRow("Sip password", sipPasswordLbl);
	parametersLayout->addRow("Sip registration uri", sipRegistrationUriLbl);
	parametersLayout->addRow("Sip uri", sipUriLbl);
	parametersLayout->addRow("Sip username", sipUserNameLbl);
	parametersLayout->addRow("Updated at", updatedAtLbl);
	mainLayout->addLayout(parametersLayout);
}

void MainWindow::onButtonClicked(QAbstractButton *button)
{
	QString name = button->objectName();
	QString url = buildUrl(name);
	auto requestType = button->property("requestType").value<QNetworkAccessManager::Operation>();
	QNetworkRequest request(url);
	if (name == "userinfo")
	{
		request.setHeader(QNetworkRequest::ContentTypeHeader, "Authorization");
		request.setRawHeader("Authorization", buildData(name));
	}
	else
	{
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	}
	if (requestType == QNetworkAccessManager::GetOperation)
	{
		networkManager->get(request);
	}
	else if (requestType == QNetworkAccessManager::PostOperation)
	{
		networkManager->post(request, buildData(name));
	}
}

QString MainWindow::buildUrl(const QString &extraPath)
{
	return  mServerUrl +
			((mServerUrl.right(1) != "/") ? "/" : "") +
			extraPath;
}

QByteArray MainWindow::buildData(const QString &extraPath)
{
	QString result;
	if (extraPath == "authenticate")
	{
		result = QString("email=%0&password=%1").arg(mUserName, mUserPassword);
	}
	if (extraPath == "userinfo")
	{
		result = QString("Bearer %0").arg(mToken);
	}
	return result.toUtf8();
}

void MainWindow::setToken(const QString &token)
{
	mToken = token;
	if (QAbstractButton *button = findChild<QAbstractButton*>("userinfo"))
	{
		button->setDisabled(mToken.isEmpty());
	}
}

void MainWindow::updateView(QJsonObject jsonObject, const QString &parentKey)
{
	for (QJsonObject::iterator it = jsonObject.begin(); it != jsonObject.end(); it++)
	{
		QString key = (!parentKey.isEmpty() ? parentKey + "." : "") + it.key();
		QJsonValueRef value = it.value();
		if (value.isObject())
		{
			updateView(value.toObject(), key);
			continue;
		}

		if (QLabel *label = findChild<QLabel*>(key))
		{
			updateLabel(label, value);
		}
	}
}

void MainWindow::updateLabel(QLabel *label, const QJsonValueRef &value)
{
	label->setText(value.toVariant().toString());
}

Q_DECLARE_METATYPE(QNetworkAccessManager::Operation)
