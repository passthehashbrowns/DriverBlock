// DriverLoader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "windows.h"
#include "stdio.h"
#include "tchar.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <varargs.h>
#include <string.h>
#include <string>

__int64 install_driver(LPCWSTR driver_path) {
    SC_HANDLE scm, service;
    __int64 status = -1;

    scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!scm)
    {
        std::cout << "Error opening SCM. Are you admin?" << std::endl;
        return -1;
    }

    service = CreateService(scm, L"TargetDriver", L"TargetDriver", SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, driver_path, NULL, NULL, NULL, NULL, NULL);
    if (GetLastError() == ERROR_SERVICE_EXISTS)
    {
        std::cout << "Service already exists." << std::endl;
    }

    if (!service)
    {
        std::cout << "Error creating service." << std::endl;
        return -1;
    }
    std::cout << "Driver loaded!" << std::endl;
    return 0;
}

__int64 uninstall_driver(LPCWSTR service_name) {
    SC_HANDLE scm, service;
    SERVICE_STATUS ServiceStatus;

    scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!scm) return 0;
    service = OpenService(scm, service_name, SERVICE_ALL_ACCESS);
    if (service) {
        ControlService(service, SERVICE_CONTROL_STOP, &ServiceStatus);
    };
    DeleteService(service);
    CloseServiceHandle(service);
    std::cout << "Driver unloaded." << std::endl;
    return 1;
    CloseServiceHandle(scm);
    return 0;
}

int main(int argc, char* argv[])
{
    std::string str;
    //std::getline(std::cin, str);
    install_driver((LPCWSTR)argv[1]);
    std::getline(std::cin, str);
    uninstall_driver(L"TargetDriver");
    return 0;
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
