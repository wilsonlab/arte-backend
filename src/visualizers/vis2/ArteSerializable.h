#ifndef ARTE_SERIALIZABLE_H_
#define ARTE_SERIALIZABLE_H_


// A simple abstract class that will wrap the different preference objects used in the UI objects. This will be used to dump preferences to binary files so they can be loaded later on.
class ArteSerializable {
public:
    virtual void inflateFromBytes(byte b*, int* len) = 0;
    virtual void deflateToBytes(byte b*, int* len) = 0;
};


#endif