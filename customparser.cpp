// Complete me
#include "customparser.h"
#include "parser.h"
#include <iostream>
#include <sstream>

using namespace std;

const char* error_msg_1 = "Cannot read integer n";
const char* error_msg_2 = "Error in content parsing";
const char* error_msg_3 = "Error in user parsing";

// To Do - Complete this function
void CustomFormatParser::parse(std::istream& is, std::vector<Content*>& content, std::vector<User*>& users)
{
  // Erase any old contents
  content.clear();
  users.clear();

  // TO DO - Add your code below.
  int n;
  string tmp;
  if (getline(is>>ws, tmp)) {
    stringstream ss1(tmp);

    if (ss1 >> n) {
      for (int i = 0; i < n; i++) {
        string buf;
        int content_id;
        int content_type;
        if (getline(is>>ws, buf)) {
          stringstream ss(buf);
          if (ss >> content_id) {
            if (ss >> content_type) {
              string content_name;
              // getline(is, content_name, '\t');
              if (getline(is >> ws, content_name)) {
                int num_views;
                if (is >> num_views) {
                  int total_stars;
                  if (is >> total_stars) {
                    int rating;
                    if (is >> rating) {
                      Content* a;
                      if (content_type == 0) {
                        a = new Movie(content_id, content_name, num_views, total_stars, rating);
                      } else {
                        int num_episodes;
                        if (is >> num_episodes) {
                          a = new Series(content_id, content_name, num_views, total_stars, rating, num_episodes);
                        } else {
                          throw ParserError(error_msg_2);
                        } 
                      }
                      content.push_back(a);

                      getline(is, buf);
                      if (getline(is>>ws, buf)) {
                        stringstream ss(buf);      
                        string viewer;
                        while(ss >> viewer) {
                          content[content.size()-1]->addViewer(viewer);
                        }
                      }else {
                        throw ParserError(error_msg_2);
                      }  

                    }else {
                      throw ParserError(error_msg_2);
                    }
                  }else {
                    throw ParserError(error_msg_2);
                  }
                }else {
                  throw ParserError(error_msg_2);
                }
              }else {
                throw ParserError(error_msg_2);
              }
            }else {
              throw ParserError(error_msg_2);
            }
          }else {
            throw ParserError(error_msg_2);
          }
        } else {
          throw ParserError(error_msg_2);
        }
      }
    } else {
      throw ParserError(error_msg_1);
    }

    string user_name;
    while (is >> user_name) {
      int rate_limit;
      if (is >> rate_limit) {
        User* newU = new User(user_name, rate_limit);
        cout << "User:" << user_name << "(" << rate_limit << ") created" << endl;
        int viewed_id;
        while (is >> viewed_id) {
          newU->addToHistory(viewed_id);
          cout << "History added: " << viewed_id << endl;
        }
        is.clear();
        users.push_back(newU);
      }else {
        throw ParserError(error_msg_3);
      }

    }

    for (size_t i = 0; i < users.size(); i++) {
      cout << "User name: "<< users[i]->uname << endl; 
      for (size_t j = 0; j < users[i]->history.size(); j++) {
        cout << "History: ";
        cout << users[i]->history[j] << endl;
      }
      cout << endl;
    }
  } 

}
