#include "strmsrv.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
using namespace std;

// To do - Complete this function
StreamService::StreamService()

{
  cUser_ = NULL;
  // Update as needed
}

// To do - Complete this function
StreamService::~StreamService()
{
  while(!users_.empty()) {
    delete users_[users_.size() - 1];
    users_.pop_back();
  }

  while(!content_.empty()) {
    delete content_[content_.size() - 1];
    content_.pop_back();
  }

}

// Complete - Do not alter
void StreamService::readAndParseData(std::istream& is, Parser& p)
{
  p.parse(is, content_, users_);
  cout << "Read " << content_.size() << " content items." << endl;
  cout << "Read " << users_.size() << " users." << endl;
}

// To do - Complete this function
void StreamService::userLogin(const std::string& uname)
{
  bool found = false;
  for (size_t i = 0; i < users_.size(); i++) {
    if (users_[i]->uname == uname) {
      if (cUser_ == NULL) {
        cUser_ = users_[i];
      } else {
        throw runtime_error("Another user is logged in");
      }
      found = true;
    } 
  }
  if (!found) {
    throw invalid_argument("Invalid user name");
  }
}

// To do - Complete this function
void StreamService::userLogout()
{
  if (cUser_ != NULL) {
    cUser_ = NULL;
  }
}

// To do - Complete this function
std::vector<CID_T> StreamService::searchContent(const std::string& partial) const
{
  std::vector<CID_T> results;
  for(size_t i = 0; i < content_.size(); i++){
    if (partial == "*" || (content_[i]->name().find(partial) != string::npos)) {
      results.push_back(i);
    }
  }
  return results;
}

// Complete - Do not alter
std::vector<CID_T> StreamService::getUserHistory() const
{
  throwIfNoCurrentUser();
  return cUser_->history;
}

// To do - Complete this function
void StreamService::watch(CID_T contentID)
{
  if (cUser_ == NULL) {
    throw UserNotLoggedInError("User not logged in");
  }

  bool found = false;
  for (size_t i = 0; i < content_.size(); i++) {
    if (content_[i]->id() == contentID) {
      if (content_[i]->rating() > cUser_->ratingLimit) {
        throw RatingLimitError("Rating limit error");
      } else {
        cUser_->history.push_back(contentID);
        content_[i]->addViewer(cUser_->uname);
      }
      found = true;
    }   
  }

  if (!found) {
    throw range_error("invalid range");
  }
}

// To do - Complete this function
void StreamService::reviewShow(CID_T contentID, int numStars)
{
  if (cUser_ == NULL) {
    throw UserNotLoggedInError("User not logged in");
  }

  if (numStars < 0 || numStars > 5) {
    throw ReviewRangeError("Range error");
  }

  bool found = false;

  for (size_t i = 0; i < content_.size(); i++) {
    if (content_[i]->id() == contentID) {
      found = true;
      content_[i]->review(numStars);
    }   
  }

  if (!found) {
    throw range_error("invalid range");
  }
}

// To do - Complete this function
CID_T StreamService::suggestBestSimilarContent(CID_T contentID) const
{
  vector<User*> usersViewedContent;
  // find users group that has watched the indicated content instance
  for (size_t i = 0; i < users_.size(); i++) {
    if (users_[i]->haveWatched(contentID) && users_[i] != cUser_) {
      usersViewedContent.push_back(users_[i]);
    }
  }

  int max_views = 0;
  int max_id = -1;
  // iterate through the content
  for (size_t i = 0; i < content_.size(); i++) {
    int actual_views = 0;
    if (content_[i]->id() != contentID && !cUser_->haveWatched(content_[i]->id())) {
      for (size_t j = 0; j < usersViewedContent.size();j++) {
        if (usersViewedContent[j]->haveWatched(content_[i]->id())) {
          actual_views++;
        } 
      }
      if (content_[i]->rating() <= cUser_->ratingLimit) {
        if (max_views < actual_views) {
          max_id = i;
          max_views = actual_views;
        }
      }
    }
  }

  if (max_id == -1) {
    for (size_t i = 0; i < content_.size(); i++) {
      if (!cUser_->haveWatched(content_[i]->id())) {
        for (size_t j = 0; j < usersViewedContent.size();j++) {
          if (!usersViewedContent[j]->haveWatched(content_[i]->id())) {
            return i;
          } else return -1;
        }
      } else return -1;
    }
  } 

  return content_[max_id]->id();
}

// To do - Complete this function
void StreamService::displayContentInfo(CID_T contentID) const
{
  // Do not alter this
  if(! isValidContentID(contentID)){
    throw std::invalid_argument("Watch: invalid contentID");
  }

  content_[contentID]->display(cout);
  // Call the display abitlity of the appropriate content object



}

// Complete - Do not alter
bool StreamService::isValidContentID(CID_T contentID) const
{
  return (contentID >= 0) && (contentID < (int) content_.size());
}

// Complete - Do not alter
void StreamService::throwIfNoCurrentUser() const
{
  if(cUser_ == NULL){
    throw UserNotLoggedInError("No user is logged in");
  }
}

// Complete - Do not alter
int StreamService::getUserIndexByName(const std::string& uname) const
{
  for(size_t i = 0; i < users_.size(); i++){
    if(uname == users_[i]->uname) {
      return (int)i;
    }
  }
  return -1;
}
