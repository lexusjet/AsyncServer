#include <SensorMessage/SensorMessage.h>

SensorMessage::SensorMessage()
{
    header.version = 0;
    header.location = 0;
    header.numberInLocation = 0;
    header.dataType = 0;
    header.data = 0;
    header.reserv = 0;
    header.extension = 0;
}

SensorMessage::~SensorMessage()
{

}

SensorMessage::SensorMessage(const SensorMessage& a)
{
    header.version = a.header.version;
    header.location = a.header.location ;
    header.numberInLocation = a.header.numberInLocation ;
    header.dataType = a.header.dataType ;
    header.data = a.header.data ;
    header.extension = a.header.extension ;
    header.reserv = a.header.reserv ;
}

void SensorMessage::operator=(const SensorMessage& a)
{
    header.version = a.header.version;
    header.location = a.header.location;
    header.numberInLocation = a.header.numberInLocation;
    header.dataType = a.header.dataType;
    header.data = a.header.data;
    header.extension = a.header.extension;
    header.reserv = a.header.reserv;
}
bool SensorMessage::operator==(const SensorMessage& other) const
{
    return header.version == other.header.version
            && header.location == other.header.location
            && header.numberInLocation == other.header.numberInLocation
            && header.dataType == other.header.dataType
            && header.data == other.header.data
            && header.extension == other.header.extension
            && header.reserv == other.header.reserv;

}

bool SensorMessage::operator!=(const SensorMessage& other) const
{
    return !(*this == other);
}

std::istream & operator>>(std::istream & stream, SensorMessage& messege)
{
    stream.read(messege.str, MESSAGE_SIZE);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const SensorMessage messege)
{
    stream.write(messege.str, MESSAGE_SIZE);
    return stream;
}

void SensorMessage::testPrint() const 
{
    std::cout << "VERSION : " << static_cast<int>(getVersion()) << std::endl <<
    "LOCAL : " << static_cast<int>(getLocation()) << std::endl <<
    "NUMBER IN LOCAL : " << static_cast<int>(getNumberInLocation()) <<std::endl <<
    "DATA TYPE : " << static_cast<int>(getDataType()) <<std::endl <<
    "DATA : " << static_cast<int>(getData()) <<std::endl <<
    "EXTENSION : " << static_cast<int>(getExtension()) <<std::endl <<
    "RESERVE : " << static_cast<int>(getReserve());
}
