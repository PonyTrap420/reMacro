#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "QObject"
#include "qnetwork.h"
#include <QtNetwork/QNetworkAccessManager>
#include "QUrlQuery"
#include "QPushButton"
#include "Macro/macro_frame.h"

#define param QString

struct Tokens{
    param ACCESS;
    param REFRESH;

    Tokens(param ACCESS, param REFRESH){
        this->ACCESS = ACCESS;
        this->REFRESH = REFRESH;
    }
};
struct User{
    param id;
    param name;
    param email;
    param about;
    param avatar;

    User(param id, param name, param email, param about, param avatar){
        this->id = id;
        this->name = name;
        this->email = email;
        this->about = about;
        this->avatar = avatar;
    }


};

class HttpClient : public QObject
{
    Q_OBJECT
    public:
        bool logged = false;
        bool isVerified = false;
        User* user = nullptr;
        Tokens* tokens = nullptr;

        QPushButton* avatar = nullptr;

        static HttpClient* getInstance();

         //Auth Controllers
         QString Login(param Email, param Pwd);
         QString Register(param Email, param Username, param Pwd);
         bool Logout(param REFRESH_TOKEN);
         bool RefreshTokens(param REFRESH_TOKEN);
         QString ForgotPwd(param Email);
         QString ResetPwd(param Token, param Pwd);
         bool EmailVerifyRequest();
         QString VerifyEmail(param Token);
         bool UpdateUser(param Email, param Username, param Pwd);

         //User Controllers
         bool UploadAvatar(param filepath, param filename);
         QString UploadMacro(param Name, param Description, MacroFrame* macro);
         QJsonDocument GetMacros(param Page);
         QJsonDocument GetUserMacros(param User);
         QJsonDocument SearchMacros(param Name, param Page);
         bool DownloadMacro(param Macro);
         bool UploadComment(param Macro, param Content);
         QJsonDocument GetComments(param Macro, param Page);

         User GetUser(param Id);
         QIcon GetIcon(param filepath, QSize size);


    private:
        QNetworkAccessManager * manager;
        QSslConfiguration config;
        QNetworkRequest request;
        QNetworkReply * reply;
        QString serverUrl;


        QJsonDocument sendRequest(QString& path, QString& method, QJsonObject* obj = nullptr, QUrlQuery* query = nullptr, bool auth = false);
        QJsonDocument sendRequest(QString &path, QString &method, QString filepath, QString filename);
        QByteArray sendRequest(QString filename);

        void SetUser(QVariantMap map);

        QString GetContentType(QString filename);

        //singleton pattern
        HttpClient();
        static HttpClient * instance;

};

#endif // HTTPCLIENT_H
