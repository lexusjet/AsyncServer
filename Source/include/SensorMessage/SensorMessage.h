#pragma once

#include <cstdint>
#include <iostream>

// Format of header
//      size: 16 bytes
//
//      1) 1 byte: version
//      2) 1 byte: location
//      3) 1 byte: number in location
//      4) 1 byte: data type
//      5) 4 byte: data
//      6) 4 byte: extension
//      7) 4 byte: reserv

struct Header{
    uint8_t version;
    uint8_t location;
    uint8_t numberInLocation;
    uint8_t dataType;
    int32_t data;
    uint32_t extension;
    uint32_t reserv;
};

constexpr const size_t MESSAGE_SIZE = sizeof(Header);
// =============================================================================
class SensorMessage
{
    public:
        SensorMessage();
        ~SensorMessage();
        SensorMessage(const SensorMessage&);
        void operator=(const SensorMessage&);

        bool operator==(const SensorMessage& other) const;
        bool operator!=(const SensorMessage& other) const;

        void testPrint() const;

        size_t messegeSize() const {return MESSAGE_SIZE;};
        const char* data() const {return str;}

        void setVersion(const uint8_t version){ header.version = version;};
        uint8_t getVersion() const {return header.version;};

        void setLocation(const uint8_t local){ header.location = local;};
        uint8_t getLocation() const {return header.location;};

        void setNumberInLocation(const uint8_t a) { header.numberInLocation = a;};
        uint8_t getNumberInLocation() const {return header.numberInLocation;};

        void setDataType(const uint8_t type) {header.dataType = type;};
        uint8_t getDataType() const { return header.dataType;};

        void setData(const int32_t data) {header.data = data;};
        int32_t getData() const { return header.data;};

        void setExtension(const uint32_t extension) {header.extension = extension;};
        uint32_t getExtension() const {return header.extension;};

        void setReserv(const uint32_t reserv) {header.reserv = reserv;};
        uint32_t getReserve() const {return header.reserv;};

    protected:

        union {
            char str[MESSAGE_SIZE];
            Header header;
        };


    friend std::istream& operator>>(std::istream & stream, SensorMessage& messege);
    friend std::ostream& operator<<(std::ostream& stream, const SensorMessage message);
};

std::istream& operator>>(std::istream & stream, SensorMessage& messege);
std::ostream& operator<<(std::ostream& stream, const SensorMessage messege);
