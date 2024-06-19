#include "SrcMain.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <thread>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <chrono>
#include <exception>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

struct Movie {
	std::string ID = "";
	int Runtime = 0;
	std::string Title = "";
	std::string URL = "";
	int year = 0;
};

std::unordered_map<std::string, Movie> moviesById;
std::unordered_map<std::string, std::vector<Movie>> moviesByName;

void handleGetMovieById(http_request request) {
	//https://microsoft.github.io/cpprestsdk/classweb_1_1http_1_1client_1_1http__client.html
	//https://microsoft.github.io/cpprestsdk/classweb_1_1uri.html
    auto paths = uri::split_path(uri::decode(request.relative_uri().path()));
    if (paths.size() < 3) {
        request.reply(status_codes::BadRequest, "Invalid request format");
        return;
    }

    std::string movieId = paths[2];
    if (moviesById.find(movieId) != moviesById.end()) {
		//https://microsoft.github.io/cpprestsdk/classweb_1_1json_1_1array.html
		//https://microsoft.github.io/cpprestsdk/classweb_1_1json_1_1value.html
        auto& movie = moviesById[movieId];
        json::value responseJson;
        responseJson["ID"] = json::value::string(movie.ID);
        responseJson["Runtime"] = json::value::number(movie.Runtime);
        responseJson["Title"] = json::value::string(movie.Title);
        responseJson["URL"] = json::value::string(movie.URL);
        responseJson["Year"] = json::value::number(movie.year);

        request.reply(status_codes::OK, responseJson);
    } else {
        json::value errorJson;
        errorJson["Error"] = json::value::string("No movie with that ID found");
        request.reply(status_codes::NotFound, errorJson);
    }
}

void handleGetMovieByName(http_request request) {
	//https://microsoft.github.io/cpprestsdk/classweb_1_1http_1_1client_1_1http__client.html
	//https://microsoft.github.io/cpprestsdk/classweb_1_1uri.html
    auto paths = uri::split_path(uri::decode(request.relative_uri().path()));
    if (paths.size() < 3) {
        request.reply(status_codes::BadRequest, "Invalid request format");
        return;
    }

    std::string movieName = paths[2];
    if (moviesByName.find(movieName) != moviesByName.end()) {
		//https://microsoft.github.io/cpprestsdk/classweb_1_1json_1_1array.html
		//https://microsoft.github.io/cpprestsdk/classweb_1_1json_1_1value.html
        json::value responseJson = json::value::array();
        auto& movieList = moviesByName[movieName];
		int index = 0;

        for (auto& movie : movieList) {
            json::value movieJson;
            movieJson["ID"] = json::value::string(movie.ID);
            movieJson["Runtime"] = json::value::number(movie.Runtime);
            movieJson["Title"] = json::value::string(movie.Title);
            movieJson["URL"] = json::value::string(movie.URL);
            movieJson["Year"] = json::value::number(movie.year);

            responseJson[index] = movieJson;
			index++;
        }

        request.reply(status_codes::OK, responseJson);
    } else {
        json::value errorJson;
        errorJson["Error"] = json::value::string("No movie by that name found");
        request.reply(status_codes::NotFound, errorJson);
    }
}

void ProcessCommandArgs(int argc, const char* argv[])
{
	// TODO: Implement
		if (argc >= 2) {
			std::string path = argv[1];
			std::ifstream file(path);
			if (file.is_open()) {
				std::string line;
				std::getline(file,line);
				while (std::getline(file,line)) {
					std::vector<std::string> fields;
					std::istringstream ss(line);
					std::string temp1;
					while (std::getline(ss, temp1, 	'\t')) {
						fields.push_back(temp1);
					}
					if (fields[1] == "movie") {
						Movie movie;
						movie.ID = fields[0];
						movie.Runtime = (fields[7] == "\\N") ? 0 : stoi(fields[7]);
						movie.Title = fields[2];
						movie.year = (fields[5] == "\\N") ? 0 : stoi(fields[5]);
						movie.URL = "https://www.imdb.com/title/" + fields[0] + "/";
						moviesById[movie.ID] = movie;
						moviesByName[movie.Title].push_back(movie);
					}
				}
			}
			file.close();
			std::cout << "here";
			
			//https://microsoft.github.io/cpprestsdk/classweb_1_1http_1_1experimental_1_1listener_1_1http__listener.html
			//https://microsoft.github.io/cpprestsdk/classweb_1_1uri.html
			http_listener listener("http://localhost:12345");
			listener.support(methods::GET, [](http_request request) {
				auto path = uri::split_path(request.relative_uri().path());
				if (path.size() >= 3 && path[1] == "id") {
					handleGetMovieById(request);
				} else if (path.size() >= 3 && path[1] == "name") {
					handleGetMovieByName(request);
				} else {
					request.reply(status_codes::BadRequest, "Invalid endpoint");
				}
			});

			try {
				listener.open().wait();
				std::cout << "Server is listening on port 12345." << std::endl;

				// Keep the server running, given by the website
				std::this_thread::sleep_for(std::chrono::seconds(50000));
			} catch (const std::exception& e) {
				std::cerr << "Error: " << e.what() << std::endl;
				return;
			}

		}
}
