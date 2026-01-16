/****************************************************************************
**
** Plugin Test Application
** Tests signal generator, signal analyzer, and positioner plugins
**
****************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <Windows.h>
#include "iplugininterface.h"

// Function pointer types for plugin factory functions
typedef ISignalGeneratorPlugin* (*CreateSignalGeneratorFunc)();
typedef ISignalAnalyzerPlugin* (*CreateSignalAnalyzerFunc)();
typedef IPositionerPlugin* (*CreatePositionerFunc)();
typedef void (*DestroyPluginFunc)(void*);

// Plugin info structure
struct PluginInfo {
    std::string name;
    std::string path;
    std::string type; // "signalgenerator", "signalanalyzer", "positioner"
};

// Available plugins
std::vector<PluginInfo> g_plugins = {
    {"SignalCore SC5511A", "../../signalgenerator/signalcore_sc5511a/build/Release/signalcore_sc5511a.dll", "signalgenerator"},
    {"Dummy Signal Generator", "../../signalgenerator/dummy/build/Release/dummy.dll", "signalgenerator"},
    {"Dummy Signal Analyzer", "../../signalanalyzer/dummy/build/Release/dummy.dll", "signalanalyzer"},
    {"Dummy Positioner", "../../positioner/dummy/build/Release/dummy.dll", "positioner"}
};

// Test Signal Generator Plugin
void testSignalGeneratorPlugin(const std::string& pluginPath) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Testing Signal Generator Plugin" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Load the DLL
    HMODULE hModule = LoadLibraryA(pluginPath.c_str());
    if (!hModule) {
        std::cerr << "Failed to load plugin DLL: " << pluginPath << std::endl;
        std::cerr << "Error code: " << GetLastError() << std::endl;
        return;
    }
    
    // Get factory functions
    CreateSignalGeneratorFunc createFunc = (CreateSignalGeneratorFunc)GetProcAddress(hModule, "createSignalGeneratorPlugin");
    DestroyPluginFunc destroyFunc = (DestroyPluginFunc)GetProcAddress(hModule, "destroyPlugin");
    
    if (!createFunc || !destroyFunc) {
        std::cerr << "Failed to get factory functions" << std::endl;
        FreeLibrary(hModule);
        return;
    }
    
    // Create plugin instance
    ISignalGeneratorPlugin* plugin = createFunc();
    if (!plugin) {
        std::cerr << "Failed to create plugin instance" << std::endl;
        FreeLibrary(hModule);
        return;
    }
    
    // Set up callbacks
    plugin->onDevicesScanned = [](const std::vector<DeviceInfo>& devices) {
        std::cout << "\n[Callback] Devices scanned: " << devices.size() << " device(s) found" << std::endl;
        for (const auto& dev : devices) {
            std::cout << "  - " << dev.name << " (SN: " << dev.serialNumber << ", Address: " << dev.address << ")" << std::endl;
        }
    };
    
    plugin->onConnected = []() {
        std::cout << "[Callback] Connected!" << std::endl;
    };
    
    plugin->onDisconnected = []() {
        std::cout << "[Callback] Disconnected!" << std::endl;
    };
    
    plugin->onRfEnabled = []() {
        std::cout << "[Callback] RF Enabled!" << std::endl;
    };
    
    plugin->onRfDisabled = []() {
        std::cout << "[Callback] RF Disabled!" << std::endl;
    };
    
    plugin->onError = [](const std::string& error) {
        std::cout << "[Callback] Error: " << error << std::endl;
    };
    
    // Test 1: Scan for devices
    std::cout << "\n[Test 1] Scanning for devices..." << std::endl;
    std::vector<DeviceInfo> devices = plugin->scanDevices();
    
    // Test 2: Connect to device
    std::cout << "\n[Test 2] Connecting to device..." << std::endl;
    if (!devices.empty()) {
        if (plugin->connectToDevice(devices[0].address)) {
            std::cout << "Connected to: " << devices[0].name << std::endl;
            
            // Test 3: Check connection status
            std::cout << "\n[Test 3] Connection status: " << (plugin->isConnected() ? "Connected" : "Not Connected") << std::endl;
            
            // Test 4: Set frequency
            std::cout << "\n[Test 4] Setting frequency to 5.5 GHz..." << std::endl;
            plugin->setFreq(5.5e9);
            
            // Test 5: Set power
            std::cout << "\n[Test 5] Setting power to -10 dBm..." << std::endl;
            plugin->setPower(-10.0);
            
            // Test 6: Enable RF
            std::cout << "\n[Test 6] Enabling RF output..." << std::endl;
            plugin->enableRf();
            std::cout << "RF Status: " << (plugin->isRfEnabled() ? "ON" : "OFF") << std::endl;
            
            // Test 7: Disable RF
            std::cout << "\n[Test 7] Disabling RF output..." << std::endl;
            plugin->disableRf();
            std::cout << "RF Status: " << (plugin->isRfEnabled() ? "ON" : "OFF") << std::endl;
            
            // Test 8: Disconnect
            std::cout << "\n[Test 8] Disconnecting..." << std::endl;
            plugin->disconnect();
        } else {
            std::cerr << "Failed to connect to device" << std::endl;
        }
    } else {
        std::cout << "No devices found, testing connect() without address..." << std::endl;
        plugin->connect();
        if (plugin->isConnected()) {
            plugin->setFreq(2.4e9);
            plugin->setPower(0.0);
            plugin->enableRf();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            plugin->disableRf();
            plugin->disconnect();
        }
    }
    
    // Cleanup
    destroyFunc(plugin);
    FreeLibrary(hModule);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Signal Generator Test Complete!" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

// Test Signal Analyzer Plugin
void testSignalAnalyzerPlugin(const std::string& pluginPath) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Testing Signal Analyzer Plugin" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Load the DLL
    HMODULE hModule = LoadLibraryA(pluginPath.c_str());
    if (!hModule) {
        std::cerr << "Failed to load plugin DLL: " << pluginPath << std::endl;
        return;
    }
    
    // Get factory functions
    CreateSignalAnalyzerFunc createFunc = (CreateSignalAnalyzerFunc)GetProcAddress(hModule, "createSignalAnalyzerPlugin");
    DestroyPluginFunc destroyFunc = (DestroyPluginFunc)GetProcAddress(hModule, "destroyPlugin");
    
    if (!createFunc || !destroyFunc) {
        std::cerr << "Failed to get factory functions" << std::endl;
        FreeLibrary(hModule);
        return;
    }
    
    // Create plugin instance
    ISignalAnalyzerPlugin* plugin = createFunc();
    if (!plugin) {
        std::cerr << "Failed to create plugin instance" << std::endl;
        FreeLibrary(hModule);
        return;
    }
    
    // Set up callbacks
    plugin->onDevicesScanned = [](const std::vector<DeviceInfo>& devices) {
        std::cout << "\n[Callback] Devices scanned: " << devices.size() << " device(s) found" << std::endl;
    };
    
    plugin->onConnected = []() {
        std::cout << "[Callback] Connected!" << std::endl;
    };
    
    plugin->onDisconnected = []() {
        std::cout << "[Callback] Disconnected!" << std::endl;
    };
    
    plugin->onPeakFound = [](const Peak& peak) {
        std::cout << "[Callback] Peak found! Freq: " << peak.frequencyHz/1e6 << " MHz, Power: " << peak.leveldBm << " dBm" << std::endl;
    };
    
    plugin->onError = [](const std::string& error) {
        std::cout << "[Callback] Error: " << error << std::endl;
    };
    
    // Test 1: Scan devices
    std::cout << "\n[Test 1] Scanning for devices..." << std::endl;
    std::vector<DeviceInfo> devices = plugin->scanDevices();
    
    // Test 2: Connect
    std::cout << "\n[Test 2] Connecting..." << std::endl;
    if (!devices.empty()) {
        plugin->connectToDevice(devices[0].address);
    } else {
        plugin->connect();
    }
    
    if (plugin->isConnected()) {
        // Test 3: Configure analyzer
        std::cout << "\n[Test 3] Configuring analyzer..." << std::endl;
        plugin->setStartFreq(2.0e9);
        plugin->setStopFreq(3.0e9);
        plugin->setRBW(100e3);
        
        // Test 4: Find peak
        std::cout << "\n[Test 4] Finding peak..." << std::endl;
        Peak peak = plugin->findPeak();
        std::cout << "Peak found: " << peak.frequencyHz/1e6 << " MHz, " << peak.leveldBm << " dBm" << std::endl;
        
        // Test 5: Find another peak
        std::cout << "\n[Test 5] Finding another peak..." << std::endl;
        peak = plugin->findPeak();
        std::cout << "Peak found: " << peak.frequencyHz/1e6 << " MHz, " << peak.leveldBm << " dBm" << std::endl;
        
        // Test 6: Disconnect
        std::cout << "\n[Test 6] Disconnecting..." << std::endl;
        plugin->disconnect();
    }
    
    // Cleanup
    destroyFunc(plugin);
    FreeLibrary(hModule);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Signal Analyzer Test Complete!" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

// Test Positioner Plugin
void testPositionerPlugin(const std::string& pluginPath) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Testing Positioner Plugin" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Load the DLL
    HMODULE hModule = LoadLibraryA(pluginPath.c_str());
    if (!hModule) {
        std::cerr << "Failed to load plugin DLL: " << pluginPath << std::endl;
        return;
    }
    
    // Get factory functions
    CreatePositionerFunc createFunc = (CreatePositionerFunc)GetProcAddress(hModule, "createPositionerPlugin");
    DestroyPluginFunc destroyFunc = (DestroyPluginFunc)GetProcAddress(hModule, "destroyPlugin");
    
    if (!createFunc || !destroyFunc) {
        std::cerr << "Failed to get factory functions" << std::endl;
        FreeLibrary(hModule);
        return;
    }
    
    // Create plugin instance
    IPositionerPlugin* plugin = createFunc();
    if (!plugin) {
        std::cerr << "Failed to create plugin instance" << std::endl;
        FreeLibrary(hModule);
        return;
    }
    
    // Set up callbacks
    plugin->onDevicesScanned = [](const std::vector<DeviceInfo>& devices) {
        std::cout << "\n[Callback] Devices scanned: " << devices.size() << " device(s) found" << std::endl;
    };
    
    plugin->onConnected = []() {
        std::cout << "[Callback] Connected!" << std::endl;
    };
    
    plugin->onDisconnected = []() {
        std::cout << "[Callback] Disconnected!" << std::endl;
    };
    
    plugin->onPositionChanged = [](double azimuth, double elevation, double polar) {
        std::cout << "[Callback] Position: Az=" << azimuth << "°, El=" << elevation << "°, Polar=" << polar << "°" << std::endl;
    };
    
    plugin->onMovementStarted = []() {
        std::cout << "[Callback] Movement started!" << std::endl;
    };
    
    plugin->onMovementStopped = []() {
        std::cout << "[Callback] Movement stopped!" << std::endl;
    };
    
    plugin->onError = [](const std::string& error) {
        std::cout << "[Callback] Error: " << error << std::endl;
    };
    
    // Test 1: Scan devices
    std::cout << "\n[Test 1] Scanning for devices..." << std::endl;
    std::vector<DeviceInfo> devices = plugin->scanDevices();
    
    // Test 2: Connect
    std::cout << "\n[Test 2] Connecting..." << std::endl;
    if (!devices.empty()) {
        plugin->connectToDevice(devices[0].address);
    } else {
        plugin->connect();
    }
    
    if (plugin->isConnected()) {
        // Test 3: Move to position
        std::cout << "\n[Test 3] Moving to position (45°, 30°)..." << std::endl;
        plugin->moveTo(45.0, 30.0);
        
        // Wait for movement
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Test 4: Stop movement
        std::cout << "\n[Test 4] Stopping movement..." << std::endl;
        plugin->stop();
        
        // Test 5: Move to home
        std::cout << "\n[Test 5] Moving to home position..." << std::endl;
        plugin->moveTo(0.0, 0.0);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Test 6: Disconnect
        std::cout << "\n[Test 6] Disconnecting..." << std::endl;
        plugin->disconnect();
    }
    
    // Cleanup
    destroyFunc(plugin);
    FreeLibrary(hModule);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Positioner Test Complete!" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

int main() {
    std::cout << "======================================" << std::endl;
    std::cout << "    Plugin Test Application" << std::endl;
    std::cout << "======================================\n" << std::endl;
    
    // Display available plugins
    std::cout << "Available Plugins:" << std::endl;
    for (size_t i = 0; i < g_plugins.size(); i++) {
        std::cout << "  " << (i+1) << ". " << g_plugins[i].name << " (" << g_plugins[i].type << ")" << std::endl;
    }
    std::cout << "  0. Test All Plugins" << std::endl;
    
    // Get user selection
    std::cout << "\nSelect plugin to test (0-" << g_plugins.size() << "): ";
    int selection;
    std::cin >> selection;
    
    if (selection < 0 || selection > static_cast<int>(g_plugins.size())) {
        std::cerr << "Invalid selection!" << std::endl;
        return 1;
    }
    
    if (selection == 0) {
        // Test all plugins
        for (const auto& plugin : g_plugins) {
            if (plugin.type == "signalgenerator") {
                testSignalGeneratorPlugin(plugin.path);
            } else if (plugin.type == "signalanalyzer") {
                testSignalAnalyzerPlugin(plugin.path);
            } else if (plugin.type == "positioner") {
                testPositionerPlugin(plugin.path);
            }
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore();
            std::cin.get();
        }
    } else {
        // Test selected plugin
        const PluginInfo& plugin = g_plugins[selection - 1];
        
        if (plugin.type == "signalgenerator") {
            testSignalGeneratorPlugin(plugin.path);
        } else if (plugin.type == "signalanalyzer") {
            testSignalAnalyzerPlugin(plugin.path);
        } else if (plugin.type == "positioner") {
            testPositionerPlugin(plugin.path);
        }
    }
    
    std::cout << "\nAll tests complete! Press Enter to exit...";
    std::cin.ignore();
    std::cin.get();
    
    return 0;
}
