#include <gtest/gtest.h>
#include <SensorMessage/SensorMessage.h>
#include <TestClient/TestClient.cpp>
#include <iostream>

/* =================================================================== */
TEST(SensorMessage, equalOperator)
{
    SensorMessage message1;
    message1.setData(1);
    message1.setDataType(2);
    message1.setLocation(3);
    message1.setExtension(4);
    message1.setNumberInLocation(5);
    message1.setReserv(6);
    message1.setVersion(7);

    SensorMessage message2;
    message2.setData(1);
    message2.setDataType(2);
    message2.setLocation(3);
    message2.setExtension(4);
    message2.setNumberInLocation(5);
    message2.setReserv(6);
    message2.setVersion(7);

    EXPECT_TRUE(message1 == message2);

    SensorMessage message3;
    EXPECT_TRUE(message3 != message2);
}


/* =================================================================== */
TEST(SensorMessage, streamOperator)
{
    SensorMessage message;
    message.setData(1);
    message.setDataType(2);
    message.setLocation(3);
    message.setExtension(4);
    message.setNumberInLocation(5);
    message.setReserv(6);
    message.setVersion(7);

    SensorMessage newMessage;
    std::ostringstream stream;
    stream << message;
    std::istringstream isString (stream.str());
    std::istream is(isString.rdbuf());
    is >> newMessage;
    std::string s1 = message.data();
    std::string s2 = newMessage.data();
    EXPECT_EQ(message, newMessage);

}