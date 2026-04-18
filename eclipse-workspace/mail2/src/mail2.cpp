#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <curl/curl.h>

std::mutex fileMutex;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

std::string fetchPage(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error fetching " << url << ": "
                      << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    return response;
}

bool isGovOrMil(const std::string& url) {
    return (url.find(".gov") != std::string::npos ||
            url.find(".mil") != std::string::npos);
}

void fetchAndSave(const std::string& url, const std::string& outputFile) {
    try {
        if (!isGovOrMil(url)) {
            std::cerr << "Skipping non-.gov/.mil URL: " << url << std::endl;
            return;
        }

        std::string html = fetchPage(url);

        std::lock_guard<std::mutex> lock(fileMutex);
        std::ofstream out(outputFile, std::ios::app);
        if (out.is_open()) {
            out << "==== " << url << " ====\n";
            out << html.substr(0, 500) << "\n\n"; // Save first 500 chars as a sample
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in thread for " << url << ": " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error in thread for " << url << std::endl;
    }
}

int main() {
    curl_global_init(CURL_GLOBAL_ALL);

    std::vector<std::string> urls = {
        "https://www.army.mil",
        "https://www.navy.mil",
        "https://www.fbi.gov",
        "https://www.cia.gov",
        "https://www.nsa.gov",
        "https://www.dhs.gov",
        "https://www.state.gov",
        "https://www.defense.gov",
        "https://www.justice.gov",
        "https://www.va.gov",
        "https://www.whitehouse.gov",
        "https://www.af.mil",
        "https://www.marines.mil",
        "https://www.spaceforce.mil",
        "https://www.loc.gov",   // Library of Congress
        "https://www.nps.gov"    // National Park Service
    };

    std::string outputFile = "results.txt";
    std::vector<std::thread> threads;

    for (const auto& url : urls) {
        threads.emplace_back(fetchAndSave, url, outputFile);
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    curl_global_cleanup();
    std::cout << "Fetching complete. Results saved to " << outputFile << std::endl;
    return 0;
}
