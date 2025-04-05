#ifndef DBFILE
#define DBFILE

extern char* username;
extern float balance;
extern float BTCPRICE;

extern void http_get_users_db();
extern void http_register_user();
extern void http_get_BTC_price();
extern void http_get_user_orders();

#endif // FILE2_H