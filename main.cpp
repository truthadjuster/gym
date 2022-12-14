#include <iostream>
#include <cstdlib>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <future>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;

class Connection {
public:
    virtual
    bool login(std::string u, std::string p) {
        std::cout << u << "/" << p << std::endl;
        std::cout << "[" << this << "]" << "[login]\n";
        return true;
    }

    Connection() { std::cout << "Connection\n"; }
    ~Connection() { std::cout << "~Connection\n"; }
};

class Database {
public:
    explicit Database(Connection& _conn): conn_(_conn){}
    bool connect(const std::string& u, const std::string& p) {
        return conn_.login(u, p);
    }
private:
    Connection & conn_;
};

namespace z
{
    int addnum(int a, int b) {
        return a + b;
    }
}

namespace y 
{
    class A {
    public:
        A(){std::cout << "!#$*\n";}
        static void f(const std::string& s){std::cout << s << std::endl;}
    };
}

namespace x 
{
    //ce y::A::f("+++");
    y::A o;
    int counter = 123;
    int f1(int a, int b) {
        std::runtime_error _exception{"fuck"};
        if (b == 0) throw _exception;
        return a-b;
    }

    class A {
    public:
        int& n() {
            f();
            Point p;
            p.x = 0;
            p.y = 0;
            std::cout << p.x << p.y;
            return n_;
        }
        int* get(){return &n_;}
        A* get2(){ return this;}
        int* get3(){ return &n_;}
    private:
        struct Point {int x; int y;}; // won't affect &n_ == this
        void f() {} // won't affect &n_ == this
        static int count; // won't affect &n_ == this
        int n_ = 123;
        int pad = 0;
    };
}

TEST(hmmm,test1)
{
    class Obj {
    public:
        explicit Obj(int n) {
            std::cout << n << std::endl;
        }
    };

    Obj o(1);
    Obj oo{2};
    //ce Obj ooo = 3; 

}

TEST(lambda,test1)
{
    int a = 3;
    int b = 4;
    auto f = [&a]() {
        std::cout << a << std::endl;
        a = 69;
    };

    f();
    ASSERT_EQ(69, a);
}

TEST(zzz,test1)
{
    auto f = std::bind(
                z::addnum, 
                std::placeholders::_1, 
                std::placeholders::_2
                );
    ASSERT_EQ(7, f(3, 4));

    Connection conn;
    auto L = std::bind(&Connection::login,
                &conn, "JOHN", std::placeholders::_1
                );
    ASSERT_TRUE(L("doe"));
}

TEST(ccc,test1)
{
    std::shared_ptr<Connection> pconn;
    ASSERT_EQ(nullptr, pconn);
    {
        pconn = std::make_shared<Connection>();
        std::cout << "<" << pconn << ">\n";
    }
    ASSERT_NE(nullptr, pconn); // omg
    pconn->login("user", "pass"); // omg
}

TEST(aaa,test4)
{
    class Mock_Connection : public Connection {
    public:
        MOCK_METHOD2(login, bool(std::string u, std::string p));
    };

    Mock_Connection mock;
    Database db(mock);

    EXPECT_CALL(mock, login(::testing::_, ::testing::_))
        .Times(1)
        .WillOnce(::testing::Return(true));

    ASSERT_TRUE(db.connect("john", "password"));
}

TEST(aaa,test3)
{
    struct S {
        int a;
        char b;
    };
    ASSERT_EQ(2*sizeof(int), sizeof(S));

    #pragma pack(push, 1)
    struct T {
        int a;
        char b;
    };
    #pragma pack(pop)
    ASSERT_EQ(sizeof(int) + sizeof(char), sizeof(T));

#ifndef _WIN32
    struct __attribute__((packed)) Q {
        int a;
        char b;
    };
    ASSERT_EQ(sizeof(int) + sizeof(char), sizeof(Q));
#endif
}

TEST(aaa,test2)
{
    const char* s = "Hello, World!";
    const char* ss = "Hello, World!";
    ASSERT_EQ(s, ss);
    ASSERT_STREQ(s, ss);

    //ce s[0] = 'x';

    std::string sss("Hello, World!");
    ASSERT_EQ(s, sss);
    ASSERT_NE(s, sss.data());
    ASSERT_NE(s, sss.c_str());

    char rrr[] = "Hello, World!";
    ASSERT_STREQ(s, rrr);
    ASSERT_NE(s, rrr);
    rrr[0] = 'x';
    ASSERT_STRNE(s, rrr);

    ASSERT_EQ(13, strlen(rrr));
    ASSERT_EQ(13, strlen(ss));
    ASSERT_EQ(14, sizeof(rrr));
    ASSERT_EQ(8, sizeof(ss));

    const char qqq[] = "Hello, World!";
    //ce qqq[0] = 'x';
    ASSERT_NE(s, qqq);
    ASSERT_STREQ(s, qqq);
    ASSERT_EQ(s, "Hello, World!");
}


TEST(aaa, test1)
{
    x::A o;
    int& n = o.n();
    int orig = n++;
    ASSERT_EQ(&n, o.get());
    ASSERT_EQ(*o.get(), orig+1);

    ASSERT_EQ(
        reinterpret_cast<void*>(&n), 
        reinterpret_cast<void*>(&o)
    );

    ASSERT_EQ(
        reinterpret_cast<void*>(&o), 
        reinterpret_cast<void*>(o.get2())
    );

    ASSERT_EQ(
        reinterpret_cast<void*>(&o), 
        reinterpret_cast<void*>(o.get3())
    );

    int p = o.n();
    ASSERT_EQ(p, orig+1);
    p++;
    ASSERT_EQ(p-1, orig+1);

    auto f = [](int a, int b) {
        std::runtime_error _exception{"cannot be"};
        if (b == 0) throw _exception;
        return a/b;
    };

    std::function<int(int,int)> gunc = f;
    gunc = f;

    typedef std::function<int(int,int)> Func;
    Func ff = f;

    ASSERT_EQ(3, f(6,2));
    ASSERT_EQ(3, ff(6,2));
    ASSERT_EQ(3, gunc(6,2));

    gunc = x::f1;
    ASSERT_EQ(4, gunc(6,2));

    try {
        f(6,0);
        FAIL() << "*** ERROR ***\n";
    } catch (std::runtime_error& e) {
        ASSERT_STREQ("cannot be", e.what());
    }

    EXPECT_THROW(f(6, 0), std::runtime_error);
}

int main(int argc, char *argv[])
{
    y::A::f("???");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
