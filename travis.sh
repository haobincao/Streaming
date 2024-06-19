#!/bin/bash
curl -L -O http://chalonverse.com/435/pa7.tar.gz || { echo "::error::Unable to download graded tests. Try again."; exit 1; }
tar xzf pa7.tar.gz || { echo "::error::Error downloading graded tests. Try again."; exit 1; }
echo "Downloading IMDB data..."
curl -O https://datasets.imdbws.com/title.basics.tsv.gz || { echo "::error::Unable to IMDB data. Try again."; exit 1; }
gunzip title.basics.tsv.gz

# Cmake into build directory
echo "Compiling..."
mkdir build
cd build
RELEASE=ON CC=gcc CXX=g++-11 cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. || exit 1
build_failed=0
make 2> >(tee diagnostics.txt >&2)
if [ "${PIPESTATUS[0]}" -ne "0" ] ; then
    echo "::error::Code did not compile!"
    echo -e "## \xF0\x9F\x9A\xA8\xF0\x9F\x9A\xA8 Code did not compile!! Your grade is currently a 0!! \xF0\x9F\x98\xAD\xF0\x9F\x98\xAD" >> ${GITHUB_STEP_SUMMARY}
	echo "### Build Log"  >> ${GITHUB_STEP_SUMMARY}
    echo -n "<pre>" >> ${GITHUB_STEP_SUMMARY}
    cat diagnostics.txt >> ${GITHUB_STEP_SUMMARY}
    echo "</pre>" >> ${GITHUB_STEP_SUMMARY}
	build_failed=1
fi

cd ..
./diagnostics-json.py
if [[ "$build_failed" == 1 ]] ; then
	exit 1
fi

cd build
# Run clang-tidy
echo "Running clang-tidy..."
../run-clang-tidy.py -quiet -header-filter=".*/src/[a-zA-Z].*" -export-fixes=tidy.yaml | tee clang-tidy.txt 
if [ "${PIPESTATUS[0]}" -ne "0" ] ; then
	echo "::warn::clang-tidy failed to run"
fi
# Return to root folder (so cwd is correct)
cd ..
./tidy-json.py

# Try to spin up server
build/main title.basics.tsv &

# Try to connect to server
echo "Waiting for server to start up..."
wget --tries=7 --waitretry=2 --retry-connrefused http://localhost:12345/movie/id/tt0092099 || { echo "::error::Could not connect to server in time! This means either your server is too slow to start, or it's immediately starting before the data is loaded, or it's returning a 404 incorrectly"; echo -e "## Grade Report\n\n## \xF0\x9F\x9A\xA8\xF0\x9F\x9A\xA8 Could not connect to server in time! Your grade is currently a 0!! \xF0\x9F\x98\xAD\xF0\x9F\x98\xAD\n\nThis could mean a variety of things like your server is too slow to start, or it's immediately starting before the data is loaded, or it's returning a 404 incorrectly." >> ${GITHUB_STEP_SUMMARY}; exit 1; }

# Run graded tests
echo "Running tests..."
tests_failed=0
timeout 30 newman run postman/pa7-tests.postman_collection.json -r cli,json-summary --color on --reporter-summary-json-export summary.json || tests_failed=1

./summary-github.py

cd build
echo -e "\n## Compiler Warnings" >> ${GITHUB_STEP_SUMMARY}
if grep -q warning diagnostics.txt; then
	echo -e "\xE2\x9A\xA0 There are compiler warnings\n" >> ${GITHUB_STEP_SUMMARY}
	echo -en "<details closed><summary>Build Log</summary><pre>" >> ${GITHUB_STEP_SUMMARY}
	cat diagnostics.txt >> ${GITHUB_STEP_SUMMARY}
	echo -e "</pre></details>\n" >> ${GITHUB_STEP_SUMMARY}
else
	echo -e "\xE2\x9C\x85 There were no compiler warnings\n" >> ${GITHUB_STEP_SUMMARY}
fi

echo -e "\n## clang-tidy Warnings" >> ${GITHUB_STEP_SUMMARY}
if grep -q "warning:" clang-tidy.txt; then
	echo -e "\xE2\x9A\xA0 There are clang-tidy warnings\n" >> ${GITHUB_STEP_SUMMARY}
	echo -en "<details closed><summary>clang-tidy Log</summary><pre>" >> ${GITHUB_STEP_SUMMARY}
	cat clang-tidy.txt >> ${GITHUB_STEP_SUMMARY}
	echo -e "</pre></details>\n" >> ${GITHUB_STEP_SUMMARY}
else
	echo -e "\xE2\x9C\x85 There were no clang-tidy warnings\n" >> ${GITHUB_STEP_SUMMARY}
fi

if [[ "$tests_failed" == 1 ]] ; then
	echo "::error::Not all graded tests passed!"
	exit 1
fi
