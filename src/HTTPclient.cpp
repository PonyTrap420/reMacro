#include "HTTPclient.h"
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "stdlib.h"
#include "QtNetwork/qnetworkrequest.h"
#include "QtNetwork/qnetworkreply.h"
#include "QFile"
#include "QHttpPart"
#include "uiUtils.h"
//singletone patterns

const char * StringToCharArr(QString str){
    return str.toLocal8Bit().constData();
}

HttpClient* HttpClient::instance = new HttpClient();

HttpClient* HttpClient::getInstance(){
    return instance;
}

HttpClient::HttpClient()
{
    serverUrl = "https://localhost:8443";
    manager = new QNetworkAccessManager(this);
    config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    config.setPeerVerifyMode(QSslSocket::QueryPeer);
}

QJsonDocument HttpClient::sendRequest(QString& path, QString& method, QJsonObject* obj, QUrlQuery* query, bool auth){
    QString requestUrl = serverUrl + path;
    QUrl url(requestUrl);
    if(query != nullptr)
        url.setQuery(query->query());
        delete query;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setSslConfiguration(config);

    if(auth){
        QString authtoken = QString("Bearer ") + tokens->ACCESS;
        request.setRawHeader("Authorization", authtoken.toLocal8Bit());
    }


    if(method == "POST"){
        QJsonDocument doc(*obj);
        QByteArray data = doc.toJson();
        delete obj;
        reply = manager->post(request, data);
    }
    else{
        reply = manager->get(request);
    }

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if(reply->error() == QNetworkReply::NoError){
        QByteArray data = reply->readAll();
        reply->deleteLater();
        return QJsonDocument::fromJson(data);
    }
    else{
        QByteArray data = reply->readAll();
        reply->deleteLater();
        return QJsonDocument::fromJson(data);
    }
}
QJsonDocument HttpClient::sendRequest(QString &path, QString &method, QString filepath, QString filename){
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(GetContentType(filename)));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"image\"; filename=\""+filename+"\""));

    QFile *file = new QFile(filepath);
    file->open(QIODevice::ReadOnly);
    imagePart.setBodyDevice(file);
    file->setParent(multiPart);

    multiPart->append(imagePart);

    QString requestUrl = serverUrl + path;
    QUrl url(requestUrl);
    QNetworkRequest request(url);
    request.setSslConfiguration(config);

    //request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary="+ multiPart->boundary());
    QString authtoken = QString("Bearer ") + tokens->ACCESS;
    request.setRawHeader("Authorization", authtoken.toLocal8Bit());


    if(method == "POST"){
        reply = manager->post(request,multiPart);
        multiPart->setParent(reply);
    }
    else{
        reply = manager->get(request);
    }
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if(reply->error() == QNetworkReply::NoError){
        QByteArray data = reply->readAll();
        reply->deleteLater();
        return QJsonDocument::fromJson(data);
    }
    else{
        QByteArray data = reply->readAll();
        reply->deleteLater();
        return QJsonDocument::fromJson(data);
    }
}
QByteArray HttpClient::sendRequest(QString filename){
    QString requestUrl = serverUrl + "/"+filename;
    QUrl url(requestUrl);
    QNetworkRequest request(url);
    request.setSslConfiguration(config);

    QString authtoken = QString("Bearer ") + tokens->ACCESS;
    request.setRawHeader("Authorization", authtoken.toLocal8Bit());


    reply = manager->get(request);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if(reply->error() == QNetworkReply::NoError){
        QByteArray data = reply->readAll();
        reply->deleteLater();
        return data;
    }
    else{
        QByteArray data = reply->readAll();
        reply->deleteLater();
        return data;
    }
}

QString HttpClient::Login(param Email, param Pwd){
    QJsonObject* data = new QJsonObject;
    (*data)["email"] = Email;
    (*data)["password"] = Pwd;


    QString path = "/auth/login";
    QString method = "POST";
    QJsonDocument json = sendRequest(path, method, data);
    QJsonObject root_obj = json.object();
    QVariantMap root_map = root_obj.toVariantMap();
    if(root_map["code"].toInt() >= 400){
        return root_map["message"].toString();
    }

    SetUser(root_map);

    logged = true;

    QString av = root_map["user"].toMap()["avatar"].toString();
    if (av.toStdString().rfind("<svg xmlns:dc=", 0) == 0) {
        QSize s = avatar->size();
        avatar->setIcon(Util::RoundImage(av.toLocal8Bit(), s, QString("SVG")));
    }
    else{
        QByteArray b = sendRequest(av);
        QSize s = avatar->size();
        avatar->setIcon(Util::RoundImage(b, s, av.mid(av.lastIndexOf(".")+1)));
    }

    return QString("");
}
QString HttpClient::Register(param Email, param Username, param Pwd){
    QJsonObject* data = new QJsonObject;
    (*data)["email"] = Email;
    (*data)["name"] = Username;
    (*data)["password"] = Pwd;


    QString path = "/auth/register";
    QString method = "POST";
    QJsonDocument json = sendRequest(path, method, data);
    QJsonObject root_obj = json.object();
    QVariantMap root_map = root_obj.toVariantMap();
    if(root_map["code"].toInt() == 400){
        return root_map["message"].toString();
    }

    qDebug()<<root_map;
    SetUser(root_map);
    logged = true;

    return root_map["user"].toMap()["avatar"].toString();

}
bool HttpClient::Logout(param REFRESH_TOKEN){
    QJsonObject* data = new QJsonObject;
    (*data)["refreshToken"] = REFRESH_TOKEN;

    QString path = "/auth/logout";
    QString method = "POST";
    QJsonDocument json = sendRequest(path, method, data);
    QJsonObject root_obj = json.object();
    QVariantMap root_map = root_obj.toVariantMap();
    if(root_map["code"].toInt() >= 400){
        return false;
    }

    logged = false;

    return true;
}
bool HttpClient::RefreshTokens(param REFRESH_TOKEN){
    return false;
}
QString HttpClient::ForgotPwd(param Email){
    QJsonObject* data = new QJsonObject;
    (*data)["email"] = Email;

    QString path = "/auth/forgotpwd";
    QString method = "POST";
    QJsonDocument json = sendRequest(path, method, data);
    QJsonObject root_obj = json.object();
    QVariantMap root_map = root_obj.toVariantMap();
    if(root_map["code"].toInt() >= 400){
        return root_map["message"].toString();
    }

    return QString("");
}
QString HttpClient::ResetPwd(param Token, param Pwd){
    QJsonObject* data = new QJsonObject;
    (*data)["password"] = Pwd;
    QUrlQuery* query;

    query = new QUrlQuery();
    query->addQueryItem("token", Token);

    QString path = "/auth/resetpwd";
    QString method = "POST";
    QJsonDocument json = sendRequest(path, method, data, query);
    QJsonObject root_obj = json.object();
    QVariantMap root_map = root_obj.toVariantMap();
    if(root_map["code"].toInt() >= 400){
        return root_map["message"].toString();
    }

    return QString("");
}
bool HttpClient::EmailVerifyRequest(){
    QJsonObject* data = new QJsonObject;

    QString path = "/auth/verifyreq";
    QString method = "POST";
    QJsonDocument json = sendRequest(path, method, data, nullptr, true);
    QJsonObject root_obj = json.object();
    QVariantMap root_map = root_obj.toVariantMap();
    if(root_map["code"].toInt() >= 400){
        return false;
    }

    return true;
}
QString HttpClient::VerifyEmail(param Token){
    QJsonObject* data = new QJsonObject;
    QUrlQuery* query;

    query = new QUrlQuery();
    query->addQueryItem("token", Token);

    QString path = "/auth/verify";
    QString method = "POST";
    QJsonDocument json = sendRequest(path, method, data, query);
    QJsonObject root_obj = json.object();
    QVariantMap root_map = root_obj.toVariantMap();
    if(root_map["code"].toInt() >= 400){
        return root_map["message"].toString();
    }

    return QString("");
}

bool HttpClient::UploadAvatar(param filepath, param filename){
    QString path = "/users/avatar";
    QString method = "POST";
    QJsonDocument json = sendRequest(path, method, filepath, filename);
    QJsonObject root_obj = json.object();
    QVariantMap root_map = root_obj.toVariantMap();
    if(root_map["code"].toInt() >= 400){
        return false;
    }

    return true;
}
QString HttpClient::UploadMacro(param Name, param Description, MacroFrame* macro){
    QJsonObject macroObj = macro->GetSequence()->Serialize();

    QJsonDocument doc(macroObj);
    QString macroStr(doc.toJson(QJsonDocument::Compact));

    QJsonObject* data = new QJsonObject;
    (*data)["name"] = Name;
    (*data)["description"] = Description;
    (*data)["macro"] = macroStr;


    QString path = "/users/uploadmacro";
    QString method = "POST";
    QJsonDocument json = sendRequest(path, method, data, nullptr, true);
    QJsonObject root_obj = json.object();
    QVariantMap root_map = root_obj.toVariantMap();
    if(root_map["code"].toInt() == 400){
        return root_map["message"].toString();
    }

    return QString("");
}
QJsonDocument HttpClient::GetMacros(param Page){
    QUrlQuery* query;

    query = new QUrlQuery();
    query->addQueryItem("page", Page);

    QString path = "/users/macros";
    QString method = "GET";
    QJsonDocument json = sendRequest(path, method, nullptr, query, true);
    return json;
}
QJsonDocument HttpClient::GetUserMacros(param User){
    QString path = "/users/macros/" + User;
    QString method = "GET";
    QJsonDocument json = sendRequest(path, method, nullptr, nullptr, true);
    return json;
}
QJsonDocument HttpClient::SearchMacros(param Name, param Page){
    QUrlQuery* query;

    query = new QUrlQuery();
    query->addQueryItem("name", Name);
    query->addQueryItem("page", Page);

    QString path = "/users/searchmacro";
    QString method = "GET";
    QJsonDocument json = sendRequest(path, method, nullptr, query, true);
    return json;
}
bool HttpClient::DownloadMacro(param Macro)
{
    QUrlQuery* query;

    query = new QUrlQuery();
    query->addQueryItem("macro", Macro);

    QString path = "/users/download";
    QString method = "GET";

    QJsonDocument json = sendRequest(path, method, nullptr, query, true);

    return true;
}
bool HttpClient::UploadComment(param Macro, param Content){
    QUrlQuery* query;

    query = new QUrlQuery();
    query->addQueryItem("macro", Macro);

    QJsonObject* data = new QJsonObject;
    (*data)["content"] = Content;

    QString path = "/users/addcomment";
    QString method = "POST";

    QJsonDocument json = sendRequest(path, method, data, query, true);

    QJsonObject root_obj = json.object();
    QVariantMap root_map = root_obj.toVariantMap();
    if(root_map["code"].toInt() >= 400){
        return false;
    }

    return true;
}
QJsonDocument HttpClient::GetComments(param Macro, param Page){
    QUrlQuery* query;

    query = new QUrlQuery();
    query->addQueryItem("macro", Macro);
    query->addQueryItem("page", Page);

    QString path = "/users/comments";
    QString method = "GET";
    QJsonDocument json = sendRequest(path, method, nullptr, query, true);
    return json;
}


User HttpClient::GetUser(param Id){
    QString path = "/users/" + Id;
    QString method = "GET";
    QJsonDocument json = sendRequest(path, method, nullptr, nullptr, true);
    QJsonObject root_obj = json.object();
    QVariantMap root_map = root_obj.toVariantMap();

    User u = User(root_map["id"].toString(), root_map["name"].toString(), root_map["email"].toString(),root_map["about"].toString(), root_map["avatar"].toString());

    return u;
}
QIcon HttpClient::GetIcon(param filepath, QSize size){
    if (filepath.toStdString().rfind("<svg xmlns:dc=", 0) == 0) {
        return Util::RoundImage(filepath.toLocal8Bit(), size, QString("SVG"));
    }
    else{
        QByteArray b = sendRequest(filepath);
        return Util::RoundImage(b, size, filepath.mid(filepath.lastIndexOf(".")+1));
    }
}

QString HttpClient::GetContentType(QString filename){
    QString filetype = filename.mid(filename.lastIndexOf(".")+1);
    if(filetype == QString("svg"))
        return QString("image/svg+xml");
    return QString("image/") + filetype;

}

void HttpClient::SetUser(QVariantMap map){
    QVariantMap usermap = map["user"].toMap();
    QVariantMap tokens_map = map["tokens"].toMap();
    QVariantMap access = tokens_map["access"].toMap();
    QVariantMap refresh = tokens_map["refresh"].toMap();

    tokens = new Tokens(access["token"].toString(), refresh["token"].toString());

    user = new User(usermap["id"].toString(), usermap["name"].toString(), usermap["email"].toString(),usermap["about"].toString(), usermap["avatar"].toString());

    if(usermap["isEmailVerified"].toBool()){
        isVerified = true;
    }
    else{
        isVerified = false;
    }
}
