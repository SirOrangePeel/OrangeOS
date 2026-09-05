#include "driver.h"

Driver::Driver() {

}

Driver::~Driver() {

}

void Driver::Activate() {

}

int Driver::Reset() {

} 

void Driver::Deactivate() {

}

DriverManager::DriverManager() {

}

DriverManager::~DriverManager() {
    numDrivers = 0;
}

void DriverManager::AddDriver(Driver* dvr) {
    drivers[numDrivers] = dvr;
    numDrivers++;
}