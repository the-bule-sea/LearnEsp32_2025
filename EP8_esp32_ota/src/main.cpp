#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <Preferences.h>  // 用于NVS存储

const char *wifi_ssid = "Mi10Pro";        // WIFI名称
const char *wifi_password = "88888888";   // WIFI密码

const char *ota_url = "http://192.168.132.140:9000/esp32/firmware.bin"; // 固件地址
const char *version_url = "http://192.168.132.140:9000/esp32/version.txt"; // 版本号文件地址

const char *defaultVersion = "V0.1"; // 默认版本

Preferences preferences;  // NVS对象

// OTA日志回调
void update_started() { Serial.println("[OTA] Update started."); }
void update_finished() { Serial.println("[OTA] Update finished, restarting..."); }
void update_progress(int cur, int total) { Serial.printf("[OTA] Progress: %d of %d bytes (%.1f%%)\n", cur, total, cur * 100.0 / total); }
void update_error(int err) { Serial.printf("[OTA] Error code: %d\n", err); }

// 执行OTA升级
t_httpUpdate_return updateBin(const char *update_url)
{
    Serial.println("[OTA] Start updating firmware...");
    WiFiClient client;
    httpUpdate.onStart(update_started);
    httpUpdate.onEnd(update_finished);
    httpUpdate.onProgress(update_progress);
    httpUpdate.onError(update_error);
    t_httpUpdate_return ret = httpUpdate.update(client, update_url);
    return ret;
}

// 读取本地版本号
String getLocalVersion()
{
    preferences.begin("ota", false);  // 打开命名空间"ota"，读写模式
    String version = preferences.getString("version", "");
    preferences.end();

    if (version == "") {
        // 第一次启动，没有存版本
        Serial.println("[OTA] No local version found, setting default.");
        preferences.begin("ota", false);
        preferences.putString("version", defaultVersion);
        preferences.end();
        version = defaultVersion;
    }
    return version;
}

// 写入本地新版本号
void setLocalVersion(String newVersion)
{
    preferences.begin("ota", false);
    preferences.putString("version", newVersion);
    preferences.end();
    Serial.printf("[OTA] Local version updated to %s\n", newVersion.c_str());
}

// 检查版本并决定是否更新
void checkVersionAndUpdate()
{
    String localVersion = getLocalVersion();
    Serial.printf("[OTA] Local version: %s\n", localVersion.c_str());

    HTTPClient http;
    http.begin(version_url);
    int httpCode = http.GET();
    if (httpCode == 200) {
        String serverVersion = http.getString();
        serverVersion.trim();
        Serial.printf("[OTA] Server version: %s\n", serverVersion.c_str());

        if (localVersion != serverVersion) {
            Serial.println("[OTA] New version detected, updating...");
            t_httpUpdate_return ret = updateBin(ota_url);
            if (ret == HTTP_UPDATE_OK) {
                Serial.println("[OTA] Update successful, saving new version...");
                setLocalVersion(serverVersion);
                delay(2000);  // 给日志一点时间打印
                ESP.restart();  // 重启设备应用新固件
            } else {
                Serial.println("[OTA] Update failed, keeping old version.");
            }
        } else {
            Serial.println("[OTA] Already up-to-date, no need to update.");
        }
    } else {
        Serial.printf("[OTA] Failed to fetch version file, HTTP code: %d\n", httpCode);
    }
    http.end();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("[System] Booting...");

    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[System] Connected to WiFi!");

    checkVersionAndUpdate();  // 检查是否需要更新
}

void loop()
{
    // 你的正常业务逻辑可以放这里
}
