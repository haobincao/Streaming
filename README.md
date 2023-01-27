# PR5

# ***3. Background***

Just read this whole page to get an idea of things you will implement when you start coding. The actual procedure and order for how you can code this assignment are on the next pages.

## **Streaming Service Components**

In our fictitious streaming service we will store information about **content** and **users**.

### **Content**

Content are **movies** and **series** that users can watch. Whether a movie or a series, all content has the following data:

- (unique) integer **ID**
- a **name** (by which it can be searched)
- a count of its **reviews** and the total number of **stars** given from all the reviews so that an average can be easily found. We will assume a scale of up to 5 stars per review. For example, if a piece of content had 3 reviews and 10 total stars, its average would be 3.33 stars.
- a parental control **rating** (0-4) where the exact values differ based on whether the content is a movie or series but in which 0 is the most general (i.e. G rating) and 4 is the most restrictive. We will handle the mapping of these numbers to the common Movie scale (G, PG, PG-13, R) and the TV Parental Guidance (TV-Y, TV-G, TV-PG, etc.), so just use the numeric values.
- a list of the **viewer** usernames who have viewed this content

While for **movies**, this information is sufficient and no “movie-specific” info is needed, **series** also need to store:

- how many **episodes** the series contains.

Because there are at least 2 **types** of **content** (i.e. movies and series), it would seem appropriate to use an inheritance hierarchy with common aspects in a **base** `Content` class and then define derived classes: `Movie` and `Series` to implement any unique behavior of the two (along with any specific data for them).

While it would be best to make a separate `.h` and `.cpp` file for each of the three classes, we will define all three classes in a single header and cpp file: `content.h/cpp`.

We encourage you to briefly skim these files now.

### **Users**

The streaming service wants to track users to identify their viewing history. Thus, our `User` objects will need to store a:

- a unique **username** (for potential future additions to the project, will assume a max of 8 characters, but this should not concern you for now.)
- a **rating limit** of the highest (most-restrictive) content this user can watch. This number is used in conjunction with the rating value (0-4), described above, stored in each Content item. Users should not be able to “watch” content that is rated higher than this limit.
- a list of the IDs of the content the user has watched.

To store this info, we will define a `User` class in `user.h/cpp`. Since there are very few non-trivial operations to perform on a User, we have made it a `struct` (which defaults to public access) to avoid the need for `get` and `set` methods.

We encourage you to look over these files now.

### **Streaming Service**

To store all the `User` and `Movie`/`Series` content, we will define a `StreamService` class in `strmsrv.h/cpp`. The primary two data members of the class are two vectors:

- one containing pointers to the User objects and
- the other containing pointers to the derived Content objects.

Because `User`s store the IDs of the content they watch and `Content` items store the usernames that have viewed that content, these values collectively form a [bipartite graph](https://en.wikipedia.org/wiki/Bipartite_graph).

![https://apollo-media.codio.com/media%2F1%2Fa8d887fb304315a9b6285a9129a03a3e-3a9666887b9c05b1.webp](https://apollo-media.codio.com/media%2F1%2Fa8d887fb304315a9b6285a9129a03a3e-3a9666887b9c05b1.webp)

The `StreamService` class will then implement operations on this graph and provide all the core processing upon the objects.

The operations include:

- reading/parsing the initial data from an input file
- allowing a user to login. The system will allow one user to “use” the service at the same time. There is no password or other access check, but we simply want to track who is the “current” user.
- allow a user to logout
- get the user’s viewing history (what content they have viewed)
- search for content given a partial name / string to filter on or a `` which indicates the user wants to see ALL content
- watch a movie or series (for our purposes, that means just update the viewing history of the user and the chosen content).
- add a review with a given number of stars to particular content
- ask for suggestions for the best, similar content to a given movie or series.

All these operations will require you to access and modify the appropriate `User`, `Movie`, and `Series` objects.

## **Input File Parsing**

We will initialize our `StreamService` object using data that is read in from a file (representing a form of database). The goal is to read and parse the data from the file to create appropriate `User`, `Movie`, and `Series` objects that can be stored in the `StreamService`.

We would like to support multiple file formats for this database. Thus, we have created an **abstract base class**, `Parser`, with a pure, virtual `parse` function as shown below.

**`class** **Parser** {
**public**:
    **virtual** ~Parser() {}
    **virtual** **void** **parse**(
            std::istream& is, 
            std::vector<Content*>& content, 
            std::vector<User*>& users) = 0;
};`

Note that we pass two vectors by **reference**. The caller, which will be the `StreamService` object, can pass its vectors to the function, which will allow the parser to fill in those vectors for the `StreamService`.

We have provided you with a **working** JSON-format parser. JSON (short for Javascript Object Notation ) is a very popular text-based format to represent objects using types: integers, booleans, strings, doubles/floats, lists of any of these, and objects composed of any of these (including other objects). Each value has an identifier to describe/lookup the associated value.

In this format items enclosed in `{}` represent an “object” and items enclosed in `[]` represent items in a list/array.

A sample file `data1.json` is provided in your code skeleton and shown below:

`{
    "content" : [
        {
            "id" : 0,
            "type" : 0,
            "name" : "Black Panther: Wakanda Forever",
            "reviews" : 4,
            "stars" : 18,
            "rating" : 3,
            "viewers" : ["user1", "user2", "user4", "user5"]
        },
        {
            "id" : 1,
            "type" : 1,
            "name" : "Stranger Things 1",
            "reviews" : 3,
            "stars" : 13,
            "rating" : 3,
            "episodes" : 8,
            "viewers" : ["user1", "user2", "user3"]
        },
        {
            "id" : 2,
            "type" : 1,
            "name" : "Stranger Things 2",
            "reviews" : 2,
            "stars" : 5,
            "rating" : 3,
            "episodes" : 9,
            "viewers" : ["user2", "user3"]
        }
    ],
    "users" : [
        {
            "uname" : "user1",
            "limit" : 3,
            "viewed" : [1]
        },
        {
            "uname" : "user2",
            "limit" : 4,
            "viewed" : [0, 1, 2]
        },
        {
            "uname" : "user3",
            "limit" : 4,
            "viewed" : [0, 2]
        },
    ]
}`

Here we see there are 3 content items: 1 movie (**type=0**) and 2 series (**type=1**) as well as 3 users. Each content and user object (defined in `{}`) has strings to identify the “member” name with the corresponding value that follows after the `:`.

Our working JSON Parser in `jsonparser.h/cpp` will read in files of this format and create `Movie`, `User`, and `Series` objects.

**DO NOT MODIFY** the JSON parser we provide, as we will use it to test your other components to ensure the data we want to test with is correctly read in.

### **Custom Format**

As you can see, the JSON format is a bit verbose (lengthy) and we could define a more compact, **custom** format. We define this custom format below and it will be your task to implement a derived parser to read in data files that use that custom format. An example of this format for the same JSON file above is shown below:

`3
0 0
    **Black** Panther: Wakanda Forever
    4
    18
    3
    user2 user3 
1 1
    **Stranger** Things 1
    3
    13
    3
    8
    user1 user2
2 1
    **Stranger** Things 2
    2
    5
    3
    9
    user2 user3
user1 3
1
user2 4
0 1 2
user3 4
1 2`

We attempt to describe the format below (descriptive comments appear after the `//` and would not actually be in the file).

`<n>  *// number of Content items to follow*<cid> <ctype>       *// Content-ID 0, content-type: 0=movie, 1 = series*
[TAB]<content-name>   *// the entire string on the line (with the TAB removed)*
[TAB]<num-reviews>    *// Integer number of reviews*
[TAB]<total-starts>   *// Integer of total stars over all reviews*
[TAB]<rating>         *// Integer 0-4 of viewer applicability*
[TAB]<viewer1-uname> ... <viewerk-uname> *// 0 or more Usernames of viewers// all one one line*
...
<cid> <ctype>       *// Content-ID n-1, content-type: 0=movie, 1 = series*
[TAB]<content-name>   *// the entire string on the line (with the TAB removed)*
[TAB]<num-reviews>    *// Integer number of reviews*
[TAB]<total-starts>   *// Integer of total stars over all reviews*
[TAB]<rating>         *// Integer 0-4 of viewer applicability*
[TAB]<viewer1-uname> ... <viewer-k-uname> *// 0 or more Usernames of viewers// all one one line*<username-1> <rating-limit>
<viewed-cid-1>...<viewed-cid-k>  *// 0 or more content IDs of viewed content*
...
<username-m> <rating-limit>
<viewed-cid-1>...<viewed-cid-k>  *// 0 or more content IDs of viewed content*`

A few notes:

- Notice that while we give you an integer that identifies how many Content objects follow, we do **NOT** provide such a value for how many `User` objects will follow the Content section. You should use different techniques then to read in the Content and Users.
- There could be any amount of whitespace between fields unless prohibited below. For example, the content item below **IS** valid:

`0 0
    **Black** Panther: Wakanda Forever
    4  18

    3
    user2 user3`

- The name of the Content will be on a single line immediately following the line containing the content ID and type. It will start with a single TAB character that you should strip off and not store.
- The usernames who viewed a particular content item will appear on the very next line after the rating (for a Movie) or after the number of episodes (for a series). There could be ANY number of usernames on that line and no integer count will precede it. This should guide you on how you read them in.
- For users, the username and rating limit could be separated by any amount of whitespace. However, the viewed content will appear on a single line immediately AFTER the rating limit (i.e. on the next line after the rating limit).

Based on these notes, consider how to use the `ifstream` and `stringstream` objects appropriately in conjuction with `operator>>` and `getline()`. Think carefully about where `operator>>` stops and how that might affect your attempt to get the next line of data.

We use the extension `.json` to determine the appropriate parser to use. Input data files ending with `.json` will be parsed with our JSON parser, and files ending with the **`.cus`** format will be parsed using your custom parser. Recall, there’s nothing in a filename that FORCES it to be a certain type. Meaning, even though we name a file with a `.cus` extension, it’s just a text file that you can edit in your editor.

## **Stream application**

At the highest level, we have created a `main()` function in `stream.cpp` that creates and calls the appropriate parser, and then implements a menu system to allow the user to perform various operations on the `StreamService`.

`===================================================
QUIT:              0
LOGIN:             1 uname
LOGOUT:            2
GET USER HISTORY:  3
SEARCH:            4 <* | string>
WATCH:             5 <content-id>
REVIEW:            6 <content-id> <number-of-stars>
SUGGEST SIMILAR:   7 <content-id>
===================================================`

Generally, a user would login via command `1`:

`1 user1`

so that the system knows what User is performing the next operations.

For example, the command:

`4 *`

would search and then display ALL content items (`*` = ALL content), while

`4 Stranger`

would show only the content with `Stranger` somewhere in its name.

`6 1 2`

Will give a review to content item `1` of `2` stars

`4 Stranger Things 1`

Will verify the review has been added.

`0`

Will exit the program

The program will need a command line argument to specify the input file that we should parse, and we will use that file’s extension to determine which parser to use.

We show a sample program run below: `./stream data1.cus`

`Read 3 content items.
Read 3 users.
Menu:
===================================================
QUIT:              0
LOGIN:             1 uname
LOGOUT:            2
GET USER HISTORY:  3
SEARCH:            4 <* | string>
WATCH:             5 <content-id>
REVIEW:            6 <content-id> <number-of-stars>
SUGGEST SIMILAR:   7 <content-id>
===================================================

Enter a command: 
1 user1

Enter a command: 
4 *
Results: 
========
ID: 0
Name: Black Panther: Wakanda Forever
Views: 2
Average review: 4.5
========
ID: 1
Name: Stranger Things 1
Views: 2
Average review: 4.3
Episodes: 8
========
ID: 2
Name: Stranger Things 2
Views: 2
Average review: 2.5
Episodes: 9

Enter a command: 
4 Stranger
Results: 
========
ID: 1
Name: Stranger Things 1
Views: 2
Average review: 4.3
Episodes: 8
========
ID: 2
Name: Stranger Things 2
Views: 2
Average review: 2.5
Episodes: 9

Enter a command: 
6 1 2

Enter a command: 
4 Stranger Things 1
Results: 
========
ID: 1
Name: Stranger Things 1
Views: 2
Average review: 3.8
Episodes: 8

Enter a command: 
0`

## **Making Content Suggestions**

We can use the connectivity of our User/Content graph to make Content suggestions. We will ask you to recommend similar Content based on the viewing history of other users who viewed a particular Content item. To be more specific, the current user, UqUq, will ask for suggestions similar to the specified Content ID, CpCp. You should find the set of users, UjUj, (Uj≠Uq)Uj≠Uq) who also watched content CpCp and then find the single content item CkCk, (Ck≠Cp)Ck≠Cp) that was viewed the **most** times by users in the set UjUj.

Consider the example below, where user 1 asks for suggestions similar to content 1.

We first find the OTHER users who viewed that content. This is the set UjUj we indicate above and should consist of: **User 2 and User 4**. We proceed to then look at the content THOSE users in UjUj have watched (but ignore all users that are not in UjUj, such as User 3). We want to find the content item that User 1 has NOT watched that has the most views from users in set UjUj.

Content 4 and Content 5 were both viewed by User 2 and 4 (and thus have a totoal of 2 views from the users in set UjUj) but Content 5 was already watched by User 1 and so we cannot suggest it.

The final result for suggestion is Content 4.

If not Content items meet the criteria, then you should return `-1`.

**Assumption:** You may assume that if there is a tie for the best suggestion (most views from user in set UjUj) that you can choose to return/suggest any of them. Likely, our tests will ensure no ties exist.

![https://apollo-media.codio.com/media%2F1%2F4a81b1c1307344cf54e115c79d7682e8-f37f3726b81ed7b0.webp](https://apollo-media.codio.com/media%2F1%2F4a81b1c1307344cf54e115c79d7682e8-f37f3726b81ed7b0.webp)

## **Exceptions**

Part of this assignment is to practice utilizing exceptions, both defining, throwing, and catching them.

As indicated in lecture, it is generally good to use an inheritance hierarchy when defining exceptions. In `strmsrv.h` we have defined two exceptions: `UserNotLoggedInError` to be thrown when an operation is performed when a user has not logged in first (i.e. there is no “current” user), a `RatingLimitError` to be thrown when a user attempts to watch a Content item whose rating is higher than the User’s rating limit, and `ReviewRangeError` to be thrown if a review is given that it not between 0-5 stars.

**`struct** **UserNotLoggedInError** : **public** std::runtime_error
{  ...  };
**struct** **RatingLimitError** : **public** std::runtime_error
{  ...  };
**struct** **ReviewRangeError** : **public** std::range_error
{  ...  };`

Similarly in `parser.h` we have defined `ParserError` which parser’s can throw when they encounter an unexpected error.

**`struct** **ParserException** : **public** std::runtime_error
{  ...  };`

You will need to ensure that appropriate exceptions are caught and error messages printed to pass all of our tests. Most of this work will occur in `stream.cpp` (where our `main()` application is defined).

# **4. Procedure & Tests**

We have broken the work into parts.

## **Task 1. User and Content classes**

1. In `content.h`, the `Content` class is complete but you will need to update the `Movie` and `Series` class to use inheritance.
    - You should add appropriate data members to the `Movie` and `Series` class, as necessary
    - You will need to decide if you need to override the implementation of `display()` for each of these classes
2. In `content.cpp`
    - implement the `Content` constructor and decide if you need to implement the destructor
    - implement the `Content::getStarAverage()`
    - implement the constructor and any overridden virtual functions of the `Movie` class. Be sure that a call to `display()` will display the `Movie` info using our expected format shown in the examples in the previous page of the Guide. (If you implement this correctly by utilizing the help of `Content::display()` you will not have to worry about meeting the format, since our function already meets it).
    - implement the constructor and any overridden virtual functions of the `Series` class. Be sure that a call to `display()` will display the `Series` info using our expected format **which includes** the number of episodes. You should ensure the display format meets the format of the examples on the previous Guide Page. Again, utilize `Content::display()` to do a majority of the work and only output additional information as needed.
    - complete the simple `int Series::numEpisodes() const` accessor to return how many episodes a series has (we use this in our tests to ensure correct results).
    - You will notice that we have implemented a **protected** virtual function, `ratingStrings()` that our **non-virtual** `Content::display()` uses to ensure the right rating strings are displayed. See the in-class exercise named `bracketer` in the *Polymorphism* section for a similar example. You should not have to add or modify any code, but we want you to understand this structure and how it works.
3. The `User` class is currently complete.

To check your C++ syntax (but not fully test your code) you can compile **just** the `Content` class file using the `-c` (compile only) flag to `g++` which compiles just one file into an intermediate form known as an object file:

`make test-content-display`

And try to run that program:

`./test-content-display`

### **Content Tests**

The following test creates a `Movie` and `Series` object, initializes its values, and then calls the `display()` functions, to ensure you display the info using our specified format.

## **Task 2. Stream Service Basic Operations**

Examine the `strmsrv.h` file, noting the function and especially the private helper functions and data members. **Please note the exceptions that our documentation indicates should be thrown by each member function** under the various potential error conditions. Then complete the functions indicated by comments in `strmsrv.cpp` which include:

- The constructor and destructor
- `userLogin()` should check for appropriate error cases, ensuring you throw the exceptions indicated in the function’s documentation in the header. If all is as it should be, set the `cUser_` pointer to point at the user object associated with the provided input username.
- `userLogout()` should reset the `cUser_` pointer to an appropriate value (think what that should be).
- `searchContent()`: If the string argument is `"*"`, return the IDs of all the content and if the string argument is anything else, return any content item that contains that string in its name. Hint: Look at the methods of the C++ `string` class especially `find()` and the constant `string::npos`. Note: search can be performed even if no user is loggedin.
- `getUserHistory()` is complete and returns the IDs of the content that the current (logged-in) user has viewed. But take note of how we use the `throwIfNoCurrentUser()` helper function to throw an exception if no user is logged in. Feel free to use this function in other operations.
- `watch()` should check the rating of the content and ensure it is suitable given the User’s rating limit. If so, update the appropriate `User` and `Content` objects. Be sure to throw the appropriate exceptions as indicated in the function’s documentation in the header.
- `reviewShow()` should add a review with the given number of stars to the specified Content. A valid review **must** be between 0-5 stars. Be sure to throw the appropriate exceptions (as indicated in the function’s documentation in the header file).
- We suggest you leave the `suggestBestSimilarContent()` implementation for later and keep the dummy/stub implementation we have given.

To check your syntax you can compile **just** this class file using the `-c` (compile only) flag to `g++` which compiles just one file into an intermediate form known as an object file:

`g++ -std=c++11 -g -Wall -c strmsrv.cpp -o strmsrv.o`

### **Tests (15 pts)**

At this point you should be able to make the entire application, though you may need to define a dummy `parse()` function for the `CustomParser` in `customparser.cpp`.

`make stream
./stream data1.json`

and you can run various menu options which should exercise the various operations you just wrote. **PLEASE** try to run some of these yourself, thinking through what should happen, how you can then view results to ensure operations occurred correctly, etc.

Once you have tested your code with the `./stream` program just described, you can run basic tests on these `StreamService` operations (along with your derived `Content` classes) with the following tests. Ensure these work before moving on.

## **Task 3. Parsing**

Now move on and implement the Custom Parser in `customerparser.h/cpp`. Use the description of the custom file format on the previous Guide page and use your knowledge of `ifstream`, `stringstream` `operator>>`, `getline()`, and `get()`.

Recall that the version of `getline()` that is a member of the `istream` class **uses `char*`s**.

[C++ documentation](https://cplusplus.com/reference/istream/istream/getline/) of `getline()`

`istream& **getline** (**char*** s, streamsize n );
istream& **getline** (**char*** s, streamsize n, **char** delim );`

There is a separate version of `getline()` that is **NOT A MEMBER** of `istream`, but simply a **Global** scope function that uses **C++ strings**.

[C++ documentation](https://cplusplus.com/reference/string/string/getline/) for the `string` version of `getline()`

`istream& **getline** (istream& is, string& str);
istream& **getline** (istream& is, string& str, **char** delim);`

Take care after using `operator>>` to extract data and then try to use `getline()`. Remember `operator>>` stops **AT/BEFORE** the trailing whitespace.

Furthermore, be sure you review your lecture notes about **when** to check `fail()` when reading data so that you don’t accidentally check for failure too late and end up USING bogus data.

### **Data files**

We have provided two small test databases in `data1.json`/`data1.cus` and `data2.json`/`data2.cus`. The items in `data1.json` and `data1.cus` are the same except for the format of representation. The similar analog is true for `data2.json` and `data2.cus`. **We strongly recommend** copying one or both of these and editing them to introduce errors or valid whitespace additions to ensure your custom parser handles these appropriately.

### **Errors to handle**

As you implement the parser, there are certain errors you need to check for and handle. On the previous Guide page we already discussed certain assumptions you MAY or MAY NOT make and where whitespace can appear.

When you do detect an error you should throw the `ParserError` exception we define in `parser.h` with one of the following “what” messages we defined (see below for more details).

**`const** **char*** error_msg_1 = "Cannot read integer n";
**const** **char*** error_msg_2 = "Error in content parsing";
**const** **char*** error_msg_3 = "Error in user parsing";`

But you must also check the following:

- If you cannot read in the initial integer, `n` that indicates how many `Content` items will follow, you should throw `ParserError` using `error_msg_1` as the `what` message.
- As you try to read `Content` items, you should try to read all the pieces of content (id, name, number of reviews, etc.) BEFORE the list of viewer usernames and then check if a failure occurred reading in those values. If so, throw `ParserError` using `error_msg_2` as the `what` message. The next line will contain the usernames of those who viewed that content and you don’t need to check that for errors.
- Once you read the `n` Content items and move into the section of user data, you should first check if you were successfully able to read in a username (any string really). If not, this should indicate the end of the file. If a username is read successfully, you should try to read in a rating limit integer, and throw `ParserError` if you are not able to, using `error_msg_2` as the `what` message.
    - If you can read in an integer rating limit, then you may assume the next line contains content IDs of their viewing history and need not worry about checking for errors as you read those content IDs.

### **Tests (42 pts)**

Test your parser using some of the `.cus` data files we’ve provided.

`make stream
./stream data1.cus`

If the program segfaults before even getting to the menu, then you need to use `gdb`, `valgrind`, and `cout` statements to debug your parsing. Expect bugs and write some `cout` statements AS YOU CODE the parser.

Once you can parse without crashing, then verify the data was read in correctly by trying some of the following:

- Run a search for `` (all) content to view the data your parsed
- Login as different users and get their viewing history to ensure you parsed users correctly

**PLEASE** try to run some of these yourself, thinking through what should happen, and how you can verify your parser works before moving on to the tests below.

Once you believe your parser works, run the tests below.

This test has some basic input files that follow the expected format.

This test uses files that have additional whitespace in the allowed areas.

This test uses files that have errors which should cause appropriate errors/exceptions to be generated.

## **Task 4. Main application and Exception handling**

In `stream.cpp` we have given a majority of the code to create and parse the input database file, receive commands and call the appropriate `StreamService` operations. Your task will be to add appropriate exception handling.

1. Near the top of `main()` we have code that will create parsers and use the appropriate one to read in the input file. Your task is to wrap that code in a `try..catch` block and, if an exception is thrown by the parser, you should handle it differently by the type of exception. If you catch:a. **ParserError**: Print the `what()` message in the `ParserError` and `return 1` from main().b. **Any other exception**: Print out the message `"Parser exception occurred"` (which we have defined in the `parser_except_msg` variable in `main()`).
2. In the main loop where we accept commands from the users, we have placed each iteration of the command loop in a `try..catch` block that will simply print out a message: `"Error processing command."` and move on to the next command. However, for certain errors, we would like to see more appropriate messages or even solve the issue. You need to add the following capabilities:
    - If a `UserNotLoggedInError` was thrown, printout the message `No user logged in` which we have defined in the string `no_user_except_msg` at the top of `main()`. Then proceed to the next iteration of the loop to continue processing commands.
    - If the user attempts to watch content that is rated higher than the User’s rating limit, and a `RatingLimitError` is thrown, output `User not permitted to view this content` which we have defined in `rating_limit_exception_msg` at the top of `main()`.
    - If a user attempts to review a content item and `ReviewRangeError` is thrown, output `Review should be between 0-5 stars` which we have defined `review_range_except_msg` in the string at the top of `main()`.

## **Task 5. Content Suggestions**

Now implement the `StreamService::suggestBestSimilarContent()` member function. Review the algorithm we describe on the previous Guide page and convert it to code. Be sure to throw the appropriate exceptions as indicated in the function’s documentation in the header.

Be sure your algorithm runs on the latest values of the Content and User data, meaning that after we read in the data file, asking for suggestions BEFORE and then again AFTER some users watch new content, can lead to different suggestions. You should always use the latest updated data and not precompute some portion of the results.

You can consider making a separate input file to test your code, but please run a few tests using `data1.json` or `data2.json`.