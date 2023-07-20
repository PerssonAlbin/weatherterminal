#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <filesystem>
#include <curl/curl.h>
#include <cctype>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

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
      std::cout << "Error" << std::endl;
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);
  }
  return response;
}

void createConfig(std::string config_path) {
  json config = json::parse(R"(
                            {
                              "api_key": "",
                              "city": "",
                              "temp_unit": ""
                            }
                            )");
  
  std::ofstream output_file(config_path);
  output_file << std::setw(4) << config << std::endl;
  
  output_file.close();
}

json loadConfig() {
  std::string homeDir = std::getenv("HOME");
  std::string config_path = homeDir + "/.config/.weather_config.json";

  if(!fs::exists(config_path)) {
    createConfig(config_path);
  }
  std::ifstream inputFile(config_path);
  json input_json;
  inputFile >> input_json;

  inputFile.close();

  return input_json; 
}


double convertToCelsius(double kelvin_temp) {
  return kelvin_temp - 273.15;
}

double convertToFahrenheit(double kelvin_temp) {
  return (kelvin_temp - 273.15) * 9/5 + 32;
}

std::string lowerCaseString(std::string str) {
  for (char& ch : str) {
    ch = std::tolower(ch);
  }
  return str;
}

int main() {
  json config = loadConfig();

  std::string api_key;
  std::string city;
  std::string temp_unit;

  try {
    api_key = config["api_key"].template get<std::string>();
    city = config["city"].template get<std::string>();
    temp_unit = config["temp_unit"].template get<std::string>();
  }
  catch (const std::exception&) {
    std::cout << "Error" << std::endl;
    std::cerr << "Incomplete config setup" << std::endl;
    return 0;
  } 

  if(api_key.empty()) {
    std::cout << "Error" << std::endl;
    std::cerr << "Missing api key in config" << std::endl;  
    return 0;
  }

  if(city.empty()) {
    std::cout << "Error" << std::endl;
    std::cerr << "Missing city in config" << std::endl;
    return 0;
  }

  if(temp_unit.empty()) {
    std::cout << "Error" << std::endl;
    std::cerr << "Missing temp unit in config" << std::endl;
    return 0;
  }

  std::string city_url = "http://api.openweathermap.org/geo/1.0/direct?q=" + city + "&appid=" + api_key;
 
  json cityInfo = APIRequest(city_url)[0][0];

  double lat;
  double lon;

  try {
    lat = cityInfo["lat"].template get<double>();
    lon = cityInfo["lon"].template get<double>();
  }
  catch(std::exception&) {
    std::cout << "Error" << std::endl;
    std::cerr << "API Request failed" << std::endl;
    return 0;
  }
  
  std::string weather_url = "https://api.openweathermap.org/data/2.5/weather?lat=" + std::to_string(lat) + "&lon=" + std::to_string(lon) + "&appid=" + api_key;
  
  json weather_info = APIRequest(weather_url)[0]["main"];

  double temp_kelvin;
  
  try {
    temp_kelvin = weather_info["temp"].template get<double>();
  }
  catch(std::exception&) {
    std::cout << "Error" << std::endl;
    std::cerr << "API request failed" << std::endl;
    return 0;
  }

  temp_unit = lowerCaseString(temp_unit);
  
  if(temp_unit == "celsius") {
    double temp_celsius = convertToCelsius(temp_kelvin);
    std::cout << int(temp_celsius) << "°" << std::endl;
  } else if(temp_unit == "fahrenheit") {
    double temp_fahrenheit = convertToFahrenheit(temp_kelvin);
    std::cout << int(temp_fahrenheit) << "°F" << std::endl;
  }
  else if(temp_unit == "kelvin") {
    std::cout << int(temp_kelvin) << "K" << std::endl;
  } else {
    std::cout << "Error" << std::endl;
    std::cerr << "temp unit not recognized. The following are supported: celsius, fahrenheit and kelvin" << std::endl;
  }

  return 0;
}
