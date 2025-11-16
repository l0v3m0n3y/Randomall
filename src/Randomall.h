#ifndef RANDOMALL_H
#define RANDOMALL_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <pplx/pplx.h>
#include <iostream>
#include <string>
#include <map>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class Randomall {
private:
    std::string api_base = "https://randomall.ru/api";
    http_client_config client_config;
    
    http_request create_request(const std::string& path, method m, const std::string& body_data = "", const std::string& content_type = "") {
        http_request request(m);
        
        // Полный набор браузерных заголовков
        request.headers().add("Accept", "application/json");
        request.headers().add("Content-Type", "application/json"); 
        request.headers().add("User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0");
        
        // Устанавливаем тело запроса, если передано
        if (!body_data.empty() && !content_type.empty()) {
            request.set_body(body_data, utility::conversions::to_string_t(content_type));
        }
        
        request.set_request_uri(utility::conversions::to_string_t(path));
        return request;
    }

    std::string build_query_params(const std::map<std::string, std::string>& params) {
        if (params.empty()) return "";
        
        std::string query = "?";
        bool first = true;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                if (!first) query += "&";
                query += param.first + "=" + param.second;
                first = false;
            }
        }
        return query;
    }

public:
    Randomall() {
        client_config.set_validate_certificates(false);
    }

    pplx::task<json::value> generate_result(int gen_id) {
        http_client client(utility::conversions::to_string_t(api_base), client_config);
        std::string path = "/gens/" + std::to_string(gen_id);
        auto request = create_request(path, methods::POST,"{}","application/json");
        
        return client.request(request)
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } else {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("HTTP Error: " + std::to_string(response.status_code())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return pplx::task_from_result(error_obj);
                }
            })
            .then([](pplx::task<json::value> previousTask) {
                try {
                    return previousTask.get();
                } catch (const std::exception& e) {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("Exception: " + std::string(e.what())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return error_obj;
                }
            });
    }
};

#endif
