#include <fstream>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

size_t WriteCallback(void *contents, size_t size, size_t nmemb, json *output) {
  size_t totalSize = size * nmemb;
  
  std::string chunk(static_cast<char*>(contents), totalSize);
  
  std::vector<json> temp_curl;
  temp_curl.emplace_back(json::parse(chunk));
  
  for (const auto& jsonObject : temp_curl) {
    *output += jsonObject;
  }
  
  return totalSize;
}

json APIRequest(const std::string& url) {
  CURL *curl;
  CURLcode res;
  json response;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);
  }
  return response;
}

json loadConfig() {
  std::string homeDir = std::getenv("HOME");
  std::string configPath = homeDir + "/.config/.weather_config.json";

  std::ifstream inputFile(configPath);
  json inputJson;
  inputFile >> inputJson;

  inputFile.close();

  return inputJson; 
}


double convertToCelsius(double temp) {
  return temp - 273.15;
}

int main() {
  json config = loadConfig();

  std::string api_key = config["api_key"].template get<std::string>();
  std::string city = config["city"].template get<std::string>();
  bool kelvin = config["kelvin"].template get<bool>();

  std::string cityUrl = "http://api.openweathermap.org/geo/1.0/direct?q=" + city + "&appid=" + api_key;
 

  json cityInfo = APIRequest(cityUrl)[0][0];

  double lat = cityInfo["lat"].template get<double>();
  double lon = cityInfo["lon"].template get<double>();
  
  std::string weatherUrl = "https://api.openweathermap.org/data/2.5/weather?lat=" + std::to_string(lat) + "&lon=" + std::to_string(lon) + "&appid=" + api_key;
  
  json weatherInfo = APIRequest(weatherUrl)[0]["main"];

  double temp = weatherInfo["temp"].template get<double>();
  
  if(!kelvin) {
    temp = convertToCelsius(temp);
  }

  std::cout << int(temp) << std::endl;

  return 0;
}
